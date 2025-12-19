#pragma once

#include "OpenKNX.h"
#include "knxprod.h"

#include "NeoPixel.h"   // from https://github.com/OpenKNX/OFM-NeoPixel
#include "Segment.h"    // Segment support from OFM-NeoPixel
#include "effects/Effect.h"      // Effect system
#include "effects/EffectPool.h"  // Effect pool for singleton instances
#include <vector>

/**
 * Thin adapter that maps ETS parameters to OFM-NeoPixel.
 * All rendering, effects, timing and power handling are delegated to the library.
 * 
 * This adapter demonstrates the ColorOrder design principles:
 * - PhysicalStrips handle hardware-specific ColorOrder (GRB, BGR, RGB, etc.)
 * - VirtualStrip provides unified RGB interface for effects
 * - Mixed hardware types can be combined seamlessly
 * 
 * Power Management features:
 * - Global current limiting across all strips
 * - Per-LED and per-channel current limits
 * - Automatic brightness limiting based on power consumption
 * 
 * Color Correction features:
 * - Gamma correction with configurable gamma values
 * - White balance correction for RGB channels
 * - Per-strip color correction configuration
 *
 * Effects System features:
 * - 10 built-in effects (Solid, Wipe, Rainbow, Pride, Confetti, Juggle, BPM, Cylon, etc.)
 * - Automatic effect configuration from ETS parameters  
 * - Runtime effect switching via KNX objects
 * - Auto-update system for smooth animations (20 FPS default)
 */
class NeoPixelBusModule : public OpenKNX::Module
{
public:
  // Segment Configuration Structure
  struct SegmentConfig {
    uint16_t startLed;                   // Start LED index
    uint16_t endLed;                     // End LED index  
    uint16_t offset;                     // Segment offset
    uint16_t grouping;                   // Grouping parameter
    uint16_t spacing;                    // Spacing between groups
    bool reverseDirection;               // Reverse direction flag
    bool mirrorEffect;                   // Mirror effect flag
    Segment* segment = nullptr;          // Pointer to actual segment

    // Saved state for power toggle
    bool savedValid = false;
    uint8_t savedR = 0;
    uint8_t savedG = 0;
    uint8_t savedB = 0;
    uint8_t savedW = 0;
    uint8_t savedBrightness = 0;

    // Saved effect information
    bool savedEffectValid = false;
    uint8_t savedEffectType = 0; // 0 = none
    uint8_t savedEffectSpeed = 0;
    uint8_t savedEffectIntensity = 0;
    uint8_t savedEffectOption1 = 0;
    uint8_t savedEffectOption2 = 0;
    uint8_t savedEffectMode = 0;
    uint8_t savedEffectReverse = 0;

    // Which rendering mode was active before power-off
    bool savedLastWasEffect = false;

    // Persistent HSV values for independent H/S/V control
    uint8_t currentH = 0;                // Current Hue (0-255)
    uint8_t currentS = 255;              // Current Saturation (0-255)
    uint8_t currentV = 0;                // Current Value (0-255)

    // Pending solid color when effect is stopped
    // User can set these while effect is running, they're applied when effect=0
    uint8_t pendingSolidR = 0;
    uint8_t pendingSolidG = 0;
    uint8_t pendingSolidB = 0;
    uint8_t pendingSolidW = 0;

    // DPT 3.007 Start/Stop dimming state
    enum DimmingChannel { NONE, BRIGHTNESS, RED, GREEN, BLUE, WHITE, WARM_WHITE, COOL_WHITE, HUE, SATURATION, VALUE };
    DimmingChannel activeDimming = NONE; // Which channel is currently dimming
    bool dimmingIncrease = false;        // Dimming direction (true = increase/up, false = decrease/down)
    uint8_t dimmingStepCode = 0;         // Current step code (0 = stopped, 1-7 = speed)
    uint32_t dimmingLastUpdate = 0;      // Last telegram time (millis)
    uint32_t dimmingNextStep = 0;        // Next dimming step time (millis)
  };

  // Virtual Strip Configuration Structure  
  struct VirtualStripConfig {
    uint8_t physicalStripIndex;          // Which physical strip (0-5)
    uint16_t virtualStartPosition;       // Where this strip starts in the virtual strip
    uint16_t ledCount;                   // Number of LEDs from this physical strip
    
    VirtualStripConfig(uint8_t stripIndex, uint16_t startPos, uint16_t count)
      : physicalStripIndex(stripIndex), virtualStartPosition(startPos), ledCount(count) {}
  };

protected:
  uint8_t _channelIndex = 0;  // Add the channel index for parameter access

public:
  NeoPixelBusModule() = default;

  const std::string name() override { return "NeoPixelBus (OAM)"; }
  const std::string version() override { return "OAM-NeoPixel-adapter 0.1"; }

  void setup(bool configured) override;
  void loop(bool configured) override;
  void processInputKo(GroupObject& ko) override;
  void processActiveDimming(); // Process DPT 3.007 start/stop dimming

  // Console integration: delegate help and commands to core NeoPixel module
  void showHelp() override;
  bool processCommand(const std::string command, bool diagnose) override;

  // Access to unified virtual strip (if created)
  VirtualStrip* getVirtualStrip() const { return _virtualStrip; }
  
  // Access to individual physical strips
  const std::vector<PhysicalStrip*>& getPhysicalStrips() const { return _physicalStrips; }

  // Access to segments
  const std::vector<SegmentConfig>& getSegments() const { return _segments; }
  uint8_t getNumberOfSegments() const { return _numberOfSegments; }
  Segment* getSegment(uint8_t index) const; // Get segment by index

  // Effects status
  bool areEffectsEnabled() const { return _effectsEnabled; }

  // Color Correction
  void updateColorCorrection();
  void forceColorCorrectionUpdate(); // Force immediate update (bypassing rate limiting)
  bool isGammaCorrectionEnabled() const { return _gammaCorrectionEnabled; }
  bool isWhiteBalanceEnabled() const { return _whiteBalanceEnabled; }
  float getGammaValue() const { return _gammaValue; }
  
  // Color correction functions
  uint8_t applyGammaCorrection(uint8_t value) const;
  void applyWhiteBalance(uint8_t& r, uint8_t& g, uint8_t& b) const;
  uint32_t correctColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) const;

  // Global brightness control
  void applyGlobalBrightness(uint8_t brightness);
  void restoreOriginalBrightness();
  uint8_t getGlobalBrightness() const { return _globalBrightness; }

  // HCL color temperature control
  void applyHclColorTemperature(uint16_t kelvin);
  void disableHclMode();
  uint16_t getCurrentHclTemperature() const { return _currentHclTemperature; }
  bool isHclModeEnabled() const { return _hclModeEnabled; }

  // Static helper functions (public for testing)
  static LedProtocol mapProtocol(uint8_t paramLedType);
  static ColorOrder mapColorOrder(uint8_t paramOrder);
  static bool isSpiProtocol(LedProtocol protocol);
  static const char* getColorOrderName(ColorOrder order);
  static const char* getProtocolName(LedProtocol protocol);

private:
  bool _initialized = false;
  uint16_t _totalLeds = 0;
  std::vector<PhysicalStrip*> _physicalStrips;
  VirtualStrip* _virtualStrip = nullptr;
  
  // Virtual Strip Configuration (NEW)
  std::vector<VirtualStripConfig> _virtualStripConfiguration; // Order and mapping of physical strips

  // Color Correction
  bool _gammaCorrectionEnabled = false;  // Gamma correction enabled
  bool _whiteBalanceEnabled = false;     // White balance enabled  
  float _gammaValue = 2.2f;              // Gamma value (default 2.2)
  uint8_t _whiteBalanceRed = 255;        // Red channel balance (0-255)
  uint8_t _whiteBalanceGreen = 255;      // Green channel balance (0-255) 
  uint8_t _whiteBalanceBlue = 255;       // Blue channel balance (0-255)
  

  // Strip Configuration
  uint8_t _swapMode = 0;                 // Swap mode (from NEOSwap parameter)
  uint16_t _skipFirstLeds = 0;           // Number of LEDs to skip at beginning

  // Segment Configuration
  std::vector<SegmentConfig> _segments;  // Configured segments
  uint8_t _numberOfSegments = 0;         // Number of segments from ETS
  // Effect Configuration  
  bool _effectsEnabled = false;          // Whether effects are enabled
  
  // Global Brightness Control
  uint8_t _globalBrightness = 255;       // Global brightness multiplier (0-255, default full)
  std::vector<uint8_t> _originalBrightness; // Store original segment brightness levels
  
  // HCL Color Temperature Control
  bool _hclModeEnabled = false;          // Whether HCL mode is active
  uint16_t _currentHclTemperature = 6500; // Current HCL temperature in Kelvin (default daylight)
  std::vector<std::array<uint8_t, 3>> _originalColors; // Store original segment RGB colors for HCL restoration
  
  // Performance & Rate Limiting
  unsigned long _lastColorUpdateMs = 0;     // Last color correction update timestamp
  static const unsigned long COLOR_UPDATE_INTERVAL_MS = 50;   // Update color every 500ms to reduce logging overhead
  uint32_t _lastLoopTime = 0;               // Last loop time for deltaTime calculation
  
  // Configuration & Setup
  void configureFromETS();            // reads ETS params and builds phys+virt layout
  void configurePowerManagement();    // Configure power management using OFM PowerManager
  void configureColorCorrection();    // Configure color correction from ETS parameters
  void configureStripOptions();       // Configure swap and skip options from ETS parameters
  void configureSegments();           // Configure segments from ETS parameters
  void configureVirtualStripOrder();  // Configure virtual strip order from ETS parameters
  void createDefaultVirtualStripOrder(); // Create default sequential strip order
  void createVirtualStripWithOrder(); // creates virtual strip with user-defined order
  
  // Power Management Implementation
  uint16_t calculateCurrentConsumption() const;
  uint8_t calculateBrightnessLimit() const;
  void applyPowerLimiting();
  
  // Strip Options Implementation
  void applySwapMode();               // Apply color channel swapping
  void configureSkipLeds();           // Configure LED skipping offset
  
  // Segment Implementation
  void createSegments();              // Create segments on virtual strip
  void applySegmentConfiguration();   // Apply segment-specific settings
  void applyGroupingAndSpacing(SegmentConfig& config); // Apply grouping and spacing pattern
  SegmentConfig createSegmentConfig(uint8_t segmentIndex); // Create segment config from ETS

  // Effect Implementation
  void configureEffects();           // Configure effects from ETS parameters
  void applyEffectToSegment(Segment* segment, uint8_t effectType); // Apply effect to specific segment
  Effect* getEffectFromType(uint8_t effectType); // Get effect instance from type ID
  void setupEffectConfiguration(Segment* segment); // Setup effect configuration from ETS
  
  // Power Management Helpers
  static uint16_t calculateLedCurrentMa(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0);
  static uint16_t getTypicalLedCurrentMa(LedProtocol protocol);

  // Color Correction Helpers
  static float mapGammaValue(uint8_t paramValue); // Map ETS gamma value to float
  static uint8_t mapWhiteBalanceValue(uint8_t paramValue); // Map ETS white balance to 0-255

  // Strip Configuration Helpers
  static uint8_t mapSwapMode(uint8_t paramValue); // Map ETS swap parameter
  void swapChannels(uint8_t& ch1, uint8_t& ch2); // Helper for channel swapping
};

extern NeoPixelBusModule openknxNeoPixelModule;
