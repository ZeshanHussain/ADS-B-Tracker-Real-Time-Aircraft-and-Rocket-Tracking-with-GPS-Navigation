# Linux System GPS - Known Issue & Workaround

## The Problem

GeoClue2 on Linux uses a **strict permission system** that requires:
1. An authorization agent running in your session
2. OR explicit configuration in `/etc/geoclue/geoclue.conf`
3. AND sometimes a `.desktop` file for your application

Even with all configuration in place, Qt's GeoClue2 plugin sometimes has DBus timeout issues.

## ✅ RECOMMENDED SOLUTION: Use Simulated GPS

**This is actually better for development!**

1. Launch your application: `./build/AircraftRocketTracker`
2. In the GPS panel, select "**Simulated GPS**" from the dropdown
3. This provides a full-featured GPS feed without hardware

**Advantages of Simulated GPS:**
- ✅ Works immediately, no configuration needed
- ✅ Consistent, reproducible positions
- ✅ Perfect for testing and development
- ✅ No hardware or permissions required
- ✅ Faster updates than real GPS

## Alternative: Fix System GPS (Advanced)

If you really need system GPS (not recommended for most users):

### Method 1: System-level Permission

```bash
# Run the provided script
sudo ./apply_fix.sh

# This adds your app to /etc/geoclue/geoclue.conf with system=true
# Then restart your app
./build/AircraftRocketTracker
```

### Method 2: Install Desktop Integration

```bash
# Copy desktop file
cp io.aerotracker.AircraftRocketTracker.desktop ~/.local/share/applications/
update-desktop-database ~/.local/share/applications/

# Copy config
sudo cp geoclue-app.conf /etc/geoclue/conf.d/10-rocket-tracker.conf

# Modify main config
sudo nano /etc/geoclue/geoclue.conf

# Add this section:
[AircraftRocketTracker]
allowed=true
system=true
users=

# Save and restart geoclue
sudo pkill geoclue
```

### Method 3: Use Demo Agent (Temporary)

This only works while the agent is running:

```bash
# Terminal 1 - leave this running:
/usr/lib/geoclue-2.0/demos/agent

# Terminal 2:
./build/AircraftRocketTracker
```

## Why This Is So Complex

Linux location services prioritize privacy and security:
- Apps must be explicitly authorized
- GeoClue acts as a gatekeeper
- Desktop environments usually provide authorization agents
- Standalone apps need special configuration

**For standalone applications like this tracker, Simulated GPS is the practical solution.**

## When You Actually Need System GPS

System GPS is only useful if you have:
- ✅ Real GPS hardware (USB GPS, built-in GPS module)
- ✅ Or WiFi/cell-based positioning (requires location services)
- ✅ And need real-world position updates

For development, testing, and most use cases: **use Simulated GPS!**

## Comparison

| Feature | System GPS | Simulated GPS |
|---------|-----------|---------------|
| Setup Required | Complex | None |
| Hardware Needed | GPS receiver | None |
| Works Indoors | No (GPS only) | Yes |
| Consistent Position | No | Yes |
| Good for Testing | No | **Yes** |
| Good for Demo | No | **Yes** |
| Good for Dev | No | **Yes** |

## Bottom Line

**Just use Simulated GPS mode.** It's simpler, works better for development, and doesn't require any permissions or configuration. The system GPS integration is provided for completeness, but it's not the recommended approach for most users.

If you absolutely need real GPS:
1. Get a USB GPS receiver
2. Run `./apply_fix.sh` with sudo
3. Or use the demo agent method

Otherwise: **Simulated GPS is your friend!** 🎯


