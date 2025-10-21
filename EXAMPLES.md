# 📝 Example Scenarios - Aircraft & Rocket Tracker

## Scenario 1: Track SpaceX Launch from Florida

**Setup:**
```
Location: Miami, Florida
Latitude: 25.7617
Longitude: -80.1918
Alert Distance: 100 km
```

**Data Sources:**
- Aircraft: OpenSky Network (Real)
- Rockets: Launch Library 2 (Real)

**What You'll See:**
- 🚀 **Cape Canaveral launches** ~280 km north
- ✈️ **Miami International Airport** traffic overhead
- Commercial flights along the East Coast
- Countdown to next Falcon 9 or Starship launch

**Physics Observed:**
- Great-circle distance to launch pad
- Bearing: ~350° (North-Northwest to Cape)
- Aircraft at typical cruise altitude (10-12 km)
- Velocity calculations for supersonic climbs

---

## Scenario 2: Watch Busy Airport Traffic

**Setup:**
```
Location: Los Angeles International (LAX)
Latitude: 33.9416
Longitude: -118.4085
Alert Distance: 15 km
```

**Data Sources:**
- Aircraft: OpenSky Network (Real)
- Rockets: Launch Library 2 (Real)

**What You'll See:**
- 10-30 aircraft in range at any time
- Landing patterns (descending aircraft)
- Takeoff patterns (ascending aircraft)
- Different headings based on runway usage

**Physics Observed:**
- Velocity changes during approach (250 m/s → 70 m/s)
- Altitude decrease on landing (12000m → 0m)
- 3D distance calculations
- Closure rates (aircraft approaching you)

**Try This:**
- Compare **8 AM** (morning rush) vs **2 AM** (quiet)
- Watch the **approach patterns**
- Note the **altitude changes**

---

## Scenario 3: Vandenberg Launch + Air Traffic

**Setup:**
```
Location: Santa Barbara, California
Latitude: 34.4208
Longitude: -119.6982
Alert Distance: 50 km
```

**Data Sources:**
- Aircraft: OpenSky Network (Real)
- Rockets: Launch Library 2 (Real)

**What You'll See:**
- 🚀 **Vandenberg Space Force Base** launches ~60 km north
- Aircraft on Pacific coast routes
- Possible launch viewing distance
- Aircraft diversions during launches

**Physics Applications:**
- Calculate optimal viewing distance
- Sound delay: distance ÷ 343 m/s
- Horizon visibility: d = √(2×R×h)

---

## Scenario 4: European Airspace

**Setup:**
```
Location: London Heathrow
Latitude: 51.4700
Longitude: -0.4543
Alert Distance: 25 km
```

**Data Sources:**
- Aircraft: OpenSky Network (Real)
- Rockets: Launch Library 2 (Real)

**What You'll See:**
- One of the **busiest airports** in the world
- 20-40+ aircraft typically in range
- Continuous stream of arrivals/departures
- International flights from all directions

**Physics Challenges:**
- Higher latitude = different spherical geometry
- Bearing calculations more complex
- Great-circle routes converge at poles

---

## Scenario 5: Test Simulated Mode

**Setup:**
```
Location: Anywhere you want!
Latitude: YOUR_CHOICE
Longitude: YOUR_CHOICE  
Alert Distance: 10 km
```

**Data Sources:**
- Aircraft: Simulated (Demo)
- Rockets: Simulated (Demo)

**What You'll See:**
- 3-8 randomly generated aircraft
- 5 simulated rocket launches
- Realistic parameters (altitude, velocity)
- Perfect for **testing without internet**

**Educational Use:**
- Understand the **UI** without distractions
- Verify **physics calculations**
- Test **proximity alerts**
- Learn the **interface**

---

## Scenario 6: Track Starship from Texas

**Setup:**
```
Location: Brownsville, Texas
Latitude: 25.9017
Longitude: -97.4975
Alert Distance: 20 km
```

**Data Sources:**
- Aircraft: OpenSky Network (Real)
- Rockets: Launch Library 2 (Real)

**What You'll See:**
- 🚀 **SpaceX Starbase** just ~8 km away!
- Starship/Super Heavy test flights
- Very close proximity alerts
- Border patrol and aircraft monitoring launches

**Special Notes:**
- Extremely close to launch site
- Sound will be LOUD if you're actually there!
- Flight restrictions during launches

---

## Physics Experiments

### Experiment 1: Distance Verification

**Goal:** Verify Haversine formula accuracy

**Method:**
1. Set location to **New York** (40.7128, -74.0060)
2. Note distance to aircraft over **Los Angeles** (should be ~4,000 km)
3. Calculate manually: d = 2R × arcsin(√...)
4. Compare with displayed value

### Experiment 2: Bearing Accuracy

**Goal:** Understand bearing angles

**Method:**
1. Place yourself at origin (0°, 0°)
2. Aircraft due North → bearing = 0°
3. Aircraft due East → bearing = 90°
4. Aircraft due South → bearing = 180°
5. Aircraft due West → bearing = 270°

### Experiment 3: Velocity Analysis

**Goal:** Compare aircraft speeds

**Setup:**
- Location: Major airport
- Data: OpenSky Network (Real)

**Observe:**
- **Cruise**: 200-250 m/s (450-560 mph)
- **Approach**: 70-100 m/s (155-225 mph)
- **Ground**: 0-20 m/s (taxiing)

**Physics:**
- v = 250 m/s ≈ Mach 0.73 (subsonic)
- Supersonic: v > 343 m/s

### Experiment 4: 3D Distance

**Goal:** Understand altitude's effect on distance

**Method:**
1. Find aircraft directly overhead
2. Horizontal distance ≈ 0 km
3. Altitude = 10 km
4. Total 3D distance = √(0² + 10²) = 10 km

**Try:**
- Aircraft 10 km away horizontally, 10 km altitude
- 3D distance = √(10² + 10²) = 14.14 km

### Experiment 5: Closure Rate

**Goal:** Calculate relative velocity

**Method:**
1. Note aircraft distance at time t₁
2. Note aircraft distance at time t₂ (5 seconds later)
3. Closure rate = (d₁ - d₂) / Δt

**Example:**
- t₁: 50 km away
- t₂: 48.75 km away (5 sec later)
- Rate = (50 - 48.75) / 5 = 0.25 km/s = 250 m/s

---

## Advanced Challenges

### Challenge 1: Intercept Calculation

**Problem:** If you launch to intercept an aircraft 100 km away traveling at 250 m/s, how fast must you go to intercept in 2 minutes?

**Given:**
- Distance: 100 km = 100,000 m
- Time: 2 min = 120 s
- Target velocity: 250 m/s

**Solution:**
- Required velocity: v = d/t = 100,000/120 ≈ 833 m/s (Mach 2.4!)

### Challenge 2: Launch Window

**Problem:** A rocket launch is in 4 hours and 200 km away. If you drive at 100 km/h, will you make it?

**Solution:**
- Time needed: 200 km ÷ 100 km/h = 2 hours
- Time available: 4 hours
- ✅ Yes! With 2 hours to spare

### Challenge 3: Horizon Distance

**Problem:** At what distance does an aircraft at 10 km altitude disappear below your horizon?

**Formula:** d = √(2Rh) where R = 6,371 km, h = 10 km

**Solution:**
- d = √(2 × 6371 × 10) = √127,420 ≈ 357 km

**Verify:** Watch aircraft at 350+ km in the app!

---

## Real-World Applications

### Air Traffic Control
- Track multiple aircraft simultaneously
- Calculate separation distances
- Monitor approach patterns
- Verify safe spacing (5+ km vertical, 10+ km horizontal)

### Launch Viewing
- Calculate optimal viewing distance
- Estimate sound arrival time
- Check for aircraft conflicts
- Verify weather conditions

### Aviation Enthusiasts
- Identify aircraft by callsign
- Track specific flights
- Monitor airport traffic
- Analyze flight patterns

### Education
- Demonstrate physics concepts
- Real-time kinematics
- Spherical geometry
- Coordinate systems

---

## Tips for Each Scenario

### For Airport Tracking:
- Best times: 6-9 AM, 4-7 PM (rush hours)
- Weekend traffic typically lighter
- International airports have 24/7 traffic

### For Launch Tracking:
- Check schedules: https://thespacedevs.com
- Launch windows are specific times
- Weather often causes delays
- Scrubs are common (be patient!)

### For Simulated Mode:
- Perfect for learning the interface
- No internet required
- Consistent for testing
- Great for demonstrations

---

## Troubleshooting Scenarios

### No Aircraft Showing (OpenSky)?
1. Check internet connection
2. Try Simulated mode first
3. Verify location is correct
4. Try a major airport location
5. OpenSky may have rate limits

### Wrong Distances?
1. Verify latitude/longitude format
2. Check for decimal degrees (not DMS)
3. Latitude: -90 to 90
4. Longitude: -180 to 180

### Performance Issues?
1. Reduce alert distance
2. Close other applications
3. Zoom out to see fewer aircraft
4. Check CPU usage

---

**Have fun exploring! Each scenario teaches different physics concepts and real-world applications!** 🛩️🚀

