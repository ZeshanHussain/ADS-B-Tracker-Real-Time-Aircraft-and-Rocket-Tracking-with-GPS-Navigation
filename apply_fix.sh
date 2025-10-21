#!/bin/bash
# Apply GeoClue2 fix - run with sudo

echo "Applying GeoClue2 configuration fix..."

# Backup original config
if [ ! -f /etc/geoclue/geoclue.conf.backup ]; then
    cp /etc/geoclue/geoclue.conf /etc/geoclue/geoclue.conf.backup
    echo "✅ Backed up original config"
fi

# Add our app to the main config if not already there
if ! grep -q "^\[AircraftRocketTracker\]" /etc/geoclue/geoclue.conf; then
    echo "" >> /etc/geoclue/geoclue.conf
    echo "[AircraftRocketTracker]" >> /etc/geoclue/geoclue.conf
    echo "allowed=true" >> /etc/geoclue/geoclue.conf
    echo "system=true" >> /etc/geoclue/geoclue.conf
    echo "users=" >> /etc/geoclue/geoclue.conf
    echo "✅ Added AircraftRocketTracker to geoclue.conf"
else
    echo "✅ AircraftRocketTracker already in config"
fi

# Restart geoclue
pkill geoclue
echo "✅ GeoClue2 service restarted"

echo ""
echo "Done! Now run your application:"
echo "  ./build/AircraftRocketTracker"


