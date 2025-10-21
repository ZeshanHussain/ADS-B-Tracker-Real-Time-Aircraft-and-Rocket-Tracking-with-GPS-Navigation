# 📡 SDR GPS Setup Guide - Offline GPS Navigation

## Overview

Use **RTL-SDR** hardware to receive GPS signals directly, enabling navigation even without internet or cellular service. Perfect for tactical operations in remote areas!

## Why SDR GPS?

### Advantages:
✅ **Works offline** - No internet/cellular required  
✅ **Independent** - Not reliant on external services  
✅ **Tactical** - Can't be tracked through GPS provider  
✅ **Cost-effective** - RTL-SDR dongles are $25-35  
✅ **Educational** - Learn GPS signal processing  

### Use Cases:
- Remote area navigation
- Backup GPS system
- GPS research and education
- Off-grid operations
- Privacy-focused navigation

## Hardware Required

### 1. RTL-SDR USB Dongle
**Recommended Models:**
- **RTL-SDR Blog V3** (~$30) - Best for GPS
- **NooElec NESDR Smart** (~$25)
- Any RTL2832U-based dongle

**Specs Needed:**
- Frequency range: 24 MHz - 1766 MHz (must include 1575 MHz L1 band)
- Sample rate: 2.4 MSPS or higher

### 2. GPS Antenna
**Options:**
- **Active GPS antenna** (~$15-30) - Recommended
  - Needs 3.3V-5V power (LNA powered)
  - SMA connector
  - 1575.42 MHz (L1 band)
- **Passive antenna** (~$5-10) - Works but weaker signal

### 3. Adapter
- **SMA to MCX/SMA adapter** if your RTL-SDR uses different connector

## Software Installation

### Step 1: Install RTL-SDR Drivers

#### Arch Linux:
```bash
sudo pacman -S rtl-sdr librtlsdr
```

#### Ubuntu/Debian:
```bash
sudo apt update
sudo apt install rtl-sdr librtlsdr-dev
```

#### Fedora:
```bash
sudo dnf install rtl-sdr
```

### Step 2: Blacklist DVB-T Drivers
The kernel DVB-T drivers interfere with RTL-SDR:

```bash
sudo nano /etc/modprobe.d/blacklist-rtl.conf
```

Add these lines:
```
blacklist dvb_usb_rtl28xxu
blacklist rtl2832
blacklist rtl2830
```

Reboot or run:
```bash
sudo rmmod dvb_usb_rtl28xxu
```

### Step 3: Test RTL-SDR
```bash
rtl_test
```

Should show: "Found Rafael Micro R820T tuner"

### Step 4: Install GPS Software

#### Option A: gpsd + gpsd-clients
```bash
# Arch
sudo pacman -S gpsd gpsd-clients

# Ubuntu/Debian  
sudo apt install gpsd gpsd-clients

# Fedora
sudo dnf install gpsd gpsd-clients
```

#### Option B: gnss-sdr (Advanced)
```bash
# Install dependencies
sudo apt install gnss-sdr

# Or build from source
git clone https://github.com/gnss-sdr/gnss-sdr
cd gnss-sdr
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

## GPS Signal Acquisition

### Method 1: Using gpsd with rtl_fm

**Start RTL-SDR GPS receiver:**
```bash
# Receive GPS L1 signal and pipe to gpsd
rtl_fm -f 1575.42M -s 2048000 -g 40 | gpsd -N -n /dev/stdin
```

**Test GPS data:**
```bash
# In another terminal
cgps -s
```

### Method 2: Using gnss-sdr

**Create config file** `gps_rtlsdr.conf`:
```ini
[GNSS-SDR]

SignalSource.implementation=Osmosdr_Signal_Source
SignalSource.item_type=gr_complex
SignalSource.sampling_frequency=2000000
SignalSource.freq=1575420000
SignalSource.gain=40
SignalSource.rf_gain=40
SignalSource.if_gain=30
SignalSource.AGC_enabled=false
SignalSource.samples=0
SignalSource.repeat=false
SignalSource.dump=false
SignalSource.dump_filename=./signal_source.dat
SignalSource.enable_throttle_control=false

DataTypeAdapter.implementation=Pass_Through

Acquisition_GPS.implementation=GPS_L1_CA_PCPS_Acquisition
Acquisition_GPS.item_type=gr_complex
Acquisition_GPS.threshold=0.008
Acquisition_GPS.doppler_max=10000
Acquisition_GPS.doppler_step=250

Tracking_GPS.implementation=GPS_L1_CA_DLL_PLL_Tracking
Tracking_GPS.item_type=gr_complex
Tracking_GPS.pll_bw_hz=40.0
Tracking_GPS.dll_bw_hz=4.0

TelemetryDecoder_GPS.implementation=GPS_L1_CA_Telemetry_Decoder
TelemetryDecoder_GPS.dump=false

Observables.implementation=Hybrid_Observables
Observables.dump=false

PVT.implementation=RTKLIB_PVT
PVT.positioning_mode=Single
PVT.output_rate_ms=100
PVT.display_rate_ms=500
PVT.nmea_dump_filename=./gnss_sdr_pvt.nmea
PVT.flag_nmea_tty_port=true
PVT.nmea_dump_devname=/dev/pts/4
```

**Run:**
```bash
gnss-sdr --config_file=gps_rtlsdr.conf
```

### Method 3: Using gr-gps (GNU Radio)

```bash
# Install GNU Radio
sudo apt install gnuradio

# Install gr-gps
git clone https://github.com/osqzss/gr-gps
cd gr-gps
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

## Integration with Your Application

### Connecting to gpsd

In `src/gpsnavigator.cpp`, add GPS reading:

```cpp
#include <gps.h>

void GPSNavigator::updateGPS() {
    if (m_gpsSource == SDR_RECEIVER) {
        struct gps_data_t gps_data;
        
        if (gps_open("localhost", "2947", &gps_data) == 0) {
            while (gps_waiting(&gps_data, 1000000)) {
                if (gps_read(&gps_data, NULL, 0) != -1) {
                    if (gps_data.status >= STATUS_GPS) {
                        m_currentLat = gps_data.fix.latitude;
                        m_currentLon = gps_data.fix.longitude;
                        m_currentAltitude = gps_data.fix.altitude;
                        m_currentSpeed = gps_data.fix.speed;
                        m_currentHeading = gps_data.fix.track;
                        m_satelliteCount = gps_data.satellites_visible;
                        m_hdop = gps_data.dop.hdop;
                        m_gpsLocked = true;
                    }
                }
            }
            gps_close(&gps_data);
        }
    }
}
```

### Compile with GPS support:
```bash
g++ ... -lgps ...
```

## Antenna Placement

### Best Performance:
1. **Outdoors** with clear sky view
2. **Window placement** - next to window
3. **Roof mounting** - optimal
4. **Avoid**: Metal structures, indoors without window

### Orientation:
- Active antenna: Ceramic patch facing up
- Keep antenna level
- Minimize cable length

## Troubleshooting

### No GPS Fix?

**1. Check signal strength:**
```bash
rtl_test -t
```

**2. Verify frequency:**
```bash
rtl_fm -f 1575.42M -s 2048000 -g 40 | aplay -r 48k -f S16_LE
```
Should hear noise/static

**3. Increase gain:**
```bash
rtl_fm -f 1575.42M -s 2048000 -g 49.6 ...
```

**4. Check antenna:**
- Is LNA powered? (3.3V on bias-tee)
- Cable connections tight?
- Antenna outdoors?

**5. Wait for satellite lock:**
- Cold start: 12+ minutes
- Warm start: 2-5 minutes
- Hot start: < 1 minute

### Poor Accuracy?

**Solutions:**
- More satellites visible = better accuracy
- Enable WAAS/EGNOS corrections
- Use differential GPS (DGPS)
- Increase averaging time

## Performance Expectations

### Acquisition Time:
- **Cold start**: 12-15 minutes
- **Warm start**: 2-5 minutes  
- **Hot start**: 30-60 seconds

### Accuracy:
- **Horizontal**: 5-15 meters (typical)
- **Vertical**: 10-30 meters
- **With WAAS**: 3-5 meters

### Number of Satellites:
- **Minimum for fix**: 4 satellites
- **Typical**: 6-10 satellites
- **Good conditions**: 12+ satellites

## Advanced: RTKLIB for cm-Accuracy

For survey-grade accuracy:

```bash
# Install RTKLIB
git clone https://github.com/tomojitakasu/RTKLIB
cd RTKLIB/app/consapp
make

# Use with correction service
./rtkrcv -o yourconfig.conf
```

## GPS Simulation for Testing

Without hardware, simulate GPS:

```bash
# Generate NMEA sentences
gpsfake -c 0.1 test.nmea
```

## Legal Notes

⚠️ **Important:**
- GPS reception is **legal worldwide** (receive-only)
- GPS jamming is **illegal** in most countries
- Transmitting on GPS frequencies is **prohibited**
- Check local regulations for antenna installation

## Cost Breakdown

| Item | Cost |
|------|------|
| RTL-SDR dongle | $25-35 |
| Active GPS antenna | $15-30 |
| SMA cable/adapter | $5-10 |
| **Total** | **$45-75** |

Compare to commercial GPS: $200-500+

## Resources

### Documentation:
- RTL-SDR Blog: https://www.rtl-sdr.com/
- GPS Tutorial: https://www.gps.gov/
- GNSS-SDR Docs: https://gnss-sdr.org/

### Software:
- gpsd: https://gpsd.gitlab.io/gpsd/
- GNSS-SDR: https://github.com/gnss-sdr/gnss-sdr
- RTKLIB: http://www.rtklib.com/

### Communities:
- r/RTLSDR on Reddit
- RTL-SDR Discord
- GNSS-SDR Google Group

## Next Steps

1. **Order hardware** (RTL-SDR + GPS antenna)
2. **Install software** (gpsd or GNSS-SDR)
3. **Test signal reception** (outdoors first)
4. **Integrate with app** (update GPSNavigator code)
5. **Field test** (drive/walk with system)

---

**Happy Navigation! 🛰️📡**

You now have a completely independent GPS system that works anywhere, anytime!


