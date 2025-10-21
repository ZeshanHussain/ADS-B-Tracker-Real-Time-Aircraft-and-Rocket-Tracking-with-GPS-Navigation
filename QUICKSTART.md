# 🚀 Quick Start Guide - Aircraft & Rocket Tracker

## What You've Built

You now have a **real-time aircraft and rocket tracking application** that combines:
- ✈️ **ADS-B aircraft tracking** (planes near you)
- 🚀 **Rocket launch monitoring** (upcoming launches worldwide)
- 📍 **GPS-based proximity alerts**
- 🎨 **Beautiful real-time map visualization**
- 📐 **Physics calculations** (distance, bearing, velocity)

## Running the Application

```bash
cd /home/zeshan/rocket
./build_direct/AircraftRocketTracker
```

## First Run Setup

### 1. Set Your Location
- In the top-left panel, enter your GPS coordinates:
  - **Latitude**: e.g., `34.0522` (Los Angeles)
  - **Longitude**: e.g., `-118.2437`
- Click **"Update Location"**

**Find your coordinates**: Right-click on Google Maps → "What's here?"

### 2. Choose Data Source

#### Aircraft Data:
- **🔴 Simulated (Demo)**: Perfect for testing! Shows 3-8 fake planes around you
- **🌐 OpenSky Network (Real)**: LIVE aircraft data from real planes! Free, no API key needed
- **📡 Local Dump1090**: If you have RTL-SDR hardware (advanced)

Start with **Simulated**, then switch to **OpenSky Network** to see REAL planes!

#### Rocket Data:
- **🔴 Simulated (Demo)**: Sample launches for testing
- **🌐 Launch Library 2 (Real)**: REAL upcoming launches (SpaceX, NASA, etc.)

### 3. Set Alert Distance
- Default: 10 km
- You'll get popup alerts when planes/rockets are within this range

## Understanding the Display

### Left Panel - Control Center

#### Nearby Aircraft List
```
FL567      |   12.3 km | 045° |  9500 m | 225 m/s
```
- **FL567**: Flight callsign
- **12.3 km**: Distance from you
- **045°**: Bearing (direction) - 0° = North, 90° = East, etc.
- **9500 m**: Altitude (about 31,000 feet)
- **225 m/s**: Velocity (about 500 mph)

#### Upcoming Launches
Shows:
- Mission name (e.g., "SpaceX | Starlink Mission")
- **T-2d 5h 30m**: Countdown to launch (T-minus format)
- Distance to launch site
- Launch location

### Map Display

#### Your Location (Center):
- 🟡 **Yellow circle** with crosshairs = YOU

#### Aircraft:
- **🔴 Red plane**: Very close (< 5 km) - ALERT!
- **🟠 Orange plane**: Nearby (5-15 km)
- **🔵 Cyan plane**: Far (> 15 km)
- Triangle points in direction of flight

#### Rockets:
- **🚀 Red rocket**: Launching soon (< 1 hour)! 
- **🚀 Orange rocket**: Launching today
- **🚀 Green rocket**: Upcoming launch
- Flames appear if launch is imminent

#### Map Features:
- **Range circles**: 5km, 10km, 25km, 50km
- **Compass**: Top-right (N = North)
- **Scale bar**: Bottom-left

### Map Controls:
- **Mouse Wheel**: Zoom in/out
- **Click on Aircraft**: Select/highlight

## Example Locations to Try

### High Air Traffic Areas:
```
Los Angeles International:
Lat: 33.9416, Lon: -118.4085

New York JFK:
Lat: 40.6413, Lon: -73.7781

London Heathrow:
Lat: 51.4700, Lon: -0.4543
```

### Near Rocket Launch Sites:
```
Cape Canaveral, Florida:
Lat: 28.5729, Lon: -80.6490

Vandenberg SFB, California:
Lat: 34.7420, Lon: -120.5724

SpaceX Starbase, Texas:
Lat: 25.9972, Lon: -97.1569
```

## Physics in Action

### What You're Seeing:

1. **Haversine Formula** - Great-circle distance on Earth's surface
   ```
   Distance = Earth_Radius × arccos(sin(lat1)×sin(lat2) + cos(lat1)×cos(lat2)×cos(Δlon))
   ```

2. **Bearing Calculations** - Direction to target
   ```
   θ = atan2(sin(Δλ)·cos(φ2), cos(φ1)·sin(φ2) − sin(φ1)·cos(φ2)·cos(Δλ))
   ```

3. **Velocity Vectors** - Aircraft speed and heading displayed visually

4. **3D Distance** - Includes altitude: `d = √(horizontal² + vertical²)`

### Physics Concepts Demonstrated:
- ✅ **Kinematics** (position, velocity)
- ✅ **Vector mathematics** (2D/3D positions)
- ✅ **Coordinate systems** (GPS → Cartesian)
- ✅ **Spherical geometry** (Earth as sphere)
- ✅ **Reference frames** (Earth-centered)

## Proximity Alerts

You'll get a popup when:
1. An aircraft enters your alert range (default 10 km)
2. A rocket launch is within 1 hour AND within 200 km

Alert shows:
- Callsign/Mission name
- Exact distance
- Bearing (direction)
- Altitude (for aircraft)
- Countdown (for rockets)

## Tips & Tricks

### For Best Results:
1. **Use OpenSky Network** for real aircraft data
2. Set your **actual GPS location** for accurate proximity alerts
3. **Zoom in** (mouse wheel) to see nearby planes better
4. **Check at airports** for high traffic
5. **Monitor launch sites** for rocket activities

### Understanding Data Updates:
- **Aircraft**: Updates every 5 seconds
- **Rockets**: Updates every 5 minutes
- **Simulated**: Generates random data each update

### Troubleshooting:

**No aircraft showing?**
- Switch to **Simulated** mode first to test
- Check your internet connection for OpenSky Network
- Try a location near a major airport

**No rockets showing?**
- Click **"🔄 Refresh Launches"** button
- May take a moment to load from API
- Check your internet connection

**Map looks weird?**
- Try zooming in/out with mouse wheel
- Check that latitude/longitude are correct
- Latitude: -90 to 90, Longitude: -180 to 180

## Advanced: Real ADS-B Reception

Want to track planes with your own antenna?

### Hardware ($25-35):
- RTL-SDR USB dongle
- 1090 MHz antenna

### Software:
```bash
sudo apt install dump1090-mutability
dump1090 --net --interactive
```

Then select **"📡 Local Dump1090"** in the app!

## Data Sources & APIs

### OpenSky Network
- **Website**: https://opensky-network.org/
- **Free**: No API key needed
- **Coverage**: Worldwide
- **Data**: Real-time ADS-B from aircraft transponders

### Launch Library 2
- **Website**: https://thespacedevs.com/
- **Free**: No API key needed
- **Coverage**: All space agencies (SpaceX, NASA, ULA, Roscosmos, etc.)
- **Data**: Launch schedules, countdowns, locations

## Building From Source

If you need to rebuild:

```bash
cd /home/zeshan/rocket
./compile_direct.sh
```

Requirements:
- C++17 compiler (g++)
- Qt6 (or Qt5) development libraries
- CMake (optional, not required for compile_direct.sh)

## What's Next?

Try these experiments:

1. **Compare Simulated vs Real**: See how realistic the simulation is!
2. **Track a Launch**: Set location near Cape Canaveral and watch the countdown
3. **Airport Traffic**: Check a major airport at different times of day
4. **Cross-Country**: Track planes over different regions
5. **Physics Experiments**: Calculate closure rates, intercept angles

## Need Help?

- Check the full **README.md** for detailed information
- Review the source code in `src/` directory
- All physics formulas are in `src/geoutils.h`

---

**Have fun tracking! ✈️🚀**

Remember: You're seeing real physics in action - every distance, bearing, and velocity calculation is using the same math that aviation and space programs use!

