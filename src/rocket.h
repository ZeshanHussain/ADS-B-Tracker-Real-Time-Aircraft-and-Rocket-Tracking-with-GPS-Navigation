#ifndef ROCKET_H
#define ROCKET_H

#include <QString>
#include <QDateTime>

class Rocket {
public:
    enum Status {
        GO,
        TBD,
        SUCCESS,
        FAILURE,
        HOLD,
        IN_FLIGHT
    };
    
    Rocket() = default;
    Rocket(const QString& id, const QString& name, const QString& mission,
           const QDateTime& launchTime, double latitude, double longitude,
           const QString& location);
    
    // Getters
    QString getId() const { return m_id; }
    QString getName() const { return m_name; }
    QString getMission() const { return m_mission; }
    QDateTime getLaunchTime() const { return m_launchTime; }
    double getLatitude() const { return m_latitude; }
    double getLongitude() const { return m_longitude; }
    QString getLocation() const { return m_location; }
    Status getStatus() const { return m_status; }
    QString getProvider() const { return m_provider; }
    
    // Setters
    void setStatus(Status status) { m_status = status; }
    void setProvider(const QString& provider) { m_provider = provider; }
    
    // Calculate time until launch
    qint64 getSecondsUntilLaunch() const;
    QString getCountdownString() const;
    
    // Check if launch is upcoming (within next 7 days)
    bool isUpcoming() const;
    
    // Calculate distance to launch site
    double distanceTo(double lat, double lon) const;
    
private:
    QString m_id;
    QString m_name;
    QString m_mission;
    QDateTime m_launchTime;
    double m_latitude;
    double m_longitude;
    QString m_location;
    Status m_status;
    QString m_provider;  // SpaceX, NASA, etc.
};

#endif // ROCKET_H

