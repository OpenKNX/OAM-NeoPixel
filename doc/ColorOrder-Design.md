# ColorOrder Design Principles

This document outlines the key design principles for ColorOrder handling in the NeoPixelBus module, enabling seamless integration of different LED hardware types with different color channel arrangements.

## Key Design Principles

### 1. VirtualStrip is ColorOrder-Agnostic

- **Always stores RGB/RGBW internally**: VirtualStrip maintains a unified RGB/RGBW buffer regardless of hardware ColorOrder
- **No color conversion in VirtualStrip layer**: Color conversion happens at the PhysicalStrip level
- **Simplifies effect development**: Effects work with standard RGB/RGBW values without considering hardware differences

```cpp
// VirtualStrip always stores RGB internally, regardless of parameter
auto virt = mgr->addVirtualStrip(100, ColorOrder::RGB);  // Always RGB internally
virt->setPixel(0, 255, 0, 0);  // Always RGB: Red=255, Green=0, Blue=0
```

### 2. PhysicalStrip Handles Hardware Differences

- **Converts RGB → hardware ColorOrder**: Each PhysicalStrip converts from RGB to its specific hardware format
- **Each PhysicalStrip can have different ColorOrder**: WS2812B (GRB), APA102 (BGR), WS2811 (RGB), etc.
- **Conversion happens once per frame**: Efficient conversion during the `show()` operation

```cpp
// Different hardware, different ColorOrder
auto strip0 = mgr->addStrip(22, 64, LedProtocol::WS2812B, ColorOrder::GRB);    // WS2812B uses GRB
auto strip1 = mgr->addStrip(23, 32, LedProtocol::WS2811, ColorOrder::RGB);     // WS2811 uses RGB
auto strip2 = mgr->addSpiStrip(9, 8, 40, LedProtocol::APA102, ColorOrder::BGR); // APA102 uses BGR

// Same RGB input, different hardware output
strip0->setPixel(0, 255, 0, 0);  // Sends [0, 255, 0] to WS2812B (G,R,B order)
strip1->setPixel(0, 255, 0, 0);  // Sends [255, 0, 0] to WS2811 (R,G,B order) 
strip2->setPixel(0, 255, 0, 0);  // Sends [0, 0, 255] to APA102 (B,G,R order)
```

### 3. Hardware Drivers are Byte-Oriented

- **Receive already-converted bytes**: Hardware drivers receive byte arrays in the correct order
- **No color logic in hardware layer**: Drivers only handle protocol-specific formatting (timing, brightness, etc.)
- **Protocol-specific formatting only**: APA102 brightness headers, WS2812B timing, etc.

```cpp
// Hardware driver receives pre-converted bytes:
// WS2812B driver: [G, R, B, G, R, B, ...]  (already in GRB order)
// APA102 driver:  [brightness, B, G, R, brightness, B, G, R, ...]  (already in BGR order)
```

## Mixed ColorOrders in ONE VirtualStrip

**This is the killer feature**: Combine strips with different ColorOrders seamlessly!

```cpp
// Example: WS2812B (GRB) + APA102 (BGR) in one logical strip
auto strip0 = mgr->addStrip(22, 64, LedProtocol::WS2812B, ColorOrder::GRB);
auto strip1 = mgr->addSpiStrip(9, 8, 40, LedProtocol::APA102, ColorOrder::BGR);

// Combine into ONE VirtualStrip
auto virt = mgr->addVirtualStrip(104, ColorOrder::RGB);  // Always RGB internally!
mgr->attachPhysicalToVirtual(virt, strip0, 0);    // WS2812B at pixels 0-63
mgr->attachPhysicalToVirtual(virt, strip1, 64);   // APA102 at pixels 64-103

// ONE segment, ONE effect across BOTH strips with DIFFERENT hardware!
auto seg = mgr->addSegment(virt, 0, 103);
seg->setEffect(EffectPool::getRainbow());
seg->setPrimaryColor(255, 0, 0, 255);  // RED on both strips ✅
```

**Result**: Both strips show the same logical colors despite different hardware!

## Setting ColorOrder

### Per PhysicalStrip (Recommended)

```cpp
// 1-Wire strips with ColorOrder
auto strip1 = mgr->addStrip(pin, count, LedProtocol::WS2812B, ColorOrder::GRB);
auto strip2 = mgr->addStrip(pin, count, LedProtocol::WS2811, ColorOrder::RGB);
auto strip3 = mgr->addStrip(pin, count, LedProtocol::SK6812, ColorOrder::GRBW);

// SPI strips with ColorOrder  
auto strip4 = mgr->addSpiStrip(mosi, sck, count, LedProtocol::APA102, ColorOrder::BGR);
auto strip5 = mgr->addSpiStrip(mosi, sck, count, LedProtocol::WS2801, ColorOrder::RGB);
```

### Console Commands

```bash
# Add 1-Wire strips with ColorOrder
neo phys add 9 64 2 1      # GPIO 9, 64 LEDs, WS2812B, ColorOrder=GRB
neo phys add 10 32 1 0     # GPIO 10, 32 LEDs, WS2811, ColorOrder=RGB

# Add SPI strips with ColorOrder
neo spi add 8 9 40 5 4     # MOSI=8, SCK=9, 40 LEDs, APA102, ColorOrder=BGR
neo spi add 11 12 50 6 0   # MOSI=11, SCK=12, 50 LEDs, WS2801, ColorOrder=RGB
```

### VirtualStrip ColorOrder (Legacy/Ignored)

VirtualStrip has a ColorOrder parameter for backward compatibility, but it's **not used for color conversion**. VirtualStrip always stores RGB/RGBW internally.

```cpp
// This ColorOrder parameter is ignored for color conversion
auto virt = mgr->addVirtualStrip(100, ColorOrder::RGB);  // Always RGB internally
auto virt2 = mgr->addVirtualStrip(100, ColorOrder::GRB); // Still RGB internally!
```

## ColorOrder Options

| ColorOrder | Description | Common Hardware |
|------------|-------------|-----------------|
| `RGB` | Red, Green, Blue | WS2811, some APA102 |
| `RBG` | Red, Blue, Green | Some LED clones |
| `GRB` | Green, Red, Blue | **WS2812B, SK6812 (most common)** |
| `GBR` | Green, Blue, Red | Some WS2812B clones |
| `BGR` | Blue, Green, Red | **APA102, SK9822** |
| `BRG` | Blue, Red, Green | Rare configurations |
| `RGBW` | Red, Green, Blue, White | SK6812 4-channel |
| `GRBW` | Green, Red, Blue, White | **SK6812 4-channel standard** |

## Implementation Flow

1. **Application Layer**: Sets RGB values using logical color API
   ```cpp
   virt->setPixel(0, 255, 0, 0);  // Logical RED
   ```

2. **VirtualStrip Layer**: Stores RGB values in unified buffer
   ```cpp
   buffer[0] = 255;  // R
   buffer[1] = 0;    // G  
   buffer[2] = 0;    // B
   ```

3. **PhysicalStrip Layer**: Converts RGB → hardware ColorOrder
   ```cpp
   // For GRB hardware:
   hwBuffer[0] = 0;    // G (from RGB.G)
   hwBuffer[1] = 255;  // R (from RGB.R)
   hwBuffer[2] = 0;    // B (from RGB.B)
   ```

4. **Hardware Driver**: Sends formatted bytes to LED hardware
   ```cpp
   // WS2812B driver sends: [0, 255, 0] as GRB timing pulses
   // APA102 driver sends: [brightness, 0, 0, 255] as SPI data
   ```

## Benefits

1. **Unified Development**: Effects work identically across all hardware types
2. **Mixed Hardware Support**: Combine different LED types in one logical strip  
3. **Hardware Abstraction**: Application code doesn't need to know hardware details
4. **Performance**: Color conversion happens once per frame, not per pixel
5. **Flexibility**: Easy to change hardware without modifying effects
6. **Future-Proof**: New LED types can be added without breaking existing code

## Example: Complete Mixed Hardware Setup

```cpp
void setupMixedLedSystem() {
    auto mgr = neoPixelModule.getManager();
    
    // Create different hardware strips
    auto ws2812 = mgr->addStrip(22, 100, LedProtocol::WS2812B, ColorOrder::GRB);
    auto apa102 = mgr->addSpiStrip(9, 8, 50, LedProtocol::APA102, ColorOrder::BGR); 
    auto ws2811 = mgr->addStrip(23, 75, LedProtocol::WS2811, ColorOrder::RGB);
    
    // Create unified virtual strip
    auto virt = mgr->addVirtualStrip(225, ColorOrder::RGB);  // Total: 100+50+75
    
    // Attach physical strips to virtual strip
    mgr->attachPhysicalToVirtual(virt, ws2812, 0);   // Offset 0-99
    mgr->attachPhysicalToVirtual(virt, apa102, 100); // Offset 100-149  
    mgr->attachPhysicalToVirtual(virt, ws2811, 150); // Offset 150-224
    
    // Create segments spanning multiple hardware types
    auto seg1 = mgr->addSegment(virt, 80, 140);  // Spans WS2812B + APA102
    auto seg2 = mgr->addSegment(virt, 120, 200); // Spans APA102 + WS2811
    
    // Apply effects - they work seamlessly across all hardware!
    seg1->setEffect(EffectPool::getRainbow());
    seg2->setEffect(EffectPool::getFire());
    
    // Same logical color appears correctly on all hardware types
    virt->setPixel(50, 255, 0, 0);   // RED on WS2812B (converted to GRB)
    virt->setPixel(125, 255, 0, 0);  // RED on APA102 (converted to BGR)
    virt->setPixel(175, 255, 0, 0);  // RED on WS2811 (stays RGB)
}
```

This design enables powerful LED installations mixing different hardware types while maintaining simple, unified application code.