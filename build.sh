#!/bin/bash

# Aircraft & Rocket Tracker - Build Script

echo "🛩️ Aircraft & Rocket Tracker - Build Script 🚀"
echo "=============================================="

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ CMake not found! Please install CMake first."
    exit 1
fi

# Check for Qt
if ! command -v qmake &> /dev/null && ! command -v qmake6 &> /dev/null; then
    echo "⚠️  Warning: Qt may not be installed."
    echo "   Install with: sudo apt install qt6-base-dev (Ubuntu/Debian)"
    echo "              or: sudo pacman -S qt6-base (Arch)"
fi

# Create build directory
echo ""
echo "📁 Creating build directory..."
mkdir -p build
cd build

# Configure
echo ""
echo "⚙️  Configuring with CMake..."
cmake ..

if [ $? -ne 0 ]; then
    echo ""
    echo "❌ CMake configuration failed!"
    echo ""
    echo "Possible solutions:"
    echo "1. Install Qt6: sudo apt install qt6-base-dev qt6-positioning-dev"
    echo "2. Or Qt5: sudo apt install qtbase5-dev qtpositioning5-dev"
    echo "3. Check that CMake can find Qt"
    exit 1
fi

# Build
echo ""
echo "🔨 Building..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo ""
    echo "❌ Build failed! Check the errors above."
    exit 1
fi

echo ""
echo "✅ Build successful!"
echo ""
echo "To run the application:"
echo "  cd build"
echo "  ./AircraftRocketTracker"
echo ""
echo "Or run directly:"
echo "  ./build/AircraftRocketTracker"

