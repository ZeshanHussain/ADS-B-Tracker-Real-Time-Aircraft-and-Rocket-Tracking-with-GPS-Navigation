#!/bin/bash

# Simple run script for Aircraft & Rocket Tracker

cd "$(dirname "$0")"

if [ ! -f "build_direct/AircraftRocketTracker" ]; then
    echo "❌ Application not built yet!"
    echo ""
    echo "Building now..."
    ./compile_direct.sh
    
    if [ $? -ne 0 ]; then
        echo ""
        echo "❌ Build failed! Please check the errors above."
        exit 1
    fi
fi

echo "🚀 Starting Aircraft & Rocket Tracker..."
echo ""
./build_direct/AircraftRocketTracker

