#ifndef GEOUTILS_H
#define GEOUTILS_H

#include <cmath>

namespace GeoUtils {
    constexpr double EARTH_RADIUS = 6371000.0; // meters
    constexpr double PI = 3.14159265358979323846;
    
    // Convert degrees to radians
    inline double toRadians(double degrees) {
        return degrees * PI / 180.0;
    }
    
    // Convert radians to degrees
    inline double toDegrees(double radians) {
        return radians * 180.0 / PI;
    }
    
    /**
     * Calculate distance between two GPS coordinates using Haversine formula
     * Physics: Uses spherical geometry and arc length (s = rθ)
     * 
     * @param lat1 Latitude of point 1 (degrees)
     * @param lon1 Longitude of point 1 (degrees)
     * @param lat2 Latitude of point 2 (degrees)
     * @param lon2 Longitude of point 2 (degrees)
     * @return Distance in meters
     */
    inline double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
        double dLat = toRadians(lat2 - lat1);
        double dLon = toRadians(lon2 - lon1);
        
        lat1 = toRadians(lat1);
        lat2 = toRadians(lat2);
        
        // Haversine formula
        double a = std::sin(dLat/2) * std::sin(dLat/2) +
                   std::sin(dLon/2) * std::sin(dLon/2) * std::cos(lat1) * std::cos(lat2);
        double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));
        
        return EARTH_RADIUS * c;
    }
    
    /**
     * Calculate bearing (direction) from point 1 to point 2
     * 
     * @return Bearing in degrees (0-360, where 0 is North)
     */
    inline double calculateBearing(double lat1, double lon1, double lat2, double lon2) {
        lat1 = toRadians(lat1);
        lat2 = toRadians(lat2);
        double dLon = toRadians(lon2 - lon1);
        
        double y = std::sin(dLon) * std::cos(lat2);
        double x = std::cos(lat1) * std::sin(lat2) -
                   std::sin(lat1) * std::cos(lat2) * std::cos(dLon);
        
        double bearing = std::atan2(y, x);
        bearing = toDegrees(bearing);
        
        // Normalize to 0-360
        return std::fmod((bearing + 360.0), 360.0);
    }
    
    /**
     * Calculate 3D distance including altitude difference
     * Uses Pythagorean theorem: d = sqrt(horizontal² + vertical²)
     */
    inline double calculate3DDistance(double lat1, double lon1, double alt1,
                                     double lat2, double lon2, double alt2) {
        double horizontalDist = calculateDistance(lat1, lon1, lat2, lon2);
        double verticalDist = alt2 - alt1;
        
        return std::sqrt(horizontalDist * horizontalDist + verticalDist * verticalDist);
    }
    
    /**
     * Calculate elevation angle to target
     * Physics: tan(θ) = opposite/adjacent
     * 
     * @return Elevation angle in degrees (positive = above horizon)
     */
    inline double calculateElevationAngle(double horizontalDistance, double altitudeDiff) {
        if (horizontalDistance == 0) return (altitudeDiff > 0) ? 90.0 : -90.0;
        return toDegrees(std::atan(altitudeDiff / horizontalDistance));
    }
    
    /**
     * Convert knots to meters per second
     */
    inline double knotsToMetersPerSecond(double knots) {
        return knots * 0.514444;
    }
    
    /**
     * Convert meters per second to knots
     */
    inline double metersPerSecondToKnots(double mps) {
        return mps / 0.514444;
    }
    
    /**
     * Convert feet to meters
     */
    inline double feetToMeters(double feet) {
        return feet * 0.3048;
    }
    
    /**
     * Convert meters to feet
     */
    inline double metersToFeet(double meters) {
        return meters / 0.3048;
    }
}

#endif // GEOUTILS_H

