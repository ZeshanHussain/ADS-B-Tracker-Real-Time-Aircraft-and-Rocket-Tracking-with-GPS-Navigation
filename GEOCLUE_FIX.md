# GeoClue2 Permission Fix

Your application is experiencing GeoClue2 DBus permission issues. Here are the solutions:

## Quick Fix (Option 1): Use Agent Configuration

1. **Copy the agent configuration file:**
   ```bash
   sudo mkdir -p /etc/geoclue/conf.d
   sudo cp geoclue-agent.conf /etc/geoclue/conf.d/10-rocket-tracker.conf
   ```

2. **Restart GeoClue2 service:**
   ```bash
   sudo systemctl restart geoclue
   ```

## Alternative Fix (Option 2): Desktop File Method

1. **Copy the desktop file:**
   ```bash
   cp rocket-tracker.desktop ~/.local/share/applications/
   update-desktop-database ~/.local/share/applications/
   ```

2. **Edit GeoClue config to whitelist your app:**
   ```bash
   sudo nano /etc/geoclue/geoclue.conf
   ```
   
   Add this section:
   ```ini
   [rocket-tracker]
   allowed=true
   system=false
   users=
   ```

3. **Restart GeoClue2:**
   ```bash
   sudo systemctl restart geoclue
   ```

## Option 3: Run with GeoClue Demo Agent

For testing purposes, you can use the demo agent:

```bash
/usr/lib/geoclue-2.0/demos/agent &
./build_direct/AircraftRocketTracker
```

## Option 4: Check Your User Permissions

Make sure your user is in the right groups:
```bash
sudo usermod -a -G wheel,video,audio,input $USER
```

Then log out and back in.

## Verify GeoClue2 is Running

```bash
systemctl status geoclue
```

If it's not running:
```bash
sudo systemctl enable --now geoclue
```

## After Applying the Fix

Run your application again:
```bash
cd /home/zeshan/rocket
./run.sh
```

The GPS should now work without permission errors!


