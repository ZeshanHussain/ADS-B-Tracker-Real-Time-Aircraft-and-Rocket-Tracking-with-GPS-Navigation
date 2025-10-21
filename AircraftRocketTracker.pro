QT += core gui widgets network positioning

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = AircraftRocketTracker
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/aircraft.cpp \
    src/rocket.cpp \
    src/geoutils.cpp \
    src/mapwidget.cpp \
    src/gpsnavigator.cpp \
    src/adsbclient.cpp \
    src/rocketclient.cpp \
    src/systemgps.cpp

HEADERS += \
    src/mainwindow.h \
    src/aircraft.h \
    src/rocket.h \
    src/geoutils.h \
    src/mapwidget.h \
    src/gpsnavigator.h \
    src/adsbclient.h \
    src/rocketclient.h \
    src/systemgps.h

# Install
target.path = /usr/local/bin
INSTALLS += target

