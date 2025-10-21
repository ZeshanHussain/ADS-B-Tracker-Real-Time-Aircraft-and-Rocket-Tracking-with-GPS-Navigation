#include "rocketclient.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>

RocketClient::RocketClient(QObject* parent)
    : QObject(parent),
      m_networkManager(new QNetworkAccessManager(this)),
      m_updateTimer(new QTimer(this)),
      m_source(SIMULATED),
      m_userLat(0.0),
      m_userLon(0.0)
{
    connect(m_updateTimer, &QTimer::timeout, this, &RocketClient::fetchData);
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &RocketClient::handleNetworkReply);
    
    setUpdateInterval(300000); // Update every 5 minutes
}

void RocketClient::setUpdateInterval(int msec) {
    m_updateTimer->setInterval(msec);
}

void RocketClient::setUserLocation(double lat, double lon) {
    m_userLat = lat;
    m_userLon = lon;
}

void RocketClient::start() {
    m_updateTimer->start();
    fetchData(); // Immediate first fetch
}

void RocketClient::stop() {
    m_updateTimer->stop();
}

void RocketClient::refresh() {
    fetchData();
}

void RocketClient::fetchData() {
    if (m_source == LAUNCH_LIBRARY) {
        // Launch Library 2 API - Free, no API key needed!
        // Get upcoming launches (next 10)
        QString url = "https://ll.thespacedevs.com/2.2.0/launch/upcoming/?limit=10&mode=detailed";
        
        QNetworkRequest request{QUrl(url)};
        request.setHeader(QNetworkRequest::UserAgentHeader, "AircraftRocketTracker/1.0");
        m_networkManager->get(request);
    } else {
        generateSimulatedData();
    }
}

void RocketClient::handleNetworkReply(QNetworkReply* reply) {
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        emit error(QString("Network error: %1").arg(reply->errorString()));
        // Fallback to simulated data on error
        generateSimulatedData();
        return;
    }
    
    QByteArray data = reply->readAll();
    parseLaunchLibraryData(data);
}

void RocketClient::parseLaunchLibraryData(const QByteArray& data) {
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        emit error("Invalid JSON response from Launch Library");
        return;
    }
    
    QJsonObject root = doc.object();
    QJsonArray results = root["results"].toArray();
    
    m_rockets.clear();
    
    for (const QJsonValue& value : results) {
        QJsonObject launch = value.toObject();
        
        QString id = launch["id"].toString();
        QString name = launch["name"].toString();
        
        // Get mission info
        QJsonObject mission = launch["mission"].toObject();
        QString missionName = mission["name"].toString();
        if (missionName.isEmpty()) {
            missionName = launch["mission"].toObject()["description"].toString();
        }
        
        // Get launch time
        QString netTime = launch["net"].toString();
        QDateTime launchTime = QDateTime::fromString(netTime, Qt::ISODate);
        
        // Get pad location
        QJsonObject pad = launch["pad"].toObject();
        QJsonObject padLocation = pad["location"].toObject();
        QString locationName = pad["name"].toString();
        
        double latitude = padLocation["latitude"].toDouble();
        double longitude = padLocation["longitude"].toDouble();
        
        // Get launch service provider
        QJsonObject lsp = launch["launch_service_provider"].toObject();
        QString provider = lsp["name"].toString();
        
        Rocket rocket(id, name, missionName, launchTime, latitude, longitude, locationName);
        rocket.setProvider(provider);
        
        // Set status
        QString statusName = launch["status"].toObject()["name"].toString().toLower();
        if (statusName.contains("go")) {
            rocket.setStatus(Rocket::GO);
        } else if (statusName.contains("success")) {
            rocket.setStatus(Rocket::SUCCESS);
        } else if (statusName.contains("failure")) {
            rocket.setStatus(Rocket::FAILURE);
        } else if (statusName.contains("hold")) {
            rocket.setStatus(Rocket::HOLD);
        }
        
        m_rockets.append(rocket);
    }
    
    emit rocketsUpdated(m_rockets);
}

void RocketClient::generateSimulatedData() {
    m_rockets.clear();
    
    // Known launch sites with coordinates
    struct LaunchSite {
        QString name;
        double lat;
        double lon;
    };
    
    QVector<LaunchSite> sites = {
        {"Cape Canaveral, FL", 28.5729, -80.6490},
        {"Vandenberg SFB, CA", 34.7420, -120.5724},
        {"Kennedy Space Center, FL", 28.5721, -80.6480},
        {"Baikonur Cosmodrome, Kazakhstan", 45.9650, 63.3050},
        {"SpaceX Starbase, TX", 25.9972, -97.1569}
    };
    
    QStringList missions = {
        "Starlink Mission", "ISS Resupply", "GPS Satellite Deployment",
        "Mars Sample Return", "Communication Satellite", "Earth Observation"
    };
    
    QStringList providers = {"SpaceX", "NASA", "ULA", "Blue Origin", "Rocket Lab"};
    
    for (int i = 0; i < 5; ++i) {
        LaunchSite site = sites[i % sites.size()];
        
        // Generate launch time (1 hour to 7 days from now)
        int secondsFromNow = QRandomGenerator::global()->bounded(3600, 7 * 86400);
        QDateTime launchTime = QDateTime::currentDateTime().addSecs(secondsFromNow);
        
        QString name = QString("%1 | %2").arg(providers[i % providers.size()])
                                         .arg(missions[i % missions.size()]);
        
        Rocket rocket(QString("SIM%1").arg(i), name, missions[i % missions.size()],
                     launchTime, site.lat, site.lon, site.name);
        rocket.setProvider(providers[i % providers.size()]);
        rocket.setStatus(Rocket::GO);
        
        m_rockets.append(rocket);
    }
    
    emit rocketsUpdated(m_rockets);
}

