#ifndef ADSBCLIENT_H
#define ADSBCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QMap>
#include "aircraft.h"

/**
 * ADS-B Client - Fetches real-time aircraft data
 * 
 * Can connect to:
 * 1. Local dump1090 server (if you have RTL-SDR receiver)
 * 2. OpenSky Network API (free, no API key needed)
 * 3. ADS-B Exchange API (requires API key)
 */
class ADSBClient : public QObject {
    Q_OBJECT
    
public:
    enum Source {
        DUMP1090_LOCAL,     // Local dump1090 server (http://localhost:8080)
        OPENSKY_NETWORK,    // OpenSky Network API
        SIMULATED           // Simulated data for testing
    };
    
    explicit ADSBClient(QObject* parent = nullptr);
    ~ADSBClient() override = default;
    
    void setSource(Source source) { m_source = source; }
    void setUpdateInterval(int msec);
    void setUserLocation(double lat, double lon);
    void setSearchRadius(double meters) { m_searchRadius = meters; }
    
    void start();
    void stop();
    
    QMap<QString, Aircraft> getAircraft() const { return m_aircraft; }
    
signals:
    void aircraftUpdated(const QMap<QString, Aircraft>& aircraft);
    void error(const QString& message);
    
private slots:
    void fetchData();
    void handleNetworkReply(QNetworkReply* reply);
    
private:
    QNetworkAccessManager* m_networkManager;
    QTimer* m_updateTimer;
    Source m_source;
    QMap<QString, Aircraft> m_aircraft;
    
    double m_userLat;
    double m_userLon;
    double m_searchRadius;  // meters
    
    void parseOpenSkyData(const QByteArray& data);
    void parseDump1090Data(const QByteArray& data);
    void generateSimulatedData();
    void removeStaleAircraft();
};

#endif // ADSBCLIENT_H

