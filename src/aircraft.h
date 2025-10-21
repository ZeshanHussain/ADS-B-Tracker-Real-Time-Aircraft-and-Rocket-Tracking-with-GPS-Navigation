#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <QString>
#include <QDateTime>

class Aircraft {
public:
    Aircraft() = default;
    Aircraft(const QString& icao, const QString& callsign, 
             double lat, double lon, double altitude, 
             double velocity, double heading);
    
    // Getters
    QString getICAO() const { return m_icao; }
    QString getCallsign() const { return m_callsign; }
    double getLatitude() const { return m_latitude; }
    double getLongitude() const { return m_longitude; }
    double getAltitude() const { return m_altitude; }
    double getVelocity() const { return m_velocity; }
    double getHeading() const { return m_heading; }
    double getVerticalRate() const { return m_verticalRate; }
    QDateTime getLastUpdate() const { return m_lastUpdate; }
    
    // Setters
    void setCallsign(const QString& callsign) { m_callsign = callsign; }
    void setPosition(double lat, double lon) { 
        m_latitude = lat; 
        m_longitude = lon;
        m_lastUpdate = QDateTime::currentDateTime();
    }
    void setAltitude(double altitude) { m_altitude = altitude; }
    void setVelocity(double velocity) { m_velocity = velocity; }
    void setHeading(double heading) { m_heading = heading; }
    void setVerticalRate(double rate) { m_verticalRate = rate; }
    
    // Check if data is stale (not updated recently)
    bool isStale() const;
    
    // Calculate distance to another position
    double distanceTo(double lat, double lon) const;
    
private:
    QString m_icao;             // ICAO 24-bit address (unique identifier)
    QString m_callsign;         // Flight callsign
    double m_latitude;          // degrees
    double m_longitude;         // degrees
    double m_altitude;          // meters
    double m_velocity;          // m/s
    double m_heading;           // degrees (0-360)
    double m_verticalRate;      // m/s (climb/descent rate)
    QDateTime m_lastUpdate;
};

#endif // AIRCRAFT_H

