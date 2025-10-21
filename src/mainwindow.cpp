#include "mainwindow.h"
#include "geoutils.h"
#include <QSplitter>
#include <QStatusBar>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_userLat(0.0),
      m_userLon(0.0)
{
    setWindowTitle("AETHER // TACTICAL AEROSPACE INTELLIGENCE PLATFORM");
    resize(1600, 1000);
    
    // Set window background
    setStyleSheet(styleSheet() + 
        "QMainWindow { background-color: #0F1216; }");
    
    // Initialize clients
    m_adsbClient = new ADSBClient(this);
    m_rocketClient = new RocketClient(this);
    
    setupUI();
    setupConnections();
    loadDefaultLocation();
    
    // Sync GPS navigator with main location
    m_gpsNavigator->setCurrentLocation(m_userLat, m_userLon);
    
    // Start tracking
    m_adsbClient->start();
    m_rocketClient->start();
    
    // Status update timer
    QTimer* statusTimer = new QTimer(this);
    connect(statusTimer, &QTimer::timeout, this, &MainWindow::updateStatus);
    statusTimer->start(1000); // Update every second
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // Left panel - Controls and lists with scroll area
    QScrollArea* leftScrollArea = new QScrollArea();
    leftScrollArea->setWidgetResizable(true);
    leftScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    leftScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    leftScrollArea->setMaximumWidth(420);
    leftScrollArea->setMinimumWidth(420);
    leftScrollArea->setStyleSheet(
        "QScrollArea { "
        "   border: none; "
        "   background-color: #0F1216; "
        "}"
        "QScrollBar:vertical { "
        "   background-color: #1A2332; "
        "   width: 14px; "
        "   border: 1px solid #2A3F5F; "
        "   border-radius: 6px; "
        "}"
        "QScrollBar::handle:vertical { "
        "   background-color: #00A8FF; "
        "   border-radius: 6px; "
        "   min-height: 30px; "
        "}"
        "QScrollBar::handle:vertical:hover { "
        "   background-color: #00C8FF; "
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { "
        "   border: none; "
        "   background: none; "
        "   height: 0px; "
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { "
        "   background: none; "
        "}"
    );
    
    QWidget* leftPanel = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    
    // Location control group
    QGroupBox* locationGroup = new QGroupBox("⬢ OBSERVATION POINT");
    QVBoxLayout* locationLayout = new QVBoxLayout(locationGroup);
    
    QHBoxLayout* latLayout = new QHBoxLayout();
    latLayout->addWidget(new QLabel("Latitude:"));
    m_latEdit = new QLineEdit();
    m_latEdit->setPlaceholderText("e.g., 34.0522");
    latLayout->addWidget(m_latEdit);
    locationLayout->addLayout(latLayout);
    
    QHBoxLayout* lonLayout = new QHBoxLayout();
    lonLayout->addWidget(new QLabel("Longitude:"));
    m_lonEdit = new QLineEdit();
    m_lonEdit->setPlaceholderText("e.g., -118.2437");
    lonLayout->addWidget(m_lonEdit);
    locationLayout->addLayout(lonLayout);
    
    QPushButton* updateLocButton = new QPushButton("Update Location");
    connect(updateLocButton, &QPushButton::clicked, this, &MainWindow::onLocationChanged);
    locationLayout->addWidget(updateLocButton);
    
    leftLayout->addWidget(locationGroup);
    
    // ADS-B source selection
    QGroupBox* adsbGroup = new QGroupBox("⬢ AEROSPACE TELEMETRY SOURCE");
    QVBoxLayout* adsbLayout = new QVBoxLayout(adsbGroup);
    m_adsbSourceCombo = new QComboBox();
    m_adsbSourceCombo->addItem("◉ SIMULATION MODE", ADSBClient::SIMULATED);
    m_adsbSourceCombo->addItem("◉ OPENSKY NETWORK [LIVE]", ADSBClient::OPENSKY_NETWORK);
    m_adsbSourceCombo->addItem("◉ DUMP1090 [LOCAL]", ADSBClient::DUMP1090_LOCAL);
    adsbLayout->addWidget(m_adsbSourceCombo);
    leftLayout->addWidget(adsbGroup);
    
    // Rocket source selection
    QGroupBox* rocketGroup = new QGroupBox("⬢ LAUNCH MONITORING SOURCE");
    QVBoxLayout* rocketLayout = new QVBoxLayout(rocketGroup);
    m_rocketSourceCombo = new QComboBox();
    m_rocketSourceCombo->addItem("◉ SIMULATION MODE", RocketClient::SIMULATED);
    m_rocketSourceCombo->addItem("◉ LAUNCH LIBRARY [LIVE]", RocketClient::LAUNCH_LIBRARY);
    rocketLayout->addWidget(m_rocketSourceCombo);
    leftLayout->addWidget(rocketGroup);
    
    // Alert settings
    QGroupBox* alertGroup = new QGroupBox("⬢ PROXIMITY DETECTION");
    QVBoxLayout* alertLayout = new QVBoxLayout(alertGroup);
    
    QHBoxLayout* distLayout = new QHBoxLayout();
    distLayout->addWidget(new QLabel("Detection Range:"));
    m_alertDistanceSpin = new QSpinBox();
    m_alertDistanceSpin->setRange(1, 100);
    m_alertDistanceSpin->setValue(10);
    m_alertDistanceSpin->setSuffix(" km");
    distLayout->addWidget(m_alertDistanceSpin);
    alertLayout->addLayout(distLayout);
    
    // Radar sweep toggle
    m_radarSweepCheckbox = new QCheckBox("◉ Radar Sweep Animation");
    m_radarSweepCheckbox->setChecked(true);
    m_radarSweepCheckbox->setToolTip("Enable classic rotating radar sweep effect");
    alertLayout->addWidget(m_radarSweepCheckbox);
    
    leftLayout->addWidget(alertGroup);
    
    // Aircraft list
    QGroupBox* aircraftGroup = new QGroupBox("⬢ AERIAL CONTACTS [ACTIVE]");
    QVBoxLayout* aircraftLayout = new QVBoxLayout(aircraftGroup);
    m_aircraftList = new QListWidget();
    m_aircraftList->setFont(QFont("Consolas", 9));
    aircraftLayout->addWidget(m_aircraftList);
    leftLayout->addWidget(aircraftGroup);
    
    // Rocket list
    QGroupBox* rocketListGroup = new QGroupBox("⬢ LAUNCH OPERATIONS [SCHEDULED]");
    QVBoxLayout* rocketListLayout = new QVBoxLayout(rocketListGroup);
    m_rocketList = new QListWidget();
    m_rocketList->setFont(QFont("Consolas", 9));
    rocketListLayout->addWidget(m_rocketList);
    
    m_refreshButton = new QPushButton("↻ REFRESH INTELLIGENCE");
    connect(m_refreshButton, &QPushButton::clicked, m_rocketClient, &RocketClient::refresh);
    rocketListLayout->addWidget(m_refreshButton);
    
    leftLayout->addWidget(rocketListGroup);
    
    // GPS Navigation controls group
    QGroupBox* gpsGroup = new QGroupBox("⬢ GPS NAVIGATION CONTROLS");
    QVBoxLayout* gpsLayout = new QVBoxLayout(gpsGroup);
    
    // Current location input
    QLabel* currentLocLabel = new QLabel("Current Location:");
    currentLocLabel->setStyleSheet("color: #00A8FF; font-weight: bold;");
    gpsLayout->addWidget(currentLocLabel);
    
    QHBoxLayout* currentLatLayout = new QHBoxLayout();
    currentLatLayout->addWidget(new QLabel("Latitude:"));
    QLineEdit* currentLatEdit = new QLineEdit();
    currentLatEdit->setPlaceholderText("e.g., 34.0522");
    currentLatLayout->addWidget(currentLatEdit);
    gpsLayout->addLayout(currentLatLayout);
    
    QHBoxLayout* currentLonLayout = new QHBoxLayout();
    currentLonLayout->addWidget(new QLabel("Longitude:"));
    QLineEdit* currentLonEdit = new QLineEdit();
    currentLonEdit->setPlaceholderText("e.g., -118.2437");
    currentLonLayout->addWidget(currentLonEdit);
    gpsLayout->addLayout(currentLonLayout);
    
    QPushButton* setCurrentButton = new QPushButton("📍 SET CURRENT LOCATION");
    setCurrentButton->setStyleSheet("font-weight: bold;");
    gpsLayout->addWidget(setCurrentButton);
    
    // Destination input
    QLabel* destLabel = new QLabel("Destination:");
    destLabel->setStyleSheet("color: #FF5252; font-weight: bold; margin-top: 10px;");
    gpsLayout->addWidget(destLabel);
    
    QHBoxLayout* destLatLayout = new QHBoxLayout();
    destLatLayout->addWidget(new QLabel("Latitude:"));
    m_destLatEdit = new QLineEdit();
    m_destLatEdit->setPlaceholderText("e.g., 37.7749");
    destLatLayout->addWidget(m_destLatEdit);
    gpsLayout->addLayout(destLatLayout);
    
    QHBoxLayout* destLonLayout = new QHBoxLayout();
    destLonLayout->addWidget(new QLabel("Longitude:"));
    m_destLonEdit = new QLineEdit();
    m_destLonEdit->setPlaceholderText("e.g., -122.4194");
    destLonLayout->addWidget(m_destLonEdit);
    gpsLayout->addLayout(destLonLayout);
    
    QLineEdit* destNameEdit = new QLineEdit();
    destNameEdit->setPlaceholderText("Destination name (optional)");
    gpsLayout->addWidget(destNameEdit);
    
    m_setDestButton = new QPushButton("🎯 SET DESTINATION");
    m_setDestButton->setStyleSheet("font-weight: bold; background-color: #2A3F5F;");
    gpsLayout->addWidget(m_setDestButton);
    
    // Navigation controls
    QHBoxLayout* navButtonLayout = new QHBoxLayout();
    m_startNavButton = new QPushButton("▶ START");
    m_startNavButton->setStyleSheet("background-color: #00A800;");
    m_stopNavButton = new QPushButton("■ STOP");
    m_stopNavButton->setStyleSheet("background-color: #A80000;");
    navButtonLayout->addWidget(m_startNavButton);
    navButtonLayout->addWidget(m_stopNavButton);
    gpsLayout->addLayout(navButtonLayout);
    
    // GPS source selection
    QLabel* gpsSourceLabel = new QLabel("GPS Source:");
    gpsSourceLabel->setStyleSheet("margin-top: 10px;");
    gpsLayout->addWidget(gpsSourceLabel);
    
    m_gpsSourceCombo = new QComboBox();
    m_gpsSourceCombo->addItem("◉ SIMULATED MODE", GPSNavigator::SIMULATED);
    m_gpsSourceCombo->addItem("◉ SYSTEM GPS", GPSNavigator::SYSTEM_GPS);
    m_gpsSourceCombo->addItem("◉ SDR RECEIVER", GPSNavigator::SDR_RECEIVER);
    gpsLayout->addWidget(m_gpsSourceCombo);
    
    // Quick destinations
    QLabel* quickDestLabel = new QLabel("Quick Destinations:");
    quickDestLabel->setStyleSheet("margin-top: 10px; font-weight: bold;");
    gpsLayout->addWidget(quickDestLabel);
    
    QPushButton* sfButton = new QPushButton("San Francisco");
    QPushButton* nyButton = new QPushButton("New York");
    QPushButton* miaButton = new QPushButton("Miami");
    
    gpsLayout->addWidget(sfButton);
    gpsLayout->addWidget(nyButton);
    gpsLayout->addWidget(miaButton);
    
    leftLayout->addWidget(gpsGroup);
    
    // Store references for later use
    QLineEdit* currentLatEditPtr = currentLatEdit;
    QLineEdit* currentLonEditPtr = currentLonEdit;
    QLineEdit* destNameEditPtr = destNameEdit;
    
    // Connect GPS controls
    connect(setCurrentButton, &QPushButton::clicked, [this, currentLatEditPtr, currentLonEditPtr]() {
        bool latOk, lonOk;
        double lat = currentLatEditPtr->text().toDouble(&latOk);
        double lon = currentLonEditPtr->text().toDouble(&lonOk);
        
        if (latOk && lonOk && lat >= -90 && lat <= 90 && lon >= -180 && lon <= 180) {
            m_userLat = lat;
            m_userLon = lon;
            m_gpsNavigator->setCurrentLocation(lat, lon);
            m_mapWidget->setUserLocation(lat, lon);
            m_adsbClient->setUserLocation(lat, lon);
            m_rocketClient->setUserLocation(lat, lon);
            m_latEdit->setText(QString::number(lat, 'f', 6));
            m_lonEdit->setText(QString::number(lon, 'f', 6));
            m_statusLabel->setText(QString("◉ LOCATION SET: [%1°, %2°]").arg(lat, 0, 'f', 4).arg(lon, 0, 'f', 4));
        } else {
            QMessageBox::warning(this, "Invalid Location", "Please enter valid GPS coordinates.\nLatitude: -90 to 90\nLongitude: -180 to 180");
        }
    });
    
    connect(m_setDestButton, &QPushButton::clicked, [this, destNameEditPtr]() {
        onSetDestination();
        if (!destNameEditPtr->text().isEmpty()) {
            m_gpsNavigator->setDestination(
                m_destLatEdit->text().toDouble(),
                m_destLonEdit->text().toDouble(),
                destNameEditPtr->text()
            );
        }
    });
    
    connect(m_startNavButton, &QPushButton::clicked, this, &MainWindow::onStartNavigation);
    connect(m_stopNavButton, &QPushButton::clicked, this, &MainWindow::onStopNavigation);
    
    connect(m_gpsSourceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        GPSNavigator::GPSSource source = static_cast<GPSNavigator::GPSSource>(
            m_gpsSourceCombo->itemData(index).toInt());
        m_gpsNavigator->setGPSSource(source);
        m_statusLabel->setText(QString("◉ GPS SOURCE: %1").arg(m_gpsSourceCombo->currentText()));
    });
    
    // Quick destination buttons
    connect(sfButton, &QPushButton::clicked, [this, destNameEditPtr]() {
        m_destLatEdit->setText("37.7749");
        m_destLonEdit->setText("-122.4194");
        destNameEditPtr->setText("San Francisco, CA");
        m_statusLabel->setText("◉ DESTINATION: San Francisco");
    });
    
    connect(nyButton, &QPushButton::clicked, [this, destNameEditPtr]() {
        m_destLatEdit->setText("40.7128");
        m_destLonEdit->setText("-74.0060");
        destNameEditPtr->setText("New York, NY");
        m_statusLabel->setText("◉ DESTINATION: New York");
    });
    
    connect(miaButton, &QPushButton::clicked, [this, destNameEditPtr]() {
        m_destLatEdit->setText("25.7617");
        m_destLonEdit->setText("-80.1918");
        destNameEditPtr->setText("Miami, FL");
        m_statusLabel->setText("◉ DESTINATION: Miami");
    });
    
    // Sync current location with GPS coordinates
    connect(this, &MainWindow::locationUpdated, [currentLatEditPtr, currentLonEditPtr](double lat, double lon) {
        currentLatEditPtr->setText(QString::number(lat, 'f', 6));
        currentLonEditPtr->setText(QString::number(lon, 'f', 6));
    });
    
    // Initialize with current location
    currentLatEditPtr->setText(QString::number(m_userLat, 'f', 6));
    currentLonEditPtr->setText(QString::number(m_userLon, 'f', 6));
    
    leftLayout->addStretch();
    
    // Set the scrollable widget
    leftScrollArea->setWidget(leftPanel);
    
    // Create tab widget for main display
    m_tabWidget = new QTabWidget();
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane { "
        "   border: 2px solid #2A3F5F; "
        "   background-color: #0F1216; "
        "}"
        "QTabBar::tab { "
        "   background-color: #1A2332; "
        "   color: #DCE1E6; "
        "   padding: 8px 20px; "
        "   border: 1px solid #2A3F5F; "
        "   border-bottom: none; "
        "   margin-right: 2px; "
        "}"
        "QTabBar::tab:selected { "
        "   background-color: #2A3F5F; "
        "   color: #00A8FF; "
        "   font-weight: bold; "
        "}"
        "QTabBar::tab:hover { "
        "   background-color: #243447; "
        "}"
    );
    
    // Aerial Surveillance Tab
    m_mapWidget = new MapWidget();
    m_tabWidget->addTab(m_mapWidget, "⬢ AERIAL SURVEILLANCE");
    
    // GPS Navigation Tab
    m_gpsNavigator = new GPSNavigator();
    m_tabWidget->addTab(m_gpsNavigator, "⬢ GPS NAVIGATION");
    
    // Add to main layout
    mainLayout->addWidget(leftScrollArea);
    mainLayout->addWidget(m_tabWidget, 1);
    
    // Status bar with enhanced Palantir styling
    m_statusLabel = new QLabel("◉ SYSTEM INITIALIZING...");
    m_statusLabel->setStyleSheet("color: #00A8FF; font-weight: bold; padding: 4px;");
    statusBar()->addWidget(m_statusLabel);
    statusBar()->setStyleSheet(
        "QStatusBar { "
        "   background-color: #0F1216; "
        "   border-top: 2px solid #00A8FF; "
        "   color: #00A8FF; "
        "}");
}

void MainWindow::setupConnections() {
    connect(m_adsbClient, &ADSBClient::aircraftUpdated,
            this, &MainWindow::onAircraftUpdated);
    
    connect(m_rocketClient, &RocketClient::rocketsUpdated,
            this, &MainWindow::onRocketsUpdated);
    
    connect(m_mapWidget, &MapWidget::proximityAlert,
            this, &MainWindow::onProximityAlert);
    
    connect(m_adsbSourceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onADSBSourceChanged);
    
    connect(m_rocketSourceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onRocketSourceChanged);
    
    connect(m_alertDistanceSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onAlertDistanceChanged);
    
    connect(m_radarSweepCheckbox, &QCheckBox::toggled,
            m_mapWidget, &MapWidget::setRadarSweepEnabled);
    
    connect(m_gpsNavigator, &GPSNavigator::locationUpdated,
            this, &MainWindow::onGPSLocationUpdated);
}

void MainWindow::onGPSSourceChanged(int index) {
    Q_UNUSED(index);
    // Implementation for GPS source change
}

void MainWindow::onSetDestination() {
    bool latOk, lonOk;
    double lat = m_destLatEdit->text().toDouble(&latOk);
    double lon = m_destLonEdit->text().toDouble(&lonOk);
    
    if (!latOk || !lonOk) {
        QMessageBox::warning(this, "Invalid Destination", 
                           "Please enter valid destination coordinates.");
        return;
    }
    
    m_gpsNavigator->setDestination(lat, lon, "Target Location");
    m_statusLabel->setText(QString("◉ DESTINATION SET: [%1°, %2°]").arg(lat, 0, 'f', 4).arg(lon, 0, 'f', 4));
}

void MainWindow::onStartNavigation() {
    m_gpsNavigator->startNavigation();
    m_statusLabel->setText("◉ NAVIGATION STARTED");
}

void MainWindow::onStopNavigation() {
    m_gpsNavigator->stopNavigation();
    m_statusLabel->setText("◉ NAVIGATION STOPPED");
}

void MainWindow::onGPSLocationUpdated(double lat, double lon) {
    // Update main location when GPS moves
    m_userLat = lat;
    m_userLon = lon;
    m_mapWidget->setUserLocation(lat, lon);
    m_adsbClient->setUserLocation(lat, lon);
    m_rocketClient->setUserLocation(lat, lon);
    emit locationUpdated(lat, lon);
}

void MainWindow::loadDefaultLocation() {
    // Default to Los Angeles (can be changed by user)
    m_userLat = 34.0522;
    m_userLon = -118.2437;
    
    m_latEdit->setText(QString::number(m_userLat, 'f', 4));
    m_lonEdit->setText(QString::number(m_userLon, 'f', 4));
    
    m_mapWidget->setUserLocation(m_userLat, m_userLon);
    m_adsbClient->setUserLocation(m_userLat, m_userLon);
    m_rocketClient->setUserLocation(m_userLat, m_userLon);
}

void MainWindow::onAircraftUpdated(const QMap<QString, Aircraft>& aircraft) {
    m_mapWidget->setAircraft(aircraft);
    
    // Update aircraft list
    m_aircraftList->clear();
    
    QVector<QPair<double, Aircraft>> sortedAircraft;
    for (const Aircraft& ac : aircraft) {
        double distance = ac.distanceTo(m_userLat, m_userLon);
        sortedAircraft.append(qMakePair(distance, ac));
    }
    
    std::sort(sortedAircraft.begin(), sortedAircraft.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    for (const auto& pair : sortedAircraft) {
        const Aircraft& ac = pair.second;
        double distance = pair.first;
        
        QString callsign = ac.getCallsign();
        if (callsign.isEmpty()) callsign = ac.getICAO();
        
        double bearing = GeoUtils::calculateBearing(m_userLat, m_userLon,
                                                    ac.getLatitude(), ac.getLongitude());
        
        QString statusIcon;
        if (distance < 5000) statusIcon = "◉"; // Critical
        else if (distance < 15000) statusIcon = "◎"; // Warning
        else statusIcon = "○"; // Nominal
        
        QString text = QString("%1 %2 │ RNG:%3km │ BRG:%4° │ ALT:%5m │ VEL:%6m/s")
                      .arg(statusIcon)
                      .arg(callsign, -8)
                      .arg(distance / 1000.0, 5, 'f', 1)
                      .arg((int)bearing, 3)
                      .arg((int)ac.getAltitude(), 5)
                      .arg((int)ac.getVelocity(), 3);
        
        QListWidgetItem* item = new QListWidgetItem(text);
        if (distance < 5000) {
            item->setForeground(QColor(255, 82, 82)); // Red for close
        } else if (distance < 15000) {
            item->setForeground(QColor(255, 170, 0)); // Orange
        } else {
            item->setForeground(QColor(0, 200, 255)); // Cyan
        }
        m_aircraftList->addItem(item);
    }
}

void MainWindow::onRocketsUpdated(const QVector<Rocket>& rockets) {
    m_mapWidget->setRockets(rockets);
    
    // Update rocket list
    m_rocketList->clear();
    
    for (const Rocket& rocket : rockets) {
        if (!rocket.isUpcoming() && rocket.getSecondsUntilLaunch() < -86400) {
            continue; // Skip old launches
        }
        
        double distance = rocket.distanceTo(m_userLat, m_userLon);
        
        QString statusIcon;
        qint64 timeToLaunch = rocket.getSecondsUntilLaunch();
        if (timeToLaunch < 0) statusIcon = "◼"; // Completed
        else if (timeToLaunch < 3600) statusIcon = "◉"; // Critical
        else if (timeToLaunch < 86400) statusIcon = "◎"; // Warning
        else statusIcon = "○"; // Nominal
        
        QString text = QString("%1 %2\n   %3 │ RNG:%4km\n   %5")
                      .arg(statusIcon)
                      .arg(rocket.getName())
                      .arg(rocket.getCountdownString(), -14)
                      .arg(distance / 1000.0, 0, 'f', 0)
                      .arg(rocket.getLocation());
        
        QListWidgetItem* item = new QListWidgetItem(text);
        
        // Color based on countdown (timeToLaunch already declared above)
        if (timeToLaunch < 0) {
            item->setForeground(Qt::gray);
        } else if (timeToLaunch < 3600) {
            item->setForeground(Qt::red);
            item->setFont(QFont("Courier", 9, QFont::Bold));
        } else if (timeToLaunch < 86400) {
            item->setForeground(QColor(255, 150, 0));
        } else {
            item->setForeground(Qt::green);
        }
        
        m_rocketList->addItem(item);
    }
}

void MainWindow::onProximityAlert(const QString& message) {
    // Show notification
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Proximity Alert!");
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
}

void MainWindow::onADSBSourceChanged(int index) {
    ADSBClient::Source source = static_cast<ADSBClient::Source>(
        m_adsbSourceCombo->itemData(index).toInt());
    
    m_adsbClient->stop();
    m_adsbClient->setSource(source);
    m_adsbClient->start();
    
    m_statusLabel->setText(QString("ADS-B Source: %1").arg(m_adsbSourceCombo->currentText()));
}

void MainWindow::onRocketSourceChanged(int index) {
    RocketClient::Source source = static_cast<RocketClient::Source>(
        m_rocketSourceCombo->itemData(index).toInt());
    
    m_rocketClient->stop();
    m_rocketClient->setSource(source);
    m_rocketClient->start();
}

void MainWindow::onLocationChanged() {
    bool latOk, lonOk;
    double lat = m_latEdit->text().toDouble(&latOk);
    double lon = m_lonEdit->text().toDouble(&lonOk);
    
    if (!latOk || !lonOk) {
        QMessageBox::warning(this, "Invalid Location", 
                           "Please enter valid latitude and longitude values.");
        return;
    }
    
    if (lat < -90 || lat > 90 || lon < -180 || lon > 180) {
        QMessageBox::warning(this, "Invalid Location",
                           "Latitude must be between -90 and 90.\nLongitude must be between -180 and 180.");
        return;
    }
    
    m_userLat = lat;
    m_userLon = lon;
    
    m_mapWidget->setUserLocation(m_userLat, m_userLon);
    m_gpsNavigator->setCurrentLocation(m_userLat, m_userLon);
    m_adsbClient->setUserLocation(m_userLat, m_userLon);
    m_rocketClient->setUserLocation(m_userLat, m_userLon);
    
    emit locationUpdated(m_userLat, m_userLon);
    
    m_statusLabel->setText(QString("◉ LOCATION UPDATED: [%1°, %2°]")
                          .arg(m_userLat, 0, 'f', 4).arg(m_userLon, 0, 'f', 4));
}

void MainWindow::onAlertDistanceChanged(int value) {
    double meters = value * 1000.0;
    m_mapWidget->setProximityDistance(meters);
    m_adsbClient->setSearchRadius(meters);
}

void MainWindow::updateStatus() {
    int aircraftCount = m_adsbClient->getAircraft().size();
    int rocketCount = m_rocketClient->getRockets().size();
    
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString status = QString("◉ SYSTEM OPERATIONAL │ TIME:%1 │ CONTACTS:%2 │ LAUNCHES:%3 │ COORDS:[%4°,%5°]")
                    .arg(timestamp)
                    .arg(aircraftCount, 3)
                    .arg(rocketCount, 2)
                    .arg(m_userLat, 0, 'f', 4)
                    .arg(m_userLon, 0, 'f', 4);
    
    m_statusLabel->setText(status);
}

