#!/bin/bash
# Run Aircraft Rocket Tracker with GeoClue demo agent for GPS access

echo "🛰️  Starting GPS-enabled Aircraft Rocket Tracker..."
echo ""

# Kill any existing agents
pkill -f "geoclue.*demos/agent" 2>/dev/null

# Start the demo agent in background (authorizes GPS for all apps)
echo "Starting GeoClue2 demo agent..."
/usr/lib/geoclue-2.0/demos/agent > /dev/null 2>&1 &
AGENT_PID=$!

if [ $? -eq 0 ]; then
    echo "✅ Demo agent started (PID: $AGENT_PID)"
else
    echo "❌ Failed to start demo agent"
    exit 1
fi

# Give it a moment to initialize
sleep 1

# Run the application
echo "🚀 Launching Aircraft Rocket Tracker..."
echo ""
cd "$(dirname "$0")"
./build/AircraftRocketTracker

# Cleanup when app exits
echo ""
echo "Stopping demo agent..."
kill $AGENT_PID 2>/dev/null
echo "✅ Cleanup complete"
