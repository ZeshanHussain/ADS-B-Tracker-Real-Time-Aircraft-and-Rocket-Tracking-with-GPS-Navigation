#include "mapwidget.h"
#include "geoutils.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>

MapWidget::MapWidget(QWidget* parent)
    : QWidget(parent),
      m_userLat(0.0),
      m_userLon(0.0),
      m_scale(100.0),  // Start at 100 meters per pixel
      m_centerLat(0.0),
      m_centerLon(0.0),
      m_proximityAlertEnabled(true),
      m_proximityDistance(10000.0), // 10 km default
      m_radarSweepEnabled(true),
      m_sweepAngle(0.0),
      m_sweepRPM(6) // 6 RPM = 1 revolution per 10 seconds
{
    setMinimumSize(800, 600);
    setMouseTracking(true);
    
    // Setup radar sweep animation
    m_sweepTimer = new QTimer(this);
    connect(m_sweepTimer, &QTimer::timeout, this, &MapWidget::updateRadarSweep);
    m_sweepTimer->start(50); // Update every 50ms for smooth animation
}

void MapWidget::setUserLocation(double lat, double lon) {
    m_userLat = lat;
    m_userLon = lon;
    m_centerLat = lat;
    m_centerLon = lon;
    update();
}

void MapWidget::setAircraft(const QMap<QString, Aircraft>& aircraft) {
    m_aircraft = aircraft;
    checkProximityAlerts();
    update();
}

void MapWidget::setRockets(const QVector<Rocket>& rockets) {
    m_rockets = rockets;
    update();
}

void MapWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Palantir-style deep black background
    painter.fillRect(rect(), QColor(10, 13, 17));
    
    // Draw tactical grid
    painter.setPen(QPen(QColor(42, 63, 95, 60), 1)); // Subtle blue grid
    int gridSpacing = 50;
    for (int x = 0; x < width(); x += gridSpacing) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += gridSpacing) {
        painter.drawLine(0, y, width(), y);
    }
    
    // Draw thicker lines every 200px for emphasis
    painter.setPen(QPen(QColor(42, 63, 95, 120), 1));
    for (int x = 0; x < width(); x += 200) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += 200) {
        painter.drawLine(0, y, width(), y);
    }
    
    // Draw tactical range circles
    QPointF center = latLonToScreen(m_centerLat, m_centerLon);
    QVector<double> ranges = {5000, 10000, 25000, 50000}; // meters
    QVector<QColor> rangeColors = {
        QColor(255, 82, 82, 80),   // Red - critical range
        QColor(255, 170, 0, 70),    // Orange - warning range
        QColor(0, 168, 255, 60),    // Cyan - monitoring range
        QColor(0, 168, 255, 40)     // Cyan - extended range
    };
    
    for (int i = 0; i < ranges.size(); ++i) {
        double radius = ranges[i] / m_scale;
        painter.setPen(QPen(rangeColors[i], 2));
        painter.drawEllipse(center, radius, radius);
    }
    
    drawRockets(painter);
    drawAircraft(painter);
    drawRadarSweep(painter); // Draw sweep over aircraft
    drawUser(painter);
    drawCompass(painter);
    drawScale(painter);
    drawInfo(painter);
}

QPointF MapWidget::latLonToScreen(double lat, double lon) const {
    // Simple equirectangular projection
    // More accurate for small areas
    double x = (lon - m_centerLon) * GeoUtils::EARTH_RADIUS * 
               std::cos(GeoUtils::toRadians(m_centerLat)) / m_scale;
    double y = -(lat - m_centerLat) * GeoUtils::EARTH_RADIUS / m_scale;
    
    return QPointF(width() / 2 + x, height() / 2 + y);
}

void MapWidget::drawUser(QPainter& painter) {
    QPointF pos = latLonToScreen(m_userLat, m_userLon);
    
    // Tactical observation point indicator
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(0, 255, 128), 2));
    
    // Outer ring
    painter.drawEllipse(pos, 12, 12);
    
    // Inner core
    painter.setBrush(QColor(0, 255, 128));
    painter.drawEllipse(pos, 4, 4);
    
    // Tactical crosshair
    painter.setPen(QPen(QColor(0, 255, 128), 2));
    painter.drawLine(pos.x() - 16, pos.y(), pos.x() - 6, pos.y());
    painter.drawLine(pos.x() + 6, pos.y(), pos.x() + 16, pos.y());
    painter.drawLine(pos.x(), pos.y() - 16, pos.x(), pos.y() - 6);
    painter.drawLine(pos.x(), pos.y() + 6, pos.x(), pos.y() + 16);
    
    // Label
    painter.setFont(QFont("Consolas", 9, QFont::Bold));
    painter.setPen(QColor(0, 255, 128));
    painter.drawText(pos.x() + 18, pos.y() - 6, "◉ OBSERVATION POINT");
}

void MapWidget::drawAircraft(QPainter& painter) {
    for (const Aircraft& aircraft : m_aircraft) {
        QPointF pos = latLonToScreen(aircraft.getLatitude(), aircraft.getLongitude());
        
        // Calculate distance from user
        double distance = aircraft.distanceTo(m_userLat, m_userLon);
        
        // Color based on distance
        QColor color;
        if (distance < 5000) {
            color = QColor(255, 50, 50); // Red - very close
        } else if (distance < 15000) {
            color = QColor(255, 150, 0); // Orange - close
        } else {
            color = QColor(0, 200, 255); // Cyan - far
        }
        
        // Highlight if selected
        if (aircraft.getICAO() == m_selectedIcao) {
            painter.setPen(QPen(Qt::yellow, 3));
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(pos, 20, 20);
        }
        
        // Draw aircraft icon (triangle pointing in heading direction)
        painter.save();
        painter.translate(pos);
        painter.rotate(aircraft.getHeading());
        
        QPolygonF plane;
        plane << QPointF(0, -8) << QPointF(-6, 6) << QPointF(0, 3) << QPointF(6, 6);
        
        painter.setBrush(color);
        painter.setPen(QPen(Qt::white, 1));
        painter.drawPolygon(plane);
        painter.restore();
        
        // Draw callsign and tactical info
        painter.setPen(color);
        painter.setFont(QFont("Consolas", 9, QFont::Bold));
        QString info = aircraft.getCallsign();
        if (info.isEmpty()) {
            info = aircraft.getICAO();
        }
        painter.drawText(pos.x() + 10, pos.y() - 10, info);
        
        // Draw detailed tactical data
        painter.setFont(QFont("Consolas", 7));
        painter.setPen(QColor(180, 190, 200));
        painter.drawText(pos.x() + 10, pos.y() + 3, 
                        QString("RNG:%1km").arg(distance / 1000.0, 0, 'f', 1));
        painter.drawText(pos.x() + 10, pos.y() + 13,
                        QString("ALT:%1m").arg((int)aircraft.getAltitude()));
    }
}

void MapWidget::drawRockets(QPainter& painter) {
    for (const Rocket& rocket : m_rockets) {
        QPointF pos = latLonToScreen(rocket.getLatitude(), rocket.getLongitude());
        
        // Calculate distance from user
        double distance = rocket.distanceTo(m_userLat, m_userLon);
        
        // Draw rocket icon
        painter.save();
        painter.translate(pos);
        
        // Rocket shape
        QPolygonF rocketShape;
        rocketShape << QPointF(0, -15) << QPointF(-5, -5) << QPointF(-5, 10) 
                    << QPointF(5, 10) << QPointF(5, -5);
        
        // Color based on countdown
        QColor rocketColor;
        qint64 timeToLaunch = rocket.getSecondsUntilLaunch();
        if (timeToLaunch < 0) {
            rocketColor = QColor(150, 150, 150); // Gray - launched
        } else if (timeToLaunch < 3600) {
            rocketColor = QColor(255, 0, 0); // Red - launching soon!
        } else if (timeToLaunch < 86400) {
            rocketColor = QColor(255, 150, 0); // Orange - today
        } else {
            rocketColor = QColor(0, 255, 100); // Green - upcoming
        }
        
        painter.setBrush(rocketColor);
        painter.setPen(QPen(Qt::white, 2));
        painter.drawPolygon(rocketShape);
        
        // Draw flames if launching soon
        if (timeToLaunch > 0 && timeToLaunch < 3600) {
            painter.setBrush(QColor(255, 150, 0, 180));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(0, 12), 4, 6);
        }
        
        painter.restore();
        
        // Draw tactical launch info
        painter.setPen(rocketColor);
        painter.setFont(QFont("Consolas", 8, QFont::Bold));
        QString launchName = rocket.getName();
        if (launchName.length() > 25) launchName = launchName.left(22) + "...";
        painter.drawText(pos.x() + 15, pos.y() - 10, launchName);
        
        painter.setFont(QFont("Consolas", 7));
        painter.setPen(QColor(180, 190, 200));
        painter.drawText(pos.x() + 15, pos.y() + 3, rocket.getCountdownString());
        painter.drawText(pos.x() + 15, pos.y() + 13, 
                        QString("RNG:%1km").arg(distance / 1000.0, 0, 'f', 0));
    }
}

void MapWidget::drawCompass(QPainter& painter) {
    int cx = width() - 70;
    int cy = 70;
    int radius = 45;
    
    // Outer ring
    painter.setPen(QPen(QColor(0, 168, 255), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPointF(cx, cy), radius, radius);
    
    // Inner ring
    painter.setPen(QPen(QColor(0, 168, 255, 100), 1));
    painter.drawEllipse(QPointF(cx, cy), radius - 5, radius - 5);
    
    // Cardinal direction lines
    painter.setPen(QPen(QColor(0, 168, 255), 1));
    for (int angle = 0; angle < 360; angle += 45) {
        double rad = qDegreesToRadians((double)angle);
        int x1 = cx + (radius - 10) * std::sin(rad);
        int y1 = cy - (radius - 10) * std::cos(rad);
        int x2 = cx + radius * std::sin(rad);
        int y2 = cy - radius * std::cos(rad);
        painter.drawLine(x1, y1, x2, y2);
    }
    
    // North indicator
    QPolygonF arrow;
    arrow << QPointF(cx, cy - radius + 8) << QPointF(cx - 5, cy - radius + 20) 
          << QPointF(cx + 5, cy - radius + 20);
    painter.setBrush(QColor(255, 82, 82));
    painter.setPen(QPen(QColor(255, 82, 82), 2));
    painter.drawPolygon(arrow);
    
    // N, S, E, W labels - tactical style
    painter.setFont(QFont("Consolas", 10, QFont::Bold));
    painter.setPen(QColor(0, 168, 255));
    painter.drawText(cx - 5, cy - radius - 8, "N");
    painter.drawText(cx - 5, cy + radius + 18, "S");
    painter.drawText(cx + radius + 12, cy + 5, "E");
    painter.drawText(cx - radius - 18, cy + 5, "W");
}

void MapWidget::drawScale(QPainter& painter) {
    int x = 20;
    int y = height() - 40;
    
    // Scale bar
    double scaleLength = 10000.0; // 10 km
    if (m_scale > 200) scaleLength = 50000.0; // 50 km
    else if (m_scale < 50) scaleLength = 5000.0; // 5 km
    
    int barWidth = scaleLength / m_scale;
    
    painter.setPen(QPen(Qt::white, 2));
    painter.drawLine(x, y, x + barWidth, y);
    painter.drawLine(x, y - 5, x, y + 5);
    painter.drawLine(x + barWidth, y - 5, x + barWidth, y + 5);
    
    painter.setFont(QFont("Arial", 10));
    painter.drawText(x + barWidth + 10, y + 5, 
                     QString("%1 km").arg(scaleLength / 1000.0, 0, 'f', 0));
}

void MapWidget::drawInfo(QPainter& painter) {
    // Tactical info panel
    int panelWidth = 360;
    int panelHeight = 170;
    int x = 10;
    int y = 10;
    
    // Semi-transparent dark panel with tactical border
    painter.setBrush(QColor(10, 13, 17, 220));
    painter.setPen(QPen(QColor(0, 168, 255), 2));
    painter.drawRect(x, y, panelWidth, panelHeight);
    
    // Title bar
    painter.fillRect(x + 1, y + 1, panelWidth - 2, 24, QColor(42, 63, 95, 180));
    
    painter.setFont(QFont("Consolas", 11, QFont::Bold));
    painter.setPen(QColor(0, 168, 255));
    painter.drawText(x + 10, y + 18, "⬢ TACTICAL SITUATION DISPLAY");
    
    painter.setFont(QFont("Consolas", 9));
    painter.setPen(QColor(220, 225, 230));
    
    // Observation point
    painter.drawText(x + 10, y + 42, 
                     QString("OBSERVATION: [%1°, %2°]")
                     .arg(m_userLat, 0, 'f', 4).arg(m_userLon, 0, 'f', 4));
    
    // Aircraft count with status indicator
    QString aircraftStatus;
    if (m_aircraft.size() == 0) aircraftStatus = "○ NO CONTACTS";
    else if (m_aircraft.size() < 5) aircraftStatus = QString("◎ %1 CONTACTS").arg(m_aircraft.size());
    else aircraftStatus = QString("◉ %1 CONTACTS").arg(m_aircraft.size());
    
    painter.setPen(m_aircraft.size() > 10 ? QColor(255, 82, 82) : QColor(0, 255, 128));
    painter.drawText(x + 10, y + 64, QString("AERIAL TARGETS: %1").arg(aircraftStatus));
    
    // Rockets count
    int upcomingRockets = 0;
    int imminentLaunches = 0;
    for (const Rocket& r : m_rockets) {
        if (r.isUpcoming()) {
            upcomingRockets++;
            if (r.getSecondsUntilLaunch() < 3600) imminentLaunches++;
        }
    }
    
    painter.setPen(imminentLaunches > 0 ? QColor(255, 82, 82) : QColor(0, 168, 255));
    QString launchStatus = imminentLaunches > 0 ? 
        QString("◉ %1 IMMINENT").arg(imminentLaunches) : 
        QString("○ %1 SCHEDULED").arg(upcomingRockets);
    painter.drawText(x + 10, y + 86, QString("LAUNCH OPS: %1").arg(launchStatus));
    
    // Detection range
    painter.setPen(QColor(220, 225, 230));
    painter.drawText(x + 10, y + 108,
                     QString("DETECTION RANGE: %1 km")
                     .arg(m_proximityDistance / 1000.0, 0, 'f', 1));
    
    // Scale info
    painter.drawText(x + 10, y + 128,
                     QString("MAP SCALE: 1px = %1m")
                     .arg(m_scale, 0, 'f', 0));
    
    // Instructions
    painter.setFont(QFont("Consolas", 8));
    painter.setPen(QColor(150, 160, 170));
    painter.drawText(x + 10, y + 148, "◆ SCROLL: Adjust Zoom");
    painter.drawText(x + 10, y + 163, "◆ CLICK: Select Target");
}

void MapWidget::checkProximityAlerts() {
    if (!m_proximityAlertEnabled) return;
    
    for (const Aircraft& aircraft : m_aircraft) {
        double distance = aircraft.distanceTo(m_userLat, m_userLon);
        
        if (distance < m_proximityDistance) {
            QString icao = aircraft.getICAO();
            if (!m_alertedAircraft.contains(icao)) {
                m_alertedAircraft.insert(icao);
                
                QString callsign = aircraft.getCallsign();
                if (callsign.isEmpty()) callsign = icao;
                
                double bearing = GeoUtils::calculateBearing(m_userLat, m_userLon,
                                                           aircraft.getLatitude(),
                                                           aircraft.getLongitude());
                
                QString message = QString("✈ Aircraft Nearby!\n%1\nDistance: %2 km\nBearing: %3°\nAltitude: %4 m")
                                 .arg(callsign)
                                 .arg(distance / 1000.0, 0, 'f', 2)
                                 .arg(bearing, 0, 'f', 0)
                                 .arg(aircraft.getAltitude(), 0, 'f', 0);
                
                emit proximityAlert(message);
            }
        } else {
            m_alertedAircraft.remove(aircraft.getICAO());
        }
    }
    
    // Check rocket launch proximity
    for (const Rocket& rocket : m_rockets) {
        if (!rocket.isUpcoming()) continue;
        
        double distance = rocket.distanceTo(m_userLat, m_userLon);
        qint64 timeToLaunch = rocket.getSecondsUntilLaunch();
        
        // Alert if launch is within 1 hour and within 200 km
        if (timeToLaunch > 0 && timeToLaunch < 3600 && distance < 200000) {
            QString id = "ROCKET_" + rocket.getId();
            if (!m_alertedAircraft.contains(id)) {
                m_alertedAircraft.insert(id);
                
                QString message = QString("🚀 ROCKET LAUNCH IMMINENT!\n%1\n%2\nDistance: %3 km")
                                 .arg(rocket.getName())
                                 .arg(rocket.getCountdownString())
                                 .arg(distance / 1000.0, 0, 'f', 0);
                
                emit proximityAlert(message);
            }
        }
    }
}

void MapWidget::mousePressEvent(QMouseEvent* event) {
    QPointF clickPos = event->pos();
    
    // Check if clicked on aircraft
    m_selectedIcao.clear();
    double minDist = 20.0; // pixels
    
    for (const Aircraft& aircraft : m_aircraft) {
        QPointF pos = latLonToScreen(aircraft.getLatitude(), aircraft.getLongitude());
        double dist = std::sqrt(std::pow(pos.x() - clickPos.x(), 2) + 
                               std::pow(pos.y() - clickPos.y(), 2));
        
        if (dist < minDist) {
            m_selectedIcao = aircraft.getICAO();
            minDist = dist;
        }
    }
    
    update();
}

void MapWidget::wheelEvent(QWheelEvent* event) {
    // Zoom in/out
    double factor = event->angleDelta().y() > 0 ? 0.8 : 1.25;
    m_scale *= factor;
    
    // Clamp scale
    if (m_scale < 10.0) m_scale = 10.0;
    if (m_scale > 1000.0) m_scale = 1000.0;
    
    update();
}

void MapWidget::mouseMoveEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    // Could implement panning here
}

void MapWidget::setRadarSweepEnabled(bool enabled) {
    m_radarSweepEnabled = enabled;
    if (enabled) {
        m_sweepTimer->start(50);
    } else {
        m_sweepTimer->stop();
    }
    update();
}

void MapWidget::updateRadarSweep() {
    if (!m_radarSweepEnabled) return;
    
    // Calculate angle increment based on RPM
    // RPM = revolutions per minute
    // Degrees per second = (RPM * 360) / 60
    // Degrees per 50ms = (RPM * 360) / 60 / (1000/50)
    double degreesPerUpdate = (m_sweepRPM * 360.0) / 60.0 / 20.0;
    
    m_sweepAngle += degreesPerUpdate;
    if (m_sweepAngle >= 360.0) {
        m_sweepAngle -= 360.0;
    }
    
    update(); // Trigger repaint
}

void MapWidget::drawRadarSweep(QPainter& painter) {
    if (!m_radarSweepEnabled) return;
    
    QPointF center = latLonToScreen(m_centerLat, m_centerLon);
    
    // Calculate maximum radius (to edge of screen)
    double maxRadius = std::sqrt(std::pow(width() / 2.0, 2) + std::pow(height() / 2.0, 2));
    
    // Draw the sweep line with gradient for fade effect
    painter.save();
    painter.translate(center);
    painter.rotate(m_sweepAngle);
    
    // Main bright sweep line
    QPen sweepPen(QColor(0, 255, 128, 220), 3);
    painter.setPen(sweepPen);
    painter.drawLine(0, 0, 0, -maxRadius);
    
    // Thicker glow effect behind the sweep
    QPen glowPen(QColor(0, 255, 128, 80), 6);
    painter.setPen(glowPen);
    painter.drawLine(0, 0, 0, -maxRadius);
    
    // Draw fading trail (classic radar look)
    int trailSegments = 20;
    for (int i = 1; i <= trailSegments; ++i) {
        double angle = -i * 3.0; // 3 degrees per segment
        int alpha = 180 * (trailSegments - i) / trailSegments; // Fade out
        
        painter.save();
        painter.rotate(angle);
        
        // Create gradient for trail
        QLinearGradient gradient(0, 0, 0, -maxRadius);
        gradient.setColorAt(0.0, QColor(0, 255, 128, alpha));
        gradient.setColorAt(0.3, QColor(0, 255, 128, alpha / 2));
        gradient.setColorAt(1.0, QColor(0, 255, 128, 0));
        
        QPen trailPen;
        trailPen.setBrush(gradient);
        trailPen.setWidth(2);
        painter.setPen(trailPen);
        painter.drawLine(0, 0, 0, -maxRadius);
        
        painter.restore();
    }
    
    painter.restore();
    
    // Optional: Draw sweep indicator in corner
    int indX = width() - 70;
    int indY = 160;
    int indRadius = 25;
    
    // Small sweep indicator
    painter.setPen(QPen(QColor(0, 168, 255, 100), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPointF(indX, indY), indRadius, indRadius);
    
    // Sweep line in indicator
    painter.save();
    painter.translate(indX, indY);
    painter.rotate(m_sweepAngle);
    painter.setPen(QPen(QColor(0, 255, 128), 2));
    painter.drawLine(0, 0, 0, -indRadius);
    painter.restore();
    
    // Label
    painter.setFont(QFont("Consolas", 7));
    painter.setPen(QColor(0, 168, 255));
    painter.drawText(indX - 25, indY + indRadius + 15, "SCANNING");
}

