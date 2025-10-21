# 🛩️ Aircraft & Rocket Tracker 🚀

A real-time C++ GUI application for tracking aircraft via ADS-B and monitoring rocket launches worldwide. Uses **Physics 1A/2B** concepts including kinematics, vectors, coordinate systems, and distance calculations.

![Version](https://img.shields.io/badge/version-1.0-blue)
![C++](https://img.shields.io/badge/C%2B%2B-17-green)
![Qt](https://img.shields.io/badge/Qt-5%2F6-brightgreen)

## ✨ Features

### ✈️ Aircraft Tracking (ADS-B)
- **Real-time aircraft positions** using ADS-B transponder data
- **Multiple data sources**:
  - 🌐 **OpenSky Network API** (real live data, no API key needed!)
  - 📡 **Local dump1090** (if you have RTL-SDR hardware)
  - 🔴 **Simulated mode** for testing
- **Distance & bearing calculations** from your GPS location
- **Proximity alerts** when aircraft enter your defined range
- Visual heading indicators and altitude display

### 🚀 Rocket Launch Tracking
- **Upcoming launches** from around the world
- Uses **Launch Library 2 API** (real data, free!)
- **Live countdowns** (T-minus format)
- Launch site locations (Cape Canaveral, Vandenberg, Baikonur, etc.)
- Distance to launch sites from your location
- **Launch alerts** for nearby launches

### 📍 GPS & Physics Features
- Set your own GPS coordinates
- **Haversine formula** for great-circle distances
- **Bearing calculations** (compass direction to targets)
- **3D distance** calculations including altitude
- **Velocity vectors** and heading displays
- Physics-based proximity detection

## 🔬 Physics Concepts Used

This application demonstrates real-world physics from **Physics 1A and 2B**:

### Physics 1A Concepts:
- **Kinematics**: Velocity, acceleration, position tracking
- **Vectors**: 2D/3D position vectors, velocity vectors, bearing
- **Coordinate Systems**: GPS coordinates (spherical), Cartesian conversion
- **Distance Formula**: Great-circle distance on Earth's surface

### Physics 2B Concepts:
- **Spherical Geometry**: Earth as a sphere, arc length calculations
- **Trigonometry**: Haversine formula, bearing angles
- **Reference Frames**: Earth-centered coordinate system
- **Doppler Effect**: (Future: relative velocity calculations)

## 🛠️ Building the Project

### Prerequisites

#### Ubuntu/Debian:
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-positioning-dev
# Or for Qt5:
sudo apt install build-essential cmake qtbase5-dev qtpositioning5-dev
```

#### Arch Linux:
```bash
sudo pacman -S base-devel cmake qt6-base qt6-positioning
# Or for Qt5:
sudo pacman -S base-devel cmake qt5-base qt5-location
```

#### Fedora:
```bash
sudo dnf install gcc-c++ cmake qt6-qtbase-devel qt6-qtpositioning-devel
```

### Build Instructions

```bash
# Clone or navigate to the project directory
cd /home/zeshan/rocket

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build
make -j$(nproc)

# Run the application
./AircraftRocketTracker
```

## 🎮 Usage

### Initial Setup:
1. **Set Your Location**: Enter your GPS coordinates (latitude, longitude)
   - Default: Los Angeles (34.0522°, -118.2437°)
   - Find your coordinates: Use Google Maps (right-click → coordinates)

2. **Choose Data Sources**:
   - **Aircraft**: Start with "Simulated" to test, then try "OpenSky Network" for real data
   - **Rockets**: "Launch Library 2" for real upcoming launches

3. **Set Alert Distance**: Default 10 km - you'll get notifications when aircraft/rockets are within this range

### Controls:
- **Mouse Wheel**: Zoom in/out on map
- **Click Aircraft**: Select to highlight
- **Update Location**: Apply new GPS coordinates
- **Refresh Launches**: Force update rocket launch data

### Display Information:

#### Aircraft List Shows:
- Callsign / ICAO code
- Distance from you (km)
- Bearing (degrees, 0° = North)
- Altitude (meters)
- Velocity (m/s)

#### Rocket List Shows:
- Mission name & launch vehicle
- Countdown timer (T-minus format)
- Distance to launch site (km)
- Launch location

#### Map Colors:
- **🟡 Yellow**: Your location
- **🔴 Red**: Aircraft very close (< 5 km)
- **🟠 Orange**: Aircraft nearby (5-15 km)
- **🔵 Cyan**: Aircraft far (> 15 km)
- **🚀 Red Rocket**: Launch imminent (< 1 hour)
- **🚀 Orange Rocket**: Launch today
- **🚀 Green Rocket**: Upcoming launch

## 📡 Real ADS-B Reception (Optional)

To receive **real live aircraft data** from your own antenna:

### Hardware Needed:
- RTL-SDR dongle (~$25-35)
- 1090 MHz antenna

### Software Setup:
```bash
# Install dump1090
sudo apt install dump1090-mutability
# Or use dump1090-fa (FlightAware fork)

# Run dump1090
dump1090 --net --interactive

# Then in the app, select "Local Dump1090" as data source
```

## 🌐 Data Sources

### OpenSky Network
- **URL**: https://opensky-network.org/
- **Free tier**: No API key required
- **Coverage**: Worldwide aircraft tracking
- **Update rate**: ~10 seconds

### Launch Library 2
- **URL**: https://thespacedevs.com/
- **Free tier**: No API key required
- **Data**: Upcoming launches from all providers (SpaceX, NASA, ULA, etc.)
- **Update rate**: Real-time countdown

## 🔔 Proximity Alerts

The app will show popup notifications when:
1. **Aircraft enters your alert range** (default 10 km)
2. **Rocket launch imminent** (< 1 hour) and within 200 km

Alert includes:
- Callsign/Mission name
- Distance from you
- Bearing (direction)
- Altitude (for aircraft)
- Countdown (for rockets)

## 📊 Technical Details

### Physics Calculations:

**Distance (Haversine Formula)**:
```
a = sin²(Δφ/2) + cos(φ1)⋅cos(φ2)⋅sin²(Δλ/2)
c = 2⋅atan2(√a, √(1−a))
d = R⋅c
```
Where φ = latitude, λ = longitude, R = Earth radius

**Bearing**:
```
θ = atan2(sin(Δλ)⋅cos(φ2), cos(φ1)⋅sin(φ2) − sin(φ1)⋅cos(φ2)⋅cos(Δλ))
```

**3D Distance**:
```
d = √(d_horizontal² + d_vertical²)
```

### Architecture:
- **Qt Framework**: Cross-platform GUI
- **Qt Network**: HTTP requests for APIs
- **Qt Positioning**: GPS coordinate handling
- **Custom Physics Engine**: All distance/bearing calculations
- **Real-time Updates**: Separate threads for aircraft/rocket data

## 🎓 Educational Value

Perfect for learning:
- **Applied Physics**: Real-world kinematics and coordinate systems
- **C++ Programming**: Modern C++17, Qt framework
- **Network Programming**: REST APIs, JSON parsing
- **GUI Development**: Event-driven programming
- **Aviation/Space**: ADS-B protocol, orbital mechanics

## 🚀 Future Enhancements

Potential features to add:
- [ ] Flight path prediction
- [ ] Historical flight tracking
- [ ] Satellite tracking (ISS, Starlink)
- [ ] Weather data integration
- [ ] Sound alerts for proximity
- [ ] Export flight data to CSV
- [ ] 3D visualization
- [ ] Multiple user locations (team tracking)

## 📝 Known Limitations

- **OpenSky Network**: Rate-limited to ~100 requests/day on free tier
- **GPS Location**: Manual entry (could add automatic GPS detection)
- **Map Projection**: Simple equirectangular (good for local areas)
- **No terrain data**: Assumes flat Earth locally

## 🤝 Contributing

This is an educational project. Feel free to:
- Add new features
- Improve physics calculations
- Add more data sources
- Enhance visualizations

## 📄 License

Open source educational project. Use freely for learning and non-commercial purposes.

## 🙏 Credits

- **OpenSky Network**: Free ADS-B data
- **The Space Devs**: Launch Library API
- **Qt Project**: GUI framework

---



