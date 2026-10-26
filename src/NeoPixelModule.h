#pragma once

#include "OpenKNX.h"
#include "knxprod.h"
#include "versions.h" // Generated module versions (MODULE_NeoPixel_Version)

#include "EffektManager.h"      // Effektmanager sequencer
#include "HclPixelTransform.h"  // HCL pixel transformation callback
#include "NeoPixel.h"           // from https://github.com/OpenKNX/OFM-NeoPixel
#include "NeoPixelEmConsole.h"  // Console bridge (OFM rendering, OAM data/actions)
#include "Segment.h"            // Segment support from OFM-NeoPixel
#include "effects/Effect.h"     // Effect system
#include "effects/EffectPool.h" // Effect pool for singleton instances
#include <map>
#include <string>
#include <vector>

// ETS FunctionProperty Definitions (from the generated NeoPixel-*.debug.xml files)
// These values are referenced in the ETS JavaScript for online functions
#define NEOPIXEL_FUNCTION_OBJECT_INDEX 158 // NeoPixel module appliance object
#define NEOPIXEL_FUNCTION_PROPERTY_ID 10   // Hardware detection property

// ─── NeoPixel Module Error/Warning Blink Codes ─────────────────────────────
// All NeoPixel status codes use the STATUS LED (OPENKNX_LEDFUNC_BASE_STATE).
// Errors:  Red + errorCode(N) → countable blink pattern (N× blink, pause, repeat)
// Warnings: Orange/Yellow + pulsing() → smooth breathing (check serial log for details)

// Error codes (Red, STATUS LED): lower value = higher severity
static constexpr uint8_t NEO_ERROR_HW_MISMATCH = 1;      // 1× blink: Firmware/ETS hardware mismatch
static constexpr uint8_t NEO_ERROR_NO_HW_CONFIGURED = 2; // 2× blink: No hardware selected in ETS (dummy 255)
static constexpr uint8_t NEO_ERROR_GPIO_CONFLICT = 3;    // 3× blink: Duplicate GPIO pin assignment
static constexpr uint8_t NEO_ERROR_STRIP_FAILED = 4;     // 4× blink: Physical strip creation failed (nullptr)
static constexpr uint8_t NEO_ERROR_VSTRIP_FAILED = 5;    // 5× blink: Virtual strip creation failed

// Warning codes (Orange/Yellow, STATUS LED): pulsing — not countable, use serial log
static constexpr uint8_t NEO_WARN_ABL_ACTIVE = 1;      // Pulsing Orange: Auto-Brightness-Limiting active
static constexpr uint8_t NEO_WARN_FLASH_DISCARDED = 2; // Pulsing Purple: Flash data discarded (config changed)
static constexpr uint8_t NEO_WARN_UNKNOWN_EFFECT = 3;  // Pulsing Yellow: Unknown effect type in ETS

// Forward declarations
class EffectConfiguration;
class ColorManagement;
class SegmentController;
class SceneManager;
class NeoPixelStudio;

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
    friend class EffectConfiguration;
    friend class ColorManagement;
    friend class SegmentController;
    friend class SceneManager;
    friend class NeoPixelStudio;

  public:
    // Segment Configuration Structure
    struct SegmentConfig
    {
        uint16_t startLed;     // Start LED index
        uint16_t endLed;       // End LED index
        uint16_t offset;       // Segment offset
        uint16_t grouping;     // Grouping parameter
        uint16_t spacing;      // Spacing between groups
        bool reverseDirection; // Reverse direction flag
        bool mirrorEffect;     // Mirror effect flag
        // 2D/3D matrix geometry (from ETS Byte 12=width, 13=height, Byte10 Bit5-7=topology)
        uint8_t matrixWidth = 0;  // 0 = no matrix (1D)
        uint8_t matrixHeight = 0; // 0 = no matrix (1D)
        uint8_t matrixDepth = 0;  // 0/1 = 2D, >1 = 3D
        LedTopology topology = LedTopology::LINEAR_1D;
        Segment* segment = nullptr; // Pointer to actual segment

        // ── Effektkette (virtual band) config ────────────────────────────
        uint8_t syncMode = 0;                 ///< 0=Aus, 1=Master, 2=Slave
        uint8_t syncOverridePolicy = 0;       ///< 0=Sync immer, 1=Lokal hat Vorrang
        uint8_t syncTimeoutSteps = 0;         ///< 0=Aus; N×10s timeout for slaves
        uint16_t virtualTotalLength = 0;      ///< total band length (0=standalone)
        uint16_t virtualOffset = 0;           ///< this segment's start in band
        uint16_t distributedMatrixWidth = 0;  ///< derived global matrix width for distributed 2D
        uint16_t distributedMatrixHeight = 0; ///< derived global matrix height for distributed 2D

        // ── Effektkette runtime state (not persisted) ─────────────────────
        uint32_t lastSyncMs = 0;         ///< millis() when last sync was received
        bool localOverride = false;      ///< local KO has overridden sync state
        uint8_t lastSyncPayload[9] = {}; ///< last payload sent by master (change detection)
        uint32_t lastSyncSentMs = 0;     ///< millis() when master last sent a sync telegram
        bool syncPayloadValid = false;   ///< lastSyncPayload contains a sent payload
        uint8_t syncFreezePayload[9] = {}; ///< master: direct state captured at EM start, sent while the EM runs
        bool syncFreezeValid = false;      ///< syncFreezePayload holds the state of the current EM run
        uint8_t lastRxPower = 0xFF;        ///< slave: power bit of the last applied telegram (0xFF = unknown)

        // ── Effektmanager ─────────────────────────────────────────────────
        EffektManagerController emController; ///< per-segment EM sequencer
        uint16_t lastCueTextKey = 0xFFFF;     ///< (emId<<8|cueNum) of the last applied long cue-text override
        uint8_t  emStopReturnMode = 0;        ///< EM-Stop-Rückkehr — 0=letzter Zustand, 1=Default, 2=Aus (from ETS)
        bool     suspendedByPower = false;    ///< EM paused by segment or global power-off, resumed by power-on
        bool     pendingPowerOnRestart = false; ///< switched on while globally suspended: start the power-on EM at global ON
        bool     globalSuspended = false;     ///< global power OFF currently suspends this segment (RAM only)
        bool     emStartedFromOff = false;    ///< EM chain started on a switched-off segment: its stop switches off again (flash: reserved[2] bit0)
        bool     hiddenEffectChange = false;  ///< effect/cue changed while not visible: re-enter it at the next ON
        bool     locked = false;              ///< segment locked via Sperre-KO → ignore all controlling KOs
        uint32_t statusEmKey = 0xFFFFFFFF;    ///< last EM state (emId/cue/runState) seen by sendEmStatusKOs()
        uint16_t statusCctKelvin = 0;         ///< last CCT command in Kelvin (0 = none since boot; not persisted)

        // Saved state for power toggle and flash persistence
        bool savedValid = false;
        uint8_t savedPower = 1; // 1 = on, 0 = off (for flash persistence)
        uint8_t savedR = 0;
        uint8_t savedG = 0;
        uint8_t savedB = 0;
        uint8_t savedWW = 0; // Warm White (or single white for RGBW)
        uint8_t savedSecondaryR = 0, savedSecondaryG = 0, savedSecondaryB = 0, savedSecondaryWW = 0, savedSecondaryCW = 0;
        uint8_t savedCW = 0; // Cool White (RGBCCT only)
        uint8_t savedBrightness = 0;

        // Saved effect information
        bool savedEffectValid = false;
        uint8_t savedEffectType = 0; // 0 = none
        // Note: Effect-specific parameters are now dynamically loaded from ETS
        // Old generic parameters (speed/intensity/option1-3) removed - replaced by effect-specific params

        // Which rendering mode was active before power-off
        bool savedLastWasEffect = false;

        // Last active scene number (0 = no scene active, 1-15 = scene number)
        uint8_t savedSceneNumber = 0;

        // Persistent HSV values for independent H/S/V control
        uint8_t currentH = 0;   // Current Hue (0-255)
        uint8_t currentS = 255; // Current Saturation (0-255)
        uint8_t currentV = 0;   // Current Value (0-255)
        bool hsvTracked = false; // last colour change went through H/S/V: currentH/S/V describe it

        // DPT 3.007 Start/Stop dimming state
        enum DimmingChannel
        {
            NONE,
            BRIGHTNESS,
            RED,
            GREEN,
            BLUE,
            WHITE,
            WARM_WHITE,
            COOL_WHITE,
            HUE,
            SATURATION,
            VALUE
        };
        DimmingChannel activeDimming = NONE; // Which channel is currently dimming
        bool dimmingIncrease = false;        // Dimming direction (true = increase/up, false = decrease/down)
        uint8_t dimmingStepCode = 0;         // Current step code (0 = stopped, 1-7 = speed)
        int16_t dimmingRemaining = -1;     // DPT 3.007 step still to go (0..255); -1 = until stop telegram
        uint32_t dimmingLastUpdate = 0;      // Last telegram time (millis)
        uint32_t dimmingNextStep = 0;        // Next dimming step time (millis)
        uint32_t dimmingLastStepMs = 0;      // time of the last ramp step (step size from elapsed time)
        uint16_t dimmingStepRest = 0;        // ramp remainder (255 * ms modulo the full ramp time)
        int16_t dimmingWhiteWW = -1;         // RGBCCT white ramp: WW written last tick; -1 = re-derive
        int16_t dimmingWhiteCW = -1;         // RGBCCT white ramp: CW written last tick
        uint16_t dimmingWhiteShare = 0x4000; // RGBCCT white ramp: WW share of WW+CW (1.15 fixed point)
        uint8_t dimmingWhiteScale = 1;       // RGBCCT white ramp: 2 when WW+CW started above 255
    };

    // Virtual Strip Configuration Structure
    struct VirtualStripConfig
    {
        uint8_t physicalStripIndex;    // Which physical strip (0-5)
        uint16_t virtualStartPosition; // Where this strip starts in the virtual strip
        uint16_t ledCount;             // Number of LEDs from this physical strip

        VirtualStripConfig(uint8_t stripIndex, uint16_t startPos, uint16_t count)
            : physicalStripIndex(stripIndex), virtualStartPosition(startPos), ledCount(count) {}
    };

  protected:
    uint8_t _channelIndex = 0; // Add the channel index for parameter access

  public:
    // Allow flash persistence class to access private members
    friend class NeoPixelFlashPersistence;

    NeoPixelBusModule();
    ~NeoPixelBusModule();

    const std::string name() override { return "NeoPixel"; }
    const std::string version() override { return MODULE_NeoPixel_Version; }

    void setup(bool configured) override;
    void loop(bool configured) override;
    void processInputKo(GroupObject& ko) override;
    void processBeforeRestart() override;      // Turn off all LEDs before restart/programming
    void processBeforeTablesUnload() override; // Turn off all LEDs before ETS programming
    void processAfterStartupDelay() override;  // Restore LED states after startup

    // Flash persistence (OGM-Common calls these automatically)
    uint16_t flashSize() override;
    void writeFlash() override;
    void readFlash(const uint8_t* data, const uint16_t size) override;

    // Console integration: delegate help and commands to core NeoPixel module
    void showHelp() override;
    bool processCommand(const std::string command, bool diagnose) override;

    // FunctionProperty: ETS online functions (hardware detection)
    bool processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t* data, uint8_t* resultData, uint8_t& resultLength) override;

    // Access to unified virtual strip (if created)
    VirtualStrip* getVirtualStrip() const { return _virtualStrip; }

    // Access to internal NeoPixel library instance (for testing/console)
    NeoPixel& getNeoPixel() { return _neoPixel; }

    // Access to individual physical strips
    const std::vector<PhysicalStrip*>& getPhysicalStrips() const { return _physicalStrips; }

    // Access to segments
    std::vector<SegmentConfig>& getSegments() { return _segments; }
    const std::vector<SegmentConfig>& getSegments() const { return _segments; }
    uint8_t getNumberOfSegments() const { return _numberOfSegments; }

    // Channel index access (for parameter reading)
    uint8_t getChannelIndex() const { return _channelIndex; }
    void setChannelIndex(uint8_t index) { _channelIndex = index; }

    // Apply effect to segment (used by flash persistence and EffectConfiguration)
    void applyEffectToSegment(Segment* segment, uint8_t effectType);
    void setupEffectConfiguration(Segment* segment); // Setup effect parameters
    Segment* getSegment(uint8_t index) const;        // Get segment by index

    // Public API for segment control (delegates to SegmentController)
    void processActiveDimming();

    // Public API for color management (delegates to ColorManagement)
    void configureColorCorrection();
    void updateColorCorrection();
    void forceColorCorrectionUpdate();
    void applyGlobalBrightness(uint8_t brightness);
    void restoreOriginalBrightness();
    void updateHclTransformContext(); // Update HCL pixel transformation context and register callback

    // Effects status
    bool areEffectsEnabled() const { return _effectsEnabled; }

    // Color Correction Status (read-only)
    bool isGammaCorrectionEnabled() const { return _gammaCorrectionEnabled; }
    bool isWhiteBalanceEnabled() const { return _whiteBalanceEnabled; }
    float getGammaValue() const { return _gammaValue; }
    uint8_t getGlobalBrightness() const { return _globalBrightness; }
    uint16_t getCurrentHclTemperature() const { return _hclAppliedKelvin; }
    bool isHclModeEnabled() const { return _hclModeEnabled; }

    // Color correction functions (inline helpers - not delegated)
    uint8_t applyGammaCorrection(uint8_t value) const;
    void applyWhiteBalance(uint8_t& r, uint8_t& g, uint8_t& b) const;
    uint32_t correctColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) const;

    // Static helper functions (public for testing)
    // ETS enum mapping lives in StripConfiguration's static helpers.
    static bool isSpiProtocol(LedProtocol protocol);
    static const char* getColorOrderName(ColorOrder order);
    static const char* getProtocolName(LedProtocol protocol);

// Debug: Show complete configuration analysis (OPENKNX_DEBUG only)
#ifdef OPENKNX_DEBUG
    void debugShowConfiguration();
#endif

  private:
    // Console bridge hooks (NeoPixelEmConsole.h) need access to private providers
    friend bool openknxNeoPixelHandleEmChainAction(uint8_t action, int arg1, int arg2);
    friend bool openknxNeoPixelHandleCueSet(uint8_t emId, uint8_t cueNum, uint8_t effectId,
                                            uint16_t durSec, uint16_t fadeMs,
                                            uint8_t bri, uint8_t r, uint8_t g, uint8_t b);
    friend bool openknxNeoPixelHandleCueParam(uint8_t emId, uint8_t cueNum, uint8_t paramIdx, uint8_t value);
    friend bool openknxNeoPixelHandleCueText(uint8_t emId, uint8_t cueNum, const char* text);
    friend int openknxNeoPixelEmSegmentCount();
    friend bool openknxNeoPixelGetEmStatus(uint8_t seg, NeoEmSegStatus& out);
    friend const EffektManagerData* openknxNeoPixelGetEmData(uint8_t emId);
    friend bool openknxNeoPixelGetChainStatus(uint8_t seg, NeoChainSegStatus& out);

    // OFM-NeoPixel library instance (not registered as separate module)
    NeoPixel _neoPixel;

    bool _initialized = false;
    bool _clearLedsAfterSetup = false; // Flag to clear LEDs after hardware initialization
#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    NeoPixelStudio* _studio = nullptr;
#endif
    bool _localTestMode = false;       // 'neo init': run loop()/console WITHOUT an ETS download (local bench tests only)
    uint16_t _totalLeds = 0;
    std::vector<PhysicalStrip*> _physicalStrips;
    std::vector<uint8_t> _physicalStripEtsIds;
    bool _liveLedLayout = false;
    bool _livePinKnown[8] = {};
    uint8_t _liveDataPins[8] = {}, _liveClockPins[8] = {};
    uint16_t _liveStripLengths[8] = {}, _liveStripSkips[8] = {};
    uint16_t _liveSegmentStarts[16] = {}, _liveSegmentEnds[16] = {};
    uint16_t configuredStripLength(uint8_t index) const;
    uint16_t configuredStripSkip(uint8_t index) const;
    VirtualStrip* _virtualStrip = nullptr;

    // Virtual Strip Configuration (NEW)
    std::vector<VirtualStripConfig> _virtualStripConfiguration; // Order and mapping of physical strips

    // Color Correction
    bool _gammaCorrectionEnabled = false; // Gamma correction enabled
    bool _whiteBalanceEnabled = false;    // White balance enabled
    float _gammaValue = 2.0f;             // Gamma value (default 2.0, ETS param 8)
    uint8_t _whiteBalanceRed = 255;       // Red channel balance (0-255)
    uint8_t _whiteBalanceGreen = 255;     // Green channel balance (0-255)
    uint8_t _whiteBalanceBlue = 255;      // Blue channel balance (0-255)

    // Strip Configuration
    uint8_t _swapMode = 0;       // Swap mode (from NEOSwap parameter)
    uint16_t _skipFirstLeds = 0; // Number of LEDs to skip at beginning

    // Segment Configuration
    std::vector<SegmentConfig> _segments; // Configured segments
    uint8_t _numberOfSegments = 0;        // Number of segments from ETS

    // Sub-module: Effect Configuration
    class EffectConfiguration* _effectConfiguration;
    bool _effectsEnabled = false; // Whether effects are enabled

    // Sub-module: Color Management
    class ColorManagement* _colorManagement;

    // Sub-module: Segment Controller
    class SegmentController* _segmentController;

    // Sub-module: Scene Manager (ETS-configurable scenes/presets)
    class SceneManager* _sceneManager;

    // HCL Pixel Transformation Context (passed to Library callback)
    HclTransformContext _hclTransformContext;

    // Global Brightness Control
    uint8_t _globalBrightness = 255; // Global brightness multiplier (0-255, default full)

    // Global Power Control
    bool _globalPowerOn = true; // Global power state (default ON)

// External Relays (max derived from knxprod.h)
#if defined(NEO_KoExternalRelay4) && defined(NEO_KoExternalRelay1)
    static constexpr uint8_t kMaxExternalRelays =
        (NEO_KoExternalRelay4 >= NEO_KoExternalRelay1)
            ? (uint8_t)(NEO_KoExternalRelay4 - NEO_KoExternalRelay1 + 1)
            : 0;
#elif defined(NEO_KoExternalRelay1)
    static constexpr uint8_t kMaxExternalRelays = 1;
#else
    static constexpr uint8_t kMaxExternalRelays = 0;
#endif
    static constexpr uint8_t kRelayStorageSize = (kMaxExternalRelays > 0) ? kMaxExternalRelays : 1;

    struct RelayTimerState
    {
        bool targetState = false;      // Desired output state
        bool pendingChange = false;    // Timer active
        unsigned long triggerTime = 0; // When to execute change (millis)
        unsigned long lastOffTime = 0; // Last off timestamp (for min off-time protection)
    };

    uint8_t _relayCount = 0;
    uint8_t _relayPins[kRelayStorageSize] = {255};
    bool _relayStates[kRelayStorageSize] = {false};
    bool _relayInverted[kRelayStorageSize] = {false};
    RelayTimerState _relayTimers[kRelayStorageSize];

    // HCL (Human Centric Lighting) / Circadian whitepoint control.
    // Applies Kelvin correction mainly to white/low-saturation pixels without overwriting
    // effect/scene colors; optionally extracts the neutral component into W for RGBW/RGBCCT.
    bool _hclModeEnabled = false;      // Whether HCL post-processing is active
    uint16_t _hclTargetKelvin = 6500;  // Target Kelvin (from KO / curve)
    uint16_t _hclAppliedKelvin = 6500; // Slewed Kelvin applied to output (for TransitionTime)
    unsigned long _lastHclApplyMs = 0; // Rate limiting for post-processing

    // Performance & Rate Limiting

    // Hardware Configuration Mismatch Detection
    bool _hwConfigMismatch = false;           // True if ETS HW ID doesn't match compiled DEVICE_HW_ID
    unsigned long _lastHwMismatchWarning = 0; // Timestamp of last warning

    // Blink code tracking (only the most severe error is shown)
    uint8_t _activeErrorCode = 0;                             // Currently active error blink code (0 = none)
    uint8_t _activeWarnCode = 0;                              // Currently active warning blink code (0 = none)
    uint8_t _warnCodeBeforeAbl = 0;                           // Warning code hidden by an active ABL warning (restored when it ends)
    unsigned long _lastColorUpdateMs = 0;                     // Last color correction update timestamp
    static const unsigned long COLOR_UPDATE_INTERVAL_MS = 50; // Update color every 500ms to reduce logging overhead

    // Flash persistence handler
    class NeoPixelFlashPersistence* _flashPersistence;

    // Power monitoring for KO updates
    unsigned long _lastPowerMonitoringMs = 0;   // Last time power stats were sent
    uint32_t _powerMonitoringIntervalMs = 5000; // Send power stats every 5 seconds

    // Power monitoring KO round: values sampled every 5 s, sent over the next loop passes as status
    // credits allow; an unfinished round continues at its next KO in the following round.
    struct PowerKoEntry
    {
        GroupObject* ko = nullptr;
        double value = 0.0;
        Dpt type;
        double minDelta = 0.0;
    };
    static constexpr uint8_t kPowerKoMax = 3 + 3 * NEOPIXEL_MAX_PHYSICAL_STRIPS; // totals + 3 per strip
    PowerKoEntry _powerKos[kPowerKoMax];
    uint8_t _powerKoCount = 0; // entries of the current round
    uint8_t _powerKoStart = 0; // entry the round starts at (rotates across rounds)
    uint8_t _powerKoDone = 0;  // entries handled in the current round
    bool _powerKoDark = false; // round sampled while global OFF keeps every segment dark: report 0

    // Configuration & Setup
    void configureFromETS(bool physicalOnly = false); // reads ETS params and builds phys+virt layout
    bool executeEmChainAction(uint8_t action, int arg1, int arg2);
    // Console EM/cue authoring (runtime-only; lost on reboot — ETS is the permanent source)
    bool executeCueSet(uint8_t emId, uint8_t cueNum, uint8_t effectId,
                       uint16_t durSec, uint16_t fadeMs,
                       uint8_t bri, uint8_t r, uint8_t g, uint8_t b);
    bool executeCueParam(uint8_t emId, uint8_t cueNum, uint8_t paramIdx, uint8_t value);
    bool executeCueText(uint8_t emId, uint8_t cueNum, const char* text);
    int  bindConsoleSegment(int managerSegIdx);

    // Console data providers (rendering happens in OFM, see NeoPixelEmConsole.h)
    int emConsoleSegmentCount() const;
    bool emConsoleGetEmStatus(uint8_t seg, NeoEmSegStatus& out);
    const EffektManagerData* emConsoleGetEmData(uint8_t emId) const;
    bool emConsoleGetChainStatus(uint8_t seg, NeoChainSegStatus& out) const;

    // OAM-owned console command: dump persisted flash contents (parsing + rendering
    // both here, since flash persistence is purely an OAM concern). onlySeg=-1 = all.
    void printFlashStateTable(int onlySeg);

    void initializeGpioPins();             // Initialize all configured GPIO pins to LOW before strip creation
    void configurePowerManagement();       // Configure power management using OFM PowerManager
    void configureStripOptions();          // Configure swap and skip options from ETS parameters
    void configureSegments();              // Configure segments from ETS parameters
    void configureVirtualStripOrder();     // Configure virtual strip order from ETS parameters
    void createDefaultVirtualStripOrder(); // Create default sequential strip order
    void createVirtualStripWithOrder();    // creates virtual strip with user-defined order

    // Power Management Implementation
    uint16_t calculateCurrentConsumption() const;
    uint8_t calculateBrightnessLimit() const;
    void applyPowerLimiting();

    // Segment Implementation
    void createSegments();            // Create segments on virtual strip
    void applySegmentConfiguration(); // Apply segment-specific settings (grouping, spacing, reverse, mirror)
    void applySegmentConfiguration(size_t segmentIndex, const SegmentConfig& config);

    // ── Effektkette ──────────────────────────────────────────────────────
    static constexpr uint32_t kSyncHeartbeatMs = 3000; ///< master re-sends an unchanged payload after this time
    static constexpr uint32_t kSyncMinGapMs = 300;     ///< master: minimum gap between change telegrams
    void composeSyncPayload(size_t segmentIndex, uint8_t* payload); ///< 9-byte master state snapshot
    void composeLiveSyncPayload(size_t segmentIndex, uint8_t* payload); ///< bytes 1..8 from the live segment
    void sendSyncTelegram(size_t segmentIndex);
    void receiveSyncTelegram(size_t segmentIndex, const uint8_t* payload, uint8_t len);
    void loopSyncMaster(); ///< Master: detect state changes (colour/speed/brightness/power) and send sync
    void loopSyncWatchdog();

    // ── Effektmanager ─────────────────────────────────────────────────────
    // Heap-allocated: 16 × (20 + 99×48) = ~76 KB — too large for static BSS on classic ESP32 (dram0 overflow).
    // Allocated lazily in setup() via new(std::nothrow), NOT as a default member initializer:
    // a ~76 KB new[] during C++ static-init aborts the boot on low-DRAM ESP32 (e.g. ESP32-WROOM
    // without PSRAM). nullptr until setup(); all users must null-check.
    EffektManagerData* _emData = nullptr; ///< Global EM definitions (16 EMs, loaded from ETS)
    // Console authoring: cue text longer than the 14-byte EffektCue field. Keyed by
    // (emId<<8|cueNum); re-applied to the segment by applyCueLongText() when that cue
    // becomes active (Scroll Text renders up to ~240 chars). RAM-only (lost on reboot).
    std::map<uint16_t, std::string> _cueLongText;
    void loopEffektManager();             ///< Called from loop() — ticks all active EMs
    void applyCueLongText(SegmentConfig& cfg); ///< Re-apply console long cue-text on a cue switch
    void loadEffektManagerFromETS();      ///< Load global EM definitions from ETS parameter blocks
    void startEffektManager(size_t segmentIndex, uint8_t emId);
    void startEffektManagerFresh(size_t segmentIndex, uint8_t emId);
    void stopEffektManager(size_t segmentIndex);
    void pauseEffektManager(size_t segmentIndex);
    void resumeEffektManager(size_t segmentIndex);
    void interruptEmIfRunning(size_t segmentIndex); ///< direct manual KO takes over a running EM
    void finishEmStop(size_t segmentIndex, uint8_t mode); ///< after an EM stop/finish: return mode + segment power
    void applySegmentDefaultState(size_t segmentIndex); ///< re-apply the segment's ETS default (effect+color+brightness)

    // ── Segment power model ───────────────────────────────────────────────
    // Visible = savedPower 1 and not suspended by global power. Off (own or global) = Segment::stop().
    static bool isSegmentVisible(const SegmentConfig& cfg) { return cfg.savedPower != 0 && !cfg.globalSuspended; }
    bool anySegmentVisible() const;
    void segmentOff(size_t segmentIndex, bool show); ///< own power OFF: pause the EM, stop and clear the segment
    void segmentOn(size_t segmentIndex);             ///< own power ON without the power-on EM edge
    void applyStartupColors(Segment* segment);
    void captureDirectColor(size_t segmentIndex);    ///< live primary colour -> saved* (no EM running)
    static void setPrimaryColorOneWhite(Segment* seg, uint8_t r, uint8_t g, uint8_t b, uint8_t w); ///< one W value, split on RGBCCT
    void segmentPowerOn(size_t segmentIndex);        ///< ON command: power-on EM on the off->on edge, else segmentOn()
    void showSegmentOutput(size_t segmentIndex);     ///< make a segment with power ON visible (resume EM or effect)
    void sceneSwitchOn(size_t segmentIndex);         ///< after a static scene/preset is applied: switch on and re-enter
    bool startPowerOnEm(size_t segmentIndex, uint8_t emId); ///< fresh start of the power-on EM; false if refused
    void reEnterEffect(size_t segmentIndex);         ///< restart the current effect (nullptr: saved or ETS effect)
    void applyBrightnessIntent(size_t segmentIndex); ///< master = (intent, 0 -> 255) x global; render or EM re-derive
    void hideIfNotVisible(size_t segmentIndex);      ///< after an effect/cue change: keep a hidden segment dark
    void globalSuspendSegment(size_t segmentIndex);  ///< global OFF step for one segment (no show)
    void globalResumeSegment(size_t segmentIndex);   ///< global ON step for one segment
    void captureSyncFreeze(size_t segmentIndex);     ///< Effektkette master: freeze the direct state before an EM run
    uint8_t effectiveStartupBehavior(size_t segmentIndex); ///< 0=Aus, 1=Letzter Zustand, 2=An (ETS-Werte) (segment overrides global)
    void sendEmStatusKOs(size_t segmentIndex);
    void sendEffectTextStatusKO(size_t segmentIndex);
    uint8_t configuredBootEm(size_t segmentIndex);
    uint8_t configuredPowerOnEm(size_t segmentIndex);
    void refreshHclMasterStateCache();                       // Update cached LightManager values + status KOs
    SegmentConfig createSegmentConfig(uint8_t segmentIndex); // Create segment config from ETS

    // Effect Implementation (delegated to EffectConfiguration)
    // Note: configureEffects(), getEffectFromType(), setupEffectConfiguration() moved to EffectConfiguration class
    // Private wrapper for configureFromETS()
    void configureEffects();

    // Power Management Helpers
    static uint16_t calculateLedCurrentMa(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0);
    static uint16_t getTypicalLedCurrentMa(LedProtocol protocol);

    // Power Monitoring
    void loopPowerMonitoring(bool dark);   // Start a round every 5 s, send its pending telegrams
    void sendPowerMonitoringKOs(bool dark); // Sample a power KO round (dark: report 0)
    void queuePowerKo(GroupObject& ko, double value, const Dpt& type, double minDelta);
    void drainPowerKos();                   // Send pending round telegrams while status credits last
    static KNXValue powerKoValue(double value, const Dpt& type);
    void clearAblWarning();        // End an active ABL warning
    LedCurrentProfile powerMeasureProfile(uint8_t currentPerLed, bool perLedMode) const; // LED profile for power measurement
    // Power monitoring KO: sent on the first value, a drop to 0 or a change >= minDelta; one status credit each
    bool sendPowerKo(GroupObject& ko, const KNXValue& value, const Dpt& type, double minDelta);
    static double powerCurrentMinDelta(uint32_t limitMa); // max(50 mA, 2 % of the limit)
    static constexpr double kPowerLoadMinDelta = 2.0;     ///< percentage points
    static constexpr double kPowerWattsMinDelta = 1.0;    ///< W

    // Blink code helpers
    void setErrorBlink(uint8_t code);                          // Set error on STATUS LED (Red + errorCode) — lowest code wins
    void setWarningBlink(uint8_t code);                        // Set warning on STATUS LED (pulsing); color derived from code
    static OpenKNX::Led::Color warnColorForCode(uint8_t code); // Single source of truth: warn code -> color
    void clearBlinkCodes();                                    // Reset all blink codes and turn off STATUS LED

    // Blink code state (for testing / diagnostics)
    uint8_t getActiveErrorCode() const { return _activeErrorCode; }
    uint8_t getActiveWarnCode() const { return _activeWarnCode; }

    // Strip Configuration Helpers
    static uint8_t mapSwapMode(uint8_t paramValue); // Map ETS swap parameter
    void swapChannels(uint8_t& ch1, uint8_t& ch2);  // Helper for channel swapping

    // Relay Helpers
    void setRelayOutput(uint8_t relayIndex, bool state);
    void scheduleRelayChange(uint8_t relayIndex, bool targetState);
    void processRelayTimers();

    /// Push the virtual buffer to the strips, or defer it while a transfer is running:
    /// syncToPhysical() would skip the busy strips and send only part of the frame.
    void showVirtualStripIfIdle();

    bool _virtualShowPending = false; ///< a send was deferred; loop() retries until it goes out
    bool _renderPaused = true;        ///< loop() skipped rendering (startup delay, nothing visible under global OFF)
    bool _startupRestoreDone = false; ///< restore and boot EM after the startup delay have run
    static constexpr uint8_t kDeferredKoMax = 128;     ///< KO updates kept during the startup delay
    static constexpr uint8_t kDeferredKoPerLoop = 8;   ///< processed per loop() pass afterwards
    static constexpr uint8_t kDeferredKoValueMax = 14; ///< largest NeoPixel input KO (14 bytes, DPT 16)
    struct DeferredKo
    {
        uint16_t number;
        uint8_t size;
        uint8_t value[kDeferredKoValueMax];
    };
    DeferredKo _deferredKo[kDeferredKoMax] = {};       ///< in arrival order, with the received value
    uint8_t _deferredKoHead = 0;                       ///< next entry to process
    uint8_t _deferredKoCount = 0;                      ///< end of the queue
    bool _deferredKoOverflow = false;
    bool _replayingDeferred = false;                   ///< processInputKo() called from the queue
    bool _drainGate = false;                           ///< startup backlog: no frame / sync until it is processed
    uint8_t _drainPassesLeft = 0;                      ///< bound for the gate (passes)
    bool deferKo(GroupObject& ko, uint16_t koNumber);  ///< queue a NeoPixel input telegram
    void replayDeferredKos(uint8_t maxCount);          ///< process queued telegrams, oldest first

    // Status objects: one item routine per scope (module / segment) computes each status KO from
    // the live state (CCT: last command) and writes it in one of three modes.
    enum class StatusMode : uint8_t
    {
        Seed,     ///< startup: fill a never-written KO without a telegram, otherwise as OnChange
        OnChange, ///< send only when the encoded value changes
        Always    ///< send the current value
    };
    enum ModuleStatusItem : uint8_t
    {
        kModPower,
        kModBrightness,
        kModRelay1,
        kModRelay2,
        kModRelay3,
        kModRelay4,
        kModHclGlobal,
        kModuleStatusCount
    };
    enum SegmentStatusItem : uint8_t
    {
        kSegPower,
        kSegBrightness,
        kSegFx,
        kSegRgb,
        kSegRgbw,
        kSegHsv,
        kSegHcl,
        kSegEffectText,
        kSegEmStatus,
        kSegEmCue,
        kSegEmRunState,
        kSegCct,
        kSegmentStatusCount
    };
    static constexpr uint8_t kStatusSegmentMax = 16;   ///< segment KO channels in the KO table (NEO_ChannelCount)
    static constexpr uint32_t kStatusPaceMs = 100;     ///< one status telegram credit per 100 ms (TP: ~20-40 ms each)
    static constexpr uint8_t kStatusBurst = 8;         ///< credits that may go out at once (TP queue: 44 low-priority frames)

    bool writeStatus(GroupObject& ko, const KNXValue& value, const Dpt& type, StatusMode mode);
    bool isSegmentLit(SegmentConfig& cfg);
    static bool segmentShowsLight(Segment* seg); ///< any LED of the segment not black in the virtual buffer
    bool syncModuleStatusItem(uint8_t item, StatusMode mode);
    bool syncSegmentStatusItem(size_t segmentIndex, uint8_t item, StatusMode mode);
    void syncModuleStatus(StatusMode mode);
    void syncSegmentStatus(size_t segmentIndex, StatusMode mode);
    void seedStatus();                                  ///< end of processAfterStartupDelay(): fill every status KO
    void queueStatus(uint8_t scope, bool always);       ///< scope 0 = module, 1+n = segment n; sent paced by processStatusQueue()
    void queueAllSegmentStatus(bool always);
    void statusAfterCommand(uint8_t scope);           ///< after a bus command: queue OnChange; a queued (startup) telegram gets the full status
    void queueStatusItem(uint8_t scope, uint8_t item, bool always);
    bool serveStatusQueue(uint16_t* queue, bool always, uint8_t& cursorScope, uint8_t& cursorItem);
    void processStatusQueue();                          ///< loop(): status telegrams within the credit budget
    uint8_t statusScopeCount() const;

    static constexpr uint8_t kStatusScopeMax = 1 + kStatusSegmentMax;
    bool _statusSeeded = false;                    ///< no status KO is written before seedStatus()
    uint16_t _statusQueueAlways[kStatusScopeMax] = {}; ///< per scope, bit = item: send the value
    uint16_t _statusQueueChange[kStatusScopeMax] = {}; ///< per scope, bit = item: send if changed
    uint8_t _statusChangeScope = 0;                ///< round-robin cursor of the change queue
    uint8_t _statusChangeItem = 0;
    uint8_t _statusAlwaysScope = 0;                ///< round-robin cursor of the send-all queue
    uint8_t _statusAlwaysItem = 0;
    bool _statusAlwaysTurn = false;                ///< alternate the queues so neither starves
    uint8_t _statusCredits = kStatusBurst;
    uint32_t _statusCreditMs = 0;
    uint16_t _hclGlobalStatusKelvin = 0;                     ///< global HCL Kelvin reported on KO 407
    uint16_t _hclSegmentStatusKelvin[kStatusSegmentMax] = {}; ///< per-segment HCL Kelvin reported on HCLState

    // LED shutdown (ETS: ParamNEO_NEOTurnOffBeforeRestart)
    bool prepareLedShutdown(); ///< Stop effects, drop relays, blank the virtual buffer
    void pumpLedShutdown();    ///< Non-blocking sequencer, one strip per loop() pass

    static constexpr uint32_t kLedShutdownTimeoutMs = 2000; ///< Per strip: give up on a wedge

    bool _turnOffBeforeRestart = false; ///< Cached at setup(); the parameter reads 0 once tables unload
    bool _ledShutdownActive = false;
    uint32_t _ledShutdownIndex = 0;
    uint32_t _ledShutdownStartMs = 0;
};

extern NeoPixelBusModule openknxNeoPixelModule;
