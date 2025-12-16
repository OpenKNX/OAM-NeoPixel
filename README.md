# OAM-NeoPixel: KNX-Controlled Addressable LED Adapter

A powerful OpenKNX firmware module for controlling addressable LED strips (WS2812B, APA102, SK6812, and many others) via KNX bus. Supports segmentation, effects, color temperature, brightness control, and state persistence.

## Features

### Core Functionality
- **Multi-Strip Support**: Control up to 6 independent LED strips simultaneously
- **Flexible Segmentation**: Divide each strip into segments for granular control
- **Rich Effect Library**: Built-in effects including fade, rainbow, pulse, sparkle, and more
- **Effect Auto-Update**: Automatic animation with configurable update speeds
- **Color Profiles**: RGB, HSV, RGBW, warm/cool white (WW/CW) support

### Control Methods
- **KNX Group Objects**: Full control via KNX telegrams
- **Per-Segment KOs**: Independent control of each segment's color, brightness, effect, and power
- **Global Controls**: Global brightness, power, and HCL (Hue-Color-Light) state
- **Relative Controls**: Dimming steps via DPT 3.007 (Control_Dimming) for incremental brightness/color changes

### Color & Lighting
- **RGB Direct Control**: Set exact RGB values
- **HSV Control**: Hue, Saturation, Value for intuitive color selection
- **Color Temperature (CCT)**: Kelvin-based warm/cool white adjustment
- **White Channels**: Dedicated warm white (WW) and cool white (CW) control for RGBW strips
- **Brightness Scaling**: Per-segment brightness with DPT 5.001 (percentage 0–100%)
- **Gamma Correction**: Configurable gamma curve for natural brightness perception
- **White Balance Correction**: Fine-tune color channel intensity

### Advanced Features
- **LED Protocol Support**: WS2812B, WS2811, SK6812, SK6805, APA102, SK9822, WS2801, LPD8806, and more
- **Color Order Configuration**: Automatic and manual color order selection (RGB, GRB, BGR, RGBW, etc.)
- **Timing Modes**: 11 adjustable timing presets (AUTO, FAST, SLOW) to optimize signal integrity for different cable lengths and LED types
- **Power Limiting**: Global and per-channel current limiting to prevent brownouts
- **Hardware Flexibility**: GPIO and SPI clock configuration for custom wiring
- **Virtual Strip Architecture**: Internal virtual strip allows flexible physical strip ordering and rearrangement

## Installation & Setup

### 1. Clone the Repository
```bash
git clone --recursive https://github.com/OpenKNX/OAM-Neopixel.git
cd OAM-Neopixel
```

### 2. Configure PlatformIO
Edit `platformio.ini` to select your board:
```ini
[env:develop_OpenKNXiaoMiniRP2040_TP]
board = seeed_xiao_rp2040
```

### 3. Build & Upload
```bash
pio run -e develop_OpenKNXiaoMiniRP2040_TP -t upload
```

### 4. Configure in ETS
- **LED Type**: Select your LED protocol (WS2812B, APA102, etc.)
- **LED Count**: Enter total number of addressable LEDs
- **Color Order**: Match your LED's byte order (GRB for WS2812B, BGR for APA102, etc.)
- **Timing Mode**: Start with AUTO; switch to SLOW_5PCT–SLOW_20PCT if LEDs flicker or don't respond
- **GPIO Configuration**: Set data pin, clock pin (for SPI), MOSI pin
- **Power Limiting**: Enable if drawing >1A; set appropriate mA limits per LED and globally
- **Segments**: Define segment start/end positions for granular control
- **Effects**: Assign effects to segments with speed and intensity parameters

## Configuration Parameters

### Strip Level
| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| LED Type | Enum | 0–10 | 0 (WS2812B) | Addressable LED protocol |
| Color Order | Enum | 0–7 | 0 (GRB) | Byte sequence for RGB channels |
| LED Count | Int | 1–1000 | 30 | Total addressable LEDs |
| Skip First LEDs | Int | 0–100 | 0 | Offset before segment 0 starts |
| Timing Mode | Enum | 0–10 | 0 (AUTO) | Signal timing optimization |
| Power Limit (mA) | Int | 100–5000 | 1000 | Global current limit |
| Gamma Correction | Bool | — | On | Apply gamma curve (2.2) |
| White Balance | Bool | — | Off | Adjust per-channel intensity |

### Segment Level
| Parameter | Type | Description |
|-----------|------|-------------|
| Segment Start | Int | LED index where segment begins |
| Segment End | Int | LED index where segment ends (inclusive) |
| Default Effect | Enum | Effect ID to start with (0=static) |
| Effect Speed | Int | Animation frame rate (0–255) |
| Effect Intensity | Int | Animation intensity/brightness (0–255) |
| Grouping | Int | LEDs per logical group (1=per-LED) |
| Spacing | Int | Dark LEDs between groups |

## Group Objects (KOs)

### Global KOs
- **Power** (KO 600): DPT 1.001 — Turn all segments on/off
- **Brightness** (KO 601): DPT 5.001 — Global brightness 0–100%
- **HCL State** (KO 602): DPT 7.600 — Color temperature 0–65535 K

### Per-Segment KOs (Block Size 39)
For segment N (0-indexed), KOs = 600 + N×39:
- **KO+0**: RGB (DPT_Colour_RGB)
- **KO+1**: HSV (DPT_Colour_RGB, H/S/V packed)
- **KO+2**: Warm White (DPT 5.010, 0–255)
- **KO+3**: Cool White (DPT 5.010, 0–255)
- **KO+4**: Color Temperature (DPT 5.010)
- **KO+5**: RGBW (DPT_Colour_RGBW)
- **KO+15**: RGB (DPT_Colour_RGB, direct set)
- **KO+16**: RGB Status (DPT_Colour_RGB)
- **KO+23**: Effect (DPT 5.010)
- **KO+24**: Effect Status (DPT_Colour_RGB)
- **KO+25**: Preset (DPT 5.010, 0–8)
- **KO+35**: Segment Power (DPT 1.001)
- **KO+36**: Segment Power Status (DPT 1.001)
- **KO+37**: Brightness (DPT 5.001, 0–100%)
- **KO+38**: Brightness Status (DPT 5.001)

**Note**: KO indices are offsets within each segment's 39-KO block. Actual KO number = 600 + segment_index×39 + index.

## Troubleshooting

### LEDs Don't Respond
1. Verify GPIO pins match ETS configuration
2. Check power supply (5V stable, sufficient current)
3. Try **Timing Mode = AUTO_LEGACY** in ETS
4. Inspect data cable for shorts or poor contacts
5. Reduce cable length or add a level shifter (3.3V→5V)

### LEDs Flicker or Show Wrong Colors
1. Switch **Timing Mode** from AUTO to SLOW_5PCT–SLOW_20PCT
2. Check **Color Order** matches LED type (GRB for WS2812B, BGR for APA102)
3. Reduce **LED Count** to isolate the problem
4. Verify ground connection is solid

### Brightness Control Has No Effect
1. Confirm brightness KO is **linked in ETS** to a group address
2. Check that the group address transmits **DPT 5.001** (0–100%), not DPT 5.010
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
- **Community**: Join OpenKNX forum for discussions

## Changelog