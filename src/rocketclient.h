#ifndef ROCKETCLIENT_H
#define ROCKETCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QVector>
#include "rocket.h"

/**
 * Rocket Launch Client - Fetches upcoming rocket launches
 * 
 * Uses Launch Library 2 API (free, no API key needed)
 * https://ll.thespacedevs.com/2.2.0/
 */
class RocketClient : public QObject {
    Q_OBJECT
    
public:
    enum Source {
        LAUNCH_LIBRARY,     // Launch Library 2 API (thespacedevs.com)
        SIMULATED           // Simulated data for testing
    };
    
    explicit RocketClient(QObject* parent = nullptr);
    ~RocketClient() override = default;
    
    void setSource(Source source) { m_source = source; }
    void setUpdateInterval(int msec);
    void setUserLocation(double lat, double lon);
    
    void start();
    void stop();
    void refresh();
    
    QVector<Rocket> getRockets() const { return m_rockets; }
    
signals:
    void rocketsUpdated(const QVector<Rocket>& rockets);
    void error(const QString& message);
    
private slots:
    void fetchData();
    void handleNetworkReply(QNetworkReply* reply);
    
private:
    QNetworkAccessManager* m_networkManager;
    QTimer* m_updateTimer;
    Source m_source;
    QVector<Rocket> m_rockets;
    
    double m_userLat;
    double m_userLon;
    
    void parseLaunchLibraryData(const QByteArray& data);
    void generateSimulatedData();
};

#endif // ROCKETCLIENT_H

