# GPS Permission Issue - Final Solution

## Current Status

You're experiencing GeoClue2 DBus permission issues. The configuration has been installed, but **GeoClue2 on Linux is complex** and often requires an authorization agent.

## ✅ WORKING SOLUTION - Use the Launch Script

**This is the simplest and most reliable way:**

```bash
./run_with_gps.sh
```

This script:
1. Starts the GeoClue2 demo agent (authorizes GPS access)
2. Launches your application  
3. Cleans up when you exit

## Alternative: Manual Launch with Demo Agent

If you prefer to run things manually:

```bash
# Terminal 1: Start demo agent and keep it running
/usr/lib/geoclue-2.0/demos/agent

# Terminal 2: Run your application
./build/AircraftRocketTracker
```

**Important:** Keep the demo agent running in Terminal 1 while using your app!

## Why Is This Needed?

GeoClue2 on Linux requires an "agent" to authorize location access for security/privacy. This agent can be:

1. **Desktop environment agent** - GNOME/KDE have built-in agents
2. **Demo agent** - Provided by GeoClue for development/testing  
3. **Custom authorization config** - Whitelisting apps (we tried this)

The demo agent is the **most reliable** method for standalone applications.

## Understanding the Errors

When you see:
```
qt.positioning.geoclue2: Unable to obtain the client: "org.freedesktop.DBus.Error.NoReply"
GPS Error: "GPS Access Denied - Check permissions"
```

This means: **No agent is authorizing your GPS access**

## Important Note About GPS Hardware

Even with permissions fixed, you may see:
```
GPS timeout - no position update in 30 seconds
```

This is **normal** if:
- ❌ You're indoors (GPS needs clear sky view)
- ❌ You don't have GPS hardware (most laptops don't)
- ❌ You're on a desktop computer (no built-in GPS)

**Solution:** Use **Simulated GPS mode** in your application! It works perfectly for testing.

## Complete Test Procedure

1. **Start with the launch script:**
   ```bash
   ./run_with_gps.sh
   ```

2. **Watch for these messages:**
   ```
   ✅ System GPS available: "geoclue2"
   ✅ Starting system GPS...
   ✅ System GPS started
   ```

3. **If you see "GPS timeout" but NO permission errors:**
   - ✅ Permissions are WORKING!
   - Just no real GPS signal (normal indoors)
   - Switch to Simulated GPS in the app

4. **If you still see permission errors:**
   - Make sure demo agent is running
   - Check: `ps aux | grep geoclue`
   - Should see both `geoclue` service AND `demos/agent`

## Desktop Environment Integration (Optional)

If you want GPS to work without the demo agent:

### For GNOME:
GNOME has a built-in agent, so it should work automatically once the config is installed and you restart your session.

### For KDE Plasma:
Install the agent: `sudo pacman -S plasma-geoclocation`

### For other desktop environments:
Use the demo agent or launch script approach.

## Verification Commands

```bash
# Check if GeoClue is running
systemctl status geoclue

# Check configuration is installed
ls -l /etc/geoclue/conf.d/10-rocket-tracker.conf

# Check for running agents
ps aux | grep geoclue

# Test DBus connection
busctl status org.freedesktop.GeoClue2
```

## Summary

**Best practice for your application:**
```bash
./run_with_gps.sh
```

This handles all the complexity automatically! 🚀

## Still Having Issues?

The application includes other GPS modes:
- **Simulated GPS** - Perfect for testing, no hardware needed
- **SDR GPS** - If you have an SDR GPS receiver
- **Manual Location** - Set location manually

Try Simulated mode - it's actually more useful for development than real GPS!


