#ifndef SYSTEMGPS_H
#define SYSTEMGPS_H

#include <QObject>
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>
#include <QTimer>

class SystemGPS : public QObject {
    Q_OBJECT
    
public:
    explicit SystemGPS(QObject* parent = nullptr);
    ~SystemGPS() override = default;
    
    void start();
    void stop();
    bool isAvailable() const { return m_source != nullptr; }
    bool hasValidPosition() const { return m_hasValidPosition; }
    
    double getLatitude() const { return m_latitude; }
    double getLongitude() const { return m_longitude; }
    double getAltitude() const { return m_altitude; }
    double getSpeed() const { return m_speed; }
    double getHeading() const { return m_heading; }
    double getAccuracy() const { return m_accuracy; }
    int getSatelliteCount() const { return m_satelliteCount; }
    
signals:
    void positionUpdated(double lat, double lon, double altitude);
    void speedUpdated(double speed);
    void headingUpdated(double heading);
    void accuracyUpdated(double accuracy);
    void gpsError(const QString& message);
    
private slots:
    void onPositionUpdated(const QGeoPositionInfo& info);
    void onPositionError(QGeoPositionInfoSource::Error error);
    void onTimeout();
    
private:
    QGeoPositionInfoSource* m_source;
    QTimer* m_timeoutTimer;
    
    bool m_hasValidPosition;
    double m_latitude;
    double m_longitude;
    double m_altitude;
    double m_speed;
    double m_heading;
    double m_accuracy;
    int m_satelliteCount;
};

#endif // SYSTEMGPS_H

