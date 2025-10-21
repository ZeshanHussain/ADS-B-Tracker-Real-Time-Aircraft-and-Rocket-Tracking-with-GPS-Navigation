#!/bin/bash
# Test script for GeoClue2 permissions

echo "=== GeoClue2 Diagnostic Tool ==="
echo ""

# Check if geoclue is installed
echo "1. Checking if geoclue is installed..."
if command -v geoclue &> /dev/null; then
    echo "   ✓ GeoClue is installed"
else
    echo "   ✗ GeoClue is NOT installed"
    echo "   Install with: sudo pacman -S geoclue"
fi
echo ""

# Check if geoclue service is running
echo "2. Checking GeoClue service status..."
if systemctl is-active --quiet geoclue; then
    echo "   ✓ GeoClue service is running"
else
    echo "   ✗ GeoClue service is NOT running"
    echo "   Start with: sudo systemctl start geoclue"
fi
echo ""

# Check configuration files
echo "3. Checking configuration files..."
if [ -d "/etc/geoclue/conf.d" ]; then
    echo "   ✓ Configuration directory exists"
    if [ -f "/etc/geoclue/conf.d/10-rocket-tracker.conf" ]; then
        echo "   ✓ Rocket tracker config found"
    else
        echo "   ✗ Rocket tracker config NOT found"
        echo "   Install with: sudo cp geoclue-agent.conf /etc/geoclue/conf.d/10-rocket-tracker.conf"
    fi
else
    echo "   ✗ Configuration directory does not exist"
    echo "   Create with: sudo mkdir -p /etc/geoclue/conf.d"
fi
echo ""

# Check DBus connection
echo "4. Testing DBus connection to GeoClue..."
if busctl status org.freedesktop.GeoClue2 &> /dev/null; then
    echo "   ✓ DBus connection successful"
else
    echo "   ✗ Cannot connect to GeoClue via DBus"
    echo "   Try: sudo systemctl restart geoclue"
fi
echo ""

# Check user permissions
echo "5. Checking user groups..."
CURRENT_GROUPS=$(groups)
echo "   Your groups: $CURRENT_GROUPS"
echo ""

# Test with demo agent
echo "6. Testing with GeoClue demo agent..."
if [ -f "/usr/lib/geoclue-2.0/demos/agent" ]; then
    echo "   ✓ Demo agent available"
    echo ""
    echo "   You can test with:"
    echo "   /usr/lib/geoclue-2.0/demos/agent &"
    echo "   ./build_direct/AircraftRocketTracker"
else
    echo "   ✗ Demo agent not found"
fi
echo ""

echo "=== Recommended Fix ==="
echo ""
echo "Run these commands to fix GeoClue2 permissions:"
echo ""
echo "  sudo mkdir -p /etc/geoclue/conf.d"
echo "  sudo cp geoclue-agent.conf /etc/geoclue/conf.d/10-rocket-tracker.conf"
echo "  sudo systemctl restart geoclue"
echo ""
echo "Then run your application:"
echo "  ./run.sh"
echo ""


