#include "rocket.h"
#include "geoutils.h"

Rocket::Rocket(const QString& id, const QString& name, const QString& mission,
               const QDateTime& launchTime, double latitude, double longitude,
               const QString& location)
    : m_id(id), m_name(name), m_mission(mission),
      m_launchTime(launchTime), m_latitude(latitude), m_longitude(longitude),
      m_location(location), m_status(TBD)
{
}

qint64 Rocket::getSecondsUntilLaunch() const {
    return QDateTime::currentDateTime().secsTo(m_launchTime);
}

QString Rocket::getCountdownString() const {
    qint64 seconds = getSecondsUntilLaunch();
    
    if (seconds < 0) {
        return "Launched";
    }
    
    int days = seconds / 86400;
    int hours = (seconds % 86400) / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    if (days > 0) {
        return QString("T-%1d %2h %3m").arg(days).arg(hours).arg(minutes);
    } else if (hours > 0) {
        return QString("T-%1h %2m %3s").arg(hours).arg(minutes).arg(secs);
    } else if (minutes > 0) {
        return QString("T-%1m %2s").arg(minutes).arg(secs);
    } else {
        return QString("T-%1s").arg(secs);
    }
}

bool Rocket::isUpcoming() const {
    qint64 seconds = getSecondsUntilLaunch();
    return seconds > 0 && seconds < (7 * 86400); // Within next 7 days
}

double Rocket::distanceTo(double lat, double lon) const {
    return GeoUtils::calculateDistance(m_latitude, m_longitude, lat, lon);
}

