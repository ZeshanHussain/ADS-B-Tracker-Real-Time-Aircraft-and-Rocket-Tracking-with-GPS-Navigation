#include "aircraft.h"
#include "geoutils.h"

Aircraft::Aircraft(const QString& icao, const QString& callsign,
                   double lat, double lon, double altitude,
                   double velocity, double heading)
    : m_icao(icao), m_callsign(callsign),
      m_latitude(lat), m_longitude(lon), m_altitude(altitude),
      m_velocity(velocity), m_heading(heading), m_verticalRate(0.0),
      m_lastUpdate(QDateTime::currentDateTime())
{
}

bool Aircraft::isStale() const {
    // Consider data stale if not updated in last 30 seconds
    return m_lastUpdate.secsTo(QDateTime::currentDateTime()) > 30;
}

double Aircraft::distanceTo(double lat, double lon) const {
    return GeoUtils::calculateDistance(m_latitude, m_longitude, lat, lon);
}

