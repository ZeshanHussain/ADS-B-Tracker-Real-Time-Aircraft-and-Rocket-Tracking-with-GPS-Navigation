# GPS Setup Guide

## Quick Start - RECOMMENDED ✅

**Use Simulated GPS mode - no configuration needed!**

1. Launch the app: `./build/AircraftRocketTracker`
2. Select "**Simulated GPS**" from the GPS source dropdown  
3. Done! You have a working GPS feed.

This is perfect for development, testing, and demo purposes.

---

## System GPS (Linux GeoClue2) - Complex Setup ⚠️

System GPS on Linux requires GeoClue2 authorization, which is complex to configure.

### Current Issue

GeoClue2 has a strict permission system that causes:
```
qt.positioning.geoclue2: Unable to obtain the client: "org.freedesktop.DBus.Error.NoReply"
GPS Error: "GPS Access Denied - Check permissions"
```

### Why This Happens

- GeoClue2 requires an authorization agent
- Qt's GeoClue2 plugin has DBus timeout issues
- Desktop apps need explicit configuration

### Fix Options

**Option 1: Modify System Config (requires sudo)**
```bash
sudo ./apply_fix.sh
```

This adds your app to `/etc/geoclue/geoclue.conf` with system-level permissions.

**Option 2: Use Simulated GPS (recommended)**
```bash
./build/AircraftRocketTracker
# Then select "Simulated GPS" in the UI
```

Much simpler and works great for testing!

**Option 3: Demo Agent (temporary)**
```bash
./run_with_gps.sh
```

Uses GeoClue demo agent while the app runs.

---

## GPS Modes Comparison

### 🎯 Simulated GPS (BEST for development)
- ✅ Works immediately
- ✅ No permissions needed
- ✅ No hardware required  
- ✅ Consistent test data
- ✅ Perfect for demos

### 📡 System GPS (via GeoClue2)
- ⚠️ Complex Linux setup
- ⚠️ Requires configuration
- ❌ Usually no real GPS hardware anyway
- ❌ Doesn't work indoors
- ❓ Only useful if you have GPS hardware

### 📻 SDR GPS
- For software-defined radio GPS receivers
- Specialized hardware required

---

## Recommendation

**For 99% of use cases: Use Simulated GPS mode!**

It's simpler, more reliable, and better for development. System GPS is only needed if you have actual GPS hardware and need real-world positioning.

---

## Documentation

- `LINUX_GPS_WORKAROUND.md` - Detailed explanation of Linux GPS issues
- `FINAL_GPS_SOLUTION.md` - Complete troubleshooting guide
- `GPS_FIX_COMPLETE.md` - GeoClue2 setup instructions
- `apply_fix.sh` - System-level permission fix script

---

## Still Want to Fix System GPS?

See `LINUX_GPS_WORKAROUND.md` for complete instructions.

**TL;DR:** It's complicated. Just use Simulated GPS mode instead! 😊


