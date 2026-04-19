# OAM-NeoPixel: KNX-Controlled Addressable LED Adapter

A powerful OpenKNX firmware module for controlling addressable LED strips (WS2812B, APA102, SK6812, and many others) via KNX bus. Supports segmentation, effects, scenes, color temperature, HCL (Human Centric Lighting), brightness control, and runtime state persistence.

## Features

### Core Functionality
- **Multi-Strip Support**: Control up to 6 independent LED strips simultaneously
- **Flexible Segmentation**: Up to 16 segments per strip for granular control
- **Rich Effect Library**: 29 built-in effects including Rainbow, Fire, Meteor, Breathing, Sparkle, Comet, and more
- **Scene Support**: Up to 10 configurable scenes per segment (DPT 18.001), storing effect, colors, and brightness
- **Color Profiles**: RGB, HSV, RGBW, RGBCCT (5-channel), warm/cool white (WW/CW) support

### Control Methods
- **KNX Group Objects**: Full control via KNX telegrams
- **Per-Segment KOs**: Independent control of each segment's color, brightness, effect, scene, and power
- **Global Controls**: Global brightness, power, and HCL (Human Centric Lighting) state
- **Relative Controls**: Dimming steps via DPT 3.007 (Control_Dimming) for incremental brightness/color changes

### Color & Lighting
- **RGB Direct Control**: Set exact RGB values
- **HSV Control**: Hue, Saturation, Value for intuitive color selection
- **Color Temperature (CCT)**: Kelvin-based warm/cool white adjustment (DPT 7.600)
- **RGBCCT (5-Channel)**: Full support for 5-channel LED strips with separate Warm White (WW) and Cool White (CW) channels for true color temperature control
- **HCL (Human Centric Lighting)**: Automatic color temperature adjustment with sun-position curves or time-based scheduling for circadian rhythm support
  - For RGBCCT strips: Directly adjusts WW/CW ratio for pure white color temperature
  - For RGB/RGBW strips: Applies Kelvin-based RGB tinting
  - Configurable globally and per segment
- **White Channels**: Dedicated warm white (WW) and cool white (CW) control for RGBW and RGBCCT strips
- **Brightness Scaling**: Per-segment brightness with DPT 5.001 (percentage 0-100%)
- **Gamma Correction**: Configurable gamma curve (1.2-2.7, default 2.0)
- **White Balance Correction**: Per-channel intensity adjustment (R/G/B, default 100% each)

### Advanced Features
- **LED Protocol Support**: 28+ protocols including WS2812B, WS2811, WS2813, SK6812, APA102, SK9822, WS2801, LPD8806, TM1814, WS2805 RGBCCT, WS2814 RGBCCT, and more
- **Color Order Configuration**: 12 color orders including RGB, GRB, BGR, RGBW, GRBW, RGBCCT, GRBCCT, RGBCTW, GRBCTW
- **Timing Modes**: 11 adjustable timing presets (AUTO, AUTO_LEGACY, SLOW_5PCT-SLOW_20PCT, FAST_5PCT-FAST_25PCT) to optimize signal integrity for different cable lengths and LED types
- **Power Limiting**: Multiple modes (disabled, global, custom-fixed, custom-per-LED) with up to 65535 mA global limit
- **External Relay Control**: Up to 4 relay outputs (hardware-dependent)
- **Hardware Flexibility**: GPIO and SPI clock configuration for custom wiring
- **Virtual Strip Architecture**: Internal virtual strip allows flexible physical strip ordering and rearrangement
- **Power Monitoring**: Total current (mA), load (%), and power (W) status KOs

## Effects

| ID | Effect | ID | Effect |
|----|--------|----|--------|
| 0 | Solid | 15 | Twinkle |
| 1 | Wipe | 16 | Sparkle |
| 2 | Rainbow | 17 | Breathing |
| 3 | Rainbow Cycle | 18 | Strobe |
| 4 | Pride2015 | 19 | Pulse |
| 5 | Confetti | 20 | Comet |
| 6 | Juggle | 21 | Meteor |
| 7 | BPM | 22 | Noise |
| 8 | Cylon | 23 | Palette |
| 9 | RGBWTest | 24 | Blitz |
| 10 | GarageDoor | 25 | Gradient |
| 11 | Fire | 26 | RGBCCTTest |
| 12 | Theater Chase | 27 | Kerze |
| 13 | Theater Chase Rainbow | 28 | Kerzen Multi |
| 14 | Sinelon | | |

## Installation & Setup

### Prerequisites
Before building this project, your development environment must be set up according to the [OpenKNX Developer Setup](https://github.com/OpenKNX/OpenKNX/wiki/Information-for-Developers). This includes PlatformIO, required toolchains, and the OpenKNX build system.

### 1. Clone the Repository
```bash
git clone --recursive https://github.com/OpenKNX/OAM-NeoPixel.git
cd OAM-NeoPixel
```

### 2. Configure PlatformIO
Select your target hardware environment in `platformio.custom.ini`. Available environments include:

| Environment | Hardware |
|-------------|----------|
| `release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1` | KNeoPix RP2040 |
| `release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V1` | KNeoPix RP2350 |
| `release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V1` | KNeoPix ESP32-S3 |
| `release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C3_V1` | KNeoPix ESP32-C3 |
| `release_OKNXHW_OPENKNXIAO_KNEOPIX_ESP32C6_V1` | KNeoPix ESP32-C6 |
| `release_OKNXHW_OPENKNXIAO_RP2040_MINI_V1` | Mini RP2040 |
| `release_OKNXHW_OPENKNXIAO_RP2350_MINI_V1` | Mini RP2350 |
| `release_OKNXHW_OPENKNXIAO_ESP32S3_MINI_V1` | Mini ESP32-S3 |
| `release_OKNXHW_OPENKNXIAO_ESP32C3_MINI_V1` | Mini ESP32-C3 |
| `release_OKNXHW_OPENKNXIAO_ESP32C6_MINI_V1` | Mini ESP32-C6 |

### 3. Build & Upload
```bash
pio run -e release_OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1 -t upload
```

### 4. Configure in ETS
- **LED Type**: Select your LED protocol (WS2812B, APA102, etc.)
- **LED Count**: Enter total number of addressable LEDs (max 16384)
- **Color Order**: Match your LED's byte order (GRB for WS2812B, BGR for APA102, etc.)
- **Timing Mode**: Start with AUTO; switch to SLOW_5PCT-SLOW_20PCT if LEDs flicker or don't respond
- **GPIO Configuration**: Set data pin, clock pin (for SPI), MOSI pin
- **Power Limiting**: Configure mode (global, per-LED) and mA limits
- **Segments**: Define segment start/end positions for granular control
- **Effects**: Assign effects to segments with speed and intensity parameters
- **Scenes**: Configure up to 10 scenes per segment with colors, brightness, and effects

## Configuration Parameters

### Strip Level
| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| LED Type | Enum | 0-31, 99 | 0 (WS2812B) | Addressable LED protocol |
| Color Order | Enum | 0-11 | 0 (GRB) | Byte sequence for color channels |
| LED Count | Int | 1-16384 | 30 | Total addressable LEDs |
| Skip First LEDs | Int | 0-65535 | 0 | Offset before segment 0 starts |
| Timing Mode | Enum | 0-10 | 0 (AUTO) | Signal timing optimization |
| Power Limit Mode | Enum | 0-3 | 1 (Global) | Disabled / Global / Custom-Fixed / Custom-Per-LED |
| Power Limit Global (mA) | Int | 0-65535 | 1000 | Global current limit |
| Power Limit Per LED (mA) | Int | 0-70 | 60 | Per-LED current limit |
| Gamma Correction | Enum | 1.2-2.7 | 2.0 | Gamma curve value |
| White Balance R/G/B | Int | 0-100% | 100% each | Per-channel intensity correction |

### Segment Level
| Parameter | Type | Description |
|-----------|------|-------------|
| Segment Start | Int | LED index where segment begins |
| Segment End | Int | LED index where segment ends (inclusive) |
| Default Effect | Enum | Effect ID to start with (0=Solid) |
| Effect Speed | Int | Animation frame rate (0-255) |
| Effect Intensity | Int | Animation intensity/brightness (0-255) |
| Grouping | Int | LEDs per logical group (1=per-LED) |
| Spacing | Int | Dark LEDs between groups |
| Scenes | 1-10 | Configurable scenes with effect, colors, brightness |
| Startup Behavior | Enum | Use Global / Off / Last State |

## Group Objects (KOs)

### Global KOs
| KO | Name | DPT | Direction | Description |
|----|------|-----|-----------|-------------|
| 400 | Power | 1.001 | Input | Turn all segments on/off |
| 401 | Power Status | 1.011 | Output | Power state feedback |
| 402 | Brightness | 5.001 | Input | Global brightness 0-100% |
| 403 | Brightness Status | 5.001 | Output | Brightness state feedback |
| 404 | Current Total | 7.600 | Output | Total current consumption (mA) |
| 405 | Load Total | 5.001 | Output | Total power load (%) |
| 406 | Power Watts Total | 14.056 | Output | Total power consumption (W) |
| 407 | HCL Global State | 7.600 | Output | HCL color temperature state (K) |
| 408-411 | Relay 1-4 | 1.001 | Input | External relay control |
| 412-415 | Relay 1-4 Status | 1.011 | Output | External relay state feedback |

### Per-Segment KOs (Block Size 40)
For segment N (1-indexed), base KO = 600 + (N-1) x 40:

| Offset | Name | DPT | Direction | Description |
|--------|------|-----|-----------|-------------|
| +0 | Power | 1.001 | Input | Segment on/off |
| +1 | Power Status | 1.011 | Output | Power state feedback |
| +2 | Brightness | 5.001 | Input | Brightness 0-100% |
| +3 | Brightness Status | 5.001 | Output | Brightness feedback |
| +4 | R | 5.010 | Input | Red channel (0-255) |
| +5 | G | 5.010 | Input | Green channel (0-255) |
| +6 | B | 5.010 | Input | Blue channel (0-255) |
| +7 | W | 5.010 | Input | White channel (0-255) |
| +8 | CCT | 7.600 | Input | Color temperature (Kelvin) |
| +9 | CCT Status | 7.600 | Output | Color temperature feedback |
| +10 | WW | 5.010 | Input | Warm White (0-255) |
| +11 | CW | 5.010 | Input | Cool White (0-255) |
| +12 | H | 5.003 | Input | Hue (0-360) |
| +13 | S | 5.001 | Input | Saturation (0-100%) |
| +14 | V | 5.001 | Input | Value (0-100%) |
| +15 | Effect | 5.010 | Input | Effect ID |
| +16 | Effect Status | 5.010 | Output | Active effect feedback |
| +17 | Scene | 18.001 | Input | Scene recall/learn |
| +18 | Scene Status | 18.001 | Output | Active scene feedback |
| +19 | RGB | 232.600 | Input | RGB combined (3 bytes) |
| +20 | RGB Status | 232.600 | Output | RGB feedback |
| +21 | HSV | 232.600 | Input | HSV combined (3 bytes) |
| +22 | HSV Status | 232.600 | Output | HSV feedback |
| +23 | RGBW | 251.600 | Input | RGBW combined (6 bytes) |
| +24 | RGBW Status | 251.600 | Output | RGBW feedback |
| +25 | Brightness Rel | 3.007 | Input | Relative dimming |
| +26-28 | R/G/B Rel | 3.007 | Input | Relative R/G/B |
| +29 | W Rel | 3.007 | Input | Relative White |
| +30 | WW Rel | 3.007 | Input | Relative Warm White |
| +31 | CW Rel | 3.007 | Input | Relative Cool White |
| +32-34 | H/S/V Rel | 3.007 | Input | Relative H/S/V |
| +35 | Effect Rel | 3.007 | Input | Relative effect switch |
| +36 | RGB Rel | 232.600 | Input | Relative RGB |
| +37 | HSV Rel | 232.600 | Input | Relative HSV |
| +38 | RGBW Rel | 251.600 | Input | Relative RGBW |
| +39 | HCL State | 7.600 | Output | Segment HCL temperature (K) |

## State Persistence

The firmware persists the following segment state to flash memory for seamless restoration after power cycles:
- **Power state** (on/off)
- **Colors** (R, G, B, WW, CW values)
- **Brightness** (0-100%)
- **Effect** (if changed via KO at runtime)
- **Active scene number**

If the effect was not changed via KO during runtime, the ETS-configured default effect is used after restart.

**Startup Behavior** (configurable per segment, with a global default):

| Mode | Global | Per-Segment | Description |
|------|--------|-------------|-------------|
| Off | Yes | Yes | LEDs remain off after restart |
| Last State | Yes | Yes | Restore last persisted state |
| ETS Parameter | Yes | No | Use ETS-configured default values |
| Use Global | No | Yes (default) | Inherit global startup setting |

## Troubleshooting

### LEDs Don't Respond
1. Verify GPIO pins match ETS configuration
2. Check power supply (5V stable, sufficient current)
3. Try **Timing Mode = AUTO_LEGACY** in ETS
4. Inspect data cable for shorts or poor contacts
5. Reduce cable length or add a level shifter (3.3V to 5V)

### LEDs Flicker or Show Wrong Colors
1. Switch **Timing Mode** from AUTO to SLOW_5PCT-SLOW_20PCT
2. Check **Color Order** matches LED type (GRB for WS2812B, BGR for APA102)
3. Reduce **LED Count** to isolate the problem
4. Verify ground connection is solid

### Brightness Control Has No Effect
1. Confirm brightness KO is **linked in ETS** to a group address
2. Check that the group address transmits **DPT 5.001** (0-100%), not DPT 5.010
3. Verify segment power is **ON** (power KO = 1)

### Long Cables or Signal Issues
- Use **SLOW_10PCT** or **SLOW_15PCT** timing mode
- Add ferrite core or twisted pair shielding
- Consider a level shifter if cable length >3m
- Reduce LED count in problem area
- Split very long strips into multiple segments controlled separately

## License

Part of the OpenKNX ecosystem. See individual library licenses.

## Support & Contributing

For issues, feature requests, or contributions:
- **GitHub Issues**: Report bugs and request features
- **Documentation**: Check the Help folder for parameter descriptions
- **Community**: Join [OpenKNX forum](https://forum.openknx.de) for discussions
- **KNX-User-Forum Thread**: [OAM-NeoPixel -- In Farben dein Zuhause erstrahlen soll](https://knx-user-forum.de/forum/projektforen/openknx/2088552-oam-neopixel-%E2%80%93-in-farben-dein-zuhause-erstrahlen-soll)

## Supported Hardware

| Device | Platform | HW ID |
|--------|----------|-------|
| OpenKNXiao KNeoPix RP2040 V1 | RP2040 | 0x1300 |
| OpenKNXiao Mini RP2040 V1 | RP2040 | 0x1301 |
| OpenKNXiao KNeoPix RP2350 V1.4 | RP2350 | 0x1310 |
| OpenKNXiao KNeoPix ESP32S3 V1 | ESP32-S3 | 0x1320 |
| OpenKNXiao Mini ESP32S3 V1 | ESP32-S3 | 0x1321 |
| OpenKNXiao KNeoPix ESP32C6 V1 | ESP32-C6 | 0x1330 |
| OpenKNXiao KNeoPix ESP32C3 V1 | ESP32-C3 | 0x1350 |
| OpenKNXiao Mini ESP32C3 V1 | ESP32-C3 | 0x1351 |

> **Hinweis:** Die GPIO-Pins für die LED-Datenleitung lassen sich in der ETS frei konfigurieren. Dadurch ist die Firmware prinzipiell mit vielen weiteren ESP32- und RP2040/RP2350-basierten Boards kompatibel -- nicht nur mit den oben gelisteten OpenKNXiao-Varianten.

## Changelog
