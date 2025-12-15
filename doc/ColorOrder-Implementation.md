# ColorOrder Implementation Guide

## Overview

The NeoPixelBus module implements a sophisticated ColorOrder system that enables seamless integration of different LED hardware types with varying color channel arrangements. This system follows key design principles that simplify development while maximizing hardware compatibility.

## Key Features

### 🎯 **ColorOrder-Agnostic VirtualStrips**
- VirtualStrips always store RGB/RGBW internally
- No color conversion at the VirtualStrip layer
- Unified interface for all effect development

### 🔧 **Hardware-Specific PhysicalStrips**
- Each PhysicalStrip handles its own ColorOrder conversion
- Supports RGB, RBG, GRB, GBR, BGR, BRG, RGBW, GRBW
- Conversion happens once per frame for optimal performance

### 🌈 **Mixed Hardware Support**
- Combine WS2812B (GRB) + APA102 (BGR) + WS2811 (RGB) in one VirtualStrip
- Same logical colors appear correctly on all hardware types
- Effects work identically across different LED protocols

## ColorOrder Matrix

| ColorOrder | Description | Common Hardware | Channels |
|------------|-------------|-----------------|----------|
| `RGB` | Red, Green, Blue | WS2811, some APA102 configs | 3 |
| `RBG` | Red, Blue, Green | Some LED clones | 3 |
| `GRB` | Green, Red, Blue | **WS2812B, SK6812** (most common) | 3 |
| `GBR` | Green, Blue, Red | Some WS2812B clones | 3 |
| `BGR` | Blue, Green, Red | **APA102, SK9822** | 3 |
| `BRG` | Blue, Red, Green | Rare configurations | 3 |
| `RGBW` | Red, Green, Blue, White | SK6812 4-channel | 4 |
| `GRBW` | Green, Red, Blue, White | **SK6812 standard** | 4 |

## Protocol Support

### 1-Wire Protocols
- **WS2812/WS2812B**: 5V RGB, 800kHz, typically GRB order
- **WS2813**: 5V RGB, 800kHz, GRB, data backup line
- **WS2815**: 12V RGB, 800kHz, GRB, data backup
- **WS2811**: 12V RGB, 400kHz, typically RGB order
- **SK6812**: 5V/12V RGBW, 800kHz, typically GRBW order

### SPI Protocols  
- **APA102**: 5V RGB+Brightness, up to 20MHz, typically BGR order
- **SK9822**: 5V RGB+Brightness, up to 15MHz, BGR order (APA102 clone)
- **WS2801**: 5V RGB, up to 25MHz, typically RGB order
- **LPD8806**: 5V RGB (7-bit), up to 20MHz

## Usage Examples

### Basic Single Strip

```cpp
auto mgr = neoPixelModule.getManager();

// WS2812B with correct ColorOrder
auto strip = mgr->addStrip(22, 100, LedProtocol::WS2812B, ColorOrder::GRB);

// Set logical RED - automatically converted to GRB hardware format
strip->setPixel(0, 255, 0, 0);  // RGB(255,0,0) → Hardware[0,255,0] (G,R,B)
strip->show();
```

### Mixed Hardware Setup

```cpp
auto mgr = neoPixelModule.getManager();

// Create different hardware strips
auto ws2812b = mgr->addStrip(22, 64, LedProtocol::WS2812B, ColorOrder::GRB);
auto apa102 = mgr->addSpiStrip(9, 8, 32, LedProtocol::APA102, ColorOrder::BGR);
auto ws2811 = mgr->addStrip(23, 48, LedProtocol::WS2811, ColorOrder::RGB);

// Create unified virtual strip (always RGB internally)
auto vstrip = mgr->addVirtualStrip(144, ColorOrder::RGB);

// Attach physical strips to virtual strip
mgr->attachPhysicalToVirtual(vstrip, ws2812b, 0);   // Offset 0-63
mgr->attachPhysicalToVirtual(vstrip, apa102, 64);   // Offset 64-95  
mgr->attachPhysicalToVirtual(vstrip, ws2811, 96);   // Offset 96-143

// Same logical color appears correctly on ALL hardware types!
vstrip->setPixel(10, 255, 0, 0);   // RED on WS2812B (→GRB conversion)
vstrip->setPixel(70, 255, 0, 0);   // RED on APA102 (→BGR conversion)
vstrip->setPixel(110, 255, 0, 0);  // RED on WS2811 (→RGB, no conversion)

// Create effects spanning multiple hardware types
auto seg = mgr->addSegment(vstrip, 50, 120);  // Spans all 3 strip types
seg->setEffect(EffectPool::getRainbow());     // Works seamlessly!
```

### RGBW Support

```cpp
// SK6812 RGBW strip
auto rgbwStrip = mgr->addStrip(24, 50, LedProtocol::SK6812, ColorOrder::GRBW);

// 4-channel colors: Red, Green, Blue, White
rgbwStrip->setPixel(0, 255, 0, 0, 0);      // Pure RED
rgbwStrip->setPixel(1, 0, 0, 0, 255);      // Pure WHITE
rgbwStrip->setPixel(2, 100, 50, 200, 75);  // Mixed with white
```

## Console Commands

### Physical Strip Management

```bash
# 1-Wire strips with ColorOrder
neo phys add <gpio> <count> <protocol> <colororder>
neo phys add 22 100 2 1    # GPIO 22, 100 LEDs, WS2812B, GRB
neo phys add 23 64 1 0     # GPIO 23, 64 LEDs, WS2811, RGB

# SPI strips with ColorOrder  
neo spi add <mosi> <sck> <count> <protocol> <colororder>
neo spi add 16 17 60 5 4   # MOSI=16, SCK=17, 60 LEDs, APA102, BGR
```

### Virtual Strip Management

```bash
# Create virtual strip (always RGB internally)
neo virt add <count> <colororder>
neo virt add 200 0         # 200 LEDs, RGB internal storage

# Attach physical to virtual
neo virt attach <virt_id> <phys_id> <offset>
neo virt attach 0 0 0      # VirtStrip 0, PhysStrip 0, Offset 0
neo virt attach 0 1 100    # VirtStrip 0, PhysStrip 1, Offset 100
```

## Configuration Parameters

### ETS Parameter Mapping

The module automatically maps ETS parameters to the appropriate ColorOrder and GPIO pins:

```cpp
// Protocol mapping (ParamNEOSTRIP_NEOLEDType)
0 = WS2812B    // Default, most common
1 = WS2811     // 12V variant  
2 = WS2813     // With backup data
3 = SK6812     // RGBW capable
5 = APA102     // SPI, high refresh
6 = WS2801     // SPI, older

// ColorOrder mapping (ParamNEOSTRIP_NEOColourOrder)  
0 = RGB        // Standard RGB order
1 = RBG        // Some clones
2 = GRB        // WS2812B/SK6812 standard ⭐
3 = GBR        // Some WS2812B clones
4 = BGR        // APA102/SK9822 standard ⭐
5 = BRG        // Rare
6 = RGBW       // 4-channel RGB+White
7 = GRBW       // 4-channel, SK6812 standard ⭐

// GPIO Pin mapping
// 1-Wire Protocols (WS2812B, SK6812, etc.):
ParamNEOSTRIP_NEODataGPIO     // Data pin for 1-Wire protocols

// SPI Protocols (APA102, WS2801, etc.):
ParamNEOSTRIP_NEOSPIMOSIGPIO  // MOSI (Master Out Slave In) pin
ParamNEOSTRIP_NEOClockGPIO    // SCK (Serial Clock) pin
```

## Implementation Architecture

### VirtualStrip Layer
- **Always RGB/RGBW internally** regardless of ColorOrder parameter
- Provides unified pixel buffer for effects and animations
- ColorOrder parameter only determines bytes-per-LED (3 vs 4)

### PhysicalStrip Layer  
- **Handles ColorOrder conversion** from RGB → hardware format
- Each strip can have different ColorOrder independently
- Conversion happens during `setPixel()` and `show()` operations

### Hardware Driver Layer
- **Receives pre-converted bytes** in correct hardware order
- No color logic, only protocol-specific formatting
- Handles timing, SPI formatting, brightness headers, etc.

## Performance Considerations

- **Color conversion happens once per frame** during `show()`
- **VirtualStrip stores RGB** - no conversion overhead for effects
- **Efficient memory usage** - no duplicate color buffers
- **Minimal CPU impact** - simple byte reordering operations

## Best Practices

### ✅ Do:
- Use hardware-specific ColorOrder for each PhysicalStrip
- Create VirtualStrips with RGB ColorOrder for consistency  
- Combine different hardware types in single VirtualStrip
- Use console commands for testing and debugging

### ❌ Don't:
- Assume all LED strips use the same ColorOrder
- Try to handle ColorOrder conversion in application code
- Mix up VirtualStrip ColorOrder (always RGB) with PhysicalStrip ColorOrder

## Troubleshooting

### Wrong Colors Displayed

**Problem**: LED shows blue when red was expected

**Solution**: Check ColorOrder mapping for your hardware
```cpp
// If colors are wrong, try different ColorOrder values:
auto strip = mgr->addStrip(pin, count, protocol, ColorOrder::GRB);  // Try GRB first
auto strip = mgr->addStrip(pin, count, protocol, ColorOrder::BGR);  // Then BGR  
auto strip = mgr->addStrip(pin, count, protocol, ColorOrder::RGB);  // Then RGB
```

### SPI Strips Not Working

**Problem**: APA102/WS2801 strips don't light up

**Solution**: Verify SPI pin configuration
```cpp
// Make sure MOSI and SCK pins are correct for your board
auto strip = mgr->addSpiStrip(mosiPin, sckPin, count, LedProtocol::APA102, ColorOrder::BGR);
```

### Mixed Hardware Issues

**Problem**: Different strips show different colors for same RGB value

**Solution**: Verify each PhysicalStrip has correct ColorOrder
```cpp
// Each hardware type needs its specific ColorOrder:
auto ws2812 = mgr->addStrip(pin1, 100, LedProtocol::WS2812B, ColorOrder::GRB);  // WS2812B → GRB
auto apa102 = mgr->addSpiStrip(pin2, pin3, 50, LedProtocol::APA102, ColorOrder::BGR);  // APA102 → BGR
```

## Advanced Features

### Custom ColorOrder Support
The system supports any ColorOrder combination - add new mappings as needed:

```cpp
// Add support for new hardware with unusual ColorOrder
case 8: return ColorOrder::BRGW;  // Custom 4-channel order
case 9: return ColorOrder::WRGB;  // White-first 4-channel
```

### Runtime ColorOrder Changes
ColorOrder can be modified at runtime for testing:

```cpp
PhysicalStrip* strip = getPhysicalStrip(0);
strip->setColorOrder(ColorOrder::BGR);  // Change from GRB to BGR
```

This ColorOrder implementation provides a robust foundation for professional LED installations with mixed hardware requirements while maintaining simple, intuitive development patterns.