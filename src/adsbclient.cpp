#include "adsbclient.h"
#include "geoutils.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>

ADSBClient::ADSBClient(QObject* parent)
    : QObject(parent),
      m_networkManager(new QNetworkAccessManager(this)),
      m_updateTimer(new QTimer(this)),
      m_source(SIMULATED),
      m_userLat(0.0),
      m_userLon(0.0),
      m_searchRadius(100000.0)  // 100 km default
{
    connect(m_updateTimer, &QTimer::timeout, this, &ADSBClient::fetchData);
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &ADSBClient::handleNetworkReply);
    
    setUpdateInterval(5000); // Update every 5 seconds
}

void ADSBClient::setUpdateInterval(int msec) {
    m_updateTimer->setInterval(msec);
}

void ADSBClient::setUserLocation(double lat, double lon) {
    m_userLat = lat;
    m_userLon = lon;
}

void ADSBClient::start() {
    m_updateTimer->start();
    fetchData(); // Immediate first fetch
}

void ADSBClient::stop() {
    m_updateTimer->stop();
}

void ADSBClient::fetchData() {
    switch (m_source) {
        case OPENSKY_NETWORK: {
            // OpenSky Network API - Free, no API key needed!
            // Bounding box around user location
            double latRange = 1.0; // ~111 km
            double lonRange = 1.0;
            
            QString url = QString("https://opensky-network.org/api/states/all?"
                                "lamin=%1&lomin=%2&lamax=%3&lomax=%4")
                            .arg(m_userLat - latRange)
                            .arg(m_userLon - lonRange)
                            .arg(m_userLat + latRange)
                            .arg(m_userLon + lonRange);
            
            QNetworkRequest request(url);
            m_networkManager->get(request);
            break;
        }
        
        case DUMP1090_LOCAL: {
            // Local dump1090 server (requires RTL-SDR hardware)
            QNetworkRequest request(QUrl("http://localhost:8080/data/aircraft.json"));
            m_networkManager->get(request);
            break;
        }
        
        case SIMULATED:
        default:
            generateSimulatedData();
            break;
    }
}

void ADSBClient::handleNetworkReply(QNetworkReply* reply) {
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        emit error(QString("Network error: %1").arg(reply->errorString()));
        return;
    }
    
    QByteArray data = reply->readAll();
    
    if (m_source == OPENSKY_NETWORK) {
        parseOpenSkyData(data);
    } else if (m_source == DUMP1090_LOCAL) {
        parseDump1090Data(data);
    }
}

void ADSBClient::parseOpenSkyData(const QByteArray& data) {
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;
    
    QJsonObject root = doc.object();
    QJsonArray states = root["states"].toArray();
    
    m_aircraft.clear();
    
    for (const QJsonValue& value : states) {
        QJsonArray state = value.toArray();
        if (state.size() < 17) continue;
        
        QString icao = state[0].toString();
        QString callsign = state[1].toString().trimmed();
        
        // Check for valid position
        if (state[5].isNull() || state[6].isNull()) continue;
        
        double lon = state[5].toDouble();
        double lat = state[6].toDouble();
        double altitude = state[7].toDouble(0.0); // meters (barometric altitude)
        double velocity = state[9].toDouble(0.0); // m/s
        double heading = state[10].toDouble(0.0); // degrees
        double verticalRate = state[11].toDouble(0.0); // m/s
        
        // Filter by distance from user
        double distance = GeoUtils::calculateDistance(m_userLat, m_userLon, lat, lon);
        if (distance > m_searchRadius) continue;
        
        Aircraft aircraft(icao, callsign, lat, lon, altitude, velocity, heading);
        aircraft.setVerticalRate(verticalRate);
        m_aircraft[icao] = aircraft;
    }
    
    emit aircraftUpdated(m_aircraft);
}

void ADSBClient::parseDump1090Data(const QByteArray& data) {
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;
    
    QJsonObject root = doc.object();
    QJsonArray aircraft = root["aircraft"].toArray();
    
    m_aircraft.clear();
    
    for (const QJsonValue& value : aircraft) {
        QJsonObject obj = value.toObject();
        
        QString icao = obj["hex"].toString();
        QString callsign = obj["flight"].toString().trimmed();
        
        if (!obj.contains("lat") || !obj.contains("lon")) continue;
        
        double lat = obj["lat"].toDouble();
        double lon = obj["lon"].toDouble();
        double altitude = GeoUtils::feetToMeters(obj["altitude"].toDouble(0.0));
        double velocity = GeoUtils::knotsToMetersPerSecond(obj["gs"].toDouble(0.0));
        double heading = obj["track"].toDouble(0.0);
        
        Aircraft ac(icao, callsign, lat, lon, altitude, velocity, heading);
        m_aircraft[icao] = ac;
    }
    
    emit aircraftUpdated(m_aircraft);
}

void ADSBClient::generateSimulatedData() {
    // Generate 3-8 simulated aircraft around user location
    int numAircraft = QRandomGenerator::global()->bounded(3, 9);
    
    m_aircraft.clear();
    
    for (int i = 0; i < numAircraft; ++i) {
        // Generate position within search radius
        double distance = 5000.0 + (QRandomGenerator::global()->bounded(static_cast<int>(m_searchRadius - 5000.0)));
        double bearing = QRandomGenerator::global()->bounded(360);
        
        // Calculate lat/lon offset
        double dLat = (distance / GeoUtils::EARTH_RADIUS) * std::cos(GeoUtils::toRadians(bearing));
        double dLon = (distance / GeoUtils::EARTH_RADIUS) * std::sin(GeoUtils::toRadians(bearing));
        
        double lat = m_userLat + GeoUtils::toDegrees(dLat);
        double lon = m_userLon + GeoUtils::toDegrees(dLon);
        
        // Generate realistic parameters
        double altitude = 3000.0 + QRandomGenerator::global()->bounded(9000); // 3-12 km
        double velocity = 100.0 + QRandomGenerator::global()->bounded(150); // 100-250 m/s
        double heading = QRandomGenerator::global()->bounded(360);
        
        QString icao = QString("SIM%1").arg(i, 3, 10, QChar('0'));
        QString callsign = QString("FL%1").arg(QRandomGenerator::global()->bounded(100, 999));
        
        Aircraft aircraft(icao, callsign, lat, lon, altitude, velocity, heading);
        m_aircraft[icao] = aircraft;
    }
    
    emit aircraftUpdated(m_aircraft);
}

void ADSBClient::removeStaleAircraft() {
    QMutableMapIterator<QString, Aircraft> it(m_aircraft);
    while (it.hasNext()) {
        it.next();
        if (it.value().isStale()) {
            it.remove();
        }
    }
}

