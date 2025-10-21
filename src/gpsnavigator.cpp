#include "gpsnavigator.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>

GPSNavigator::GPSNavigator(QWidget* parent)
    : QWidget(parent),
      m_currentLat(34.0522),
      m_currentLon(-118.2437),
      m_currentSpeed(0.0),
      m_currentHeading(0.0),
      m_currentAltitude(100.0),
      m_satelliteCount(0),
      m_hdop(99.9),
      m_mode(STANDBY),
      m_destinationLat(0.0),
      m_destinationLon(0.0),
      m_currentWaypointIndex(0),
      m_gpsSource(SIMULATED),
      m_gpsLocked(false),
      m_scale(50.0),
      m_followMode(true),
      m_simSpeed(13.9),  // 50 km/h = 13.9 m/s
      m_simHeading(0.0)
{
    setMinimumSize(800, 600);
    setMouseTracking(true);
    
    m_centerLat = m_currentLat;
    m_centerLon = m_currentLon;
    
    // Initialize system GPS
    m_systemGPS = new SystemGPS(this);
    connect(m_systemGPS, &SystemGPS::positionUpdated, 
            this, [this](double lat, double lon, double alt) {
        if (m_gpsSource == SYSTEM_GPS) {
            m_currentLat = lat;
            m_currentLon = lon;
            m_currentAltitude = alt;
            m_currentSpeed = m_systemGPS->getSpeed();
            m_currentHeading = m_systemGPS->getHeading();
            m_hdop = m_systemGPS->getAccuracy() / 10.0; // Convert accuracy to HDOP estimate
            m_satelliteCount = m_systemGPS->getSatelliteCount();
            m_gpsLocked = true;
            
            if (m_followMode) {
                m_centerLat = lat;
                m_centerLon = lon;
            }
            
            emit locationUpdated(lat, lon);
            update();
        }
    });
    
    connect(m_systemGPS, &SystemGPS::gpsError,
            this, [this](const QString& msg) {
        qWarning() << "System GPS Error:" << msg;
        emit gpsSignalLost();
    });
    
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &GPSNavigator::updateGPS);
    m_updateTimer->start(1000); // Update every second
    
    m_simulationTimer = new QTimer(this);
    connect(m_simulationTimer, &QTimer::timeout, this, &GPSNavigator::simulateMovement);
    m_simulationTimer->start(1000); // Simulate every second
    
    m_lastGPSUpdate = QDateTime::currentDateTime();
}

void GPSNavigator::setCurrentLocation(double lat, double lon) {
    m_currentLat = lat;
    m_currentLon = lon;
    
    if (m_followMode) {
        m_centerLat = lat;
        m_centerLon = lon;
    }
    
    emit locationUpdated(lat, lon);
    update();
}

void GPSNavigator::addWaypoint(const Waypoint& waypoint) {
    m_waypoints.append(waypoint);
    update();
}

void GPSNavigator::setDestination(double lat, double lon, const QString& name) {
    m_destinationLat = lat;
    m_destinationLon = lon;
    m_destinationName = name;
    
    Waypoint dest(name, lat, lon, "Destination");
    dest.color = QColor(255, 82, 82);
    dest.isDestination = true;
    
    m_waypoints.clear();
    m_waypoints.append(dest);
    
    calculateRoute();
    update();
}

void GPSNavigator::startNavigation() {
    if (m_waypoints.isEmpty()) return;
    
    m_mode = NAVIGATING;
    m_currentWaypointIndex = 0;
    calculateRoute();
    update();
}

void GPSNavigator::stopNavigation() {
    m_mode = STANDBY;
    update();
}

void GPSNavigator::clearRoute() {
    m_waypoints.clear();
    m_route.clear();
    m_mode = STANDBY;
    update();
}

void GPSNavigator::updateGPS() {
    // Check GPS source and update accordingly
    switch (m_gpsSource) {
        case SYSTEM_GPS:
            // System GPS is handled by SystemGPS class via signals
            if (m_systemGPS->isAvailable() && m_systemGPS->hasValidPosition()) {
                m_gpsLocked = true;
            } else {
                m_gpsLocked = false;
                m_satelliteCount = 0;
                m_hdop = 99.9;
            }
            break;
            
        case SDR_RECEIVER:
            // In real implementation, read from SDR GPS receiver
            // Using software like gpsd with RTL-SDR
            m_satelliteCount = 6;
            m_hdop = 1.8;
            m_gpsLocked = true;
            break;
            
        case SIMULATED:
            // Simulated GPS is always available
            m_satelliteCount = 10;
            m_hdop = 0.9;
            m_gpsLocked = true;
            break;
    }
    
    m_lastGPSUpdate = QDateTime::currentDateTime();
    
    // Check if we've reached destination
    if (m_mode == NAVIGATING && !m_waypoints.isEmpty()) {
        double distToDest = calculateDistanceToDestination();
        if (distToDest < 20.0) { // Within 20 meters
            m_mode = ARRIVED;
            emit destinationReached();
        }
    }
    
    update();
}

void GPSNavigator::setGPSSource(GPSSource source) {
    m_gpsSource = source;
    
    // Start/stop system GPS based on source
    if (source == SYSTEM_GPS) {
        if (m_systemGPS->isAvailable()) {
            m_systemGPS->start();
            qDebug() << "System GPS started";
        } else {
            qWarning() << "System GPS not available";
            emit gpsSignalLost();
        }
    } else {
        m_systemGPS->stop();
    }
}

void GPSNavigator::simulateMovement() {
    if (m_gpsSource != SIMULATED) return;
    if (m_mode != NAVIGATING) return;
    if (m_waypoints.isEmpty()) return;
    
    // Simulate driving towards destination
    double bearing = calculateBearingToDestination();
    m_currentHeading = bearing;
    
    // Move towards destination
    double distance = m_simSpeed; // Move simSpeed meters per second
    double dLat = (distance / GeoUtils::EARTH_RADIUS) * std::cos(GeoUtils::toRadians(bearing));
    double dLon = (distance / GeoUtils::EARTH_RADIUS) * std::sin(GeoUtils::toRadians(bearing));
    
    m_currentLat += GeoUtils::toDegrees(dLat);
    m_currentLon += GeoUtils::toDegrees(dLon);
    m_currentSpeed = m_simSpeed;
    
    if (m_followMode) {
        m_centerLat = m_currentLat;
        m_centerLon = m_currentLon;
    }
    
    emit locationUpdated(m_currentLat, m_currentLon);
}

void GPSNavigator::calculateRoute() {
    m_route.clear();
    
    if (m_waypoints.isEmpty()) return;
    
    // Simple straight-line route for now
    // In production, would use real routing API (OpenStreetMap, Google, etc.)
    RouteSegment segment;
    segment.start = latLonToScreen(m_currentLat, m_currentLon);
    segment.end = latLonToScreen(m_destinationLat, m_destinationLon);
    segment.distance = calculateDistanceToDestination();
    segment.bearing = calculateBearingToDestination();
    segment.instruction = QString("Proceed to %1").arg(m_destinationName);
    
    m_route.append(segment);
}

double GPSNavigator::calculateDistanceToDestination() const {
    if (m_waypoints.isEmpty()) return 0.0;
    return GeoUtils::calculateDistance(m_currentLat, m_currentLon,
                                      m_destinationLat, m_destinationLon);
}

double GPSNavigator::calculateBearingToDestination() const {
    if (m_waypoints.isEmpty()) return 0.0;
    return GeoUtils::calculateBearing(m_currentLat, m_currentLon,
                                     m_destinationLat, m_destinationLon);
}

double GPSNavigator::calculateETA() const {
    if (m_currentSpeed < 0.1) return 0.0;
    double distance = calculateDistanceToDestination();
    return distance / m_currentSpeed; // seconds
}

void GPSNavigator::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Palantir-style background
    painter.fillRect(rect(), QColor(10, 13, 17));
    
    drawMap(painter);
    drawRoute(painter);
    drawWaypoints(painter);
    drawCurrentLocation(painter);
    drawNavigationInfo(painter);
    drawGPSStatus(painter);
    drawSpeedometer(painter);
    drawCompass(painter);
}

QPointF GPSNavigator::latLonToScreen(double lat, double lon) const {
    double x = (lon - m_centerLon) * GeoUtils::EARTH_RADIUS * 
               std::cos(GeoUtils::toRadians(m_centerLat)) / m_scale;
    double y = -(lat - m_centerLat) * GeoUtils::EARTH_RADIUS / m_scale;
    
    return QPointF(width() / 2 + x, height() / 2 + y);
}

void GPSNavigator::drawMap(QPainter& painter) {
    // Tactical grid
    painter.setPen(QPen(QColor(42, 63, 95, 60), 1));
    int gridSpacing = 50;
    for (int x = 0; x < width(); x += gridSpacing) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += gridSpacing) {
        painter.drawLine(0, y, width(), y);
    }
    
    // Draw roads/grid (simplified)
    painter.setPen(QPen(QColor(80, 90, 100), 2));
    int roadSpacing = 200;
    for (int x = 0; x < width(); x += roadSpacing) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += roadSpacing) {
        painter.drawLine(0, y, width(), y);
    }
}

void GPSNavigator::drawCurrentLocation(QPainter& painter) {
    QPointF pos = latLonToScreen(m_currentLat, m_currentLon);
    
    // Draw vehicle icon (arrow pointing in heading direction)
    painter.save();
    painter.translate(pos);
    painter.rotate(m_currentHeading);
    
    // Vehicle shape
    QPolygonF vehicle;
    vehicle << QPointF(0, -15) << QPointF(-10, 10) << QPointF(0, 5) << QPointF(10, 10);
    
    painter.setBrush(QColor(0, 255, 128));
    painter.setPen(QPen(QColor(255, 255, 255), 2));
    painter.drawPolygon(vehicle);
    
    painter.restore();
    
    // Location accuracy circle
    painter.setPen(QPen(QColor(0, 255, 128, 80), 2));
    painter.setBrush(Qt::NoBrush);
    double accuracyRadius = 10 * m_hdop; // Visual representation
    painter.drawEllipse(pos, accuracyRadius, accuracyRadius);
}

void GPSNavigator::drawWaypoints(QPainter& painter) {
    for (const Waypoint& wp : m_waypoints) {
        QPointF pos = latLonToScreen(wp.latitude, wp.longitude);
        
        // Waypoint marker
        if (wp.isDestination) {
            // Destination flag
            painter.setBrush(QColor(255, 82, 82));
            painter.setPen(QPen(Qt::white, 2));
            
            QPolygonF flag;
            flag << QPointF(pos.x(), pos.y() - 20) << QPointF(pos.x() + 15, pos.y() - 15)
                 << QPointF(pos.x(), pos.y() - 10);
            painter.drawPolygon(flag);
            painter.drawLine(pos.x(), pos.y(), pos.x(), pos.y() - 20);
        } else {
            // Regular waypoint
            painter.setBrush(wp.color);
            painter.setPen(QPen(Qt::white, 2));
            painter.drawEllipse(pos, 8, 8);
        }
        
        // Label
        painter.setFont(QFont("Consolas", 9, QFont::Bold));
        painter.setPen(wp.color);
        painter.drawText(pos.x() + 15, pos.y() - 5, wp.name);
    }
}

void GPSNavigator::drawRoute(QPainter& painter) {
    if (m_route.isEmpty() || m_mode == STANDBY) return;
    
    // Draw route line
    painter.setPen(QPen(QColor(0, 168, 255), 4));
    
    QPointF start = latLonToScreen(m_currentLat, m_currentLon);
    QPointF end = latLonToScreen(m_destinationLat, m_destinationLon);
    
    painter.drawLine(start, end);
    
    // Draw direction arrows along route
    int numArrows = 5;
    for (int i = 1; i < numArrows; ++i) {
        double t = i / (double)numArrows;
        QPointF arrowPos = start + t * (end - start);
        
        double bearing = calculateBearingToDestination();
        painter.save();
        painter.translate(arrowPos);
        painter.rotate(bearing);
        
        QPolygonF arrow;
        arrow << QPointF(0, -6) << QPointF(-4, 2) << QPointF(4, 2);
        painter.setBrush(QColor(0, 168, 255));
        painter.drawPolygon(arrow);
        painter.restore();
    }
}

void GPSNavigator::drawNavigationInfo(QPainter& painter) {
    int panelWidth = 400;
    int panelHeight = 200;
    int x = 10;
    int y = 10;
    
    // Info panel
    painter.setBrush(QColor(10, 13, 17, 220));
    painter.setPen(QPen(QColor(0, 168, 255), 2));
    painter.drawRect(x, y, panelWidth, panelHeight);
    
    // Title bar
    painter.fillRect(x + 1, y + 1, panelWidth - 2, 26, QColor(42, 63, 95, 180));
    
    painter.setFont(QFont("Consolas", 12, QFont::Bold));
    painter.setPen(QColor(0, 168, 255));
    painter.drawText(x + 10, y + 19, "⬢ NAVIGATION SYSTEM");
    
    painter.setFont(QFont("Consolas", 10));
    painter.setPen(QColor(220, 225, 230));
    
    int yPos = y + 45;
    int lineHeight = 20;
    
    // Current location
    painter.drawText(x + 10, yPos, 
                     QString("POSITION: [%1°, %2°]")
                     .arg(m_currentLat, 0, 'f', 6).arg(m_currentLon, 0, 'f', 6));
    yPos += lineHeight;
    
    // Speed
    painter.setPen(m_currentSpeed > 33.3 ? QColor(255, 170, 0) : QColor(220, 225, 230));
    painter.drawText(x + 10, yPos,
                     QString("SPEED: %1 km/h (%2 mph)")
                     .arg(m_currentSpeed * 3.6, 0, 'f', 1)
                     .arg(m_currentSpeed * 2.237, 0, 'f', 1));
    yPos += lineHeight;
    
    // Heading
    painter.setPen(QColor(220, 225, 230));
    QString headingDir;
    if (m_currentHeading >= 337.5 || m_currentHeading < 22.5) headingDir = "N";
    else if (m_currentHeading < 67.5) headingDir = "NE";
    else if (m_currentHeading < 112.5) headingDir = "E";
    else if (m_currentHeading < 157.5) headingDir = "SE";
    else if (m_currentHeading < 202.5) headingDir = "S";
    else if (m_currentHeading < 247.5) headingDir = "SW";
    else if (m_currentHeading < 292.5) headingDir = "W";
    else headingDir = "NW";
    
    painter.drawText(x + 10, yPos,
                     QString("HEADING: %1° [%2]")
                     .arg((int)m_currentHeading).arg(headingDir));
    yPos += lineHeight;
    
    // Navigation status
    if (m_mode == NAVIGATING && !m_waypoints.isEmpty()) {
        double distance = calculateDistanceToDestination();
        double bearing = calculateBearingToDestination();
        double eta = calculateETA();
        
        painter.setPen(QColor(0, 168, 255));
        painter.drawText(x + 10, yPos,
                         QString("DESTINATION: %1").arg(m_destinationName));
        yPos += lineHeight;
        
        painter.setPen(QColor(220, 225, 230));
        painter.drawText(x + 10, yPos,
                         QString("DISTANCE: %1 km").arg(distance / 1000.0, 0, 'f', 2));
        yPos += lineHeight;
        
        painter.drawText(x + 10, yPos,
                         QString("BEARING: %1°").arg((int)bearing));
        yPos += lineHeight;
        
        if (eta > 0) {
            int etaMin = eta / 60;
            painter.setPen(etaMin < 5 ? QColor(255, 82, 82) : QColor(0, 255, 128));
            painter.drawText(x + 10, yPos,
                             QString("ETA: %1 min").arg(etaMin));
        }
    } else if (m_mode == ARRIVED) {
        painter.setFont(QFont("Consolas", 11, QFont::Bold));
        painter.setPen(QColor(0, 255, 128));
        painter.drawText(x + 10, yPos, "◉ DESTINATION REACHED");
    } else {
        painter.setPen(QColor(150, 160, 170));
        painter.drawText(x + 10, yPos, "○ STANDBY MODE");
    }
}

void GPSNavigator::drawGPSStatus(QPainter& painter) {
    int panelWidth = 250;
    int panelHeight = 120;
    int x = width() - panelWidth - 10;
    int y = 10;
    
    painter.setBrush(QColor(10, 13, 17, 220));
    painter.setPen(QPen(m_gpsLocked ? QColor(0, 255, 128) : QColor(255, 82, 82), 2));
    painter.drawRect(x, y, panelWidth, panelHeight);
    
    // Title
    painter.fillRect(x + 1, y + 1, panelWidth - 2, 24, 
                     m_gpsLocked ? QColor(0, 80, 40, 180) : QColor(80, 20, 20, 180));
    
    painter.setFont(QFont("Consolas", 10, QFont::Bold));
    painter.setPen(m_gpsLocked ? QColor(0, 255, 128) : QColor(255, 82, 82));
    
    QString statusIcon = m_gpsLocked ? "◉" : "○";
    painter.drawText(x + 10, y + 17, QString("%1 GPS STATUS").arg(statusIcon));
    
    painter.setFont(QFont("Consolas", 9));
    painter.setPen(QColor(220, 225, 230));
    
    int yPos = y + 40;
    
    // GPS Source
    QString sourceName;
    switch (m_gpsSource) {
        case SYSTEM_GPS: sourceName = "SYSTEM GPS"; break;
        case SDR_RECEIVER: sourceName = "SDR RECEIVER"; break;
        case SIMULATED: sourceName = "SIMULATED"; break;
    }
    painter.drawText(x + 10, yPos, QString("SOURCE: %1").arg(sourceName));
    yPos += 18;
    
    // Satellites
    painter.setPen(m_satelliteCount >= 4 ? QColor(0, 255, 128) : QColor(255, 170, 0));
    painter.drawText(x + 10, yPos,
                     QString("SATELLITES: %1").arg(m_satelliteCount));
    yPos += 18;
    
    // HDOP (accuracy)
    painter.setPen(m_hdop < 2.0 ? QColor(0, 255, 128) : QColor(255, 170, 0));
    painter.drawText(x + 10, yPos,
                     QString("HDOP: %1").arg(m_hdop, 0, 'f', 1));
    yPos += 18;
    
    // Altitude
    painter.setPen(QColor(220, 225, 230));
    painter.drawText(x + 10, yPos,
                     QString("ALT: %1 m").arg((int)m_currentAltitude));
}

void GPSNavigator::drawSpeedometer(QPainter& painter) {
    int cx = width() - 110;
    int cy = height() - 110;
    int radius = 90;
    
    // Speedometer circle
    painter.setBrush(QColor(10, 13, 17, 220));
    painter.setPen(QPen(QColor(0, 168, 255), 3));
    painter.drawEllipse(QPointF(cx, cy), radius, radius);
    
    // Speed markings
    painter.setPen(QPen(QColor(150, 160, 170), 1));
    for (int speed = 0; speed <= 120; speed += 20) {
        double angle = 225 - (speed / 120.0) * 270; // 225° to -45°
        double rad = qDegreesToRadians(angle);
        
        int x1 = cx + (radius - 15) * std::cos(rad);
        int y1 = cy - (radius - 15) * std::sin(rad);
        int x2 = cx + (radius - 5) * std::cos(rad);
        int y2 = cy - (radius - 5) * std::sin(rad);
        
        painter.drawLine(x1, y1, x2, y2);
    }
    
    // Speed needle
    double speedKmh = m_currentSpeed * 3.6;
    if (speedKmh > 120) speedKmh = 120;
    
    double needleAngle = 225 - (speedKmh / 120.0) * 270;
    double needleRad = qDegreesToRadians(needleAngle);
    
    painter.setPen(QPen(QColor(255, 82, 82), 3));
    painter.drawLine(cx, cy,
                     cx + (radius - 20) * std::cos(needleRad),
                     cy - (radius - 20) * std::sin(needleRad));
    
    // Center dot
    painter.setBrush(QColor(255, 82, 82));
    painter.drawEllipse(QPointF(cx, cy), 5, 5);
    
    // Speed text
    painter.setFont(QFont("Consolas", 16, QFont::Bold));
    painter.setPen(QColor(0, 168, 255));
    painter.drawText(cx - 30, cy + 35, QString("%1").arg((int)speedKmh));
    
    painter.setFont(QFont("Consolas", 8));
    painter.drawText(cx - 15, cy + 48, "km/h");
}

void GPSNavigator::drawCompass(QPainter& painter) {
    int cx = 100;
    int cy = height() - 110;
    int radius = 80;
    
    // Compass circle
    painter.setBrush(QColor(10, 13, 17, 220));
    painter.setPen(QPen(QColor(0, 168, 255), 3));
    painter.drawEllipse(QPointF(cx, cy), radius, radius);
    
    // Rotate for current heading
    painter.save();
    painter.translate(cx, cy);
    painter.rotate(-m_currentHeading);
    
    // North arrow
    QPolygonF northArrow;
    northArrow << QPointF(0, -radius + 15) << QPointF(-8, -radius + 35) 
               << QPointF(0, -radius + 30) << QPointF(8, -radius + 35);
    painter.setBrush(QColor(255, 82, 82));
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(northArrow);
    
    // South arrow
    QPolygonF southArrow;
    southArrow << QPointF(0, radius - 15) << QPointF(-8, radius - 35) 
               << QPointF(0, radius - 30) << QPointF(8, radius - 35);
    painter.setBrush(QColor(220, 225, 230));
    painter.drawPolygon(southArrow);
    
    painter.restore();
    
    // Heading text
    painter.setFont(QFont("Consolas", 20, QFont::Bold));
    painter.setPen(QColor(0, 168, 255));
    painter.drawText(cx - 25, cy + 8, QString("%1°").arg((int)m_currentHeading));
}

void GPSNavigator::mousePressEvent(QMouseEvent* event) {
    // Toggle follow mode on double-click
    if (event->button() == Qt::LeftButton) {
        m_followMode = !m_followMode;
        update();
    }
}

void GPSNavigator::wheelEvent(QWheelEvent* event) {
    // Zoom
    double factor = event->angleDelta().y() > 0 ? 0.8 : 1.25;
    m_scale *= factor;
    
    if (m_scale < 5.0) m_scale = 5.0;
    if (m_scale > 500.0) m_scale = 500.0;
    
    update();
}

void GPSNavigator::mouseMoveEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    // Could implement panning here
}

