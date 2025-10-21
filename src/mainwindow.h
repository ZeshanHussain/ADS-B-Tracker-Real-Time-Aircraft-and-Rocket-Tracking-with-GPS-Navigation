#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QListWidget>
#include <QMessageBox>
#include <QTabWidget>
#include <QScrollArea>
#include <QCheckBox>
#include "mapwidget.h"
#include "gpsnavigator.h"
#include "adsbclient.h"
#include "rocketclient.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;
    
private slots:
    void onAircraftUpdated(const QMap<QString, Aircraft>& aircraft);
    void onRocketsUpdated(const QVector<Rocket>& rockets);
    void onProximityAlert(const QString& message);
    void onADSBSourceChanged(int index);
    void onRocketSourceChanged(int index);
    void onLocationChanged();
    void onAlertDistanceChanged(int value);
    void onGPSSourceChanged(int index);
    void onSetDestination();
    void onStartNavigation();
    void onStopNavigation();
    void onGPSLocationUpdated(double lat, double lon);
    void updateStatus();

signals:
    void locationUpdated(double lat, double lon);
    
private:
    void setupUI();
    void setupConnections();
    void loadDefaultLocation();
    
    // UI Components
    QTabWidget* m_tabWidget;
    MapWidget* m_mapWidget;
    GPSNavigator* m_gpsNavigator;
    QListWidget* m_aircraftList;
    QListWidget* m_rocketList;
    QLabel* m_statusLabel;
    
    QLineEdit* m_latEdit;
    QLineEdit* m_lonEdit;
    QSpinBox* m_alertDistanceSpin;
    QCheckBox* m_radarSweepCheckbox;
    QComboBox* m_adsbSourceCombo;
    QComboBox* m_rocketSourceCombo;
    QComboBox* m_gpsSourceCombo;
    QPushButton* m_refreshButton;
    QPushButton* m_setDestButton;
    QPushButton* m_startNavButton;
    QPushButton* m_stopNavButton;
    QLineEdit* m_destLatEdit;
    QLineEdit* m_destLonEdit;
    
    // Data clients
    ADSBClient* m_adsbClient;
    RocketClient* m_rocketClient;
    
    // State
    double m_userLat;
    double m_userLon;
};

#endif // MAINWINDOW_H

