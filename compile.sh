#!/bin/bash

# Complete compilation script for Aircraft & Rocket Tracker with GPS

echo "🛩️ AETHER // TACTICAL AEROSPACE INTELLIGENCE PLATFORM 🚀"
echo "=========================================================="

# Check for Qt6
if pkg-config --exists Qt6Core; then
    QT_VERSION=6
    echo "✅ Found Qt6"
else
    echo "❌ Qt6 not found!"
    exit 1
fi

# Get flags
QT_CFLAGS=$(pkg-config --cflags Qt6Core Qt6Gui Qt6Widgets Qt6Network Qt6Positioning)
QT_LIBS=$(pkg-config --libs Qt6Core Qt6Gui Qt6Widgets Qt6Network Qt6Positioning)
MOC="/usr/lib/qt6/moc"

# Create/clean build directory
echo ""
echo "📁 Setting up build directory..."
rm -rf build_direct
mkdir -p build_direct
cd build_direct

echo ""
echo "📝 Generating MOC files..."
$MOC ../src/mainwindow.h -o moc_mainwindow.cpp
$MOC ../src/mapwidget.h -o moc_mapwidget.cpp
$MOC ../src/gpsnavigator.h -o moc_gpsnavigator.cpp
$MOC ../src/adsbclient.h -o moc_adsbclient.cpp
$MOC ../src/rocketclient.h -o moc_rocketclient.cpp
$MOC ../src/systemgps.h -o moc_systemgps.cpp

echo "✅ MOC files generated"

echo ""
echo "🔨 Compiling source files..."
g++ -c -std=c++17 -fPIC $QT_CFLAGS \
    ../src/main.cpp \
    ../src/mainwindow.cpp \
    ../src/aircraft.cpp \
    ../src/rocket.cpp \
    ../src/geoutils.cpp \
    ../src/mapwidget.cpp \
    ../src/gpsnavigator.cpp \
    ../src/adsbclient.cpp \
    ../src/rocketclient.cpp \
    ../src/systemgps.cpp \
    moc_mainwindow.cpp \
    moc_mapwidget.cpp \
    moc_gpsnavigator.cpp \
    moc_adsbclient.cpp \
    moc_rocketclient.cpp \
    moc_systemgps.cpp

if [ $? -ne 0 ]; then
    echo ""
    echo "❌ Compilation failed!"
    exit 1
fi

echo "✅ Compilation successful"

echo ""
echo "🔗 Linking..."
g++ -o AircraftRocketTracker \
    main.o \
    mainwindow.o \
    aircraft.o \
    rocket.o \
    geoutils.o \
    mapwidget.o \
    gpsnavigator.o \
    adsbclient.o \
    rocketclient.o \
    systemgps.o \
    moc_mainwindow.o \
    moc_mapwidget.o \
    moc_gpsnavigator.o \
    moc_adsbclient.o \
    moc_rocketclient.o \
    moc_systemgps.o \
    $QT_LIBS

if [ $? -ne 0 ]; then
    echo ""
    echo "❌ Linking failed!"
    exit 1
fi

echo "✅ Linking successful"

echo ""
echo "🎉 BUILD COMPLETE!"
echo ""
echo "⬢ SYSTEM OPERATIONAL"
echo ""
echo "To launch:"
echo "  cd build_direct"
echo "  ./AircraftRocketTracker"
echo ""
echo "Or:"
echo "  ./build_direct/AircraftRocketTracker"
echo ""

