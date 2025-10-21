#include "systemgps.h"
#include <QDebug>

SystemGPS::SystemGPS(QObject* parent)
    : QObject(parent),
      m_source(nullptr),
      m_timeoutTimer(new QTimer(this)),
      m_hasValidPosition(false),
      m_latitude(0.0),
      m_longitude(0.0),
      m_altitude(0.0),
      m_speed(0.0),
      m_heading(0.0),
      m_accuracy(0.0),
      m_satelliteCount(0)
{
    // Try to create position source from system
    m_source = QGeoPositionInfoSource::createDefaultSource(this);
    
    if (m_source) {
        qDebug() << "System GPS available:" << m_source->sourceName();
        qDebug() << "NOTE: On Linux, GeoClue2 requires authorization agent or configuration.";
        qDebug() << "      If you see permission errors, use Simulated GPS mode instead.";
        qDebug() << "      See FINAL_GPS_SOLUTION.md for setup instructions.";
        
        // Configure position source
        m_source->setUpdateInterval(1000); // Update every second
        m_source->setPreferredPositioningMethods(
            QGeoPositionInfoSource::SatellitePositioningMethods |
            QGeoPositionInfoSource::NonSatellitePositioningMethods
        );
        
        // Connect signals
        connect(m_source, &QGeoPositionInfoSource::positionUpdated,
                this, &SystemGPS::onPositionUpdated);
        connect(m_source, 
                QOverload<QGeoPositionInfoSource::Error>::of(&QGeoPositionInfoSource::errorOccurred),
                this, &SystemGPS::onPositionError);
        
        // Timeout timer
        m_timeoutTimer->setInterval(30000); // 30 second timeout
        connect(m_timeoutTimer, &QTimer::timeout, this, &SystemGPS::onTimeout);
    } else {
        qWarning() << "System GPS not available on this device";
        emit gpsError("System GPS not available. Try simulated mode or SDR GPS.");
    }
}

void SystemGPS::start() {
    if (!m_source) {
        emit gpsError("No GPS source available");
        return;
    }
    
    qDebug() << "Starting system GPS...";
    m_source->startUpdates();
    m_timeoutTimer->start();
}

void SystemGPS::stop() {
    if (m_source) {
        qDebug() << "Stopping system GPS";
        m_source->stopUpdates();
        m_timeoutTimer->stop();
    }
}

void SystemGPS::onPositionUpdated(const QGeoPositionInfo& info) {
    m_timeoutTimer->start(); // Reset timeout
    
    if (!info.isValid()) {
        qWarning() << "Invalid position info received";
        return;
    }
    
    // Extract position data
    QGeoCoordinate coord = info.coordinate();
    
    if (coord.isValid()) {
        m_hasValidPosition = true;
        m_latitude = coord.latitude();
        m_longitude = coord.longitude();
        m_altitude = coord.altitude();
        
        // Horizontal accuracy (HDOP equivalent)
        if (info.hasAttribute(QGeoPositionInfo::HorizontalAccuracy)) {
            m_accuracy = info.attribute(QGeoPositionInfo::HorizontalAccuracy);
            emit accuracyUpdated(m_accuracy);
        }
        
        // Speed
        if (info.hasAttribute(QGeoPositionInfo::GroundSpeed)) {
            m_speed = info.attribute(QGeoPositionInfo::GroundSpeed);
            emit speedUpdated(m_speed);
        }
        
        // Direction/Heading
        if (info.hasAttribute(QGeoPositionInfo::Direction)) {
            m_heading = info.attribute(QGeoPositionInfo::Direction);
            emit headingUpdated(m_heading);
        }
        
        // Estimate satellite count from accuracy
        // Better accuracy = more satellites
        if (m_accuracy < 5) m_satelliteCount = 12;
        else if (m_accuracy < 10) m_satelliteCount = 10;
        else if (m_accuracy < 20) m_satelliteCount = 8;
        else if (m_accuracy < 50) m_satelliteCount = 6;
        else m_satelliteCount = 4;
        
        qDebug() << "GPS Update:" << m_latitude << "," << m_longitude 
                 << "Alt:" << m_altitude << "Acc:" << m_accuracy;
        
        emit positionUpdated(m_latitude, m_longitude, m_altitude);
    } else {
        qWarning() << "Received invalid coordinates";
    }
}

void SystemGPS::onPositionError(QGeoPositionInfoSource::Error error) {
    QString errorMsg;
    
    switch (error) {
        case QGeoPositionInfoSource::AccessError:
            errorMsg = "GPS Access Denied - Check permissions";
            break;
        case QGeoPositionInfoSource::ClosedError:
            errorMsg = "GPS Connection Closed";
            break;
        case QGeoPositionInfoSource::NoError:
            return; // No error
        default:
            errorMsg = "Unknown GPS Error";
            break;
    }
    
    qWarning() << "GPS Error:" << errorMsg;
    emit gpsError(errorMsg);
}

void SystemGPS::onTimeout() {
    qWarning() << "GPS timeout - no position update in 30 seconds";
    emit gpsError("GPS signal lost - waiting for position fix");
}

