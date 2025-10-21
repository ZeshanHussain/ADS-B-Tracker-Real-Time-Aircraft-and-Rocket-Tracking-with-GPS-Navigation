#ifndef GPSNAVIGATOR_H
#define GPSNAVIGATOR_H

#include <QWidget>
#include <QPainter>
#include <QVector>
#include <QPointF>
#include <QTimer>
#include <QDateTime>
#include "geoutils.h"
#include "systemgps.h"

struct Waypoint {
    QString name;
    double latitude;
    double longitude;
    QString description;
    QColor color;
    bool isDestination;
    
    Waypoint(const QString& n, double lat, double lon, const QString& desc = "")
        : name(n), latitude(lat), longitude(lon), description(desc),
          color(QColor(0, 168, 255)), isDestination(false) {}
};

struct RouteSegment {
    QPointF start;
    QPointF end;
    double distance;  // meters
    double bearing;   // degrees
    QString instruction;
};

class GPSNavigator : public QWidget {
    Q_OBJECT
    
public:
    enum GPSSource {
        SYSTEM_GPS,     // Built-in GPS
        SDR_RECEIVER,   // RTL-SDR GPS receiver
        SIMULATED       // Simulated movement
    };
    
    enum NavigationMode {
        STANDBY,
        NAVIGATING,
        ARRIVED
    };
    
    explicit GPSNavigator(QWidget* parent = nullptr);
    ~GPSNavigator() override = default;
    
    void setCurrentLocation(double lat, double lon);
    void addWaypoint(const Waypoint& waypoint);
    void setDestination(double lat, double lon, const QString& name = "Destination");
    void startNavigation();
    void stopNavigation();
    void clearRoute();
    
    void setGPSSource(GPSSource source);
    GPSSource getGPSSource() const { return m_gpsSource; }
    
    double getCurrentSpeed() const { return m_currentSpeed; }
    double getCurrentHeading() const { return m_currentHeading; }
    
signals:
    void locationUpdated(double lat, double lon);
    void destinationReached();
    void gpsSignalLost();
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    
private slots:
    void updateGPS();
    void simulateMovement();
    
private:
    // GPS data
    double m_currentLat;
    double m_currentLon;
    double m_currentSpeed;      // m/s
    double m_currentHeading;    // degrees
    double m_currentAltitude;   // meters
    int m_satelliteCount;
    double m_hdop;              // Horizontal Dilution of Precision
    
    // Navigation
    NavigationMode m_mode;
    QVector<Waypoint> m_waypoints;
    QVector<RouteSegment> m_route;
    double m_destinationLat;
    double m_destinationLon;
    QString m_destinationName;
    int m_currentWaypointIndex;
    
    // GPS source
    GPSSource m_gpsSource;
    bool m_gpsLocked;
    QDateTime m_lastGPSUpdate;
    
    // Display
    double m_scale;             // Meters per pixel
    double m_centerLat;
    double m_centerLon;
    bool m_followMode;          // Auto-center on current location
    
    // Timers
    QTimer* m_updateTimer;
    QTimer* m_simulationTimer;
    
    // System GPS
    SystemGPS* m_systemGPS;
    
    // Simulation (for testing)
    double m_simSpeed;          // m/s
    double m_simHeading;        // degrees
    
    // Drawing helpers
    QPointF latLonToScreen(double lat, double lon) const;
    void drawMap(QPainter& painter);
    void drawCurrentLocation(QPainter& painter);
    void drawWaypoints(QPainter& painter);
    void drawRoute(QPainter& painter);
    void drawNavigationInfo(QPainter& painter);
    void drawGPSStatus(QPainter& painter);
    void drawSpeedometer(QPainter& painter);
    void drawCompass(QPainter& painter);
    
    // Route calculation
    void calculateRoute();
    double calculateDistanceToDestination() const;
    double calculateBearingToDestination() const;
    double calculateETA() const; // seconds
};

#endif // GPSNAVIGATOR_H

