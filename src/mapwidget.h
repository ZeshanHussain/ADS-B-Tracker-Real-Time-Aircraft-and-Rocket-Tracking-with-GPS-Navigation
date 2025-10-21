#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QMap>
#include <QVector>
#include <QTimer>
#include "aircraft.h"
#include "rocket.h"

class MapWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit MapWidget(QWidget* parent = nullptr);
    ~MapWidget() override = default;
    
    void setUserLocation(double lat, double lon);
    void setAircraft(const QMap<QString, Aircraft>& aircraft);
    void setRockets(const QVector<Rocket>& rockets);
    void setProximityAlert(bool enabled) { m_proximityAlertEnabled = enabled; }
    void setProximityDistance(double meters) { m_proximityDistance = meters; }
    void setRadarSweepEnabled(bool enabled);
    void setRadarSweepSpeed(int rpm) { m_sweepRPM = rpm; }
    
signals:
    void proximityAlert(const QString& message);
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    
private slots:
    void updateRadarSweep();
    
private:
    double m_userLat;
    double m_userLon;
    QMap<QString, Aircraft> m_aircraft;
    QVector<Rocket> m_rockets;
    
    double m_scale;             // Meters per pixel
    double m_centerLat;
    double m_centerLon;
    
    bool m_proximityAlertEnabled;
    double m_proximityDistance; // meters
    QSet<QString> m_alertedAircraft;
    
    QString m_selectedIcao;
    
    // Radar sweep animation
    bool m_radarSweepEnabled;
    double m_sweepAngle;
    int m_sweepRPM;
    QTimer* m_sweepTimer;
    
    QPointF latLonToScreen(double lat, double lon) const;
    void drawCompass(QPainter& painter);
    void drawScale(QPainter& painter);
    void drawUser(QPainter& painter);
    void drawAircraft(QPainter& painter);
    void drawRockets(QPainter& painter);
    void drawInfo(QPainter& painter);
    void drawRadarSweep(QPainter& painter);
    
    void checkProximityAlerts();
};

#endif // MAPWIDGET_H

