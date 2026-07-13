#include "NeoPixelModule.h"
#include "ColorManagement.h"
#include "EffectConfiguration.h"
#include "HardwareMappingLogic.h"
#include "SceneManager.h"
#include "SegmentController.h"
#include "StripConfiguration.h"

// Include Timer module for astronomical sunrise/sunset calculations (if available)
#if defined(__has_include)
    #if __has_include("Timer.h")
        #define OPENKNX_TIMER_MODULE
        #include "Timer.h"
    #else
        #warning "Timer module not found - Sun position-based HCL curve will not work without sunrise/sunset times from OAM layer"
    #endif
#else
    #warning "Timer module not found - Sun position-based HCL curve will not work without sunrise/sunset times from OAM layer"
#endif

// Include generated effect parameters if available
#if defined(__has_include)
    #if __has_include("EffectParameterMapping.h")
        #define EFFECT_PARAMETER_MAPPING_GENERATED
        #include "EffectParameterMapping.h"
        #include "EffectTypeMapping.h"
    #endif
#else
    // Fallback for older compilers: can define via build flags
    #ifdef EFFECT_PARAMETER_MAPPING_GENERATED
        #include "EffectParameterMapping.h"
        #include "EffectTypeMapping.h"
    #endif
#endif

#include "HCL/LightManagerApi.h"
#include "NeoPixelFlashPersistence.h"
#include "OpenKNX.h"
#include "PhysicalStripConfig.h"
#include "colorhelper.h"
#include "knxprod.h"
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <new>
#include <string>
#include <vector>
// #define NEOPIXEL_MODULE_TEST_ENV
#ifdef NEOPIXEL_MODULE_TEST_ENV
    #include "test/test.h"
#endif

// DPT 3.007 step code to delta mapping (exponential)
// Standard KNX/ETS: stepCode 1 = largest (100%), 7 = smallest (~2%), 0 = stop
static inline int16_t dpt3_007_delta(uint8_t stepCode)
{
    static const uint8_t factors[8] = {0, 64, 32, 16, 8, 4, 2, 1};
    return (int16_t)((factors[stepCode & 0x07] * 255u + 32u) / 64u);
}

// Helper: Start/stop dimming for a segment channel
static void startStopDimming(NeoPixelBusModule::SegmentConfig& config,
                             NeoPixelBusModule::SegmentConfig::DimmingChannel channel,
                             uint8_t rel, uint8_t segmentIndex)
{
    // Decode ETS values: 1-7=start decreasing, 8-9=stop, 10-16=start increasing
    if (rel >= 1 && rel <= 16)
    {
        if (rel == 8 || rel == 9)
        {
            // Stop: stepCode 0
            config.activeDimming = NeoPixelBusModule::SegmentConfig::NONE;
            config.dimmingStepCode = 0;
        }
        else if (rel >= 10 && rel <= 16)
        {
            // Start/continue increasing
            uint8_t stepCode = rel - 9; // 10→1, 11→2, ..., 16→7
            config.activeDimming = channel;
            config.dimmingIncrease = true;
            config.dimmingStepCode = stepCode;
            config.dimmingLastUpdate = millis();
            config.dimmingNextStep = millis();
        }
        else
        { // 1-7
            // Start/continue decreasing
            config.activeDimming = channel;
            config.dimmingIncrease = false;
            config.dimmingStepCode = rel; // 1→100%, 7→~2%
            config.dimmingLastUpdate = millis();
            config.dimmingNextStep = millis();
        }
    }
}

static uint8_t resolveHclMaster(uint8_t requestedMaster, uint8_t availableMasterCount)
{
    if (requestedMaster == 0 || requestedMaster > availableMasterCount)
    {
        return 0;
    }

    return requestedMaster;
}

NeoPixelBusModule openknxNeoPixelModule;

// ============================================================================
// Console bridge (NeoPixelEmConsole.h): the OFM renders all neo em/cue/chain
// output; the OAM only provides raw data and executes mutating actions.
// ============================================================================

int openknxNeoPixelEmSegmentCount()
{
    return openknxNeoPixelModule.emConsoleSegmentCount();
}

bool openknxNeoPixelGetEmStatus(uint8_t seg, NeoEmSegStatus& out)
{
    return openknxNeoPixelModule.emConsoleGetEmStatus(seg, out);
}

const EffektManagerData* openknxNeoPixelGetEmData(uint8_t emId)
{
    return openknxNeoPixelModule.emConsoleGetEmData(emId);
}

bool openknxNeoPixelGetChainStatus(uint8_t seg, NeoChainSegStatus& out)
{
    return openknxNeoPixelModule.emConsoleGetChainStatus(seg, out);
}

bool openknxNeoPixelHandleEmChainAction(uint8_t action, int arg1, int arg2)
{
    return openknxNeoPixelModule.executeEmChainAction(action, arg1, arg2);
}

bool openknxNeoPixelHandleCueSet(uint8_t emId, uint8_t cueNum, uint8_t effectId,
                                 uint16_t durSec, uint16_t fadeMs,
                                 uint8_t bri, uint8_t r, uint8_t g, uint8_t b)
{
    return openknxNeoPixelModule.executeCueSet(emId, cueNum, effectId, durSec, fadeMs, bri, r, g, b);
}

bool openknxNeoPixelHandleCueParam(uint8_t emId, uint8_t cueNum, uint8_t paramIdx, uint8_t value)
{
    return openknxNeoPixelModule.executeCueParam(emId, cueNum, paramIdx, value);
}

bool openknxNeoPixelHandleCueText(uint8_t emId, uint8_t cueNum, const char* text)
{
    return openknxNeoPixelModule.executeCueText(emId, cueNum, text);
}

NeoPixelBusModule::NeoPixelBusModule()
    : _flashPersistence(nullptr), _effectConfiguration(nullptr), _colorManagement(nullptr), _segmentController(nullptr), _sceneManager(nullptr)
{
    _flashPersistence = new NeoPixelFlashPersistence(this);
    _effectConfiguration = new EffectConfiguration(this);
    _colorManagement = new ColorManagement(this);
    _segmentController = new SegmentController(this);
    _sceneManager = new SceneManager(this);
}

NeoPixelBusModule::~NeoPixelBusModule()
{
    delete _flashPersistence;
    delete _effectConfiguration;
    delete _colorManagement;
    delete _segmentController;
    delete _sceneManager;
    delete[] _emData;
}

void NeoPixelBusModule::setup(bool configured)
{
    // Allocate the Effektmanager data table here (heap ready), NOT as a global static
    // initializer: this ~8 KB new[] (16 EMs x ~500 B) at C++ static-init time can abort the
    // boot on low-DRAM ESP32 (e.g. ESP32-WROOM without PSRAM). new(std::nothrow) degrades
    // gracefully on tight memory:
    // the device still boots and runs, only the Effektmanager is unavailable.
    if (!_emData)
    {
        _emData = new (std::nothrow) EffektManagerData[EM_COUNT]();
        if (!_emData)
            logErrorP("Effektmanager disabled: out of memory for EM table (%u bytes)",
                      (unsigned)(EM_COUNT * sizeof(EffektManagerData)));
    }

#ifdef NEOPIXEL_MODULE_TEST_ENV
    // Test mode: Initialize core module first, then setup test environment
    if (!_neoPixel.isInitialized())
    {
        _neoPixel.init();
        _neoPixel.setup(true); // Must call setup() to set _initialized flag
    }
    setup_test_environment(_neoPixel);
    _initialized = true;
    #ifdef NEOPIXEL_BLINK_CODE_TEST
    test_blink_codes(*this);
    #endif
#else
    if (configured)
    {
        // Initialize NeoPixel library first (creates manager)
        if (!_neoPixel.isInitialized())
        {
            _neoPixel.init();
        }

        // Configure strips, segments and effects from ETS parameters
        configureFromETS();
        _initialized = true;
        // Initialize all physical strips (PIO, DMA, etc.) - MUST be after configureFromETS()
        // which creates the PhysicalStrip objects, but BEFORE any show() calls
        _neoPixel.setup(configured);

        // Setup HCL pixel transformation context after strips and segments are available.
        updateHclTransformContext(); // Setup HCL pixel transformation callback

        // Now that hardware is initialized, clear LEDs if requested during configuration
        if (_clearLedsAfterSetup && _virtualStrip)
        {
            _virtualStrip->show();
            logInfoP("LEDs cleared - will restore saved state after startup delay");
            _clearLedsAfterSetup = false;
        }

        // Initialize HCL status KOs from the current LightManager snapshot.
        refreshHclMasterStateCache();
        sendPowerMonitoringKOs(); // Update Power monitoring KOs
    }
    else
    {
        logInfoP("NeoPixelBusModule setup skipped - not configured");
        // Initialize GPIOs early to prevent floating states and send LED RESET signals
        // NOTE: This only sets pins LOW - LEDs retain their color until proper protocol
        //       data is sent (happens in configureFromETS() after strip creation)
        initializeGpioPins();
    }
#endif
}

void NeoPixelBusModule::loop(bool configured)
{
    // _localTestMode ('neo init') lets the loop run without an ETS download for bench tests.
    if ((!configured && !_localTestMode) || !_initialized) return;


    // Check for hardware configuration mismatch and warn periodically
    if (_hwConfigMismatch)
    {
        unsigned long now = millis();
        if (now - _lastHwMismatchWarning >= 5000)
        { // Every 5 seconds
            _lastHwMismatchWarning = now;
#ifdef DEVICE_HW_ID
    #ifdef ParamNEO_NeoPixelHardwareSelect
            uint16_t selectedHwId = (uint16_t)ParamNEO_NeoPixelHardwareSelect; // stored selection = DEVICE_HW_ID

            if (selectedHwId == 255)
            {
                // ETS has dummy value - user hasn't configured hardware yet
                const char* compiledHwName = HardwareMapping::getHardwareName(HardwareMapping::mapDeviceHwIdToIndex(DEVICE_HW_ID));
                logErrorP("*** No hardware configured! Use 'Hardware auto-detect' button in ETS or manually select: %s (ID: 0x%04X)",
                          compiledHwName ? compiledHwName : "Unknown", DEVICE_HW_ID);
            }
            else
            {
                // Hardware configured but mismatched - compare the stable DEVICE_HW_IDs directly
                const char* compiledHwName = HardwareMapping::getHardwareName(HardwareMapping::mapDeviceHwIdToIndex(DEVICE_HW_ID));
                const char* etsHwName = HardwareMapping::getHardwareName(HardwareMapping::mapDeviceHwIdToIndex(selectedHwId));
                logErrorP("!!! HW MISMATCH: Firmware=%s (ID 0x%04X) vs ETS=%s (ID 0x%04X) - Update ETS config!",
                          compiledHwName ? compiledHwName : "Unknown", DEVICE_HW_ID,
                          etsHwName ? etsHwName : "Unknown", selectedHwId);
            }
    #endif
#endif
        }
    }

    // ETS/KO-dependent housekeeping. These publish/read KNX group objects and assume
    // a loaded KO table + ETS configuration. In _localTestMode ('neo init', no ETS
    // download) the KO table is NOT loaded, so touching KOs faults — skip them and
    // only run the rendering path below.
    if (!_localTestMode)
    {
        // Update LightManager-backed HCL snapshots and publish status KOs.
        refreshHclMasterStateCache();

        // Process relay timers for delayed on/off switching
        processRelayTimers();

        // Effektkette: check slave watchdog timeouts + master change detection
        loopSyncWatchdog();
        loopSyncMaster();
    }

    // Effektmanager: tick all active sequencers (no-op in local test mode — EMs are ETS-only)
    loopEffektManager();

    // If global power is OFF, skip all effect processing and pixel updates
    if (!_globalPowerOn) return;

    // Process active DPT 3.007 start/stop dimming
    processActiveDimming();

    // Call library loop() for auto-update timer and effect processing
    _neoPixel.loop(configured);

    // Send power monitoring KOs periodically (KO writes — skip without ETS config)
    if (!_localTestMode)
    {
        unsigned long now = millis();
        if (now - _lastPowerMonitoringMs >= _powerMonitoringIntervalMs)
        {
            _lastPowerMonitoringMs = now;
            // logDebugP("Power monitoring update: Sending KOs");
            sendPowerMonitoringKOs();
        }
    }
}

void NeoPixelBusModule::processBeforeRestart()
{
    // Check if LED shutdown is enabled in ETS
    if (!ParamNEO_NEOTurnOffBeforeRestart)
    {
        logInfoP("LED shutdown before restart is disabled in ETS");
        return;
    }

    // Turn off all LEDs before ETS programming or device restart
    logInfoP("Turning off all LEDs before restart/programming");

    // Turn off external relays for safety
    if (_relayCount > 0)
    {
        for (uint8_t i = 0; i < _relayCount; ++i)
        {
            setRelayOutput(i, false);
        }
    }

    if (!_initialized) return;

    // Stop all effects and clear all LEDs
    for (auto& segConfig : _segments)
    {
        if (segConfig.segment && segConfig.segment->hasEffect())
        {
            segConfig.segment->clearEffect(); // Remove effect (stops further updates)
        }
    }

    // Use library method to clear and show all LEDs
    _neoPixel.clearAll();  // Clear all segments
    _neoPixel.updateAll(); // Send to hardware

    // Wait for DMA completion before restart (max 100ms timeout)
    if (_virtualStrip)
    {
        _virtualStrip->waitForCompletion(100);
    }
}

void NeoPixelBusModule::processBeforeTablesUnload()
{
    // Let's do for now the same as processBeforeRestart
    processBeforeRestart();
}

// ============================================================================
// Flash State Persistence - Delegate to NeoPixelFlashPersistence
// ============================================================================

uint16_t NeoPixelBusModule::flashSize()
{
    return _flashPersistence ? _flashPersistence->calculateFlashSize() : 0;
}

void NeoPixelBusModule::writeFlash()
{
    if (_flashPersistence)
    {
        _flashPersistence->writeToFlash();
    }
}

void NeoPixelBusModule::readFlash(const uint8_t* data, const uint16_t size)
{
    if (_flashPersistence)
    {
        _flashPersistence->readFromFlash(data, size);
    }
}

void NeoPixelBusModule::processAfterStartupDelay()
{
    // ETS base effect configuration is already applied in configureEffects().
    // StartupEM (per segment) must now be evaluated and started afterwards,
    // so it can intentionally override the static ETS base effect.
    if (_segments.empty())
    {
        logInfoP("Startup complete - ETS configuration active");
        return;
    }

    // Restore last runtime state ("Letzter Zustand") from flash onto the live segments; must run
    // here so the restored effect/colour/brightness become the ACTIVE state (reported by ETS
    // sync-back readback). Only acts on segments set to "Letzter Zustand"; StartupEM below is
    // evaluated afterwards and can override it (priority EM > Effekt > Solid).
    if (_flashPersistence)
    {
        _flashPersistence->restoreStatesAfterStartup();
    }

    uint8_t previousChannel = _channelIndex;
    for (size_t i = 0; i < _segments.size(); ++i)
    {
        auto& cfg = _segments[i];
        if (!cfg.segment) continue;

        _channelIndex = static_cast<uint8_t>(i);
        uint8_t startupEmId = static_cast<uint8_t>(ParamNEO_NEOStartupEM);

        if (startupEmId == EM_NONE) continue;

        startEffektManager(i, startupEmId);

        if (cfg.emController.activeEmId() == startupEmId)
        {
            logInfoP("Segment %d: Startup-EM %d gestartet (ueberschreibt Basis-Effekt)",
                     static_cast<int>(i) + 1, static_cast<int>(startupEmId));
        }
        else
        {
            logWarningP("Segment %d: Startup-EM %d konnte nicht gestartet werden (EM aktiv/konfiguriert?)",
                        static_cast<int>(i) + 1, static_cast<int>(startupEmId));
        }
    }
    _channelIndex = previousChannel;

    logInfoP("Startup complete - ETS configuration active");
}

// Process active start/stop dimming for all segments
void NeoPixelBusModule::processActiveDimming()
{
    _segmentController->processActiveDimming();
}

void NeoPixelBusModule::setRelayOutput(uint8_t relayIndex, bool state)
{
    if (relayIndex >= kRelayStorageSize) return;
    if (_relayCount == 0 || relayIndex >= _relayCount) return;

    uint8_t pin = _relayPins[relayIndex];
    if (pin == 255) return;

    _relayStates[relayIndex] = state;
    bool physicalState = _relayInverted[relayIndex] ? !state : state;
    digitalWrite(pin, physicalState ? HIGH : LOW);

    // Track off-time for minimum off-time protection
    if (!state)
    {
        _relayTimers[relayIndex].lastOffTime = millis();
    }

// Send status feedback to KNX bus
#if defined(NEO_KoExternalRelay1State)
    if (relayIndex == 0)
    {
        bool changed = KoNEO_ExternalRelay1State.valueNoSendCompare(state, DPT_Switch);
        if (changed) KoNEO_ExternalRelay1State.objectWritten();
    }
#endif
#if defined(NEO_KoExternalRelay2State)
    if (relayIndex == 1)
    {
        bool changed = KoNEO_ExternalRelay2State.valueNoSendCompare(state, DPT_Switch);
        if (changed) KoNEO_ExternalRelay2State.objectWritten();
    }
#endif
#if defined(NEO_KoExternalRelay3State)
    if (relayIndex == 2)
    {
        bool changed = KoNEO_ExternalRelay3State.valueNoSendCompare(state, DPT_Switch);
        if (changed) KoNEO_ExternalRelay3State.objectWritten();
    }
#endif
#if defined(NEO_KoExternalRelay4State)
    if (relayIndex == 3)
    {
        bool changed = KoNEO_ExternalRelay4State.valueNoSendCompare(state, DPT_Switch);
        if (changed) KoNEO_ExternalRelay4State.objectWritten();
    }
#endif

    logDebugP("Relay %d set to %s (GPIO %d)", relayIndex + 1, state ? "ON" : "OFF", pin);
}

void NeoPixelBusModule::scheduleRelayChange(uint8_t relayIndex, bool targetState)
{
    if (relayIndex >= kRelayStorageSize) return;
    if (_relayCount == 0 || relayIndex >= _relayCount) return;

    // Check minimum off-time (only when turning ON)
    if (targetState && !_relayStates[relayIndex])
    {
        uint16_t minOffSeconds = 0;

#if defined(ParamNEO_NEOExternalRelay1MinOffTime)
        if (relayIndex == 0) minOffSeconds = (uint16_t)ParamNEO_NEOExternalRelay1MinOffTime;
#endif
#if defined(ParamNEO_NEOExternalRelay2MinOffTime)
        if (relayIndex == 1) minOffSeconds = (uint16_t)ParamNEO_NEOExternalRelay2MinOffTime;
#endif
#if defined(ParamNEO_NEOExternalRelay3MinOffTime)
        if (relayIndex == 2) minOffSeconds = (uint16_t)ParamNEO_NEOExternalRelay3MinOffTime;
#endif
#if defined(ParamNEO_NEOExternalRelay4MinOffTime)
        if (relayIndex == 3) minOffSeconds = (uint16_t)ParamNEO_NEOExternalRelay4MinOffTime;
#endif

        if (minOffSeconds > 0 && _relayTimers[relayIndex].lastOffTime != 0)
        {
            unsigned long now = millis();
            unsigned long minOffMillis = minOffSeconds * 1000UL;

            // Handle millis() overflow with signed comparison (same as processRelayTimers)
            long elapsed = (long)(now - _relayTimers[relayIndex].lastOffTime);
            if (elapsed >= 0 && (unsigned long)elapsed < minOffMillis)
            {
                // Min off-time not yet elapsed
                unsigned long remaining = minOffMillis - (unsigned long)elapsed;
                logInfoP("Relay %d: Ignoring ON command (min off-time %ds not elapsed, %lus remaining)",
                         relayIndex + 1, minOffSeconds, remaining / 1000);
                return; // Block turn-on
            }
        }
    }

    // Read delay parameters (in seconds) based on target state
    uint16_t delaySeconds = 0;

#if defined(ParamNEO_NEOExternalRelay1OnDelay)
    if (relayIndex == 0)
    {
        delaySeconds = targetState
                           ? (uint16_t)ParamNEO_NEOExternalRelay1OnDelay
                           : (uint16_t)ParamNEO_NEOExternalRelay1OffDelay;
    }
#endif
#if defined(ParamNEO_NEOExternalRelay2OnDelay)
    if (relayIndex == 1)
    {
        delaySeconds = targetState
                           ? (uint16_t)ParamNEO_NEOExternalRelay2OnDelay
                           : (uint16_t)ParamNEO_NEOExternalRelay2OffDelay;
    }
#endif
#if defined(ParamNEO_NEOExternalRelay3OnDelay)
    if (relayIndex == 2)
    {
        delaySeconds = targetState
                           ? (uint16_t)ParamNEO_NEOExternalRelay3OnDelay
                           : (uint16_t)ParamNEO_NEOExternalRelay3OffDelay;
    }
#endif
#if defined(ParamNEO_NEOExternalRelay4OnDelay)
    if (relayIndex == 3)
    {
        delaySeconds = targetState
                           ? (uint16_t)ParamNEO_NEOExternalRelay4OnDelay
                           : (uint16_t)ParamNEO_NEOExternalRelay4OffDelay;
    }
#endif

    // Cancel any pending timer for this relay
    _relayTimers[relayIndex].pendingChange = false;

    if (delaySeconds == 0)
    {
        // No delay - execute immediately
        setRelayOutput(relayIndex, targetState);
    }
    else
    {
        // Schedule delayed execution
        _relayTimers[relayIndex].targetState = targetState;
        _relayTimers[relayIndex].pendingChange = true;
        _relayTimers[relayIndex].triggerTime = millis() + (delaySeconds * 1000UL);
        logInfoP("Relay %d change to %s scheduled in %d seconds",
                 relayIndex + 1, targetState ? "ON" : "OFF", delaySeconds);
    }
}

void NeoPixelBusModule::processRelayTimers()
{
    if (_relayCount == 0) return;

    unsigned long now = millis();
    for (uint8_t i = 0; i < _relayCount; ++i)
    {
        if (_relayTimers[i].pendingChange)
        {
            // Handle millis() overflow (wraps every ~49 days)
            if ((long)(now - _relayTimers[i].triggerTime) >= 0)
            {
                // Time to execute
                setRelayOutput(i, _relayTimers[i].targetState);
                _relayTimers[i].pendingChange = false;
            }
        }
    }
}

void NeoPixelBusModule::processInputKo(GroupObject& ko)
{
    // Get the KO number for routing to correct channel/segment
    uint16_t koNumber = ko.asap();

    // ── Effektmanager KOs (NEOEM module, KoOffset=1240, BlockSize=4) ─────
#ifdef NEO_KoEmStart
    {
        int emChannel = NEO_KoCalcChannel(koNumber);
        int emKoIndex = (emChannel >= 0) ? NEO_KoCalcIndex(koNumber) : -1;
        // The EM control KOs (NEO_KoEmStart..NEO_KoEmRunState) live in the segment block.
        // Intercept ONLY those here; every other segment KO falls through to the dispatch below.
        if (emKoIndex >= NEO_KoEmStart && emKoIndex <= NEO_KoEmRunState)
        {
            // A locked segment ignores its EM CONTROL KOs — but NOT the internal Effektkette
            // SyncChain telegram (master->slave coordination): dropping sync for a locked slave
            // would desync it and can break the whole chain (slave watchdog timeout).
            if ((size_t)emChannel < _segments.size() && _segments[emChannel].locked)
            {
    #ifdef NEO_KoSyncChain
                if (emKoIndex != NEO_KoSyncChain)
    #endif
                    return;
            }
            switch (emKoIndex)
            {
                case NEO_KoEmStart:
                {
                    uint8_t emId = ko.value(DPT_Value_1_Ucount);
                    logInfoP("Segment %d: EffektManager Start = %d", emChannel, emId);
                    startEffektManager((size_t)emChannel, emId);
                    // A direct EM start makes the EM the active source — drop any persisted scene marker
                    // so "Letzter Zustand" restores the EM, not an older scene (restore checks scene first).
                    // Guard on "THIS emId is now active": a refused start (suspended/disabled/empty) — even
                    // while a DIFFERENT EM keeps running — must NOT wipe the scene marker. EM-action scenes
                    // go through recallScene(), not this KO, so they keep their marker regardless.
                    if (emId != EM_NONE && _segments[emChannel].emController.activeEmId() == emId)
                        _segments[emChannel].savedSceneNumber = 0;
                    break;
                }
                case NEO_KoEmStop:
                {
                    logInfoP("Segment %d: EffektManager Stop", emChannel);
                    stopEffektManager((size_t)emChannel);
                    break;
                }
    #ifdef NEO_KoEmPause
                case NEO_KoEmPause:
                {
                    bool doPause = ko.value(DPT_Switch); // 1 = Pause, 0 = Resume
                    logInfoP("Segment %d: EffektManager %s", emChannel, doPause ? "Pause" : "Resume");
                    if (doPause) pauseEffektManager((size_t)emChannel);
                    else
                        resumeEffektManager((size_t)emChannel);
                    break;
                }
    #endif
    #ifdef NEO_KoEmCueSet
                case NEO_KoEmCueSet:
                {
                    // Jump to a specific cue of the RUNNING EM (input for StateEngine/Logic).
                    // No-op if no EM is running / cue invalid (executeEmChainAction handles bounds).
                    uint8_t cueNum = ko.value(DPT_Value_1_Ucount);
                    logInfoP("Segment %d: Cue setzen = %d", emChannel, cueNum);
                    executeEmChainAction(NEO_EM_CUE, (int)emChannel, (int)cueNum);
                    break;
                }
    #endif
    #ifdef NEO_KoEffectText
                case NEO_KoEffectText:
        #ifdef NEO_KoEffectTextAppend
                case NEO_KoEffectTextAppend:
        #endif
                {
                    // Set/append text of the running effect (any source: ETS, Cue, Scene started it).
                    // EffectText replaces (empty = clear), EffectTextAppend appends 14-char chunks
                    // (long texts up to 240 chars, e.g. assembled via Logikmodul). No-op if the
                    // active effect has no STRING parameter.
                    if ((size_t)emChannel >= _segments.size()) break;
                    Segment* seg = _segments[emChannel].segment;
                    Effect* effect = seg ? seg->getEffect() : nullptr;
                    if (!effect) break;
                    const char* text = ko.value(DPT_String_8859_1);
                    char buf[sizeof(EffectConfig::effectText)];
        #ifdef NEO_KoEffectTextAppend
                    if (emKoIndex == NEO_KoEffectTextAppend)
                    {
                        const char* current = seg->getConfig().effectText;
                        size_t len = strlen(current);
                        if (len >= sizeof(buf) - 1) break; // buffer full, ignore append
                        memcpy(buf, current, len);
                        buf[len] = '\0';
                        if (text) strncpy(buf + len, text, sizeof(buf) - 1 - len);
                        buf[sizeof(buf) - 1] = '\0';
                        text = buf;
                    }
        #endif
                    uint8_t pc = effect->getParameterCount();
                    for (uint8_t i = 0; i < pc; i++)
                    {
                        if (effect->getParameterType(i) == ParameterType::PARAM_STRING)
                        {
                            effect->setParameter(seg, i, (uint32_t)(uintptr_t)text);
                            logInfoP("Segment %d: Effekt-Text = '%s'", emChannel, text ? text : "");
                            sendEffectTextStatusKO((size_t)emChannel);
                            break;
                        }
                    }
                    break;
                }
    #endif
    #ifdef NEO_KoSyncChain
                case NEO_KoSyncChain:
                {
                    // Effektkette: slave receives sync telegram from master
                    if ((size_t)emChannel < _segments.size() && _segments[emChannel].syncMode == 2)
                    {
                        uint8_t raw[9] = {};
                        const uint8_t* dpt = ko.valueRef();
                        for (uint8_t b = 0; b < 9 && b < ko.valueSize(); b++)
                            raw[b] = dpt[b];
                        receiveSyncTelegram((size_t)emChannel, raw, 9);
                    }
                    break;
                }
    #endif
                default:
                    break;
            }
            return; // handled
        }
    }
#endif
    // Global NeoPixel KOs
    if (koNumber == NEO_KoPower)
    {
        bool powerState = ko.value(DPT_Switch);
        logInfoP("Global Power KO: %s", powerState ? "ON" : "OFF");

        // Update global power state flag
        _globalPowerOn = powerState;

        if (powerState == false)
        {
            // Suspend any running EM/chain so it does NOT keep ticking while powered off (a paused
            // EM's tick() is a no-op). suspendedByPower marks these so Power-On resumes exactly
            // them — distinct from a manual Pause or a direct-KO interrupt (which stops).
            for (size_t i = 0; i < _segments.size(); i++)
            {
                auto& cfg = _segments[i];
                if (cfg.segment && cfg.emController.isRunning() && !cfg.emController.isPaused())
                {
                    pauseEffektManager(i);
                    cfg.suspendedByPower = true;
                }
            }
            // Power off - turn off all LEDs immediately by clearing all segments
            if (!_segments.empty() && _virtualStrip)
            {
                // Clear each segment individually
                for (auto& segConfig : _segments)
                {
                    if (segConfig.segment)
                    {
                        segConfig.segment->clearAll(); // Clear RGBW pixels
                    }
                }
                _virtualStrip->show();
            }
        }
        else
        {
            // Power-On resumes EMs that were suspended by a power-off (not manual pauses).
            // suspendedByPower is shared with the per-segment Power KO, so only revive a segment
            // whose OWN power is still ON (savedPower != 0) — otherwise a globally-restored bus would
            // light a segment the user had individually switched off.
            for (size_t i = 0; i < _segments.size(); i++)
            {
                auto& cfg = _segments[i];
                if (cfg.segment && cfg.suspendedByPower && cfg.savedPower != 0)
                {
                    cfg.suspendedByPower = false;
                    resumeEffektManager(i); // resume() also re-applies master brightness
                }
            }
        }

        // Send state feedback
        bool changed = KoNEO_PowerState.valueNoSendCompare(powerState, DPT_Switch);
        if (changed) KoNEO_PowerState.objectWritten();
        return;
    }

    if (koNumber == NEO_KoBrightness)
    {
        // DPT 5.001 sends 0-100 (percentage), need to scale to 0-255 for internal use
        uint8_t brightnessPercent = ko.value(DPT_Scaling);
        uint8_t brightness = (brightnessPercent * 255) / 100;
        logInfoP("Global Brightness KO: %d%% (scaled to %d/255)", brightnessPercent, brightness);

        // Apply brightness to all segments
        applyGlobalBrightness(brightness);

        // Send state feedback (send back percentage)
        bool changed = KoNEO_BrightnessState.valueNoSendCompare(brightnessPercent, DPT_Scaling);
        if (changed) KoNEO_BrightnessState.objectWritten();
        return;
    }

#if defined(NEO_KoExternalRelay1)
    if (koNumber == NEO_KoExternalRelay1)
    {
        bool state = ko.value(DPT_Switch);
        scheduleRelayChange(0, state);
        return;
    }
#endif

#if defined(NEO_KoExternalRelay2)
    if (koNumber == NEO_KoExternalRelay2)
    {
        bool state = ko.value(DPT_Switch);
        scheduleRelayChange(1, state);
        return;
    }
#endif

#if defined(NEO_KoExternalRelay3)
    if (koNumber == NEO_KoExternalRelay3)
    {
        bool state = ko.value(DPT_Switch);
        scheduleRelayChange(2, state);
        return;
    }
#endif

#if defined(NEO_KoExternalRelay4)
    if (koNumber == NEO_KoExternalRelay4)
    {
        bool state = ko.value(DPT_Switch);
        scheduleRelayChange(3, state);
        return;
    }
#endif

    // Channel-specific KOs (segment-based) - Calculate which channel this KO belongs to
    int channel = NEO_KoCalcChannel(koNumber);
    if (channel >= 0)
    {
        uint8_t oldChannelIndex = _channelIndex;
        _channelIndex = channel;

        int koIndex = NEO_KoCalcIndex(koNumber);

        // IMPORTANT: "channel" here refers to segment index, not physical strip index
        // The KNX channels correspond to logical segments that users can control
        // Physical strips are hardware configuration and not directly controlled via KOs

        // Only log non-status KOs to avoid console spam from read requests
        if (koIndex != NEO_KoSegmentPowerState &&
            koIndex != NEO_KoSegmentBrightnessState &&
            koIndex != NEO_KoCCTState &&
            koIndex != NEO_KoFxState &&
            koIndex != NEO_KoSceneState &&
            koIndex != NEO_KoRGBState &&
            koIndex != NEO_KoHSVState &&
            koIndex != NEO_KoRGBWState &&
            koIndex != NEO_KoHCLState)
        {
            logInfoP("Segment Channel %d KO Index %d received (_segments.size=%d, _numberOfSegments=%d)",
                     channel, koIndex, (int)_segments.size(), _numberOfSegments);
        }

        // Validate channel against segment count, not physical strip count
        if (channel >= _segments.size())
        {
            logWarningP("KO for channel %d ignored - only %d segments configured (_numberOfSegments=%d)",
                        channel, (int)_segments.size(), _numberOfSegments);
            _channelIndex = oldChannelIndex;
            return;
        }

        // Get the target segment for this channel
        Segment* targetSegment = (channel < _segments.size()) ? _segments[channel].segment : nullptr;
        if (!targetSegment)
        {
            logWarningP("Channel %d has no valid segment, ignoring KO", channel);
            _channelIndex = oldChannelIndex;
            return;
        }

        // If this segment is locked (Sperre-KO), ignore ALL controlling KOs. The Lock KO itself
        // stays active (to unlock); status/output KOs (WriteFlag=Disabled) are never received.
        if (_segments[channel].locked && koIndex != NEO_KoLock)
        {
            _channelIndex = oldChannelIndex;
            return;
        }

        // A direct manual command takes over from a running EM on this segment: halt the EM
        // (without restoring pre-EM state — the command below sets the new visual). Excluded:
        // read/status KOs, Scene recall (stops the EM itself), brightness (dims the EM, not stops
        // it), and Cue +/- / Lock (EM/segment control, not a visual takeover).
        if (koIndex != NEO_KoSegmentPowerState && koIndex != NEO_KoSegmentBrightnessState &&
            koIndex != NEO_KoCCTState && koIndex != NEO_KoFxState && koIndex != NEO_KoSceneState &&
            koIndex != NEO_KoRGBState && koIndex != NEO_KoHSVState && koIndex != NEO_KoRGBWState &&
            koIndex != NEO_KoHCLState && koIndex != NEO_KoScene &&
            koIndex != NEO_KoSegmentBrightness && koIndex != NEO_KoBriRel &&
            koIndex != NEO_KoCueRel && koIndex != NEO_KoLock)
        {
            interruptEmIfRunning((size_t)channel);
        }

        switch (koIndex)
        {
            case NEO_KoR:
            {
                uint8_t red = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d Red: %d", channel, red);

                SegmentConfig& cfg = _segments[channel];

                // Store as pending (for effect restore)
                cfg.pendingSolidR = red;
                cfg.pendingSolidG = cfg.savedValid ? cfg.savedG : 0;
                cfg.pendingSolidB = cfg.savedValid ? cfg.savedB : 0;
                cfg.pendingSolidWW = cfg.savedValid ? cfg.savedWW : 0;

                // Update saved values - preserve existing G, B, W
                cfg.savedR = red;
                if (!cfg.savedValid)
                {
                    cfg.savedG = 0;
                    cfg.savedB = 0;
                    cfg.savedWW = 0;
                }
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                {
                    uint8_t g = cfg.savedG;
                    uint8_t b = cfg.savedB;
                    targetSegment->setPrimaryColor(red, g, b, 0);
                    logInfoP("Segment %d: Updated primary color (Red=%d)", channel, red);
                }

                // Send combined RGB/RGBW status feedback
                _channelIndex = channel;
                uint8_t r, g, b;
                r = cfg.pendingSolidR;
                g = cfg.pendingSolidG;
                b = cfg.pendingSolidB;
                // Check if virtual strip has white channel (4 bytes per LED)
                if (_virtualStrip && _virtualStrip->getBytesPerLed() == 4)
                {
                    // Has white channel - send RGBW status
                    uint8_t w = cfg.pendingSolidWW;
                    uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
                    bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
                    if (changed) KoNEO_RGBWState.objectWritten();
                }
                else
                {
                    // No white channel - send RGB status
                    uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b; // RGB
                    bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
                    if (changed) KoNEO_RGBState.objectWritten();
                }
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoG:
            {
                uint8_t green = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d Green: %d", channel, green);

                SegmentConfig& cfg = _segments[channel];

                // Store as pending (for effect restore)
                cfg.pendingSolidR = cfg.savedValid ? cfg.savedR : 0;
                cfg.pendingSolidG = green;
                cfg.pendingSolidB = cfg.savedValid ? cfg.savedB : 0;
                cfg.pendingSolidWW = cfg.savedValid ? cfg.savedWW : 0;

                // Update saved values - preserve existing R, B, W
                if (!cfg.savedValid)
                {
                    cfg.savedR = 0;
                    cfg.savedB = 0;
                    cfg.savedWW = 0;
                }
                cfg.savedG = green;
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                {
                    uint8_t r = cfg.savedR;
                    uint8_t b = cfg.savedB;
                    targetSegment->setPrimaryColor(r, green, b, 0);
                    logInfoP("Segment %d: Updated primary color (Green=%d)", channel, green);
                }

                // Send combined RGB/RGBW status feedback
                _channelIndex = channel;
                uint8_t r, g, b;
                r = cfg.pendingSolidR;
                g = cfg.pendingSolidG;
                b = cfg.pendingSolidB;
                // Check if virtual strip has white channel (4 bytes per LED)
                if (_virtualStrip && _virtualStrip->getBytesPerLed() == 4)
                {
                    // Has white channel - send RGBW status
                    uint8_t w = cfg.pendingSolidWW;
                    uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
                    bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
                    if (changed) KoNEO_RGBWState.objectWritten();
                }
                else
                {
                    // No white channel - send RGB status
                    uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b; // RGB
                    bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
                    if (changed) KoNEO_RGBState.objectWritten();
                }
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoB:
            {
                uint8_t blue = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d Blue: %d", channel, blue);

                SegmentConfig& cfg = _segments[channel];

                // Store as pending (for effect restore)
                cfg.pendingSolidR = cfg.savedValid ? cfg.savedR : 0;
                cfg.pendingSolidG = cfg.savedValid ? cfg.savedG : 0;
                cfg.pendingSolidB = blue;
                cfg.pendingSolidWW = cfg.savedValid ? cfg.savedWW : 0;

                // Update saved values - preserve existing R, G, W
                if (!cfg.savedValid)
                {
                    cfg.savedR = 0;
                    cfg.savedG = 0;
                    cfg.savedWW = 0;
                }
                cfg.savedB = blue;
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                {
                    uint8_t r = cfg.savedR;
                    uint8_t g = cfg.savedG;
                    targetSegment->setPrimaryColor(r, g, blue, 0);
                    logInfoP("Segment %d: Updated primary color (Blue=%d)", channel, blue);
                }

                // Send combined RGB/RGBW status feedback
                _channelIndex = channel;
                uint8_t r, g, b;
                r = cfg.pendingSolidR;
                g = cfg.pendingSolidG;
                b = cfg.pendingSolidB;
                // Check if virtual strip has white channel (4 bytes per LED)
                if (_virtualStrip && _virtualStrip->getBytesPerLed() == 4)
                {
                    // Has white channel - send RGBW status
                    uint8_t w = cfg.pendingSolidWW;
                    uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w;
                    bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
                    if (changed) KoNEO_RGBWState.objectWritten();
                }
                else
                {
                    // No white channel - send RGB status
                    uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b; // RGB
                    bool changed = KoNEO_RGBState.valueNoSendCompare(rgb, DPT_Colour_RGB);
                    if (changed) KoNEO_RGBState.objectWritten();
                }
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoW:
            {
                uint8_t white = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d White: %d", channel, white);

                SegmentConfig& cfg = _segments[channel];

                // Store as pending (for effect restore)
                cfg.pendingSolidR = cfg.savedValid ? cfg.savedR : 0;
                cfg.pendingSolidG = cfg.savedValid ? cfg.savedG : 0;
                cfg.pendingSolidB = cfg.savedValid ? cfg.savedB : 0;
                cfg.pendingSolidWW = white;

                // Update saved values - preserve existing R, G, B
                if (!cfg.savedValid)
                {
                    cfg.savedR = 0;
                    cfg.savedG = 0;
                    cfg.savedB = 0;
                }
                cfg.savedWW = white;
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                {
                    uint8_t r = cfg.savedR;
                    uint8_t g = cfg.savedG;
                    uint8_t b = cfg.savedB;
                    targetSegment->setPrimaryColor(r, g, b, white);
                    logInfoP("Segment %d: Updated primary color (White=%d)", channel, white);
                }

                // Send RGBW status feedback (white channel update)
                _channelIndex = channel;
                uint8_t r, g, b;
                r = cfg.pendingSolidR;
                g = cfg.pendingSolidG;
                b = cfg.pendingSolidB;
                uint8_t w = white;
                uint32_t rgbw = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w; // RGBW
                bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
                if (changed) KoNEO_RGBWState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoCCT:
            {
                uint16_t cct = ko.value(DPT_Value_Temp);
                logInfoP("Segment %d CCT: %dK", channel, cct);

                SegmentConfig& cfg = _segments[channel];

                // Apply color temperature to segment - convert Kelvin to RGB
                uint8_t r, g, b;
                ColorHelper::kelvinToRGB(cct, r, g, b);
                targetSegment->setPrimaryColor(r, g, b, 0);

                // Save color for effect restore
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                cfg.savedWW = 0;
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;
                cfg.savedLastWasEffect = false;

                // Send status feedback
                _channelIndex = channel;
                bool changed = KoNEO_CCTState.valueNoSendCompare(cct, DPT_Value_Temp);
                if (changed) KoNEO_CCTState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoH:
            {
                // DPT 5.003 is 1 byte on the wire: 0..255  <->  0..360°
                uint8_t raw = ko.value(DPT_Value_1_Ucount); // read raw byte (no scaling)

                // optional: treat 360° (=255) as 0° for hue wrap
                uint8_t hue = (raw == 255) ? 0 : raw;

                // log degrees (use >=16-bit!)
                uint16_t deg = (uint32_t(raw) * 360u + 127u) / 255u;
                logInfoP("Hue: %u° (raw=%u) -> HSV hue=%u", (unsigned)deg, (unsigned)raw, (unsigned)hue);

                // Use persistent S and V from config
                SegmentConfig& cfg = _segments[channel];
                uint8_t s = cfg.currentS;
                uint8_t v = cfg.currentV;
                // Preserve S and V - don't reset them when H changes
                // If V is 0, default to full brightness to make color visible
                if (v == 0)
                {
                    v = 255;
                }

                // Convert HSV to RGB
                uint8_t r, g, b;
                ColorHelper::hsvToRGB(hue, s, v, r, g, b);

                // Store as pending (for effect restore) - preserve W if strip has white channel
                cfg.pendingSolidR = r;
                cfg.pendingSolidG = g;
                cfg.pendingSolidB = b;
                if (!cfg.savedValid || (_virtualStrip && _virtualStrip->getBytesPerLed() != 4))
                {
                    cfg.pendingSolidWW = 0;
                }

                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, 0);
                logInfoP("Segment %d: Updated primary color (Hue)", channel);

                // Store and persist HSV values
                cfg.currentH = hue;
                cfg.currentS = s;
                cfg.currentV = v;

                // Send HSV status feedback - convert to RGB using actual V value
                _channelIndex = channel;
                ColorHelper::hsvToRGB(cfg.currentH, cfg.currentS, cfg.currentV, r, g, b);
                uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
                bool changed = KoNEO_HSVState.valueNoSendCompare(rgb, DPT_Colour_RGB);
                if (changed) KoNEO_HSVState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoS:
            {
                // DPT 5.001 sends 0-100 (percentage), need to scale to 0-255 for internal use
                uint8_t saturationPercent = ko.value(DPT_Scaling);
                uint8_t saturation = (saturationPercent * 255) / 100;
                logInfoP("Segment %d Saturation: %d%% (scaled to %d/255)", channel, saturationPercent, saturation);

                // Use persistent H and V from config
                SegmentConfig& cfg = _segments[channel];
                uint8_t h = cfg.currentH;
                uint8_t v = cfg.currentV;
                // Preserve H and V - don't reset them when S changes
                // If V is 0, default to full brightness to make color visible
                if (v == 0)
                {
                    v = 255;
                }

                // Convert HSV to RGB
                uint8_t r, g, b;
                ColorHelper::hsvToRGB(h, saturation, v, r, g, b);

                // Store as pending (for effect restore) - preserve W if strip has white channel
                cfg.pendingSolidR = r;
                cfg.pendingSolidG = g;
                cfg.pendingSolidB = b;
                if (!cfg.savedValid || (_virtualStrip && _virtualStrip->getBytesPerLed() != 4))
                {
                    cfg.pendingSolidWW = 0;
                }

                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, 0);
                logInfoP("Segment %d: Updated primary color (Saturation)", channel);

                // Store HSV values
                cfg.currentH = h;
                cfg.currentS = saturation;
                cfg.currentV = v;

                // Send HSV status feedback - convert to RGB using actual V value
                _channelIndex = channel;
                ColorHelper::hsvToRGB(cfg.currentH, cfg.currentS, cfg.currentV, r, g, b);
                uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
                bool changed = KoNEO_HSVState.valueNoSendCompare(rgb, DPT_Colour_RGB);
                if (changed) KoNEO_HSVState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoV:
            {
                // DPT 5.001 sends 0-100 (percentage), need to scale to 0-255 for internal use
                uint8_t valuePercent = ko.value(DPT_Scaling);
                uint8_t value = (valuePercent * 255) / 100;
                logInfoP("Segment %d Value: %d%% (scaled to %d/255)", channel, valuePercent, value);

                // Use persistent H and S from config
                SegmentConfig& cfg = _segments[channel];
                uint8_t h = cfg.currentH;
                uint8_t s = cfg.currentS;
                // Preserve H and S - don't reset them when V changes

                // Convert HSV to RGB
                uint8_t r, g, b;
                ColorHelper::hsvToRGB(h, s, value, r, g, b);

                // Store as pending (for effect restore) - preserve W if strip has white channel
                cfg.pendingSolidR = r;
                cfg.pendingSolidG = g;
                cfg.pendingSolidB = b;
                if (!cfg.savedValid || (_virtualStrip && _virtualStrip->getBytesPerLed() != 4))
                {
                    cfg.pendingSolidWW = 0;
                }

                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, 0);
                logInfoP("Segment %d: Updated primary color (Value)", channel);

                // Store HSV values
                cfg.currentH = h;
                cfg.currentS = s;
                cfg.currentV = value;

                // Send HSV status feedback - convert to RGB using actual V value
                _channelIndex = channel;
                ColorHelper::hsvToRGB(cfg.currentH, cfg.currentS, cfg.currentV, r, g, b);
                uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
                bool changed = KoNEO_HSVState.valueNoSendCompare(rgb, DPT_Colour_RGB);
                if (changed) KoNEO_HSVState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoRGB:
            {
                uint32_t rgb = ko.value(DPT_Colour_RGB);
                uint8_t r = (rgb >> 16) & 0xFF;
                uint8_t g = (rgb >> 8) & 0xFF;
                uint8_t b = rgb & 0xFF;
                logInfoP("Segment %d RGB: R=%d G=%d B=%d", channel, r, g, b);

                SegmentConfig& cfg = _segments[channel];

                // Store as pending (for effect restore) - preserve W if strip has white channel
                cfg.pendingSolidR = r;
                cfg.pendingSolidG = g;
                cfg.pendingSolidB = b;
                if (!cfg.savedValid || (_virtualStrip && _virtualStrip->getBytesPerLed() != 4))
                {
                    cfg.pendingSolidWW = 0;
                }

                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, 0);
                logInfoP("Segment %d: Updated primary color (RGB)", channel);

                // Send status feedback
                _channelIndex = channel;
                uint32_t feedbackRgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
                bool changed = KoNEO_RGBState.valueNoSendCompare(feedbackRgb, DPT_Colour_RGB);
                if (changed) KoNEO_RGBState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoHSV:
            {
                uint32_t hsv = ko.value(DPT_Colour_RGB); // Using same DPT for now
                logInfoP("Segment %d HSV: 0x%08X", channel, hsv);

                // Extract HSV components from the 32-bit value
                // Assuming format: 0x00HHSSVV (H=hue, S=saturation, V=value)
                uint8_t h = (hsv >> 16) & 0xFF; // Hue
                uint8_t s = (hsv >> 8) & 0xFF;  // Saturation
                uint8_t v = hsv & 0xFF;         // Value/Brightness

                // Convert HSV to RGB
                uint8_t r, g, b;
                ColorHelper::hsvToRGB(h, s, v, r, g, b);

                SegmentConfig& cfg = _segments[channel];

                // Store as pending (for effect restore) - preserve W if strip has white channel
                cfg.pendingSolidR = r;
                cfg.pendingSolidG = g;
                cfg.pendingSolidB = b;
                if (!cfg.savedValid || (_virtualStrip && _virtualStrip->getBytesPerLed() != 4))
                {
                    cfg.pendingSolidWW = 0;
                }

                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedBrightness = targetSegment->getBrightness();
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, 0);
                logInfoP("Segment %d: Updated primary color (HSV)", channel);

                // Send status feedback
                _channelIndex = channel;
                uint32_t feedbackHsv = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
                bool changed = KoNEO_HSVState.valueNoSendCompare(feedbackHsv, DPT_Colour_RGB);
                if (changed) KoNEO_HSVState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoCueRel:
            {
                // Relative cue step (DPT 3.7) on the RUNNING EM. bit3 = direction (1=next,
                // 0=previous), bits0-2 = step (0 = stop/no-op). Stays in the current run-state
                // (a paused EM gets the new cue, shown on resume). Does NOT interrupt.
                uint8_t raw = ko.value(Dpt(5, 10));
                if ((raw & 0x07) != 0)
                {
                    SegmentConfig& cfgQ = _segments[channel];
                    uint8_t emId = cfgQ.emController.activeEmId();
                    if (emId >= 1 && emId <= EM_COUNT && _emData)
                    {
                        uint8_t cur = cfgQ.emController.activeCueNum(); // 1-based
                        uint8_t cueCount = _emData[emId - 1].header.cueCount;
                        uint8_t target = cur;
                        if (raw & 0x08)
                        {
                            if (cur < cueCount) target = cur + 1;
                        } // next cue
                        else
                        {
                            if (cur > 1) target = cur - 1;
                        } // previous cue
                        if (target != cur && target >= 1)
                        {
                            cfgQ.emController.triggerCue(target, targetSegment, _emData);
                            sendEmStatusKOs((size_t)channel);
                        }
                    }
                }
                break;
            }

            case NEO_KoLock:
            {
                // Sperre — 1 = lock (ignore controlling KOs), 0 = unlock. Gated above.
                bool lock = ko.value(DPT_Switch);
                _segments[channel].locked = lock;
                logInfoP("Segment %d: Sperre %s", channel, lock ? "EIN" : "AUS");
                break;
            }

            case NEO_KoFxRel:
            {
                // "Effekt schalten" (DPT 3.7), 4-bit dim control: bit3 = direction (1=next,
                // 0=previous), bits0-2 = step (0 = stop/no-op). Steps by one effect ID through
                // the EffectPool.
                uint8_t raw = ko.value(Dpt(5, 10));
                if ((raw & 0x07) != 0)
                {
                    uint8_t cur = getTypeFromEffect(targetSegment->getEffect());
                    uint8_t count = EffectPool::getEffectCount();
                    uint8_t next = cur;
                    if (raw & 0x08)
                    {
                        if ((uint8_t)(cur + 1) < count) next = cur + 1;
                    } // next
                    else
                    {
                        if (cur > 0) next = cur - 1;
                    } // previous
                    if (next != cur)
                    {
                        logInfoP("Segment %d Effekt schalten: %d -> %d", channel, cur, next);
                        applyEffectToSegment(targetSegment, next);
                        _effectConfiguration->setupEffectConfiguration(targetSegment);
                        SegmentConfig& cfgFx = _segments[channel];
                        cfgFx.savedEffectType = next;
                        cfgFx.savedEffectValid = (next > 0); // mirror the absolute NEO_KoFx handler

                        // Effektkette: a master propagates the stepped effect to its slaves (like NEO_KoFx).
                        if (cfgFx.syncMode == 1) sendSyncTelegram((size_t)channel);

                        // Status feedback so visualisations / feedback logic track the stepped effect.
                        _channelIndex = channel;
                        if (KoNEO_FxState.valueNoSendCompare(next, DPT_SceneNumber))
                            KoNEO_FxState.objectWritten();
                        _channelIndex = oldChannelIndex;
                    }
                }
                break;
            }

            case NEO_KoFx:
            {
                uint8_t effect = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d Effect: %d", channel, effect);

                // Remember selected effect for restore
                SegmentConfig& cfg = _segments[channel];

                // Apply effect to segment dynamically FIRST (this clears the pixel buffer)
                applyEffectToSegment(targetSegment, effect);

                // THEN restore primary color for ALL effects (after the clear)
                // Effects like Wipe, Rainbow, etc. use the primary color from EffectConfig
                // This must happen AFTER applyEffectToSegment to avoid the clear wiping our color

                // Determine which color to use
                bool hasPendingColor = (cfg.pendingSolidR > 0 || cfg.pendingSolidG > 0 || cfg.pendingSolidB > 0 || cfg.pendingSolidWW > 0);

                if (hasPendingColor)
                {
                    // User set colors during the previous effect - use those
                    targetSegment->setPrimaryColor(cfg.pendingSolidR, cfg.pendingSolidG, cfg.pendingSolidB, cfg.pendingSolidWW);
                    logInfoP("Segment %d: Applied pending color (R=%d G=%d B=%d W=%d) to effect %d",
                             channel, cfg.pendingSolidR, cfg.pendingSolidG, cfg.pendingSolidB, cfg.pendingSolidWW, effect);
                    // Clear pending after applying
                    cfg.pendingSolidR = 0;
                    cfg.pendingSolidG = 0;
                    cfg.pendingSolidB = 0;
                    cfg.pendingSolidWW = 0;
                }
                else if (cfg.savedValid)
                {
                    // No pending colors - use the saved color
                    targetSegment->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW);
                    if (effect == 0)
                    {
                        // For Solid effect, also restore brightness
                        targetSegment->setBrightness(cfg.savedBrightness);
                    }
                    logInfoP("Segment %d: Applied saved color (R=%d G=%d B=%d W=%d) to effect %d",
                             channel, cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, effect);
                }
                else
                {
                    // No saved color - use ETS Default-Farbe (ETS-Parameterwert)
                    uint32_t color = ParamNEO_NEOSegmentStartupColor;
                    uint8_t r = (color >> 16) & 0xFF;
                    uint8_t g = (color >> 8) & 0xFF;
                    uint8_t b = color & 0xFF;
                    uint8_t w = ParamNEO_NEOSegmentStartupW;

                    targetSegment->setPrimaryColor(r, g, b, w);

                    if (effect == 0)
                    {
                        // For Solid effect, also apply startup brightness
                        uint8_t brightness = ParamNEO_NEOSegmentStartupBrightness;
                        targetSegment->setBrightness(brightness);
                        logInfoP("Segment %d: No saved color, using ETS Default-Farbe (R=%d G=%d B=%d W=%d, Brightness=%d) for Solid",
                                 channel, r, g, b, w, brightness);
                    }
                    else
                    {
                        logInfoP("Segment %d: No saved color, using ETS Default-Farbe (R=%d G=%d B=%d W=%d) for effect %d",
                                 channel, r, g, b, w, effect);
                    }
                }
                // Auto-update will render the change on next cycle
                cfg.savedEffectType = effect;
                cfg.savedEffectValid = (effect > 0);
                // Note: Effect-specific parameters are now stored in ETS, no need to snapshot

                // Effektkette: Master sends sync telegram to all slaves
                if (cfg.syncMode == 1) sendSyncTelegram((size_t)channel);

                // Send status feedback
                _channelIndex = channel;
                bool changed = KoNEO_FxState.valueNoSendCompare(effect, DPT_SceneNumber);
                if (changed) KoNEO_FxState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoScene:
            {
                // DPT 18.001 Scene Control: bit 7 = learn flag, bits 0-5 = scene number (0-63)
                // Read raw byte directly — ko.value(DPT_SceneControl) only returns index 0 (learn bit)
                uint8_t raw = ko.valueRef()[0];
                bool learn = (raw & 0x80) != 0;
                uint8_t sceneNumber = (raw & 0x3F) + 1; // Convert 0-based DPT to 1-based internal
                logInfoP("Segment %d Scene Control: scene=%d, learn=%d", channel, sceneNumber, learn);

                if (learn)
                {
                    // Store current segment state into the scene slot
                    bool stored = _sceneManager->storeScene(channel, sceneNumber, targetSegment);
                    if (stored)
                    {
                        logInfoP("Segment %d: Scene %d stored successfully", channel, sceneNumber);
                    }
                    else
                    {
                        logWarningP("Segment %d: storeScene FAILED for scene %d", channel, sceneNumber);
                    }
                }
                else
                {
                    // Recall scene
                    bool sceneApplied = _sceneManager->recallScene(channel, sceneNumber, targetSegment);

                    if (sceneApplied)
                    {
                        // Save scene number for flash persistence
                        _segments[channel].savedSceneNumber = sceneNumber;
                    }
                    else
                    {
                        // Fallback: hardcoded presets for backward compatibility
                        // (only used when scene number exceeds configured SceneCount)
                        _segments[channel].savedSceneNumber = 0; // Clear scene number
                        switch (sceneNumber)
                        {
                            case 1: // Red
                                targetSegment->setPrimaryColor(255, 0, 0, 0);
                                targetSegment->setBrightness(255);
                                break;
                            case 2: // Green
                                targetSegment->setPrimaryColor(0, 255, 0, 0);
                                targetSegment->setBrightness(255);
                                break;
                            case 3: // Blue
                                targetSegment->setPrimaryColor(0, 0, 255, 0);
                                targetSegment->setBrightness(255);
                                break;
                            case 4: // White
                                targetSegment->setPrimaryColor(255, 255, 255, 0);
                                targetSegment->setBrightness(255);
                                break;
                            case 5: // Warm White (using color temperature)
                            {
                                uint8_t r, g, b;
                                ColorHelper::kelvinToRGB(2700, r, g, b); // Warm white ~2700K
                                targetSegment->setPrimaryColor(r, g, b, 0);
                                targetSegment->setBrightness(255);
                            }
                            break;
                            case 6: // Cool White
                            {
                                uint8_t r, g, b;
                                ColorHelper::kelvinToRGB(6500, r, g, b); // Cool white ~6500K
                                targetSegment->setPrimaryColor(r, g, b, 0);
                                targetSegment->setBrightness(255);
                            }
                            break;
                            case 7:                                     // Rainbow Effect
                                applyEffectToSegment(targetSegment, 1); // Rainbow effect
                                targetSegment->setBrightness(200);
                                break;
                            case 8: // Off
                                targetSegment->setPrimaryColor(0, 0, 0, 0);
                                targetSegment->setBrightness(0);
                                break;
                            default:
                                logWarningP("Unknown preset %d for segment %d", sceneNumber, channel);
                                break;
                        }
                    }
                }
                break;
            }

            case NEO_KoSegmentPower:
            {
                bool power = ko.value(DPT_Switch);
                // logDebugP("Segment %d Power: %s", channel, power ? "ON" : "OFF");

                SegmentConfig& cfg = _segments[channel];

                if (power)
                {
                    // Per-segment power intent is ON regardless of the global state.
                    cfg.savedPower = 1;

                    // If this segment's EM was suspended by a power-off, resume it (not the
                    // direct-state restore below). But while GLOBAL power is OFF, only record the
                    // intent and keep the EM suspended — global Power-On then revives it. This stops a
                    // single segment-on from defeating the global off (EM ticking + immediate show()).
                    if (cfg.suspendedByPower)
                    {
                        if (_globalPowerOn)
                        {
                            cfg.suspendedByPower = false;
                            resumeEffektManager(channel); // resume() also re-applies master brightness
                            if (_virtualStrip) _virtualStrip->show();
                        }
                        _channelIndex = channel;
                        if (KoNEO_SegmentPowerState.valueNoSendCompare(power, DPT_Switch))
                            KoNEO_SegmentPowerState.objectWritten();
                        break;
                    }

                    // logDebugP("Segment %d Power ON: savedValid=%d, savedLastWasEffect=%d, savedEffectValid=%d, type=%d, savedRGB=(%d,%d,%d), savedBri=%d",
                    //           channel, cfg.savedValid, cfg.savedLastWasEffect, cfg.savedEffectValid, cfg.savedEffectType,
                    //           cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedBrightness);

                    // (cfg.savedPower already set to 1 above, before the suspended-EM branch)

                    if (cfg.savedLastWasEffect && cfg.savedEffectValid)
                    {
                        // Restore effect (including Solid effect type=0!)
                        applyEffectToSegment(targetSegment, cfg.savedEffectType);
                        _effectConfiguration->setupEffectConfiguration(targetSegment);
                        targetSegment->setBrightness(cfg.savedBrightness == 0 ? 255 : cfg.savedBrightness);

                        // For Solid effect (type=0), restore saved colors
                        if (cfg.savedEffectType == static_cast<uint8_t>(PT_NEOEffectType::Solid))
                        {
                            targetSegment->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW);
                        }

                        // Resume effect and reset state (in case stopped during Power OFF)
                        targetSegment->resume();
                        if (targetSegment->getEffect())
                        {
                            targetSegment->getEffect()->reset();
                        }
                    }
                    else if (cfg.savedValid)
                    {
                        // IMPORTANT: Set brightness FIRST before setPrimaryColor() to ensure pixels are visible
                        targetSegment->setBrightness(cfg.savedBrightness == 0 ? 255 : cfg.savedBrightness);
                        // savedWW is always 0 for RGB strips, contains actual value for RGBW strips
                        targetSegment->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW);
                    }
                    else
                    {
                        if (targetSegment->getBrightness() == 0)
                        {
                            targetSegment->setBrightness(255);
                        }
                    }

                    // Force immediate show() to update PowerManager with new pixel data — but only
                    // when global power is ON; otherwise the restored state stays in the buffer and
                    // appears when global Power-On resumes rendering (segment-on must not defeat
                    // global-off).
                    if (_globalPowerOn && _virtualStrip)
                    {
                        _virtualStrip->show();
                    }
                }
                else
                {
                    // Suspend a running EM so it does NOT keep ticking while this segment is off.
                    if (cfg.emController.isRunning() && !cfg.emController.isPaused())
                    {
                        pauseEffektManager(channel);
                        cfg.suspendedByPower = true;
                    }

                    // Snapshot PRIMARY COLOR (not current pixels!)
                    // Effects constantly change pixel colors, but primaryColor is the base color
                    uint32_t primaryRGBW = targetSegment->getPrimaryColor();

                    // Extract RGBW components from uint32_t (format: 0xRRGGBBWW)
                    cfg.savedR = (primaryRGBW >> 24) & 0xFF;
                    cfg.savedG = (primaryRGBW >> 16) & 0xFF;
                    cfg.savedB = (primaryRGBW >> 8) & 0xFF;
                    cfg.savedWW = (primaryRGBW) & 0xFF;
                    // Save the MASTER (intent) brightness, not the render value: a power-suspended EM
                    // is paused, so getBrightness() would be the frozen cue-scaled render (master*cue/255).
                    // Persisting master keeps "Letzter Zustand" consistent for EM segments.
                    cfg.savedBrightness = targetSegment->getMasterBrightness();
                    cfg.savedValid = true;

                    // Update power state for flash persistence
                    cfg.savedPower = 0;

                    // Save current effect type (convert Effect* to type ID)
                    Effect* currentEffect = targetSegment->getEffect();
                    cfg.savedEffectType = getTypeFromEffect(currentEffect);

                    // Track if effect was active (Solid effect type=0 IS an effect!)
                    cfg.savedLastWasEffect = (currentEffect != nullptr);
                    cfg.savedEffectValid = cfg.savedLastWasEffect;

                    // logDebugP("Segment %d Power OFF: snapshot RGB=(%d,%d,%d), W=%d, Bri=%d, effectType=%d",
                    //           channel, cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedBrightness, cfg.savedEffectType);

                    // NOW stop effect and clear pixels
                    targetSegment->stop();
                    if (_virtualStrip && _virtualStrip->getBytesPerLed() == 4)
                    {
                        targetSegment->setPrimaryColor(0, 0, 0, 0);
                    }
                    else
                    {
                        targetSegment->setPrimaryColor(0, 0, 0, 0);
                    }

                    // Force immediate sync and show to clear PhysicalStrip buffers
                    // Must use NeoPixelManager to ensure PhysicalStrip buffers are updated!
                    NeoPixelManager* manager = _neoPixel.getManager();
                    if (manager)
                    {
                        manager->syncAll();
                        manager->showAll();
                    }
                }

                _channelIndex = channel;
                bool changed = KoNEO_SegmentPowerState.valueNoSendCompare(power, DPT_Switch);
                if (changed) KoNEO_SegmentPowerState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            case NEO_KoSegmentBrightness:
            {
                // DPT 5.001 sends 0-100 (percentage), need to scale to 0-255 for internal use
                uint8_t brightnessPercent = ko.value(DPT_Scaling);
                uint8_t brightness = (brightnessPercent * 255) / 100;
                logInfoP("Segment %d Brightness: %d%% (scaled to %d/255)", channel, brightnessPercent, brightness);

                // Get segment config to access saved colors
                SegmentConfig& cfgB = _segments[channel];

                // Persist desired brightness for power-restore (before global scaling)
                cfgB.savedBrightness = brightness;

                // Apply global brightness scaling if active (< 255)
                uint8_t effectiveBrightness = brightness;
                if (_globalBrightness < 255)
                {
                    effectiveBrightness = (brightness * _globalBrightness) / 255;
                }

                // Master brightness = user/KO intent (after global scaling). The Effektmanager
                // renders cues at master*cue/255, so dimming via this KO survives cue switches.
                targetSegment->setMasterBrightness(effectiveBrightness);
                if (cfgB.emController.isRunning())
                    // EM owns the render brightness — re-derive it from the new master immediately.
                    cfgB.emController.reapplyMasterBrightness(targetSegment);
                else
                    // No EM: master and render brightness are the same.
                    targetSegment->setBrightness(effectiveBrightness);

                // Send status feedback (send back percentage)
                _channelIndex = channel;
                bool changed = KoNEO_SegmentBrightnessState.valueNoSendCompare(brightnessPercent, DPT_Scaling);
                if (changed) KoNEO_SegmentBrightnessState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            // Warm White and Cool White control
            case NEO_KoWW:
            {
                uint8_t ww = ko.value(DPT_Value_1_Ucount); // 5.010
                // logDebugP("Segment %d Warm White: %d", channel, ww);

                // Store warm white in primaryWW, preserve cool white (5-channel RGBCCT)
                uint8_t r = targetSegment->getConfig().r();
                uint8_t g = targetSegment->getConfig().g();
                uint8_t b = targetSegment->getConfig().b();
                uint8_t cw = targetSegment->getConfig().cw();
                targetSegment->setPrimaryColor(r, g, b, ww, cw);
                break;
            }

            case NEO_KoCW:
            {
                uint8_t cw = ko.value(DPT_Value_1_Ucount); // 5.010
                // logDebugP("Segment %d Cool White: %d", channel, cw);

                // Store cool white in primaryCW, preserve warm white (5-channel RGBCCT)
                uint8_t r = targetSegment->getConfig().r();
                uint8_t g = targetSegment->getConfig().g();
                uint8_t b = targetSegment->getConfig().b();
                uint8_t ww = targetSegment->getConfig().ww();
                targetSegment->setPrimaryColor(r, g, b, ww, cw);
                break;
            }

            // RGBW combined control
            case NEO_KoRGBW:
            {
                uint32_t rgbw = ko.value(DPT_Colour_RGBW);
                uint8_t r = (rgbw >> 24) & 0xFF; // RGBW
                uint8_t g = (rgbw >> 16) & 0xFF;
                uint8_t b = (rgbw >> 8) & 0xFF;
                uint8_t w = rgbw & 0xFF;
                // logDebugP("Segment %d RGBW: R=%d G=%d B=%d W=%d", channel, r, g, b, w);

                // Store in config and apply
                targetSegment->setPrimaryColor(r, g, b, w);

                // Persist desired static color for power-restore
                {
                    SegmentConfig& cfg = _segments[channel];
                    cfg.savedR = r;
                    cfg.savedG = g;
                    cfg.savedB = b;
                    cfg.savedWW = w;
                    cfg.savedBrightness = targetSegment->getBrightness();
                    cfg.savedValid = true;
                    cfg.savedLastWasEffect = false;
                }

                // Send status feedback
                _channelIndex = channel;
                bool changed = KoNEO_RGBWState.valueNoSendCompare(rgbw, DPT_Colour_RGBW);
                if (changed) KoNEO_RGBWState.objectWritten();
                _channelIndex = oldChannelIndex;
                break;
            }

            // Relative controls (DPT 3.007 - relative dimming)
            case NEO_KoBriRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::BRIGHTNESS, rel, channel);
                break;
            }

            case NEO_KoRRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::RED, rel, channel);
                break;
            }

            case NEO_KoGRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::GREEN, rel, channel);
                break;
            }

            case NEO_KoBRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::BLUE, rel, channel);
                break;
            }

            case NEO_KoWRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::WHITE, rel, channel);
                break;
            }

            case NEO_KoWWRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::WARM_WHITE, rel, channel);
                break;
            }

            case NEO_KoCWRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::COOL_WHITE, rel, channel);
                break;
            }

            case NEO_KoHRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::HUE, rel, channel);
                break;
            }

            case NEO_KoSRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::SATURATION, rel, channel);
                break;
            }

            case NEO_KoVRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                startStopDimming(_segments[channel], SegmentConfig::VALUE, rel, channel);
                break;
            }

            case NEO_KoRGBRel:
            {
                // RGB relative control - expects 3-byte RGB value with relative changes
                uint32_t rgbRel = ko.value(DPT_Colour_RGB);
                uint8_t rRel = (rgbRel >> 16) & 0xFF;
                uint8_t gRel = (rgbRel >> 8) & 0xFF;
                uint8_t bRel = rgbRel & 0xFF;

                // Get current RGB values
                uint8_t currentR = targetSegment->getConfig().r();
                uint8_t currentG = targetSegment->getConfig().g();
                uint8_t currentB = targetSegment->getConfig().b();
                uint8_t currentW = targetSegment->getConfig().w();

                // Apply relative changes (treating 128 as no change, <128 as decrease, >128 as increase)
                int16_t newR = currentR + ((int16_t)rRel - 128);
                int16_t newG = currentG + ((int16_t)gRel - 128);
                int16_t newB = currentB + ((int16_t)bRel - 128);

                // Constrain to valid range
                newR = constrain(newR, 0, 255);
                newG = constrain(newG, 0, 255);
                newB = constrain(newB, 0, 255);

                // logDebugP("Segment %d RGB Relative: R=%d->%d, G=%d->%d, B=%d->%d", channel,
                //          currentR, newR, currentG, newG, currentB, newB);

                // Update config and apply
                targetSegment->setPrimaryColor((uint8_t)newR, (uint8_t)newG, (uint8_t)newB, currentW);
                break;
            }

            case NEO_KoHSVRel:
            {
                // HSV relative control - expects 3-byte HSV value with relative changes
                uint32_t hsvRel = ko.value(DPT_Colour_RGB); // Using RGB DPT for 3-byte HSV data
                uint8_t hRel = (hsvRel >> 16) & 0xFF;       // Hue relative (scaled to 0-255)
                uint8_t sRel = (hsvRel >> 8) & 0xFF;        // Saturation relative
                uint8_t vRel = hsvRel & 0xFF;               // Value relative

                // Get current RGB values and convert to HSV
                uint8_t currentR = targetSegment->getConfig().r();
                uint8_t currentG = targetSegment->getConfig().g();
                uint8_t currentB = targetSegment->getConfig().b();
                uint8_t currentW = targetSegment->getConfig().w();

                uint8_t currentH, currentS, currentV;
                ColorHelper::rgbToHSV(currentR, currentG, currentB, currentH, currentS, currentV);

                // Apply relative changes (treating 128 as no change, <128 as decrease, >128 as increase)
                int16_t newH = currentH + ((int16_t)hRel - 128);
                int16_t newS = currentS + ((int16_t)sRel - 128);
                int16_t newV = currentV + ((int16_t)vRel - 128);

                // Constrain values to valid ranges
                if (newH < 0) newH += 256;
                else if (newH >= 256)
                    newH -= 256; // Hue wraps around in 0-255 range
                newS = constrain(newS, 0, 255);
                newV = constrain(newV, 0, 255);

                logInfoP("Segment %d HSV Relative: H=%d->%d, S=%d->%d, V=%d->%d", channel,
                         currentH, newH, currentS, newS, currentV, newV);

                // Convert back to RGB and apply
                uint8_t newR, newG, newB;
                ColorHelper::hsvToRGB((uint8_t)newH, (uint8_t)newS, (uint8_t)newV, newR, newG, newB);

                // Update config and apply
                targetSegment->setPrimaryColor(newR, newG, newB, currentW);
                break;
            }

            case NEO_KoRGBWRel:
            {
                // RGBW relative control - expects 4-byte RGBW value with relative changes
                uint32_t rgbwRel = ko.value(DPT_Colour_RGBW);
                uint8_t rRel = (rgbwRel >> 24) & 0xFF;
                uint8_t gRel = (rgbwRel >> 16) & 0xFF;
                uint8_t bRel = (rgbwRel >> 8) & 0xFF;
                uint8_t wRel = rgbwRel & 0xFF;

                // Get current RGBW values
                uint8_t currentR = targetSegment->getConfig().r();
                uint8_t currentG = targetSegment->getConfig().g();
                uint8_t currentB = targetSegment->getConfig().b();
                uint8_t currentW = targetSegment->getConfig().w();

                // Apply relative changes (treating 128 as no change, <128 as decrease, >128 as increase)
                int16_t newR = currentR + ((int16_t)rRel - 128);
                int16_t newG = currentG + ((int16_t)gRel - 128);
                int16_t newB = currentB + ((int16_t)bRel - 128);
                int16_t newW = currentW + ((int16_t)wRel - 128);

                // Constrain to valid range
                newR = constrain(newR, 0, 255);
                newG = constrain(newG, 0, 255);
                newB = constrain(newB, 0, 255);
                newW = constrain(newW, 0, 255);

                logInfoP("Segment %d RGBW Relative: R=%d->%d, G=%d->%d, B=%d->%d, W=%d->%d", channel,
                         currentR, newR, currentG, newG, currentB, newB, currentW, newW);

                // Update config and apply
                targetSegment->setPrimaryColor((uint8_t)newR, (uint8_t)newG, (uint8_t)newB, (uint8_t)newW);
                break;
            }

            // Status-only KOs (prevent "Unhandled KO Index" warnings)
            case NEO_KoSegmentPowerState:
            case NEO_KoSegmentBrightnessState:
            case NEO_KoCCTState:
            case NEO_KoFxState:
            case NEO_KoSceneState:
            case NEO_KoRGBState:
            case NEO_KoHSVState:
            case NEO_KoRGBWState:
            case NEO_KoHCLState:
                // These are status/output-only KOs, ignore incoming writes
                break;

            default:
                logInfoP("Segment %d Unhandled KO Index: %d", channel, koIndex);
                break;
        }

        // Restore channel index
        _channelIndex = oldChannelIndex;

        // Auto-update cycle will render changes
    }
}

// ============================================================================
// Effektkette — Sync functions
// ============================================================================

/**
 * @brief Compose and send a 6-byte sync telegram on this segment's SyncChain KO.
 *
 * Payload layout:
 *   Byte 0: Flags  (bit0 = power on/off)
 *   Byte 1: EffectType
 *   Byte 2: Speed
 *   Byte 3: R (primary colour)
 *   Byte 4: G
 *   Byte 5: B
 *   Byte 6: W  (warm white / single white)
 *   Byte 7: Brightness
 *   Byte 8: CW (cool white, RGBCCT)
 */
void NeoPixelBusModule::composeSyncPayload(size_t segmentIndex, uint8_t* payload)
{
    auto& cfg = _segments[segmentIndex];
    auto& seg = *cfg.segment;
    const auto& ecfg = seg.getConfig();

    payload[0] = seg.getLedState() != LedState::IDLE ? 0x01 : 0x00; // power
    payload[1] = ecfg.effectType;
    payload[2] = ecfg.speed;
    payload[3] = ecfg.r();
    payload[4] = ecfg.g();
    payload[5] = ecfg.b();
    payload[6] = ecfg.ww();
    payload[7] = seg.getBrightness();
    payload[8] = ecfg.cw();
}

void NeoPixelBusModule::sendSyncTelegram(size_t segmentIndex)
{
#ifdef NEO_KoSyncChain
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (cfg.syncMode != 1 || !cfg.segment) return; // only masters send

    uint8_t oldCh = _channelIndex;
    _channelIndex = (uint8_t)segmentIndex;

    uint8_t payload[9];
    composeSyncPayload(segmentIndex, payload);

    auto& ko = KoNEO_SyncChain;
    uint8_t* ref = ko.valueRef();
    for (uint8_t b = 0; b < 9; b++)
        ref[b] = payload[b];
    for (uint8_t b = 9; b < ko.valueSize(); b++)
        ref[b] = 0;
    ko.objectWritten();

    // Remember what we sent for change detection in loopSyncMaster()
    memcpy(cfg.lastSyncPayload, payload, sizeof(cfg.lastSyncPayload));
    cfg.syncPayloadValid = true;
    cfg.lastSyncSentMs = millis();

    logInfoP("Effektkette [%zu]: sent sync power=%d effect=%d speed=%d rgb=(%d,%d,%d) w=%d bri=%d cw=%d",
             segmentIndex, payload[0], payload[1], payload[2], payload[3], payload[4], payload[5],
             payload[6], payload[7], payload[8]);

    _channelIndex = oldCh;
#endif
}

/**
 * @brief Apply an incoming 9-byte sync telegram to a slave segment.
 */
void NeoPixelBusModule::receiveSyncTelegram(size_t segmentIndex,
                                            const uint8_t* payload, uint8_t len)
{
    if (segmentIndex >= _segments.size() || len < 9) return;
    auto& cfg = _segments[segmentIndex];
    if (cfg.syncMode != 2 || !cfg.segment) return; // only slaves receive

    // Check override policy: if local override is active and policy = lokal hat Vorrang → ignore
    if (cfg.syncOverridePolicy == 1 && cfg.localOverride) return;

    cfg.lastSyncMs = millis();

    bool powerOn = (payload[0] & 0x01) != 0;
    uint8_t effectId = payload[1];
    uint8_t speed = payload[2];
    uint8_t r = payload[3], g = payload[4], b = payload[5];
    uint8_t w = payload[6], brightness = payload[7], cw = payload[8];

    if (!powerOn)
    {
        cfg.segment->clearAll();
        logInfoP("Effektkette [%zu]: sync power off", segmentIndex);
        return;
    }

    // Apply colour + speed + brightness to segment config
    auto& ecfg = cfg.segment->getConfig();
    ecfg.speed = speed;
    cfg.segment->setPrimaryColor(r, g, b, w, cw);
    cfg.segment->setBrightness(brightness);

    // Apply effect only when it actually changed (re-applying resets the effect each telegram)
    bool effectChanged = (cfg.segment->getLedState() == LedState::IDLE) ||
                         (getTypeFromEffect(cfg.segment->getEffect()) != effectId);
    if (effectChanged)
    {
        uint8_t oldCh = _channelIndex;
        _channelIndex = (uint8_t)segmentIndex;
        applyEffectToSegment(cfg.segment, effectId);
        // Colour was set before the effect clear — re-apply it
        cfg.segment->setPrimaryColor(r, g, b, w, cw);
        cfg.segment->setBrightness(brightness);
        _channelIndex = oldCh;
    }

    logInfoP("Effektkette [%zu]: received sync effect=%d speed=%d bri=%d%s",
             segmentIndex, effectId, speed, brightness, effectChanged ? " (effect applied)" : "");
}

/**
 * @brief Called every loop() iteration — master segments detect local state changes
 *        (power, effect, speed, colour, brightness) and broadcast a sync telegram.
 *        Rate-limited to one telegram per 300 ms per segment to keep bus load low
 *        during dimming ramps.
 */
void NeoPixelBusModule::loopSyncMaster()
{
#ifdef NEO_KoSyncChain
    uint32_t now = millis();
    for (size_t i = 0; i < _segments.size(); i++)
    {
        auto& cfg = _segments[i];
        if (cfg.syncMode != 1 || !cfg.segment) continue; // only masters

        uint8_t payload[9];
        composeSyncPayload(i, payload);

        if (cfg.syncPayloadValid && memcmp(payload, cfg.lastSyncPayload, sizeof(payload)) == 0)
            continue; // nothing changed

        if (cfg.syncPayloadValid && (now - cfg.lastSyncSentMs) < 300u)
            continue; // rate limit — retry next loop

        sendSyncTelegram(i);
    }
#endif
}

/**
 * @brief Called every loop() iteration — checks if any slave has timed out waiting for sync.
 */
void NeoPixelBusModule::loopSyncWatchdog()
{
    uint32_t now = millis();
    for (auto& cfg : _segments)
    {
        if (cfg.syncMode != 2) continue;         // only slaves
        if (cfg.syncTimeoutSteps == 0) continue; // no timeout configured
        if (cfg.lastSyncMs == 0) continue;       // never received any sync yet

        uint32_t timeoutMs = (uint32_t)cfg.syncTimeoutSteps * 10000u;
        if (now - cfg.lastSyncMs > timeoutMs)
        {
            // Timeout: turn off segment
            if (cfg.segment) cfg.segment->clearAll();
            cfg.lastSyncMs = 0; // reset so we don't keep clearing every frame
            logInfoP("Effektkette slave timeout — segment turned off");
        }
    }
}

// Console: show help entries by delegating to core NeoPixel module
void NeoPixelBusModule::showHelp()
{
    // Print the 'neo' command group header via core module
    _neoPixel.showHelp();
    openknx.console.printHelpLine("neo led status", "Show active NeoPixel blink codes");
    openknx.console.printHelpLine("neo led error <1-5>", "Test error blink code on STATUS LED");
    openknx.console.printHelpLine("neo led warn <1-3>", "Test warning pulsing on STATUS LED");
    openknx.console.printHelpLine("neo led clear", "Clear all NeoPixel blink codes");
    openknx.console.printHelpLine("neo flash [seg]", "Show persisted flash contents (scene/EM/color/effect per segment)");
#ifdef OPENKNX_DEBUG
    // Add NeoPixel-specific debug commands
    openknx.console.printHelpLine("neoa", "Show NeoPixel ETS configuration analysis");
#endif
}

// Console: process commands by delegating 'neo' prefixed commands
bool NeoPixelBusModule::processCommand(const std::string command, bool diagnose)
{
#ifdef OPENKNX_DEBUG
    // Handle NeoPixel-specific debug commands
    if (command == "neoa")
    {
        debugShowConfiguration();
        return true;
    }
#endif

    // neo init — bring the module up WITHOUT an ETS download so the manual console commands
    // (neo phys/virt/seg/effect/cue/em ...) are accepted and loop() renders. Production
    // configures via ETS; this is a console bring-up (runtime-only, lost on reboot).
    if (command == "neo init")
    {
        if (_initialized)
        {
            logInfoP("neo init: already initialized (testMode=%d)", (int)_localTestMode);
            return true;
        }
        _neoPixel.init();           // create the manager
        _neoPixel.setup(true);      // sets the library's _initialized flag (no strips yet)
        _neoPixel.setAutoUpdate(true); // ETS normally enables this; do it here so effects animate
        _initialized   = true;      // module-level init flag (gates loop() + console)
        _localTestMode = true;      // let loop() run without ETS 'configured'
        logInfoP("neo init: NeoPixel up in LOCAL TEST MODE (no ETS config).");
        logInfoP("  Build: neo phys add <gpio> <n> <type> | neo virt add <n> (+ neo virt attach <v> <p>) | neo seg add <v> <s> <e> | neo effect set <s> <id>");
        logInfoP("  NOTE: runtime-only (lost on reboot). Build EMs/cues via neo em config / neo cue set.");
        return true;
    }

    // neo led <subcommand>
    if (command.compare(0, 8, "neo led ") == 0)
    {
        std::string sub = command.substr(8);

        if (sub == "status")
        {
            logInfoP("Active error code: %d (0=none)", _activeErrorCode);
            logInfoP("Active warn  code: %d (0=none)", _activeWarnCode);
            return true;
        }
        else if (sub == "clear")
        {
            _activeErrorCode = 0;
            _activeWarnCode = 0;
            auto* led = openknx.ledFunctions.get(OPENKNX_LEDFUNC_BASE_STATE);
            if (led)
            {
                led->errorCode(0); // Disable error mode
                led->off();        // Turn off pulsing
            }
            logInfoP("Blink codes cleared");
            return true;
        }
        else if (sub.compare(0, 6, "error ") == 0)
        {
            uint8_t code = (uint8_t)atoi(sub.c_str() + 6);
            if (code < 1 || code > 5)
            {
                logInfoP("Usage: neo led error <1-5>");
                return true;
            }
            // Reset tracking so test code always applies
            _activeErrorCode = 0;
            _activeWarnCode = 0;
            setErrorBlink(code);
            return true;
        }
        else if (sub.compare(0, 5, "warn ") == 0)
        {
            uint8_t code = (uint8_t)atoi(sub.c_str() + 5);
            if (code < 1 || code > 3)
            {
                logInfoP("Usage: neo led warn <1-3> (1=Orange/ABL, 2=Purple/Flash, 3=Yellow/Effect)");
                return true;
            }
            // Reset tracking so test code always applies
            _activeErrorCode = 0;
            _activeWarnCode = 0;
            setWarningBlink(code); // color derived from code (single source: warnColorForCode)
            return true;
        }
        else
        {
            logInfoP("Usage: neo led status|clear|error <1-5>|warn <1-3>");
            return true;
        }
    }

    // neo flash [seg] — dump persisted flash contents (OAM-owned: flash
    // persistence is an OAM concern, so both parsing and rendering live here)
    if (command == "neo flash" || command.compare(0, 10, "neo flash ") == 0)
    {
        std::string sub = (command.length() > 10) ? command.substr(10) : "";
        if (sub == "?")
        {
            logInfoP("Usage: neo flash [seg]  — show persisted flash contents (all segments or one, seg=0-based)");
            return true;
        }
        int onlySeg = sub.empty() ? -1 : atoi(sub.c_str());
        if (onlySeg < -1) onlySeg = -1;
        printFlashStateTable(onlySeg);
        return true;
    }

    return _neoPixel.processCommand(command, diagnose);
}

// ============================================================================
// Console data providers — rendering happens in OFM (NeoPixelConsole.cpp),
// the OAM only delivers raw data snapshots (see NeoPixelEmConsole.h).
// ============================================================================

int NeoPixelBusModule::emConsoleSegmentCount() const
{
    return (int)_segments.size();
}

bool NeoPixelBusModule::emConsoleGetEmStatus(uint8_t seg, NeoEmSegStatus& out)
{
    if (seg >= _segments.size()) return false;

    const auto& cfg = _segments[seg];
    out.hasSegment = (cfg.segment != nullptr);
    out.activeEmId = cfg.emController.activeEmId();
    out.activeCueNum = cfg.emController.activeCueNum();
    out.running = cfg.emController.isRunning();
    out.lastEmId = cfg.emController.lastEmId();

    // Read startup EM from ETS parameters (channel-indexed access)
    uint8_t oldCh = _channelIndex;
    _channelIndex = seg;
    out.startupEm = (uint8_t)ParamNEO_NEOStartupEM;
    _channelIndex = oldCh;

    return true;
}

const EffektManagerData* NeoPixelBusModule::emConsoleGetEmData(uint8_t emId) const
{
    if (emId == 0 || emId > EM_COUNT || !_emData) return nullptr;
    return &_emData[emId - 1];
}

bool NeoPixelBusModule::emConsoleGetChainStatus(uint8_t seg, NeoChainSegStatus& out) const
{
    if (seg >= _segments.size()) return false;

    const auto& cfg = _segments[seg];
    out.syncMode = cfg.syncMode;
    out.overridePolicy = cfg.syncOverridePolicy;
    out.timeoutSteps = cfg.syncTimeoutSteps;
    out.localOverride = cfg.localOverride;
    out.lastSyncMs = cfg.lastSyncMs;
    out.virtualTotalLength = cfg.virtualTotalLength;
    out.virtualOffset = cfg.virtualOffset;
    return true;
}

void NeoPixelBusModule::printFlashStateTable(int onlySeg)
{
    // The saved* fields are the decoded SegmentFlashState as restored at boot
    // (see NeoPixelFlashPersistence::readFromFlash). The repurposed flash bytes
    // map as: Scene = reserved[0] (savedSceneNumber), EM = reserved[1]
    // (emController.lastEmId()). Output uses the shared console logger (no module
    // prefix) to stay visually consistent with neo phys / neo seg / neo em.
    const char* SEP = "═════════════════════════════════════════════════════════════════════════════";

    openknx.logger.log("");
    openknx.logger.color(CONSOLE_HEADLINE_COLOR);
    openknx.logger.log(SEP);
    openknx.logger.log("  Flash Persistence Contents (restored at boot)");
    openknx.logger.log(SEP);
    openknx.logger.color(0);

    if (_segments.empty())
    {
        openknx.logger.log("No segments created.");
    }
    else
    {
        openknx.logger.log("Seg │ Valid │ Pwr │  R  │  G  │  B  │ WW  │ CW  │ Bri │ Fx │ LastFx │ Scene │ EM");
        openknx.logger.log("────┼───────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼────┼────────┼───────┼────");

        for (size_t i = 0; i < _segments.size(); i++)
        {
            if (onlySeg >= 0 && (int)i != onlySeg) continue;

            const auto& cfg = _segments[i];
            if (cfg.segment == nullptr)
            {
                openknx.logger.logWithValues("%3d │ %-5s │ <no segment>", (int)i, "-");
                continue;
            }

            openknx.logger.logWithValues("%3d │ %-5s │ %-3s │ %3d │ %3d │ %3d │ %3d │ %3d │ %3d │ %2d │ %-6s │ %5d │ %3d",
                                         (int)i,
                                         cfg.savedValid ? "yes" : "no",
                                         cfg.savedPower ? "on" : "off",
                                         (int)cfg.savedR, (int)cfg.savedG, (int)cfg.savedB,
                                         (int)cfg.savedWW, (int)cfg.savedCW,
                                         (int)cfg.savedBrightness,
                                         (int)cfg.savedEffectType,
                                         cfg.savedEffectValid ? (cfg.savedLastWasEffect ? "effect" : "color") : "-",
                                         (int)cfg.savedSceneNumber,
                                         (int)cfg.emController.lastEmId());
        }
    }

    openknx.logger.color(CONSOLE_HEADLINE_COLOR);
    openknx.logger.log(SEP);
    openknx.logger.color(0);
    openknx.logger.log("Legend: Scene=reserved[0] (0=none), EM=reserved[1] (0=none), Fx=effect ID (0=none),");
    openknx.logger.log("        LastFx=last output kind, Valid=flash held data for this segment.");
    openknx.logger.log("");
}

// ============================================================================
// Console actions — mutating neo em/cue/chain commands (parsed and rendered
// in OFM, executed here)
// ============================================================================

bool NeoPixelBusModule::executeEmChainAction(uint8_t action, int arg1, int arg2)
{
    if (!_emData) return false; // EM table not allocated (out of memory) -> EM disabled

    // Console EM/cue authoring. Here arg1 is an EM id (CONFIG/CLEAR) or a MANAGER segment
    //    index (BIND) — NOT an OAM segment — so handle before the segment-index check below.
    switch (action)
    {
        case NEO_EM_CONFIG:
        {
            if (arg1 < 1 || arg1 > EM_COUNT) return false;
            EffektManagerHeader& h = _emData[arg1 - 1].header;
            h.loop     = (arg2 & 0xFF) ? 1 : 0;
            h.nextEmId = (uint8_t)((arg2 >> 8) & 0xFF);
            h.enabled  = 1; // mark active (cueCount comes from 'neo cue set')
            return true;
        }
        case NEO_CUE_CLEAR:
        {
            if (arg1 < 1 || arg1 > EM_COUNT) return false;
            _emData[arg1 - 1].header.cueCount = 0;
            return true;
        }
        case NEO_EM_BIND:
            return bindConsoleSegment(arg1) >= 0;
        default:
            break; // fall through to the segment-indexed actions
    }

    if (arg1 < 0 || arg1 >= (int)_segments.size()) return false;
    const size_t seg = (size_t)arg1;

    switch (action)
    {
        case NEO_EM_START:
        {
            if (arg2 < 0 || arg2 > EM_COUNT) return false;
            startEffektManager(seg, (uint8_t)arg2);
            // Same as the NEO_KoEmStart KO: a successful direct start makes the EM the active source,
            // so drop any persisted scene marker (restore checks scene before EM). Guard on "this emId
            // is now active" so a refused start (while a different EM runs) does not wipe the marker.
            if (arg2 != EM_NONE && _segments[seg].emController.activeEmId() == (uint8_t)arg2)
                _segments[seg].savedSceneNumber = 0;
            return true;
        }
        case NEO_EM_STOP:
        {
            stopEffektManager(seg);
            return true;
        }
        case NEO_EM_CUE:
        {
            if (arg2 < 1 || arg2 > EM_CUE_COUNT) return false;

            auto& cfg = _segments[seg];
            if (!cfg.segment)
            {
                openknxNeoPixelConsolePrintf("Segment %d has no segment instance", (int)seg);
                return false;
            }
            if (!cfg.emController.triggerCue((uint8_t)arg2, cfg.segment, _emData))
            {
                openknxNeoPixelConsolePrintf("Segment %d: cue trigger failed (active EM required, cue must be within configured range)", (int)seg);
                return false;
            }
            sendEmStatusKOs(seg);
            return true;
        }
        case NEO_CHAIN_SET:
        {
            if (arg2 < 0 || arg2 > 2) return false;
            auto& cfg = _segments[seg];
            cfg.syncMode = (uint8_t)arg2;
            cfg.localOverride = false;
            if (arg2 != 2) cfg.lastSyncMs = 0;
            return true;
        }
        case NEO_CHAIN_OVERRIDE:
        {
            if (arg2 != 0 && arg2 != 1) return false;
            _segments[seg].localOverride = (arg2 == 1);
            return true;
        }
        case NEO_CHAIN_TRIGGER:
        {
            if (_segments[seg].syncMode != 1)
            {
                openknxNeoPixelConsolePrintf("Segment %d is not in master mode", (int)seg);
                return false;
            }
            sendSyncTelegram(seg);
            return true;
        }
        default:
            return false;
    }
}

// Console EM/cue authoring: define one cue in _emData (params seeded with effect defaults).
bool NeoPixelBusModule::executeCueSet(uint8_t emId, uint8_t cueNum, uint8_t effectId,
                                      uint16_t durSec, uint16_t fadeMs,
                                      uint8_t bri, uint8_t r, uint8_t g, uint8_t b)
{
    if (!_emData) return false;
    if (emId < 1 || emId > EM_COUNT) return false;
    if (cueNum < 1 || cueNum > EM_CUE_COUNT) return false;

    EffektManagerData& em = _emData[emId - 1];
    EffektCue& c = em.cues[cueNum - 1];
    c = EffektCue{};            // zero all fields
    c.effectId    = effectId;
    // Seed params with the chosen effect's OWN defaults. A zeroed cue would leave every
    // param at 0, and applyCue()'s "value < min → default" substitution never fires for
    // params whose min is 0 (e.g. Clock BlinkColon default 1, Snake BodyHue default 85=green).
    // Without this the clock never blinks and the snake body renders red (hue 0) — both come
    // straight from the cue carrying 0 instead of the effect default.
    if (Effect* eff = EffectPool::getEffectByIndex(effectId))
    {
        uint8_t pc = eff->getParameterCount();
        for (uint8_t i = 0; i < pc && i < EM_PARAM_COUNT; i++)
        {
            c.params[i] = (uint8_t)eff->getParameterDefault(i);
        }
    }
    c.r = r; c.g = g; c.b = b; c.w = 0;
    c.brightness  = bri;
    c.durationSec = durSec;
    c.fadeMs      = fadeMs;
    c.cueName[0]    = '\0';
    // A cue carries no text by default -> stays empty. This keeps a runtime text set via
    // the Effekt-Text KO authoritative across cue changes (see the guard in EffektManager),
    // and ScrollText renders its own DEFAULT_TEXT at render time when the text is truly empty.
    c.effectText[0] = '\0';
    if (cueNum > em.header.cueCount) em.header.cueCount = cueNum; // grow active cue count
    return true;
}

bool NeoPixelBusModule::executeCueParam(uint8_t emId, uint8_t cueNum, uint8_t paramIdx, uint8_t value)
{
    if (!_emData) return false;
    if (emId < 1 || emId > EM_COUNT) return false;
    if (cueNum < 1 || cueNum > EM_CUE_COUNT) return false;
    if (paramIdx >= EM_PARAM_COUNT) return false;

    EffektCue& c = _emData[emId - 1].cues[cueNum - 1];
    // Clamp against the cue's effect so an out-of-range value can't break the effect.
    if (Effect* eff = EffectPool::getEffectByIndex(c.effectId))
    {
        if (paramIdx >= eff->getParameterCount()) return false;
        uint32_t v = value;
        uint32_t lo = eff->getParameterMin(paramIdx);
        uint32_t hi = eff->getParameterMax(paramIdx);
        if (v < lo) v = lo;
        if (v > hi) v = hi;
        value = (uint8_t)v;
    }
    c.params[paramIdx] = value;
    return true;
}

bool NeoPixelBusModule::executeCueText(uint8_t emId, uint8_t cueNum, const char* text)
{
    if (!_emData) return false;
    if (emId < 1 || emId > EM_COUNT) return false;
    if (cueNum < 1 || cueNum > EM_CUE_COUNT) return false;

    EffektCue& c = _emData[emId - 1].cues[cueNum - 1];
    const char* t = text ? text : "";
    strncpy(c.effectText, t, sizeof(c.effectText) - 1);
    c.effectText[sizeof(c.effectText) - 1] = '\0';

    // Text longer than the 14-byte cue field is kept in a RAM side-store and re-applied to
    // the segment whenever this cue is active (applyCueLongText), so Scroll Text can show the
    // full string (~240 chars) even though the ETS-mirrored cue itself only holds 13.
    const uint16_t key = ((uint16_t)emId << 8) | cueNum;
    if (strlen(t) > sizeof(c.effectText) - 1)
        _cueLongText[key] = t;
    else
        _cueLongText.erase(key);
    return true;
}

// Wrap a console (manager) segment into the OAM _segments list (with an emController)
// so loopEffektManager() ticks it. Returns the OAM segment index, or -1.
int NeoPixelBusModule::bindConsoleSegment(int managerSegIdx)
{
    auto* mgr = _neoPixel.getManager();
    if (!mgr) { openknxNeoPixelConsolePrintf("bind: not initialized (run 'neo init')"); return -1; }
    if (managerSegIdx < 0 || (uint32_t)managerSegIdx >= mgr->getSegmentCount())
    {
        openknxNeoPixelConsolePrintf("bind: manager segment %d not found (have %u)",
                                     managerSegIdx, (unsigned)mgr->getSegmentCount());
        return -1;
    }
    Segment* seg = mgr->getSegment((uint32_t)managerSegIdx);
    if (!seg) { openknxNeoPixelConsolePrintf("bind: manager segment %d is null", managerSegIdx); return -1; }

    for (size_t i = 0; i < _segments.size(); i++)
        if (_segments[i].segment == seg)
        {
            openknxNeoPixelConsolePrintf("bind: already bound as OAM segment %u", (unsigned)i);
            return (int)i;
        }

    SegmentConfig cfg;
    cfg.segment  = seg;
    cfg.startLed = seg->getStartLed();
    cfg.endLed   = seg->getEndLed();
    _segments.push_back(cfg);
    const size_t idx = _segments.size() - 1;
    openknxNeoPixelConsolePrintf("bind: manager seg %d -> OAM segment %u  (now: neo em start %u <em>)",
                                 managerSegIdx, (unsigned)idx, (unsigned)idx);
    return (int)idx;
}

// FunctionProperty: Handle ETS online functions (hardware detection)
bool NeoPixelBusModule::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t* data, uint8_t* resultData, uint8_t& resultLength)
{
    // ALWAYS log when called - even if wrong object/property
    logDebugP("==> processFunctionProperty CALLED: ObjectIndex=%d, PropertyId=%d, Length=%d", objectIndex, propertyId, length);
    logDebugP("    Expected: ObjectIndex=%d, PropertyId=%d", NEOPIXEL_FUNCTION_OBJECT_INDEX, NEOPIXEL_FUNCTION_PROPERTY_ID);

    // Check if this is our hardware detection function property
    if (objectIndex != NEOPIXEL_FUNCTION_OBJECT_INDEX || propertyId != NEOPIXEL_FUNCTION_PROPERTY_ID)
    {
        logErrorP("Hardware detection triggered from ETS with unexpected object/property (ObjectIndex: %d, PropertyId: %d). Expected %d/%d",
                  objectIndex, propertyId, NEOPIXEL_FUNCTION_OBJECT_INDEX, NEOPIXEL_FUNCTION_PROPERTY_ID);
        return false;
    }

// Read hardware ID from compile-time define
#ifdef DEVICE_HW_ID
    uint16_t hwId = DEVICE_HW_ID;

    // Return format: [0, hwId_high, hwId_low]
    resultData[0] = 0;             // Success
    resultData[1] = (hwId >> 8);   // High byte
    resultData[2] = (hwId & 0xFF); // Low byte
    resultLength = 3;

    logDebugP("==> Hardware detection SUCCESS: DEVICE_HW_ID = 0x%04X (resultLength=%d)", hwId, resultLength);
    return true;
#else
    // Hardware ID not defined in build
    resultData[0] = 2; // Error: hardware ID not available
    resultLength = 1;
    logErrorP("Hardware detection failed: DEVICE_HW_ID not defined");
    return false;
#endif
}

// =============================================================================
// GPIO Pin Initialization
// =============================================================================

void NeoPixelBusModule::initializeGpioPins()
{
    logInfoP("Initializing GPIO pins - sending LED RESET signals...");

    uint8_t initializedCount = 0;

    // Iterate through ALL possible port indices (0-based) defined by hardware mapping
    // This sets all hardware GPIOs to LOW and sends RESET to LEDs
    // NOTE: WS2812 LEDs will RESET but keep their previous color in latches!
    // To actually turn LEDs OFF, we must send RGB(0,0,0) via the timing protocol
    // (this happens later in configureFromETS() after strip creation)
    for (uint8_t portIndex = 0; portIndex < NEOPIXEL_HW_PORT_COUNT; portIndex++)
    {
        uint8_t gpio = mapPortIndexToGpio(portIndex);

        // Skip undefined GPIOs (255 = not defined in platformio.hardware.ini)
        if (gpio == 255) continue;

        // Robust GPIO initialization to prevent undefined LED states
        // Step 1: Reset pin to INPUT to clear any previous state/hardware functions
        pinMode(gpio, INPUT);
        // Step 2: Set to LOW while still INPUT (sets pull-down if available)
        digitalWrite(gpio, LOW);
        // Step 3: Switch to OUTPUT mode
        pinMode(gpio, OUTPUT);
        // Step 4: Ensure LOW state in OUTPUT mode
        digitalWrite(gpio, LOW);
        // Step 5: CRITICAL - Hold LOW for 300µs to send LED RESET signal for all LED types
        // WS2812/WS2812B/SK6812: >50µs (typ. 280µs)
        // WS2813: >300µs, WS2815: >280µs, APA106: >50µs, TM1814: >200µs
        // 300µs is safe for ALL common LED types to reset and turn off
        delayMicroseconds(300);
        initializedCount++;

        logDebugP("Port %d: GPIO %d set to LOW", portIndex + 1, gpio);
    }

    logInfoP("GPIO initialization complete - %d pins set to LOW (RESET sent to LEDs)", initializedCount);
}

// =============================================================================
// Configuration & Setup
// =============================================================================

void NeoPixelBusModule::configureFromETS()
{
    // Initialize the OFM-NeoPixel module first
    _neoPixel.init();

// Log hardware configuration and check for mismatch
#ifdef DEVICE_HW_ID
    uint8_t hwIndex = getCurrentHardwareIndex();
    const char* hwName = HardwareMapping::getHardwareName(hwIndex);
    logInfoP("Hardware: %s (ID: 0x%04X, Index: %d) - Compile-time mode",
             hwName ? hwName : "Unknown", DEVICE_HW_ID, hwIndex);

    // Check if ETS configuration matches compiled hardware.
    // The ETS hardware selection stores the unique DEVICE_HW_ID (build-independent identity),
    // so we compare it directly against this firmware's compiled DEVICE_HW_ID. This is robust
    // regardless of how many hardware variants the firmware/product was built with, and stays
    // valid across firmware/ETS updates (adding/removing other hardware cannot shift it).
    #ifdef ParamNEO_NeoPixelHardwareSelect
    uint16_t selectedHwId = (uint16_t)ParamNEO_NeoPixelHardwareSelect; // stored selection = DEVICE_HW_ID
    if (selectedHwId != DEVICE_HW_ID)
    {
        _hwConfigMismatch = true;
        const char* compiledHwName = HardwareMapping::getHardwareName(HardwareMapping::mapDeviceHwIdToIndex(DEVICE_HW_ID));
        const char* etsHwName = HardwareMapping::getHardwareName(HardwareMapping::mapDeviceHwIdToIndex(selectedHwId));
        logErrorP("!!!! HARDWARE MISMATCH !!!!");
        logErrorP("  Compiled for:  %s (ID: 0x%04X)",
                  compiledHwName ? compiledHwName : "Unknown", DEVICE_HW_ID);
        logErrorP("  ETS configured: %s (ID: 0x%04X)",
                  etsHwName ? etsHwName : "Unknown", selectedHwId);
        logErrorP("  GPIO Port configuration from ETS will be IGNORED, to prevent damage and unexpected behaviors!");
        logErrorP("  Please Choose the correct Hardware in ETS or use correct firmware for this device.");
        logErrorP("!!!! HARDWARE MISMATCH !!!!");

        if (selectedHwId == 255)
        {
            // No hardware configured in ETS (dummy value)
            setErrorBlink(NEO_ERROR_NO_HW_CONFIGURED); // 2× blink
        }
        else
        {
            // True hardware mismatch
            setErrorBlink(NEO_ERROR_HW_MISMATCH); // 1× blink
        }
    }
    else
    {
        _hwConfigMismatch = false;
    }
    #endif
#else
    #ifdef ParamNEO_NeoPixelHardwareSelect
    uint16_t selectedHwId = (uint16_t)ParamNEO_NeoPixelHardwareSelect; // stored selection = DEVICE_HW_ID
    const char* hwName = HardwareMapping::getHardwareName(HardwareMapping::mapDeviceHwIdToIndex(selectedHwId));
    logInfoP("Hardware: %s (ID: 0x%04X) - ETS runtime selection",
             hwName ? hwName : "Unknown", selectedHwId);
    _hwConfigMismatch = false; // No mismatch in runtime mode
    #else
    logWarningP("Hardware selection not available - using default configuration");
    _hwConfigMismatch = false;
    #endif
#endif

    // Track used GPIO pins to avoid conflicts
    std::vector<uint8_t> usedPins;

// Available GPIO pins in order (PIN1-PIN8)
#if defined(OKNXHW_OPENKNXIAO_RP2040_V1_COMMON)
    const uint8_t availablePins[] = {
        KNXIAO_RP2040_PIN1, KNXIAO_RP2040_PIN2, KNXIAO_RP2040_PIN3, KNXIAO_RP2040_PIN4,
        KNXIAO_RP2040_PIN5, KNXIAO_RP2040_PIN6, KNXIAO_RP2040_PIN7, KNXIAO_RP2040_PIN8};
#elif defined(OKNXHW_OPENKNXIAO_ESP32S3_V1_COMMON)
    const uint8_t availablePins[] = {
        KNXIAO_ESP32S3_PIN1, KNXIAO_ESP32S3_PIN2, KNXIAO_ESP32S3_PIN3, KNXIAO_ESP32S3_PIN4,
        KNXIAO_ESP32S3_PIN5, KNXIAO_ESP32S3_PIN6, KNXIAO_ESP32S3_PIN7, KNXIAO_ESP32S3_PIN8};
#else
    const uint8_t availablePins[] = {27, 28, 29, 6, 7, 2, 4, 3}; // Fallback
#endif
    const uint8_t numAvailablePins = sizeof(availablePins) / sizeof(availablePins[0]);
    uint8_t nextPinIndex = 0; // Index into availablePins array

    // Helper lambda to get next available pin(s)
    auto getNextPin = [&](uint8_t count = 1) -> bool {
        // Check if we have enough pins available
        if (nextPinIndex + count > numAvailablePins)
        {
            return false; // Not enough pins
        }
        nextPinIndex += count;
        return true;
    };

    // Helper lambda to check if a pin is already used
    auto isPinUsed = [&usedPins](uint8_t pin) {
        return std::find(usedPins.begin(), usedPins.end(), pin) != usedPins.end();
    };

    // CRITICAL SAFETY GUARD: Pins reserved by the KNX/BCU subsystem must NEVER be driven by a LED strip.
    // On KNeoPix XIAO hardware KNX_UART_TX_PIN == GPIO0 and KNX_UART_RX_PIN == GPIO1. The ETS defaults
    // (manual GPIO mode + DataGPIO=0) would otherwise let an unconfigured strip take over the KNX TX line,
    // which kills the TPUart bus initialization (endless "Try Initialize", never "BCU connected").
    // This check rejects such strips regardless of the configuration mode (manual / hardware port / auto).
    auto isReservedPin = [](uint8_t pin) -> bool {
#if defined(KNX_UART_TX_PIN)
        if (pin == (uint8_t)KNX_UART_TX_PIN) return true;
#endif
#if defined(KNX_UART_RX_PIN)
        if (pin == (uint8_t)KNX_UART_RX_PIN) return true;
#endif
#if defined(SAVE_INTERRUPT_PIN)
        if (pin == (uint8_t)SAVE_INTERRUPT_PIN) return true;
#endif
        return false;
    };

    // Configure external relays (max derived from knxprod.h) and reserve their GPIO pins
    _relayCount = std::min<uint8_t>(kMaxExternalRelays, ParamNEO_NEOExternalRelayCount);
    for (uint8_t i = 0; i < kRelayStorageSize; ++i)
    {
        _relayPins[i] = 255;
        _relayStates[i] = false;
        _relayInverted[i] = false;
    }

    if (_relayCount > 0)
    {
        if (_hwConfigMismatch)
        {
            logErrorP("External relays disabled due to hardware mismatch");
            _relayCount = 0;
        }
        else
        {
            auto resolveRelayGpio = [](uint8_t relayIndex) -> uint8_t {
                // Get port selection from ETS
                uint8_t portIndex;
                switch (relayIndex)
                {
                    case 0: portIndex = (uint8_t)ParamNEO_NEOExternalRelay1Port; break;
                    case 1: portIndex = (uint8_t)ParamNEO_NEOExternalRelay2Port; break;
                    case 2: portIndex = (uint8_t)ParamNEO_NEOExternalRelay3Port; break;
                    case 3: portIndex = (uint8_t)ParamNEO_NEOExternalRelay4Port; break;
                    default: portIndex = 255; break;
                }

                // Check if "Manuell" (Value 10) is selected in Port dropdown
                if (portIndex == 10)
                {
                    // Return manual GPIO number from ETS parameter
                    switch (relayIndex)
                    {
#if defined(ParamNEO_NEOExternalRelay1GPIO)
                        case 0: return (uint8_t)ParamNEO_NEOExternalRelay1GPIO;
#endif
#if defined(ParamNEO_NEOExternalRelay2GPIO)
                        case 1: return (uint8_t)ParamNEO_NEOExternalRelay2GPIO;
#endif
#if defined(ParamNEO_NEOExternalRelay3GPIO)
                        case 2: return (uint8_t)ParamNEO_NEOExternalRelay3GPIO;
#endif
#if defined(ParamNEO_NEOExternalRelay4GPIO)
                        case 3: return (uint8_t)ParamNEO_NEOExternalRelay4GPIO;
#endif
                        default: return 255;
                    }
                }

                // Otherwise use hardware port mapping (portIndex 0-N)

                if (!isHardwarePortSelected(portIndex))
                {
                    return 255;
                }
                return mapPortIndexToGpio(portIndex);
            };

            for (uint8_t i = 0; i < _relayCount; ++i)
            {
                uint8_t gpio = resolveRelayGpio(i);
                _relayPins[i] = gpio;

                if (gpio == 255)
                {
                    logWarningP("Relay %d: No valid GPIO selected in ETS", i + 1);
                    continue;
                }

                if (isPinUsed(gpio))
                {
                    logErrorP("Relay %d: GPIO %d already in use", i + 1, gpio);
                }
                else
                {
                    usedPins.push_back(gpio);
                    logInfoP("Relay %d: Reserved GPIO %d", i + 1, gpio);
                }

                pinMode(gpio, OUTPUT);
                digitalWrite(gpio, LOW); // Default OFF for safety
                _relayStates[i] = false;
                logInfoP("Relay %d: Initial state OFF", i + 1);

// Read output logic (invert) setting per relay
#if defined(ParamNEO_NEOExternalRelay1OutputLogic)
                if (i == 0) _relayInverted[i] = (ParamNEO_NEOExternalRelay1OutputLogic != 0);
#endif
#if defined(ParamNEO_NEOExternalRelay2OutputLogic)
                if (i == 1) _relayInverted[i] = (ParamNEO_NEOExternalRelay2OutputLogic != 0);
#endif
#if defined(ParamNEO_NEOExternalRelay3OutputLogic)
                if (i == 2) _relayInverted[i] = (ParamNEO_NEOExternalRelay3OutputLogic != 0);
#endif
#if defined(ParamNEO_NEOExternalRelay4OutputLogic)
                if (i == 3) _relayInverted[i] = (ParamNEO_NEOExternalRelay4OutputLogic != 0);
#endif
                if (_relayInverted[i]) logInfoP("Relay %d: Output logic INVERTED (LOW = ON)", i + 1);
            }
        }
    }

    // 1) Determine number of strips from ETS (match the runtime strip limit)
    const uint8_t maxStrips = std::max<uint8_t>(1, std::min<uint8_t>(NEOPIXEL_MAX_PHYSICAL_STRIPS, ParamNEO_NEONumberOfLEDStrips));
    _totalLeds = 0;

    // CRITICAL: Delete old PhysicalStrip objects before clearing vector!
    // This prevents memory leaks and deregisters old DMA handlers
    for (auto* strip : _physicalStrips)
    {
        if (strip) delete strip;
    }
    _physicalStrips.clear();

    // Clear virtual strip configuration
    _virtualStripConfiguration.clear();

    // PRE-SCAN: Mark all manually configured GPIO pins as used BEFORE auto-allocation starts
    // This prevents auto-allocation from using pins that are manually configured on other strips
    logInfoP("Pre-scanning %d strips for manual GPIO configurations...", maxStrips);
    for (uint8_t i = 0; i < maxStrips; ++i)
    {
        // CRITICAL: Set module _channelIndex so ParamNEOSTRIP_* macros read from correct strip
        setChannelIndex(i);
        logInfoP("PRE-SCAN Strip %d: Set _channelIndex = %d", i, _channelIndex);

        const uint16_t pixels = (uint16_t)ParamNEOSTRIP_NEOLength;
        logInfoP("PRE-SCAN Strip %d: Read pixels = %d (from offset %d)", i, pixels, NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize + NEOSTRIP_NEOLength);
        if (pixels == 0) continue; // Skip strips with 0 LEDs

        const uint8_t ledTypeParam = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const LedProtocol proto = StripConfiguration::mapProtocol(ledTypeParam);
        const bool gpioManualConfig = (bool)ParamNEOSTRIP_NEOGPIOManual;

        if (gpioManualConfig)
        {
            if (isSpiProtocol(proto))
            {
                // SPI protocols need 2 pins - read from manual GPIO parameters
                uint8_t mosiGpio = (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO;
                uint8_t sckGpio = (uint8_t)ParamNEOSTRIP_NEOClockGPIO;

                if (!isPinUsed(sckGpio))
                {
                    usedPins.push_back(sckGpio);
                    logInfoP("Strip %d: Pre-marked manual SCK GPIO %d as used", i, sckGpio);
                }
                else
                {
                    logWarningP("Strip %d: Manual SCK GPIO %d already marked as used!", i, sckGpio);
                }

                if (!isPinUsed(mosiGpio))
                {
                    usedPins.push_back(mosiGpio);
                    logInfoP("Strip %d: Pre-marked manual MOSI GPIO %d as used", i, mosiGpio);
                }
                else
                {
                    logWarningP("Strip %d: Manual MOSI GPIO %d already marked as used!", i, mosiGpio);
                }
            }
            else
            {
                // 1-Wire protocols need 1 pin - read from manual GPIO parameter
                const uint8_t dataGpio = (uint8_t)ParamNEOSTRIP_NEODataGPIO;

                if (!isPinUsed(dataGpio))
                {
                    usedPins.push_back(dataGpio);
                    logInfoP("Strip %d: Pre-marked manual Data GPIO %d as used", i, dataGpio);
                }
                else
                {
                    logWarningP("Strip %d: Manual Data GPIO %d already marked as used!", i, dataGpio);
                }
            }
        }
        else
        {
            // Hardware-assisted mode: Get GPIO from hardware-specific port selection
            // CRITICAL: Ignore hardware port config if there's a hardware mismatch!
            if (_hwConfigMismatch)
            {
                logErrorP("Strip %d: Hardware mismatch detected - ignoring hardware port selection", i);
                // Don't pre-mark any hardware ports, force manual configuration
            }
            else
            {
                uint8_t dataPortSelection = getDataPortSelectionForHardwareIndex(i);

                if (isHardwarePortSelected(dataPortSelection))
                {
                    uint8_t dataGpio = mapPortIndexToGpio(dataPortSelection);

                    if (dataGpio != 255) // Valid GPIO
                    {
                        if (!isPinUsed(dataGpio))
                        {
                            usedPins.push_back(dataGpio);
                            logInfoP("Strip %d: Pre-marked hardware Data GPIO %d (port %d) as used", i, dataGpio, dataPortSelection);
                        }
                        else
                        {
                            logErrorP("Strip %d: Hardware Data GPIO %d (port %d) already marked as used!", i, dataGpio, dataPortSelection);
                        }

                        // For SPI protocols, also mark clock GPIO
                        if (isSpiProtocol(proto))
                        {
                            uint8_t clockPortSelection = getClockPortSelectionForHardwareIndex(i);

                            if (isHardwarePortSelected(clockPortSelection))
                            {
                                uint8_t sckGpio = mapPortIndexToGpio(clockPortSelection);

                                if (sckGpio != 255) // Valid GPIO
                                {
                                    if (!isPinUsed(sckGpio))
                                    {
                                        usedPins.push_back(sckGpio);
                                        logInfoP("Strip %d: Pre-marked hardware SCK GPIO %d (port %d) as used", i, sckGpio, clockPortSelection);
                                    }
                                    else
                                    {
                                        logErrorP("Strip %d: Hardware SCK GPIO %d (port %d) already marked as used!", i, sckGpio, clockPortSelection);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    logInfoP("Pre-scan complete: %d manual GPIO pins marked as used", (int)usedPins.size());

    // 2) Create all physical strips in configuration order (auto-allocation will skip pre-marked pins)
    for (uint8_t i = 0; i < maxStrips; ++i)
    {
        // Set channel index for strip parameter access
        // ParamNEOSTRIP_* macros use _channelIndex to calculate correct EEPROM address
        _channelIndex = i;

        const uint8_t ledTypeParam = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const LedProtocol proto = StripConfiguration::mapProtocol(ledTypeParam);

        // Get color order from ETS setting for all protocols
        ColorOrder order = StripConfiguration::mapColorOrder((uint8_t)ParamNEOSTRIP_NEOColourOrder);
        const uint8_t ledType = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const uint8_t dataGpio = (uint8_t)ParamNEOSTRIP_NEODataGPIO;
        const uint16_t pixels = (uint16_t)ParamNEOSTRIP_NEOLength;

        // DEBUG: Log all parameters for this strip
        logInfoP("Strip %d: LEDType=%d, pixels=%d (channelIndex=%d)", i, ledType, pixels, _channelIndex);
        logInfoP("  -> Offset calculation: base=%d + (ch=%d * size=%d) = %d",
                 NEOSTRIP_ParamBlockOffset, _channelIndex, NEOSTRIP_ParamBlockSize,
                 NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize);

        // DEBUG: SPI-specific parameters
        if (isSpiProtocol(proto))
        {
            const uint8_t mosiGpio = (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO;
            const uint8_t sckGpio = (uint8_t)ParamNEOSTRIP_NEOClockGPIO;
            logInfoP("  -> SPI: MOSI=%d (offset %d+%d=%d), SCK=%d (offset %d+%d=%d)",
                     mosiGpio,
                     NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize, NEOSTRIP_NEOSPIMOSIGPIO,
                     NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize + NEOSTRIP_NEOSPIMOSIGPIO,
                     sckGpio,
                     NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize, NEOSTRIP_NEOClockGPIO,
                     NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize + NEOSTRIP_NEOClockGPIO);
        }

        // Skip strips with 0 LEDs configured
        if (pixels == 0)
        {
            logInfoP("Strip %d: Skipped (0 LEDs configured)", i);
            continue;
        }

        // Determine if this is an SPI protocol and get appropriate GPIO pins
        PhysicalStrip* phys = nullptr;
        if (isSpiProtocol(proto))
        {
            uint8_t mosiGpio, sckGpio;

            // Check if manual GPIO configuration is enabled
            bool gpioManualConfig = (bool)ParamNEOSTRIP_NEOGPIOManual;

            if (gpioManualConfig)
            {
                // Use ETS configured GPIO pins (already pre-marked as used during pre-scan)
                mosiGpio = (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO;
                sckGpio = (uint8_t)ParamNEOSTRIP_NEOClockGPIO;
            }
            else
            {
                // Hardware-assisted mode: Use hardware-specific port mapping
                // CRITICAL: Block hardware port usage if there's a hardware mismatch!
                if (_hwConfigMismatch)
                {
                    logErrorP("Strip %d: Hardware mismatch - cannot use hardware ports! Please configure GPIO manually.", i);
                    continue; // Skip this strip completely
                }

                uint8_t dataPortSelection = getDataPortSelectionForHardwareIndex(i);
                uint8_t clockPortSelection = getClockPortSelectionForHardwareIndex(i);

                if (isHardwarePortSelected(dataPortSelection) && isHardwarePortSelected(clockPortSelection))
                {
                    // Map port indices to actual GPIO numbers
                    mosiGpio = mapPortIndexToGpio(dataPortSelection);
                    sckGpio = mapPortIndexToGpio(clockPortSelection);

                    if (mosiGpio == 255 || sckGpio == 255)
                    {
                        logErrorP("Strip %d: Invalid hardware port mapping (data:%d->%d, clock:%d->%d)",
                                  i, dataPortSelection, mosiGpio, clockPortSelection, sckGpio);
                        // Fallback to automatic allocation
                        mosiGpio = 19;
                        sckGpio = 18;
                    }
                    else
                    {
                        logInfoP("Strip %d: Using hardware ports (MOSI:%d, SCK:%d)", i, mosiGpio, sckGpio);
                    }
                }
                else
                {
                    // Fallback to automatic pin allocation if hardware ports not selected
                    logWarningP("Strip %d: No hardware ports selected, using automatic allocation", i);

                    bool foundPins = false;
                    while (nextPinIndex + 1 < numAvailablePins)
                    {
                        sckGpio = availablePins[nextPinIndex];
                        mosiGpio = availablePins[nextPinIndex + 1];

                        // Check if both pins are free
                        if (!isPinUsed(sckGpio) && !isPinUsed(mosiGpio))
                        {
                            foundPins = true;
                            // Mark pins as used immediately
                            usedPins.push_back(sckGpio);
                            usedPins.push_back(mosiGpio);
                            nextPinIndex += 2; // Consume both pins
                            break;
                        }
                        // If conflict, skip to next pin and try again
                        nextPinIndex++;
                    }

                    if (!foundPins)
                    {
                        logErrorP("Strip %d: No available GPIO pins for SPI strip!", i);
                        // Fallback to last resort pins
                        sckGpio = 18;
                        mosiGpio = 19;
                        // Mark fallback pins as used
                        usedPins.push_back(sckGpio);
                        usedPins.push_back(mosiGpio);
                    }
                }
            }

            // Read SPI frequency from ETS (if manual config enabled)
            uint32_t spiFrequency = 10000000; // Default: 10 MHz
            bool spiClkManual = (ParamNEOSTRIP_NEOSPICLKMode == 1);

            if (spiClkManual)
            {
                // Map ETS enum value to frequency in Hz
                uint8_t spiClkValue = (uint8_t)ParamNEOSTRIP_NEOSPICLK;
                switch (spiClkValue)
                {
                    case 0: spiFrequency = 1000000; break;   // 1 MHz
                    case 1: spiFrequency = 2000000; break;   // 2 MHz
                    case 2: spiFrequency = 4000000; break;   // 4 MHz
                    case 3: spiFrequency = 8000000; break;   // 8 MHz
                    case 4: spiFrequency = 12000000; break;  // 12 MHz
                    case 5: spiFrequency = 16000000; break;  // 16 MHz
                    case 6: spiFrequency = 20000000; break;  // 20 MHz
                    default: spiFrequency = 10000000; break; // Fallback: 10 MHz
                }
            }

            // Use manager directly for frequency parameter (NeoPixel wrapper doesn't expose this overload yet)
            auto mgr = _neoPixel.getManager();

            // SAFETY: never drive KNX/system-reserved pins (e.g. GPIO0/1 = KNX TX/RX on KNeoPix)
            if (isReservedPin(mosiGpio) || isReservedPin(sckGpio))
            {
                logErrorP("Strip %d: REFUSED - SPI GPIO(s) collide with reserved KNX/system pins (MOSI=%d, SCK=%d). Strip disabled to protect KNX bus.",
                          i, mosiGpio, sckGpio);
                continue; // Skip this strip completely
            }

            phys = mgr->addSpiStrip(mosiGpio, sckGpio, pixels, proto, order, spiFrequency);
            // Configure SPI strip-specific settings BEFORE init()
            // The config is created in the PhysicalStrip constructor
            if (phys)
            {
                auto* cfg = phys->getConfig();
                SpiStripConfig* spiCfg = cfg && cfg->isSpiConfig() ? static_cast<SpiStripConfig*>(cfg) : nullptr;

                if (spiCfg)
                {
                    // Read "Überspringe erste LEDs" parameter for this strip
                    uint16_t skipLeds = ParamNEOSTRIP_NEOSkipFirstLEDs;

                    // Store skipFirstLeds in config (VirtualStrip will force them to black)
                    spiCfg->setSkipFirstLeds(skipLeds);

                    // Set dummy LED mode based on skipLeds parameter:
                    // - skipLeds=0: setDummyLedMode(0) - all LEDs active
                    // - skipLeds>0: setDummyLedMode(1) - sacrifice LED#0, then (skipLeds-1) LEDs forced black
                    if (skipLeds > 0)
                    {
                        spiCfg->setDummyLedMode(1); // Physical dummy LED (sacrifice LED#0)
                        logInfoP("SPI Strip %d: Skip mode enabled - %d LEDs will stay black (1 dummy + %d forced)", i, skipLeds, skipLeds - 1);
                    }
                    else
                    {
                        spiCfg->setDummyLedMode(0); // No dummy LED
                        logInfoP("SPI Strip %d: All LEDs active (no skipping)", i);
                    }

                    // Apply the configuration (prepares for init)
                    phys->applyConfig();
                    logInfoP("SPI Strip %d: Configuration applied successfully", i);
                }
                else
                {
                    logErrorP("SPI Strip %d: Failed to cast config to SpiStripConfig!", i);
                }
            }
            else
            {
                logErrorP("SPI Strip %d: addSpiStrip returned nullptr!", i);
            }

            logInfoP("SPI Strip %d: %d LEDs, MOSI=%d, SCK=%d, Protocol=%s, ColorOrder=%s, Freq=%d Hz%s",
                     i, pixels, mosiGpio, sckGpio, getProtocolName(proto), getColorOrderName(order),
                     spiFrequency, gpioManualConfig ? " (Manual)" : " (Auto)");
        }
        else
        {
            // 1-Wire protocols use Data GPIO
            uint8_t dataGpioPin;

            // Check if manual GPIO configuration is enabled
            bool gpioManualConfig = (bool)ParamNEOSTRIP_NEOGPIOManual;

            if (gpioManualConfig)
            {
                // Use ETS configured GPIO pin (already pre-marked as used during pre-scan)
                dataGpioPin = dataGpio;
            }
            else
            {
                // Hardware-assisted mode: Use hardware-specific port mapping
                // CRITICAL: Block hardware port usage if there's a hardware mismatch!
                if (_hwConfigMismatch)
                {
                    logErrorP("Strip %d: Hardware mismatch - cannot use hardware ports! Please configure GPIO manually.", i);
                    continue; // Skip this strip completely
                }

                uint8_t dataPortSelection = getDataPortSelectionForHardwareIndex(i);

                if (isHardwarePortSelected(dataPortSelection))
                {
                    // Map port index to actual GPIO number
                    dataGpioPin = mapPortIndexToGpio(dataPortSelection);

                    if (dataGpioPin == 255)
                    {
                        logErrorP("Strip %d: Invalid hardware port mapping (data:%d->%d)",
                                  i, dataPortSelection, dataGpioPin);
                        // Fallback to automatic allocation
                        dataGpioPin = dataGpio;
                    }
                    else
                    {
                        logInfoP("Strip %d: Using hardware port (Data:%d)", i, dataGpioPin);
                    }
                }
                else
                {
                    // Fallback to automatic pin allocation if hardware port not selected
                    logWarningP("Strip %d: No hardware port selected, using automatic allocation", i);

                    bool foundPin = false;
                    while (nextPinIndex < numAvailablePins)
                    {
                        dataGpioPin = availablePins[nextPinIndex];

                        // Check if pin is free
                        if (!isPinUsed(dataGpioPin))
                        {
                            foundPin = true;
                            // Mark pin as used immediately
                            usedPins.push_back(dataGpioPin);
                            nextPinIndex++; // Consume this pin
                            break;
                        }
                        // If conflict, skip to next pin
                        nextPinIndex++;
                    }

                    if (!foundPin)
                    {
                        logErrorP("Strip %d: No available GPIO pins for 1-Wire strip!", i);
                        // Fallback to ETS parameter
                        dataGpioPin = dataGpio;
                        usedPins.push_back(dataGpioPin);
                    }
                }
            }

            // SAFETY: never drive KNX/system-reserved pins (e.g. GPIO0/1 = KNX TX/RX on KNeoPix)
            if (isReservedPin(dataGpioPin))
            {
                logErrorP("Strip %d: REFUSED - Data GPIO %d collides with reserved KNX/system pin. Strip disabled to protect KNX bus.",
                          i, dataGpioPin);
                continue; // Skip this strip completely
            }

            phys = _neoPixel.addStrip(dataGpioPin, pixels, proto, order);
            logInfoP("1-Wire Strip %d: %d LEDs, GPIO=%d, Protocol=%s, ColorOrder=%s%s",
                     i, pixels, dataGpioPin, getProtocolName(proto), getColorOrderName(order),
                     gpioManualConfig ? " (Manual)" : " (Auto)");
            if (phys)
            {
                auto* cfg = phys->getConfig();
                if (cfg)
                {
                    uint16_t skipLeds = ParamNEOSTRIP_NEOSkipFirstLEDs;
                    cfg->setSkipFirstLeds(skipLeds);

                    if (skipLeds > 0)
                    {
                        logInfoP("1-Wire Strip %d: Skip mode enabled - first %d LEDs will stay black", i, skipLeds);
                    }
                }
            }
        }

        if (phys)
        {
            _totalLeds += pixels;
            _physicalStrips.push_back(phys);

            // ETS "Timing" value -> target kHz. Order MUST match the NEOTiming enum
            // (NeoPixel.share.xml) and the duplicate table in StripConfiguration.cpp. SPI: no-op.
            static const uint16_t timingFreqTable[16] = {
                800, 960, 640, 680, 720, 760, 840, 880, 920, 750, 765, 770, 775, 780, 785, 790};
            const uint8_t timingSel = (uint8_t)ParamNEOSTRIP_NEOTiming & 0x0F;
            const uint16_t freqKhz = timingFreqTable[timingSel];
            // 3:7:6:4 ratio; only T1H matters on PIO (T1H_ns = 600000 / kHz)
            const uint16_t t1h = (uint16_t)(600000UL / freqKhz);
            const uint16_t t0h = (uint16_t)(t1h / 2);
            const uint16_t t0l = (uint16_t)((uint32_t)t1h * 7 / 6);
            const uint16_t t1l = (uint16_t)((uint32_t)t1h * 4 / 6);
            if (phys->setCustomTiming(t0h, t0l, t1h, t1l, 0))
                logInfoP("Strip %d: Timing = %u kHz (T1H=%u ns)", i, freqKhz, t1h);

            // Configure color correction for this strip
            bool colorCalibMaster = (bool)ParamNEOSTRIP_NEOColorCalibrationMaster;
            if (colorCalibMaster && ((bool)ParamNEOSTRIP_NEOGammaCorrection || true))
            {
                configureColorCorrection();
                updateColorCorrection(); // Set correction parameters on VirtualStrip once

                auto* cfg = phys->getConfig();
                if (cfg)
                {
                    // Configure gamma correction in the physical strip config
                    if (_gammaCorrectionEnabled)
                    {
                        cfg->setGammaCorrection(_gammaValue);
                        logInfoP("Strip %d: Gamma correction enabled (value=%.2f)", i, _gammaValue);
                    }

                    // Configure white balance in the physical strip config
                    if (_whiteBalanceEnabled)
                    {
                        cfg->setWhiteBalance(_whiteBalanceRed, _whiteBalanceGreen, _whiteBalanceBlue);
                        logInfoP("Strip %d: White balance enabled (R:%d G:%d B:%d)", i, _whiteBalanceRed, _whiteBalanceGreen, _whiteBalanceBlue);
                    }
                }

                logInfoP("Strip %d: Color correction enabled - Gamma=%s(%.1f), WhiteBalance=%s(R:%d G:%d B:%d)",
                         i,
                         _gammaCorrectionEnabled ? "ON" : "OFF", _gammaValue,
                         _whiteBalanceEnabled ? "ON" : "OFF", _whiteBalanceRed, _whiteBalanceGreen, _whiteBalanceBlue);
            }

            // Configure power limiting for this strip
            auto* cfg = phys->getConfig();
            if (cfg)
            {
                uint8_t powerMode = (uint8_t)ParamNEOSTRIP_NEOpowerLimitCombined;
                cfg->setPowerLimitMode(powerMode);

                if (powerMode == 2) // FixedValue
                {
                    uint16_t maxCurrent = (uint16_t)ParamNEOSTRIP_NEOcurrentPerChannel;
                    cfg->setMaxCurrentMa(maxCurrent);
                    logInfoP("Strip %d: Power limit FixedValue - %d mA", i, maxCurrent);
                }
                else if (powerMode == 3) // PerLED
                {
                    uint8_t currentPerLed = (uint8_t)ParamNEOSTRIP_NEOcurrentPerLED;
                    cfg->setCurrentPerLedMa(currentPerLed);
                    logInfoP("Strip %d: Power limit PerLED - %d mA/LED", i, currentPerLed);
                }
                else if (powerMode == 1)
                {
                    logInfoP("Strip %d: Power limit UseGlobal", i);
                }
                else if (powerMode == 0)
                {
                    logInfoP("Strip %d: Power limit Disabled", i);
                }
            }

            // Configure strip options (swap mode and skip LEDs)
            configureStripOptions();
            if (_swapMode > 0 || _skipFirstLeds > 0)
            {
                logInfoP("Strip %d: Strip options - SwapMode=%d, SkipFirstLEDs=%d",
                         i, _swapMode, _skipFirstLeds);
            }
        }
        else
        {
            logErrorP("Failed to create strip %d", i);
        }
    }

    logInfoP("Created %d physical strips with total %d LEDs", maxStrips, _totalLeds);

    // 3) Configure virtual strip order from ETS parameters
    configureVirtualStripOrder();

    // 4) Configure segments once, now that we know the virtual strip layout
    // Segments are defined on the virtual strip
    if (_totalLeds > 0)
    {
        configureSegments();
        if (_numberOfSegments > 0)
        {
            logInfoP("Configured %d segments for virtual strip (%d total LEDs)", _numberOfSegments, _totalLeds);
        }
    }

    // Load global Effektmanager definitions (NEOEM channel block) from ETS.
    // startEffektManager()/console/KO paths consume _emData and require this.
    loadEffektManagerFromETS();

    // 5) Create virtual strip using configured order and apply segments
    if (!_physicalStrips.empty() && _totalLeds > 0)
    {
        createVirtualStripWithOrder();

        // Configure effects for segments (now that virtual strip and segments exist)
        if (_numberOfSegments > 0)
        {
            _effectConfiguration->configureEffects();
            logInfoP("Configured %d segments with ETS effects and startup behavior", _numberOfSegments);

            // Note: Startup behavior is already applied:
            //   - Mode "Aus": brightness=0
            //   - Mode "Letzter Zustand": Will be restored from flash in processAfterStartupDelay()
            //   - Mode "ETS-Parameterwert": Active with ETS configuration
        }

        // Configure power management using OFM PowerManager
        configurePowerManagement();

        // Apply initial color correction to ensure ETS settings take effect immediately
        if (_gammaCorrectionEnabled || _whiteBalanceEnabled || _swapMode > 0)
        {
            forceColorCorrectionUpdate();
            logInfoP("Applied initial color correction settings");
        }
    }

    logInfoP("Configuration complete: %d physical strips, %d total LEDs, %d segments",
             maxStrips, _totalLeds, _numberOfSegments);
}

void NeoPixelBusModule::loadEffektManagerFromETS()
{
#if defined(ParamNEOEM_NEOEMEnabled)
    auto mapEtsEffectTypeToRuntimeIndex = [](uint8_t effectType) -> uint8_t {
    #ifdef EFFECT_PARAMETER_MAPPING_GENERATED
        Effect* target = getEffectFromType(effectType);
        if (!target) return effectType;

        const uint8_t effectCount = EffectPool::getEffectCount();
        for (uint8_t idx = 0; idx < effectCount; ++idx)
        {
            if (EffectPool::getEffectByIndex(idx) == target)
            {
                return idx;
            }
        }
    #endif
        return effectType;
    };

    auto clearCue = [](EffektCue& cue) {
        cue.effectId = 0;
        for (uint8_t i = 0; i < EM_PARAM_COUNT; ++i)
            cue.params[i] = 0;
        cue.r = 0;
        cue.g = 0;
        cue.b = 0;
        cue.w = 0;
        cue.brightness = 0;
        cue.durationSec = 0;
        cue.fadeMs = 0;
        cue.cueName[0] = '\0';
        cue.effectText[0] = '\0';
    };

    auto loadCueFromEts = [&](uint8_t cueIndex, EffektCue& cue) {
        uint8_t effect = 0;
        uint8_t p0 = 0;
        uint8_t p1 = 0;
        uint8_t p2 = 0;
        uint8_t p3 = 0;
        uint8_t p4 = 0;
        uint8_t p5 = 0;
        uint8_t p6 = 0;
        uint8_t p7 = 0;
        uint8_t p8 = 0;
        uint8_t p9 = 0;
        uint32_t color = 0;
        uint8_t w = 0;
        uint8_t brightness = 0;
        uint16_t duration = 0;
        uint16_t fade = 0;
        std::string cueName;
        std::string effectText;

        switch (cueIndex)
        {
            case 0:
                effect = (uint8_t)ParamNEOEM_NEOEMCue1Effect;
                p0 = (uint8_t)ParamNEOEM_NEOEMCue1Param0;
                p1 = (uint8_t)ParamNEOEM_NEOEMCue1Param1;
                p2 = (uint8_t)ParamNEOEM_NEOEMCue1Param2;
                p3 = (uint8_t)ParamNEOEM_NEOEMCue1Param3;
                p4 = (uint8_t)ParamNEOEM_NEOEMCue1Param4;
                p5 = (uint8_t)ParamNEOEM_NEOEMCue1Param5;
                p6 = (uint8_t)ParamNEOEM_NEOEMCue1Param6;
                p7 = (uint8_t)ParamNEOEM_NEOEMCue1Param7;
                p8 = (uint8_t)ParamNEOEM_NEOEMCue1Param8;
                p9 = (uint8_t)ParamNEOEM_NEOEMCue1Param9;
                color = (uint32_t)ParamNEOEM_NEOEMCue1Color;
                w = (uint8_t)ParamNEOEM_NEOEMCue1W;
                brightness = (uint8_t)ParamNEOEM_NEOEMCue1Brightness;
                duration = (uint16_t)ParamNEOEM_NEOEMCue1Duration;
                fade = (uint16_t)ParamNEOEM_NEOEMCue1Fade;
                cueName = ParamNEOEM_NEOEMCue1TextStr;
                effectText = ParamNEOEM_NEOEMCue1EffectTextStr;
                break;
            case 1:
                effect = (uint8_t)ParamNEOEM_NEOEMCue2Effect;
                p0 = (uint8_t)ParamNEOEM_NEOEMCue2Param0;
                p1 = (uint8_t)ParamNEOEM_NEOEMCue2Param1;
                p2 = (uint8_t)ParamNEOEM_NEOEMCue2Param2;
                p3 = (uint8_t)ParamNEOEM_NEOEMCue2Param3;
                p4 = (uint8_t)ParamNEOEM_NEOEMCue2Param4;
                p5 = (uint8_t)ParamNEOEM_NEOEMCue2Param5;
                p6 = (uint8_t)ParamNEOEM_NEOEMCue2Param6;
                p7 = (uint8_t)ParamNEOEM_NEOEMCue2Param7;
                p8 = (uint8_t)ParamNEOEM_NEOEMCue2Param8;
                p9 = (uint8_t)ParamNEOEM_NEOEMCue2Param9;
                color = (uint32_t)ParamNEOEM_NEOEMCue2Color;
                w = (uint8_t)ParamNEOEM_NEOEMCue2W;
                brightness = (uint8_t)ParamNEOEM_NEOEMCue2Brightness;
                duration = (uint16_t)ParamNEOEM_NEOEMCue2Duration;
                fade = (uint16_t)ParamNEOEM_NEOEMCue2Fade;
                cueName = ParamNEOEM_NEOEMCue2TextStr;
                effectText = ParamNEOEM_NEOEMCue2EffectTextStr;
                break;
            case 2:
                effect = (uint8_t)ParamNEOEM_NEOEMCue3Effect;
                p0 = (uint8_t)ParamNEOEM_NEOEMCue3Param0;
                p1 = (uint8_t)ParamNEOEM_NEOEMCue3Param1;
                p2 = (uint8_t)ParamNEOEM_NEOEMCue3Param2;
                p3 = (uint8_t)ParamNEOEM_NEOEMCue3Param3;
                p4 = (uint8_t)ParamNEOEM_NEOEMCue3Param4;
                p5 = (uint8_t)ParamNEOEM_NEOEMCue3Param5;
                p6 = (uint8_t)ParamNEOEM_NEOEMCue3Param6;
                p7 = (uint8_t)ParamNEOEM_NEOEMCue3Param7;
                p8 = (uint8_t)ParamNEOEM_NEOEMCue3Param8;
                p9 = (uint8_t)ParamNEOEM_NEOEMCue3Param9;
                color = (uint32_t)ParamNEOEM_NEOEMCue3Color;
                w = (uint8_t)ParamNEOEM_NEOEMCue3W;
                brightness = (uint8_t)ParamNEOEM_NEOEMCue3Brightness;
                duration = (uint16_t)ParamNEOEM_NEOEMCue3Duration;
                fade = (uint16_t)ParamNEOEM_NEOEMCue3Fade;
                cueName = ParamNEOEM_NEOEMCue3TextStr;
                effectText = ParamNEOEM_NEOEMCue3EffectTextStr;
                break;
            case 3:
                effect = (uint8_t)ParamNEOEM_NEOEMCue4Effect;
                p0 = (uint8_t)ParamNEOEM_NEOEMCue4Param0;
                p1 = (uint8_t)ParamNEOEM_NEOEMCue4Param1;
                p2 = (uint8_t)ParamNEOEM_NEOEMCue4Param2;
                p3 = (uint8_t)ParamNEOEM_NEOEMCue4Param3;
                p4 = (uint8_t)ParamNEOEM_NEOEMCue4Param4;
                p5 = (uint8_t)ParamNEOEM_NEOEMCue4Param5;
                p6 = (uint8_t)ParamNEOEM_NEOEMCue4Param6;
                p7 = (uint8_t)ParamNEOEM_NEOEMCue4Param7;
                p8 = (uint8_t)ParamNEOEM_NEOEMCue4Param8;
                p9 = (uint8_t)ParamNEOEM_NEOEMCue4Param9;
                color = (uint32_t)ParamNEOEM_NEOEMCue4Color;
                w = (uint8_t)ParamNEOEM_NEOEMCue4W;
                brightness = (uint8_t)ParamNEOEM_NEOEMCue4Brightness;
                duration = (uint16_t)ParamNEOEM_NEOEMCue4Duration;
                fade = (uint16_t)ParamNEOEM_NEOEMCue4Fade;
                cueName = ParamNEOEM_NEOEMCue4TextStr;
                effectText = ParamNEOEM_NEOEMCue4EffectTextStr;
                break;
            case 4:
                effect = (uint8_t)ParamNEOEM_NEOEMCue5Effect;
                p0 = (uint8_t)ParamNEOEM_NEOEMCue5Param0;
                p1 = (uint8_t)ParamNEOEM_NEOEMCue5Param1;
                p2 = (uint8_t)ParamNEOEM_NEOEMCue5Param2;
                p3 = (uint8_t)ParamNEOEM_NEOEMCue5Param3;
                p4 = (uint8_t)ParamNEOEM_NEOEMCue5Param4;
                p5 = (uint8_t)ParamNEOEM_NEOEMCue5Param5;
                p6 = (uint8_t)ParamNEOEM_NEOEMCue5Param6;
                p7 = (uint8_t)ParamNEOEM_NEOEMCue5Param7;
                p8 = (uint8_t)ParamNEOEM_NEOEMCue5Param8;
                p9 = (uint8_t)ParamNEOEM_NEOEMCue5Param9;
                color = (uint32_t)ParamNEOEM_NEOEMCue5Color;
                w = (uint8_t)ParamNEOEM_NEOEMCue5W;
                brightness = (uint8_t)ParamNEOEM_NEOEMCue5Brightness;
                duration = (uint16_t)ParamNEOEM_NEOEMCue5Duration;
                fade = (uint16_t)ParamNEOEM_NEOEMCue5Fade;
                cueName = ParamNEOEM_NEOEMCue5TextStr;
                effectText = ParamNEOEM_NEOEMCue5EffectTextStr;
                break;
            case 5:
                effect = (uint8_t)ParamNEOEM_NEOEMCue6Effect;
                p0 = (uint8_t)ParamNEOEM_NEOEMCue6Param0;
                p1 = (uint8_t)ParamNEOEM_NEOEMCue6Param1;
                p2 = (uint8_t)ParamNEOEM_NEOEMCue6Param2;
                p3 = (uint8_t)ParamNEOEM_NEOEMCue6Param3;
                p4 = (uint8_t)ParamNEOEM_NEOEMCue6Param4;
                p5 = (uint8_t)ParamNEOEM_NEOEMCue6Param5;
                p6 = (uint8_t)ParamNEOEM_NEOEMCue6Param6;
                p7 = (uint8_t)ParamNEOEM_NEOEMCue6Param7;
                p8 = (uint8_t)ParamNEOEM_NEOEMCue6Param8;
                p9 = (uint8_t)ParamNEOEM_NEOEMCue6Param9;
                color = (uint32_t)ParamNEOEM_NEOEMCue6Color;
                w = (uint8_t)ParamNEOEM_NEOEMCue6W;
                brightness = (uint8_t)ParamNEOEM_NEOEMCue6Brightness;
                duration = (uint16_t)ParamNEOEM_NEOEMCue6Duration;
                fade = (uint16_t)ParamNEOEM_NEOEMCue6Fade;
                cueName = ParamNEOEM_NEOEMCue6TextStr;
                effectText = ParamNEOEM_NEOEMCue6EffectTextStr;
                break;
            case 6:
                effect = (uint8_t)ParamNEOEM_NEOEMCue7Effect;
                p0 = (uint8_t)ParamNEOEM_NEOEMCue7Param0;
                p1 = (uint8_t)ParamNEOEM_NEOEMCue7Param1;
                p2 = (uint8_t)ParamNEOEM_NEOEMCue7Param2;
                p3 = (uint8_t)ParamNEOEM_NEOEMCue7Param3;
                p4 = (uint8_t)ParamNEOEM_NEOEMCue7Param4;
                p5 = (uint8_t)ParamNEOEM_NEOEMCue7Param5;
                p6 = (uint8_t)ParamNEOEM_NEOEMCue7Param6;
                p7 = (uint8_t)ParamNEOEM_NEOEMCue7Param7;
                p8 = (uint8_t)ParamNEOEM_NEOEMCue7Param8;
                p9 = (uint8_t)ParamNEOEM_NEOEMCue7Param9;
                color = (uint32_t)ParamNEOEM_NEOEMCue7Color;
                w = (uint8_t)ParamNEOEM_NEOEMCue7W;
                brightness = (uint8_t)ParamNEOEM_NEOEMCue7Brightness;
                duration = (uint16_t)ParamNEOEM_NEOEMCue7Duration;
                fade = (uint16_t)ParamNEOEM_NEOEMCue7Fade;
                cueName = ParamNEOEM_NEOEMCue7TextStr;
                effectText = ParamNEOEM_NEOEMCue7EffectTextStr;
                break;
            case 7:
                effect = (uint8_t)ParamNEOEM_NEOEMCue8Effect;
                p0 = (uint8_t)ParamNEOEM_NEOEMCue8Param0;
                p1 = (uint8_t)ParamNEOEM_NEOEMCue8Param1;
                p2 = (uint8_t)ParamNEOEM_NEOEMCue8Param2;
                p3 = (uint8_t)ParamNEOEM_NEOEMCue8Param3;
                p4 = (uint8_t)ParamNEOEM_NEOEMCue8Param4;
                p5 = (uint8_t)ParamNEOEM_NEOEMCue8Param5;
                p6 = (uint8_t)ParamNEOEM_NEOEMCue8Param6;
                p7 = (uint8_t)ParamNEOEM_NEOEMCue8Param7;
                p8 = (uint8_t)ParamNEOEM_NEOEMCue8Param8;
                p9 = (uint8_t)ParamNEOEM_NEOEMCue8Param9;
                color = (uint32_t)ParamNEOEM_NEOEMCue8Color;
                w = (uint8_t)ParamNEOEM_NEOEMCue8W;
                brightness = (uint8_t)ParamNEOEM_NEOEMCue8Brightness;
                duration = (uint16_t)ParamNEOEM_NEOEMCue8Duration;
                fade = (uint16_t)ParamNEOEM_NEOEMCue8Fade;
                cueName = ParamNEOEM_NEOEMCue8TextStr;
                effectText = ParamNEOEM_NEOEMCue8EffectTextStr;
                break;
            case 8:
                effect = (uint8_t)ParamNEOEM_NEOEMCue9Effect;
                p0 = (uint8_t)ParamNEOEM_NEOEMCue9Param0;
                p1 = (uint8_t)ParamNEOEM_NEOEMCue9Param1;
                p2 = (uint8_t)ParamNEOEM_NEOEMCue9Param2;
                p3 = (uint8_t)ParamNEOEM_NEOEMCue9Param3;
                p4 = (uint8_t)ParamNEOEM_NEOEMCue9Param4;
                p5 = (uint8_t)ParamNEOEM_NEOEMCue9Param5;
                p6 = (uint8_t)ParamNEOEM_NEOEMCue9Param6;
                p7 = (uint8_t)ParamNEOEM_NEOEMCue9Param7;
                p8 = (uint8_t)ParamNEOEM_NEOEMCue9Param8;
                p9 = (uint8_t)ParamNEOEM_NEOEMCue9Param9;
                color = (uint32_t)ParamNEOEM_NEOEMCue9Color;
                w = (uint8_t)ParamNEOEM_NEOEMCue9W;
                brightness = (uint8_t)ParamNEOEM_NEOEMCue9Brightness;
                duration = (uint16_t)ParamNEOEM_NEOEMCue9Duration;
                fade = (uint16_t)ParamNEOEM_NEOEMCue9Fade;
                cueName = ParamNEOEM_NEOEMCue9TextStr;
                effectText = ParamNEOEM_NEOEMCue9EffectTextStr;
                break;
            case 9:
                effect = (uint8_t)ParamNEOEM_NEOEMCue10Effect;
                p0 = (uint8_t)ParamNEOEM_NEOEMCue10Param0;
                p1 = (uint8_t)ParamNEOEM_NEOEMCue10Param1;
                p2 = (uint8_t)ParamNEOEM_NEOEMCue10Param2;
                p3 = (uint8_t)ParamNEOEM_NEOEMCue10Param3;
                p4 = (uint8_t)ParamNEOEM_NEOEMCue10Param4;
                p5 = (uint8_t)ParamNEOEM_NEOEMCue10Param5;
                p6 = (uint8_t)ParamNEOEM_NEOEMCue10Param6;
                p7 = (uint8_t)ParamNEOEM_NEOEMCue10Param7;
                p8 = (uint8_t)ParamNEOEM_NEOEMCue10Param8;
                p9 = (uint8_t)ParamNEOEM_NEOEMCue10Param9;
                color = (uint32_t)ParamNEOEM_NEOEMCue10Color;
                w = (uint8_t)ParamNEOEM_NEOEMCue10W;
                brightness = (uint8_t)ParamNEOEM_NEOEMCue10Brightness;
                duration = (uint16_t)ParamNEOEM_NEOEMCue10Duration;
                fade = (uint16_t)ParamNEOEM_NEOEMCue10Fade;
                cueName = ParamNEOEM_NEOEMCue10TextStr;
                effectText = ParamNEOEM_NEOEMCue10EffectTextStr;
                break;
            default:
                clearCue(cue);
                return;
        }

        cue.effectId = mapEtsEffectTypeToRuntimeIndex(effect);
        cue.params[0] = p0;
        cue.params[1] = p1;
        cue.params[2] = p2;
        cue.params[3] = p3;
        cue.params[4] = p4;
        cue.params[5] = p5;
        cue.params[6] = p6;
        cue.params[7] = p7;
        cue.params[8] = p8;
        cue.params[9] = p9;
        cue.r = (uint8_t)((color >> 16) & 0xFFu);
        cue.g = (uint8_t)((color >> 8) & 0xFFu);
        cue.b = (uint8_t)(color & 0xFFu);
        cue.w = w;
        cue.brightness = brightness;
        cue.durationSec = duration;
        cue.fadeMs = fade;
        std::memset(cue.cueName, 0, sizeof(cue.cueName));
        std::strncpy(cue.cueName, cueName.c_str(), sizeof(cue.cueName) - 1);
        std::memset(cue.effectText, 0, sizeof(cue.effectText));
        std::strncpy(cue.effectText, effectText.c_str(), sizeof(cue.effectText) - 1);
    };

    if (!_emData) return; // EM table not allocated (out of memory) -> nothing to load

    const uint8_t oldChannel = _channelIndex;
    for (uint8_t em = 0; em < EM_COUNT; ++em)
    {
        _channelIndex = em;

        auto& data = _emData[em];
        std::memset(&data, 0, sizeof(data));

        // EM-Beschreibung ist ETS-only (nicht gespeichert) -> nicht aufs Geraet uebernehmen.
        // header.reserved[0-15] bleibt 0 (memset oben).

        uint8_t cueCount = (uint8_t)ParamNEOEM_NEOEMCueCount;
        if (cueCount > 10) cueCount = 10;
        data.header.cueCount = cueCount;
        data.header.loop = ParamNEOEM_NEOEMLoop ? 1 : 0;

        uint8_t nextEm = (uint8_t)ParamNEOEM_NEOEMNext;
        data.header.nextEmId = (nextEm <= EM_COUNT) ? nextEm : EM_NONE;
        data.header.enabled = (uint8_t)ParamNEOEM_NEOEMEnabled;

        for (uint8_t cue = 0; cue < 10; ++cue)
        {
            loadCueFromEts(cue, data.cues[cue]);
        }
        for (uint8_t cue = 10; cue < EM_CUE_COUNT; ++cue)
        {
            clearCue(data.cues[cue]);
        }
    }
    _channelIndex = oldChannel;

    logInfoP("Loaded %d Effektmanager definitions from ETS", (int)EM_COUNT);
#else
    if (!_emData) return; // EM table not allocated (out of memory)
    std::memset(_emData, 0, EM_COUNT * sizeof(EffektManagerData));
    logWarningP("NEOEM parameter block missing - Effektmanager disabled");
#endif
}

// ============================================================================
// helpers
// ============================================================================

void NeoPixelBusModule::configureVirtualStripOrder()
{
    _virtualStripConfiguration.clear();

    // Check if we have duplicate configuration (user may have made mistakes)
    bool hasDuplicates = ParamNEO_VirtualStripHasDuplicates;
    if (hasDuplicates)
    {
        logWarningP("Virtual strip configuration has duplicates - using default order (simple sequential order)");
        // Fall back to simple sequential order
        createDefaultVirtualStripOrder();
        return;
    }

    logInfoP("Configuring virtual strip order from ETS parameters");

    constexpr size_t kMaxVirtualStripPositions = NEOPIXEL_MAX_PHYSICAL_STRIPS;

    // Read the virtual strip position parameters (1-8 positions for 1-8 strips)
    // Value 0 = not used, Values 1-8 = physical strip index (1-based)
    uint8_t positions[kMaxVirtualStripPositions] = {
        static_cast<uint8_t>(ParamNEO_VirtualStripPos1),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos2),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos3),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos4),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos5),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos6),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos7),
        static_cast<uint8_t>(ParamNEO_VirtualStripPos8)};

    // Read the start positions (calculated by ETS JavaScript)
    uint16_t startPositions[kMaxVirtualStripPositions] = {
        static_cast<uint16_t>(ParamNEO_VirtualStripStart1),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart2),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart3),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart4),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart5),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart6),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart7),
        static_cast<uint16_t>(ParamNEO_VirtualStripStart8)};

    // Build the virtual strip configuration based on positions
    for (size_t pos = 0; pos < kMaxVirtualStripPositions; pos++)
    {
        uint8_t physStripIndex = positions[pos];

        // Skip unused positions (value 0) or invalid indices
        if (physStripIndex == 0 || physStripIndex > _physicalStrips.size())
        {
            continue;
        }

        // Convert from 1-based to 0-based indexing
        physStripIndex -= 1;

        // Get the LED count for this physical strip
        if (physStripIndex < _physicalStrips.size() && _physicalStrips[physStripIndex])
        {
            uint16_t ledCount = _physicalStrips[physStripIndex]->getLedCount();
            uint16_t virtualStart = startPositions[pos];

            // Convert from 1-based (ETS user-friendly) to 0-based (internal indexing)
            // ETS shows "Start LED = 1" but internally we need offset 0
            uint16_t virtualStartZeroBased = (virtualStart > 0) ? (virtualStart - 1) : 0;

            _virtualStripConfiguration.emplace_back(physStripIndex, virtualStartZeroBased, ledCount);

            logInfoP("Virtual position %d: Physical strip %d (%d LEDs) starts at virtual position %d (ETS: %d)",
                     static_cast<int>(pos + 1), physStripIndex + 1, ledCount, virtualStartZeroBased, virtualStart);
        }
        else
        {
            logErrorP("Invalid physical strip index %d at virtual position %d", physStripIndex + 1, pos + 1);
        }
    }

    // Validate the configuration
    if (_virtualStripConfiguration.empty())
    {
        logWarningP("No valid virtual strip configuration found - using default order");
        createDefaultVirtualStripOrder();
    }
    else
    {
        logInfoP("Virtual strip configuration complete: %d physical strips mapped",
                 static_cast<int>(_virtualStripConfiguration.size()));
    }
}

void NeoPixelBusModule::createVirtualStripWithOrder()
{
    if (_physicalStrips.empty() || _totalLeds == 0 || _virtualStripConfiguration.empty())
    {
        logErrorP("Cannot create virtual strip: insufficient configuration");
        return;
    }

    // Determine if any physical strip requires RGBW (4 bytes) or RGBCCT (5 bytes per LED)
    bool needsRGBW = false;
    bool needsRGBCCT = false;
    for (const auto* phys : _physicalStrips)
    {
        if (phys)
        {
            ColorOrder order = phys->getColorOrder();
            // Check for 5-channel first (RGBCCT, GRBCCT, RGBCTW, GRBCTW)
            if (order == ColorOrder::RGBCCT || order == ColorOrder::GRBCCT ||
                order == ColorOrder::RGBCTW || order == ColorOrder::GRBCTW)
            {
                needsRGBCCT = true;
                break; // 5-channel is the maximum, no need to check further
            }
            // Check for 4-channel (RGBW, GRBW)
            else if (order == ColorOrder::RGBW || order == ColorOrder::GRBW)
            {
                needsRGBW = true;
            }
        }
    }

    // Create virtual strip: RGBCCT if any 5-channel, RGBW if any 4-channel, otherwise RGB
    ColorOrder virtualOrder;
    if (needsRGBCCT)
    {
        virtualOrder = ColorOrder::GRBCCT; // Standard 5-channel order
    }
    else if (needsRGBW)
    {
        virtualOrder = ColorOrder::RGBW;
    }
    else
    {
        virtualOrder = ColorOrder::RGB;
    }

    _virtualStrip = _neoPixel.addVirtualStrip(_totalLeds, virtualOrder);
    if (!_virtualStrip)
    {
        logErrorP("Failed to create virtual strip");
        return;
    }

    if (needsRGBCCT)
    {
        logInfoP("VirtualStrip created with RGBCCT support (5 bytes/LED) for 5-channel physical strips");
    }
    else if (needsRGBW)
    {
        logInfoP("VirtualStrip created with RGBW support (4 bytes/LED) for RGBW physical strips");
    }

    auto mgr = _neoPixel.getManager();
    if (!mgr)
    {
        logErrorP("NeoPixelManager not available for virtual strip creation");
        return;
    }

    // Attach physical strips to virtual strip according to the configured order
    for (const auto& config : _virtualStripConfiguration)
    {
        if (config.physicalStripIndex >= _physicalStrips.size())
        {
            logErrorP("Invalid physical strip index %d in virtual configuration", config.physicalStripIndex);
            continue;
        }

        PhysicalStrip* phys = _physicalStrips[config.physicalStripIndex];
        if (!phys)
        {
            logErrorP("Physical strip %d is null", config.physicalStripIndex);
            continue;
        }

        // Apply skip offset if configured for this strip
        uint8_t oldChannelIndex = _channelIndex;
        _channelIndex = config.physicalStripIndex;
        uint16_t skipLeds = ParamNEOSTRIP_NEOSkipFirstLEDs;
        _channelIndex = oldChannelIndex;

        uint16_t effectiveLeds = config.ledCount;
        uint16_t attachOffset = config.virtualStartPosition;

        // Apply skip offset if configured
        if (skipLeds > 0 && skipLeds < effectiveLeds)
        {
            logDebugP("Strip %d: Skipping first %d LEDs, effective range: %d-%d",
                      config.physicalStripIndex, skipLeds, skipLeds, effectiveLeds - 1);
        }

        if (mgr->attachPhysicalToVirtual(_virtualStrip, phys, attachOffset))
        {
            logInfoP("Attached Physical Strip %d to Virtual Strip at position %d (length %d, skip %d)",
                     config.physicalStripIndex, attachOffset, effectiveLeds, skipLeds);
        }
        else
        {
            logErrorP("Failed to attach Physical Strip %d to Virtual Strip", config.physicalStripIndex);
        }
    }

    logInfoP("Created Virtual Strip with user-defined order: %d LEDs, %d physical strips",
             _totalLeds, static_cast<int>(_virtualStripConfiguration.size()));
    logInfoP("ColorOrder Design: VirtualStrip=RGB (internal), PhysicalStrips=hardware-specific");

    // HCL pixel transformation callback will be registered later in updateHclTransformContext().

    // Configure color correction parameters on VirtualStrip
    // (These are applied during rendering, NOT in-place!)
    // NOTE: setColorCorrection removed from VirtualStrip - color correction deactivated for now
    /*
    if (_virtualStrip) {
      _virtualStrip->setColorCorrection(
        _gammaCorrectionEnabled, _gammaValue,
        _whiteBalanceEnabled, _whiteBalanceRed, _whiteBalanceGreen, _whiteBalanceBlue,
        _swapMode
      );
      logInfoP("VirtualStrip color correction configured: Gamma=%s(%.1f), WB=%s, Swap=%d",
               _gammaCorrectionEnabled ? "ON" : "OFF", _gammaValue,
               _whiteBalanceEnabled ? "ON" : "OFF",
               _swapMode);
    }
    */

    // Create segments after virtual strip is ready
    if (_numberOfSegments > 0)
    {
        createSegments();
        applySegmentConfiguration();
        logInfoP("Created %d segments on virtual strip", _numberOfSegments);
    }
}

void NeoPixelBusModule::createDefaultVirtualStripOrder()
{
    _virtualStripConfiguration.clear();

    // Create simple sequential order: Strip 1, Strip 2, Strip 3, etc.
    uint16_t currentStart = 0;
    for (size_t i = 0; i < _physicalStrips.size(); i++)
    {
        if (_physicalStrips[i])
        {
            uint16_t ledCount = _physicalStrips[i]->getLedCount();
            _virtualStripConfiguration.emplace_back(static_cast<uint8_t>(i), currentStart, ledCount);

            logInfoP("Default order: Physical strip %d (%d LEDs) starts at virtual position %d",
                     static_cast<int>(i), ledCount, currentStart);

            currentStart += ledCount;
        }
    }
}

bool NeoPixelBusModule::isSpiProtocol(LedProtocol protocol)
{
    switch (protocol)
    {
        case LedProtocol::APA102:
        case LedProtocol::APA102_CLONE:
        case LedProtocol::SK9822:
        case LedProtocol::WS2801:
        case LedProtocol::LPD8806:
            return true;
        default:
            return false;
    }
}

const char* NeoPixelBusModule::getColorOrderName(ColorOrder order)
{
    switch (order)
    {
        case ColorOrder::RGB: return "RGB";
        case ColorOrder::RBG: return "RBG";
        case ColorOrder::GRB: return "GRB";
        case ColorOrder::GBR: return "GBR";
        case ColorOrder::BGR: return "BGR";
        case ColorOrder::BRG: return "BRG";
        case ColorOrder::RGBW: return "RGBW";
        case ColorOrder::GRBW: return "GRBW";
        // 5-Channel Color Orders
        case ColorOrder::RGBCCT: return "RGBCCT";
        case ColorOrder::GRBCCT: return "GRBCCT";
        case ColorOrder::RGBCTW: return "RGBCTW";
        case ColorOrder::GRBCTW: return "GRBCTW";
        default: return "UNKNOWN";
    }
}

const char* NeoPixelBusModule::getProtocolName(LedProtocol protocol)
{
    switch (protocol)
    {
        case LedProtocol::WS2812: return "WS2812";
        case LedProtocol::WS2812B: return "WS2812B";
        case LedProtocol::WS2813: return "WS2813";
        case LedProtocol::WS2815: return "WS2815";
        case LedProtocol::WS2811: return "WS2811";
        case LedProtocol::SK6812: return "SK6812";
        case LedProtocol::SK6805: return "SK6805";
        case LedProtocol::WS2814: return "WS2814";
        case LedProtocol::TM1814: return "TM1814";
        case LedProtocol::GS8208: return "GS8208";
        case LedProtocol::APA102: return "APA102";
        case LedProtocol::SK9822: return "SK9822";
        case LedProtocol::WS2801: return "WS2801";
        case LedProtocol::LPD8806: return "LPD8806";
        // 5-Channel RGBCCT Protocols
        case LedProtocol::SK6812_RGBCCT: return "SK6812_RGBCCT";
        case LedProtocol::WS2814_RGBCCT: return "WS2814_RGBCCT";
        case LedProtocol::WS2805_RGBCCT: return "WS2805_RGBCCT";
        default: return "UNKNOWN";
    }
}

// ============================================================================
// Power Management Implementation
// ============================================================================

void NeoPixelBusModule::configurePowerManagement()
{
    auto mgr = _neoPixel.getManager();
    if (!mgr)
    {
        logErrorP("NeoPixelManager not available for power management configuration");
        return;
    }

    auto* powerManager = mgr->getPowerManager();
    if (!powerManager)
    {
        logErrorP("PowerManager not available");
        return;
    }

    // Read GLOBAL power parameters (Combined: 0=Disabled, 1=Global, 2=PerChannel, 3=PerLED)
    uint8_t powerLimitCombined = (uint8_t)ParamNEO_NEOpowerLimitCombined;

    // Configure power management in OFM
    powerManager->setEnabled(powerLimitCombined != 0); // Enabled if not 0

    if (powerLimitCombined != 0)
    {
        // Read all ETS parameters
        uint16_t powerLimitGlobal = (uint16_t)ParamNEO_NEOpowerLimitGlobal;
        uint16_t currentPerChannel = (uint16_t)ParamNEO_NEOcurrentPerChannel;
        uint8_t currentPerLed = (uint8_t)ParamNEO_NEOcurrentPerLED;
        uint8_t autoBrightnessLimit = (uint8_t)ParamNEO_NEOautoBrightnessLimit;
        uint8_t powerLimitThreshold = (uint8_t)ParamNEO_NEOpowerLimitThreshold;
        uint8_t ablSlewRate = (uint8_t)ParamNEO_NEOablSlewRatePercent;

        // Set power limit mode based on combined value
        // ETS values: 0=Disabled, 1=Fester Wert (GLOBAL), 2=Pro LED (PER_LED)
        PowerLimitMode mode;
        switch (powerLimitCombined)
        {
            case 1: mode = PowerLimitMode::GLOBAL; break;  // "Fester Wert (mA)"
            case 2: mode = PowerLimitMode::PER_LED; break; // "Pro LED (automatisch)"
            default: mode = PowerLimitMode::GLOBAL; break;
        }
        powerManager->setPowerLimitMode(mode);

        // Configure limits based on mode
        // Note: PER_CHANNEL mode is not exposed in ETS (only GLOBAL and PER_LED)
        switch (mode)
        {
            case PowerLimitMode::GLOBAL:
                // "Fester Wert (mA)" - use global limit parameter
                powerManager->setMaxCurrent(powerLimitGlobal);
                break;

            case PowerLimitMode::PER_LED:
                // "Pro LED (automatisch)" - use per-LED parameter
                powerManager->setMaxCurrentPerLed(currentPerLed);
                break;

            case PowerLimitMode::PER_CHANNEL:
                // Not used in ETS, but keep for completeness
                powerManager->setMaxCurrentPerChannel(currentPerChannel);
                break;
        }

        // Set soft-limiting threshold
        powerManager->setThresholdPercent(powerLimitThreshold);

        // Set auto brightness limit
        powerManager->setMaxBrightnessPercent(autoBrightnessLimit);

        // Set brightness slew rate
        powerManager->setBrightnessSlewRate(ablSlewRate);

        // Configure LED current profile based on ETS settings and detected strip type
        LedCurrentProfile profile;

        // Check if we have any RGBW (4-channel) or RGBCCT (5-channel) strips
        bool hasRgbwStrip = false;
        bool hasRgbcctStrip = false;
        for (const auto& strip : _physicalStrips)
        {
            if (strip)
            {
                ColorOrder order = strip->getColorOrder();
                // 5-channel: RGBCCT, GRBCCT, RGBCTW, GRBCTW
                if (order == ColorOrder::RGBCCT || order == ColorOrder::GRBCCT ||
                    order == ColorOrder::RGBCTW || order == ColorOrder::GRBCTW)
                {
                    hasRgbcctStrip = true;
                    break; // 5-channel is the maximum
                }
                // 4-channel: RGBW, GRBW
                else if (order == ColorOrder::RGBW || order == ColorOrder::GRBW)
                {
                    hasRgbwStrip = true;
                }
            }
        }

        if (currentPerLed > 0 && mode != PowerLimitMode::PER_LED)
        {
            // User specified custom current per LED
            // For RGB: divide by 3, RGBW: divide by 4, RGBCCT: divide by 5
            if (hasRgbcctStrip)
            {
                uint8_t perChannel = currentPerLed / 5;
                profile = LedCurrentProfile(perChannel, perChannel, perChannel, perChannel, perChannel);
            }
            else if (hasRgbwStrip)
            {
                uint8_t perChannel = currentPerLed / 4;
                profile = LedCurrentProfile(perChannel, perChannel, perChannel, perChannel);
            }
            else
            {
                uint8_t perChannel = currentPerLed / 3;
                profile = LedCurrentProfile(perChannel, perChannel, perChannel, 0);
            }
        }
        else
        {
            // Use default profile based on strip type
            if (hasRgbcctStrip)
            {
                profile = LedProfiles::SK6812_RGBCCT;
            }
            else if (hasRgbwStrip)
            {
                profile = LedProfiles::SK6812_RGBW;
            }
            else
            {
                profile = LedProfiles::WS2812B;
            }
        }

        powerManager->setLedProfile(profile);

        // Log configuration
        const char* modeNames[] = {"GLOBAL", "PER_CHANNEL", "PER_LED"};
        switch (mode)
        {
            case PowerLimitMode::GLOBAL:
                logInfoP("Power Management: %s mode - Limit=%dmA, Profile=R:%dmA G:%dmA B:%dmA WW:%dmA CW:%dmA",
                         modeNames[0], powerLimitGlobal, profile.redMA, profile.greenMA, profile.blueMA, profile.warmWhiteMA, profile.coolWhiteMA);
                break;
            case PowerLimitMode::PER_LED:
                logInfoP("Power Management: %s mode - %dmA per LED, Profile=R:%dmA G:%dmA B:%dmA WW:%dmA CW:%dmA",
                         modeNames[2], currentPerLed, profile.redMA, profile.greenMA, profile.blueMA, profile.warmWhiteMA, profile.coolWhiteMA);
                break;
            case PowerLimitMode::PER_CHANNEL:
                logInfoP("Power Management: %s mode - Limit=%dmA per channel, Profile=R:%dmA G:%dmA B:%dmA WW:%dmA CW:%dmA",
                         modeNames[1], currentPerChannel, profile.redMA, profile.greenMA, profile.blueMA, profile.warmWhiteMA, profile.coolWhiteMA);
                break;
        }

        logInfoP("  Threshold=%d%%, MaxBrightness=%d%%, SlewRate=%d%%/s",
                 powerLimitThreshold, autoBrightnessLimit, ablSlewRate);
    }
    else
    {
        logInfoP("Power Management: DISABLED");
    }
}

// ============================================================================
// Global Brightness Control Implementation (delegated to ColorManagement)
// ============================================================================

void NeoPixelBusModule::applyGlobalBrightness(uint8_t brightness)
{
    _colorManagement->applyGlobalBrightness(brightness);
}

void NeoPixelBusModule::restoreOriginalBrightness()
{
    _colorManagement->restoreOriginalBrightness();
}

// ============================================================================
// Color Correction Implementation (delegated to ColorManagement)
// ============================================================================

void NeoPixelBusModule::configureColorCorrection()
{
    _colorManagement->configureColorCorrection();
}

void NeoPixelBusModule::updateColorCorrection()
{
    _colorManagement->updateColorCorrection();
}

void NeoPixelBusModule::forceColorCorrectionUpdate()
{
    _colorManagement->forceColorCorrectionUpdate();
}

// ============================================================================
// Strip Options Implementation
// ============================================================================

// Configure strip options from ETS parameters
void NeoPixelBusModule::configureStripOptions()
{
    // Swap mode configuration
    _swapMode = mapSwapMode(static_cast<uint8_t>(ParamNEOSTRIP_NEOSwap));
    logDebugP("Swap mode configured: %d", _swapMode);

    // Skip first LEDs - now handled automatically in VirtualStrip::syncToPhysical()
    _skipFirstLeds = ParamNEOSTRIP_NEOSkipFirstLEDs;
    if (_skipFirstLeds > 0)
    {
        logInfoP("Skip first LEDs configured: %d LEDs (handled by OFM-NeoPixel)", _skipFirstLeds);
    }
}

// Map ETS swap parameter to swap mode
uint8_t NeoPixelBusModule::mapSwapMode(uint8_t paramValue)
{
    // 3-bit parameter: 0-7 possible values
    // 0 = No swap
    // 1 = R<->G swap
    // 2 = R<->B swap
    // 3 = G<->B swap
    // 4 = R->B->G->R rotate
    // 5 = R->G->B->R rotate
    // 6-7 = Reserved for future use
    return paramValue & 0x07; // Mask to ensure 3-bit value
}

// Helper for channel swapping
void NeoPixelBusModule::swapChannels(uint8_t& ch1, uint8_t& ch2)
{
    uint8_t temp = ch1;
    ch1 = ch2;
    ch2 = temp;
}

// ============================================================================
// Segment Configuration Implementation
// ============================================================================

// Configure segments from ETS parameters
void NeoPixelBusModule::configureSegments()
{
    // Read number of segments from ETS
    _numberOfSegments = ParamNEO_NEONumberOfSegments;

    logInfoP("configureSegments: Reading _numberOfSegments = %d from ETS", _numberOfSegments);

    if (_numberOfSegments == 0)
    {
        logDebugP("No segments configured");
        return;
    }

    // Clear existing segments
    _segments.clear();
    _segments.reserve(_numberOfSegments);

    // Configure each segment from ETS parameters
    for (uint8_t i = 0; i < _numberOfSegments; i++)
    {
        SegmentConfig config = createSegmentConfig(i);
        _segments.push_back(config);

        logInfoP("Segment %d: Start=%d, End=%d, Offset=%d, Group=%d, Spacing=%d, Reverse=%s, Mirror=%s",
                 i, config.startLed, config.endLed, config.offset, config.grouping, config.spacing,
                 config.reverseDirection ? "YES" : "NO", config.mirrorEffect ? "YES" : "NO");
    }

    logInfoP("Configured %d unique segments (each with individual ETS parameters), _segments.size() = %d",
             _numberOfSegments, (int)_segments.size());
}

// Create segment configuration from ETS parameters
NeoPixelBusModule::SegmentConfig NeoPixelBusModule::createSegmentConfig(uint8_t segmentIndex)
{
    SegmentConfig config;

    // Store current channel index and set to segment index for parameter access
    // This is crucial: NEO_ParamCalcIndex uses _channelIndex to calculate per-segment parameters
    // Without this, all segments would read identical parameter values from index 0
    uint8_t oldChannelIndex = _channelIndex;
    _channelIndex = segmentIndex;

    // Read segment parameters from ETS (these are now truly per-segment parameters)
    uint16_t etsStartLed = ParamNEO_NEOSegmentStart;
    uint16_t etsEndLed = ParamNEO_NEOSegmentEnd;

    // Convert from 1-based (ETS user-friendly) to 0-based (internal indexing)
    // ETS shows "Start LED = 1" but internally we need index 0
    config.startLed = (etsStartLed > 0) ? (etsStartLed - 1) : 0;
    config.endLed = (etsEndLed > 0) ? (etsEndLed - 1) : 0;
    config.offset = ParamNEO_NEOSegmentOffset;
    config.grouping = ParamNEO_NEOSegmentGrouping;
    config.spacing = ParamNEO_NEOSegmentSpacing;
    config.reverseDirection = ParamNEO_NEOSegmentReverseDirection;
    config.mirrorEffect = ParamNEO_NEOSegmentMirrorEffect;

    // 2D/3D matrix geometry — read from reserved bytes (0 = 1D, no matrix)
    // ParamNEO_NEOMatrixWidth/Height/Topology are defined once the ETS XML is updated.
#ifdef ParamNEO_NEOMatrixWidth
    config.matrixWidth = (uint8_t)ParamNEO_NEOMatrixWidth;
    config.matrixHeight = (uint8_t)ParamNEO_NEOMatrixHeight;
    config.matrixDepth = (uint8_t)ParamNEO_NEOMatrixDepth;
    config.topology = (LedTopology)ParamNEO_NEOSegmentTopology;
#else
    config.matrixWidth = 0;
    config.matrixHeight = 0;
    config.matrixDepth = 0;
    config.topology = LedTopology::LINEAR_1D;
#endif

    // ── Effektkette (virtual band) params ────────────────────────────────
#ifdef ParamNEO_NEOSyncMode
    config.syncMode = (uint8_t)ParamNEO_NEOSyncMode;
    config.syncOverridePolicy = (uint8_t)ParamNEO_NEOSyncOverridePolicy;
    config.syncTimeoutSteps = (uint8_t)ParamNEO_NEOSyncTimeout;
    config.virtualTotalLength = (uint16_t)ParamNEO_NEOVirtualTotalLength;
    config.virtualOffset = (uint16_t)ParamNEO_NEOVirtualOffset;
#else
    config.syncMode = 0;
    config.syncOverridePolicy = 0;
    config.syncTimeoutSteps = 0;
    config.virtualTotalLength = 0;
    config.virtualOffset = 0;
#endif

    config.distributedMatrixWidth = 0;
    config.distributedMatrixHeight = 0;
    if (config.matrixWidth > 1 && config.matrixHeight > 1 &&
        config.virtualTotalLength > 0 &&
        config.virtualTotalLength > ((uint16_t)config.matrixWidth * config.matrixHeight) &&
        (config.virtualTotalLength % config.matrixHeight) == 0)
    {
        config.distributedMatrixWidth = config.virtualTotalLength / config.matrixHeight;
        config.distributedMatrixHeight = config.matrixHeight;
    }

    logDebugP("Segment %d: Read ETS params - Start=%d, End=%d, Offset=%d, Group=%d, Spacing=%d (ETS Start=%d, End=%d)",
              segmentIndex, config.startLed, config.endLed, config.offset, config.grouping, config.spacing,
              etsStartLed, etsEndLed);

    // Validate segment bounds
    if (config.startLed > config.endLed)
    {
        logErrorP("Segment %d: Invalid range - Start(%d) > End(%d)",
                  segmentIndex, config.startLed, config.endLed);
        // Swap them
        uint16_t temp = config.startLed;
        config.startLed = config.endLed;
        config.endLed = temp;
    }

    // Ensure segment fits within total LED count
    if (config.startLed >= _totalLeds)
    {
        logWarningP("Segment %d: Start LED(%d) exceeds total LEDs(%d), clamping to 0",
                    segmentIndex, config.startLed, _totalLeds);
        config.startLed = 0;
    }

    if (config.endLed >= _totalLeds)
    {
        logWarningP("Segment %d: End LED(%d) exceeds total LEDs(%d), adjusting",
                    segmentIndex, config.endLed, _totalLeds);
        config.endLed = _totalLeds - 1;
    }

    // Restore original channel index
    _channelIndex = oldChannelIndex;

    return config;
}

// Create segments on virtual strip
void NeoPixelBusModule::createSegments()
{
    if (!_virtualStrip || _segments.empty())
    {
        return;
    }

    auto mgr = _neoPixel.getManager();
    if (!mgr)
    {
        logErrorP("NeoPixelManager not available for segment creation");
        return;
    }

    // Create actual segments
    for (size_t i = 0; i < _segments.size(); i++)
    {
        SegmentConfig& config = _segments[i];

        // Calculate effective start/end considering offset
        uint16_t effectiveStart = config.startLed + config.offset;
        uint16_t effectiveEnd = config.endLed + config.offset;

        // Ensure bounds are within virtual strip
        if (effectiveEnd >= _virtualStrip->getLedCount())
        {
            effectiveEnd = _virtualStrip->getLedCount() - 1;
            logWarningP("Segment %zu: Adjusted end to %d (within virtual strip bounds)", i, effectiveEnd);
        }

        if (effectiveStart <= effectiveEnd)
        {
            config.segment = mgr->addSegment(_virtualStrip, effectiveStart, effectiveEnd);

            if (config.segment)
            {
                logInfoP("Created segment %zu: LEDs %d-%d (length %d)",
                         i, effectiveStart, effectiveEnd, effectiveEnd - effectiveStart + 1);
            }
            else
            {
                logErrorP("Failed to create segment %zu", i);
            }
        }
        else
        {
            logErrorP("Segment %zu: Invalid effective range %d-%d", i, effectiveStart, effectiveEnd);
        }
    }
}

// Apply segment-specific configuration
void NeoPixelBusModule::applySegmentConfiguration()
{
    for (size_t i = 0; i < _segments.size(); i++)
    {
        SegmentConfig& config = _segments[i];
        if (!config.segment) continue;

        // Apply grouping and spacing via Segment API
        if (config.grouping > 1 || config.spacing > 0)
        {
            config.segment->setGrouping(config.grouping);
            config.segment->setSpacing(config.spacing);
            logInfoP("Segment %zu: Applied grouping=%d, spacing=%d (virtual length=%d)",
                     i, config.grouping, config.spacing, config.segment->getLength());
        }

        // Apply offset (shift effect start position)
        if (config.offset > 0)
        {
            config.segment->setOffset(config.offset);
            logInfoP("Segment %zu: Offset=%d LEDs", i, config.offset);
        }

        // Apply reverse direction
        if (config.reverseDirection)
        {
            config.segment->setReverse(true);
            logInfoP("Segment %zu: Reverse direction enabled", i);
        }

        // Apply mirror effect
        if (config.mirrorEffect)
        {
            config.segment->setMirror(true);
            logInfoP("Segment %zu: Mirror effect enabled", i);
        }

        // Apply 2D/3D matrix geometry
        if (config.matrixWidth > 1 && config.matrixHeight > 1)
        {
            if (config.matrixDepth > 1)
                config.segment->setGeometry(config.matrixWidth, config.matrixHeight,
                                            config.matrixDepth, config.topology);
            else
                config.segment->setGeometry(config.matrixWidth, config.matrixHeight,
                                            config.topology);
            logInfoP("Segment %zu: Matrix %dx%dx%d topology=%d",
                     i, config.matrixWidth, config.matrixHeight,
                     config.matrixDepth > 1 ? config.matrixDepth : 1,
                     (int)config.topology);
        }

        // Apply Effektkette (virtual band)
        if (config.syncMode != 0 && config.virtualTotalLength > 0)
        {
            config.segment->setVirtualBand(config.virtualTotalLength, config.virtualOffset);
            logInfoP("Segment %zu: Effektkette mode=%d totalLen=%d offset=%d",
                     i, config.syncMode, config.virtualTotalLength, config.virtualOffset);

            if (config.distributedMatrixWidth > 0)
            {
                logInfoP("Segment %zu: Distributed2D global=%dx%d local=%dx%d",
                         i,
                         config.distributedMatrixWidth,
                         config.distributedMatrixHeight,
                         config.matrixWidth,
                         config.matrixHeight);
            }
        }

        logDebugP("Applied configuration to segment %zu", i);
    }
}

// Get segment by index
Segment* NeoPixelBusModule::getSegment(uint8_t index) const
{
    if (index < _segments.size())
    {
        return _segments[index].segment;
    }
    return nullptr;
}

// =============================================================================
// HCL Management (Human Centric Lighting)
// =============================================================================

void NeoPixelBusModule::refreshHclMasterStateCache()
{
    const bool lightManagerEnabled = HCL::masterManager.isEnabled();
    const uint8_t masterCount = lightManagerEnabled ? HCL::masterManager.getMasterCount() : 0;
    const bool globalBlocked = lightManagerEnabled ? HCL::masterManager.isApplyBlocked() : true;
    static uint16_t lastLoggedKelvin[16] = {0};
    static uint8_t lastLoggedBrightness[16] = {0};
    static bool lastLoggedBlocked[16] = {true};

    for (size_t index = 0; index < _hclTransformContext.masterStates.size(); ++index)
    {
        NeoHclMasterState& state = _hclTransformContext.masterStates[index];
        state.kelvin = 0;
        state.brightness = 0;
        state.blocked = true;

        const uint8_t masterNum = static_cast<uint8_t>(index + 1);
        if (masterNum <= masterCount)
        {
            const auto value = HCL::masterManager.getCurrentValue(masterNum);
            state.kelvin = value.kelvin;
            state.brightness = value.brightness;
            state.blocked = globalBlocked || HCL::masterManager.isMasterApplyBlocked(masterNum);
        }

        if (state.kelvin == 0)
        {
            state.cachedKelvin = 0;
        }
    }

    _hclTransformContext.globalHclConfig.resolvedMaster =
        resolveHclMaster(_hclTransformContext.globalHclConfig.lightManagerMaster, masterCount);

    for (size_t index = 0; index < _hclTransformContext.masterStates.size(); ++index)
    {
        const NeoHclMasterState& state = _hclTransformContext.masterStates[index];
        if (state.kelvin == lastLoggedKelvin[index] && state.brightness == lastLoggedBrightness[index] &&
            state.blocked == lastLoggedBlocked[index])
        {
            continue;
        }

        uint8_t kelvinR = 0;
        uint8_t kelvinG = 0;
        uint8_t kelvinB = 0;
        if (state.kelvin > 0)
        {
            HclPixelTransform::kelvinToRGB(state.kelvin, kelvinR, kelvinG, kelvinB);
        }

        logInfoP("HCL master %u: kelvin=%u brightness=%u blocked=%d kelvinRgb=(%u,%u,%u)",
                 static_cast<unsigned>(index + 1),
                 state.kelvin,
                 state.brightness,
                 state.blocked,
                 kelvinR,
                 kelvinG,
                 kelvinB);

        lastLoggedKelvin[index] = state.kelvin;
        lastLoggedBrightness[index] = state.brightness;
        lastLoggedBlocked[index] = state.blocked;
    }

    for (NeoHclSegmentConfig& segCfg : _hclTransformContext.segmentConfigs)
    {
        if (segCfg.hclMode == NeoHclMode::Custom)
        {
            segCfg.customHclConfig.resolvedMaster =
                resolveHclMaster(segCfg.customHclConfig.lightManagerMaster, masterCount);
        }
    }

    static uint16_t lastGlobalKelvin = 0;
    uint16_t globalKelvin = 0;
    if (_hclTransformContext.globalHclConfig.resolvedMaster > 0)
    {
        globalKelvin = _hclTransformContext.masterStates[_hclTransformContext.globalHclConfig.resolvedMaster - 1].kelvin;
    }

    if (globalKelvin != lastGlobalKelvin)
    {
        KoNEO_HCLState.value(globalKelvin, Dpt(7, 600));
        KoNEO_HCLState.objectWritten();
        lastGlobalKelvin = globalKelvin;
    }

    static uint16_t lastSegmentKelvin[16] = {0};
    bool anyHclActive = globalKelvin > 0;

    for (size_t index = 0; index < _hclTransformContext.segmentConfigs.size() && index < _segments.size(); ++index)
    {
        const NeoHclSegmentConfig& segCfg = _hclTransformContext.segmentConfigs[index];
        uint8_t resolvedMaster = 0;

        if (segCfg.hclMode == NeoHclMode::Global)
        {
            resolvedMaster = _hclTransformContext.globalHclConfig.resolvedMaster;
        }
        else if (segCfg.hclMode == NeoHclMode::Custom)
        {
            resolvedMaster = segCfg.customHclConfig.resolvedMaster;
        }

        uint16_t segmentKelvin = 0;
        if (resolvedMaster > 0)
        {
            segmentKelvin = _hclTransformContext.masterStates[resolvedMaster - 1].kelvin;
            anyHclActive = anyHclActive || (segmentKelvin > 0);
        }

        if (index < 16 && segmentKelvin != lastSegmentKelvin[index])
        {
            const uint8_t savedChannelIndex = _channelIndex;
            _channelIndex = static_cast<uint8_t>(index);
            KoNEO_HCLState.value(segmentKelvin, Dpt(7, 600));
            KoNEO_HCLState.objectWritten();
            _channelIndex = savedChannelIndex;
            lastSegmentKelvin[index] = segmentKelvin;
        }
    }

    _hclModeEnabled = anyHclActive;
    _hclTargetKelvin = globalKelvin;
    _hclAppliedKelvin = globalKelvin;
}

// =============================================================================
// HCL Pixel Transform Context Update
// =============================================================================

/**
 * @brief Update HCL pixel transformation context and register callback
 *
 * This method builds the LightManager-backed HclTransformContext from current segment configurations
 * and ETS parameters, then registers the Library's pixel transformation callback
 * with VirtualStrip. This allows HCL to be applied to pixels without the Library
 * directly accessing ETS parameters.
 */
void NeoPixelBusModule::updateHclTransformContext()
{
    if (!_virtualStrip) return;

    auto decodeHclApplyMode = [](uint8_t value) {
        switch (value)
        {
            case 1: return NeoHclApplyMode::AllColors;
            case 2: return NeoHclApplyMode::HighSaturation;
            default: return NeoHclApplyMode::WhiteOnly;
        }
    };

    // Clear and rebuild context
    _hclTransformContext.segments.clear();
    _hclTransformContext.segmentConfigs.clear();

    // Build segment list and configurations
    for (size_t i = 0; i < _segments.size(); i++)
    {
        Segment* segment = _segments[i].segment;
        if (!segment) continue;

        _hclTransformContext.segments.push_back(segment);

        // Read segment HCL configuration from ETS
        uint8_t savedChannelIndex = _channelIndex;
        _channelIndex = i;

        NeoHclSegmentConfig segCfg;

        // Read HCL source mode (0=Disabled, 1=Global LightManager, 2=Segment LightManager)
        uint8_t hclMode = ParamNEO_NEOHCLMode;

        if (hclMode == 0)
        {
            segCfg.hclMode = NeoHclMode::Disabled;
        }
        else if (hclMode == 1)
        {
            segCfg.hclMode = NeoHclMode::Global;
        }
        else
        {
            segCfg.hclMode = NeoHclMode::Custom;
            segCfg.customHclConfig.lightManagerMaster = ParamNEO_NEOHCLMaster;
            segCfg.customHclConfig.applyMode = decodeHclApplyMode(ParamNEO_NEOHCLApplyMode);
        }

        _hclTransformContext.segmentConfigs.push_back(segCfg);

        _channelIndex = savedChannelIndex;
    }

    // Build Pixel→Segment Lookup Table for O(1) segment lookup
    // This eliminates the O(n) for-loop on EVERY pixel (8,280x/second)
    // Reduces segment lookups from ~24,840/sec to 0!
    const uint16_t totalLEDs = _virtualStrip->getTotalPhysicalLeds();
    _hclTransformContext.pixelToSegmentIndex.clear();
    _hclTransformContext.pixelToSegmentIndex.resize(totalLEDs, 0xFF); // 0xFF = no segment

    for (size_t segIdx = 0; segIdx < _hclTransformContext.segments.size(); segIdx++)
    {
        Segment* seg = _hclTransformContext.segments[segIdx];
        if (!seg) continue;

        const uint16_t startLed = seg->getStartLed();
        const uint16_t length = seg->getLength();
        const uint16_t endLed = startLed + length;

        // Mark all pixels of this segment
        for (uint16_t pixelIdx = startLed; pixelIdx < endLed && pixelIdx < totalLEDs; pixelIdx++)
        {
            _hclTransformContext.pixelToSegmentIndex[pixelIdx] = static_cast<uint8_t>(segIdx);
        }
    }

    logInfoP("Built pixel→segment lookup table: %u LEDs, %u segments",
             totalLEDs, _hclTransformContext.segments.size());

    // Build global LightManager-backed HCL configuration from ETS parameters.
    {
        uint8_t savedChannelIndex = _channelIndex;
        _channelIndex = 0xFF; // Use global context
        _hclTransformContext.globalHclConfig.lightManagerMaster = ParamNEO_HCLMaster;
        _hclTransformContext.globalHclConfig.applyMode = decodeHclApplyMode(ParamNEO_HCLApplyMode);

        _channelIndex = savedChannelIndex;
    }

    refreshHclMasterStateCache();

    // Register HCL pixel transformation callback only if at least one source is configured.
    bool hclActive = false;
    if (_hclTransformContext.globalHclConfig.lightManagerMaster > 0)
    {
        hclActive = true;
    }
    else
    {
        for (const auto& segCfg : _hclTransformContext.segmentConfigs)
        {
            if (segCfg.hclMode == NeoHclMode::Custom && segCfg.customHclConfig.lightManagerMaster > 0)
            {
                hclActive = true;
                break;
            }
        }
    }

    if (hclActive)
    {
        _virtualStrip->setPixelTransformCallback(HclPixelTransform::Callback, &_hclTransformContext);
        logInfoP("HCL pixel transformation callback registered (LightManager-backed, %d segments)", _hclTransformContext.segments.size());
    }
    else
    {
        _virtualStrip->setPixelTransformCallback(nullptr, nullptr);
        logInfoP("HCL pixel transformation callback NOT registered (no LightManager source configured)");
    }
}

// =============================================================================
// Debug: Configuration Analysis (OPENKNX_DEBUG only)
// =============================================================================

#ifdef OPENKNX_DEBUG
void NeoPixelBusModule::debugShowConfiguration()
{
    logInfoP("════════════════════════════════════════════════════════════════════════");
    logInfoP("  NeoPixel ETS Configuration Analysis");
    logInfoP("════════════════════════════════════════════════════════════════════════");
    logInfoP("");

    // ========================================================================
    // Hardware Configuration
    // ========================================================================
    logInfoP("▶ Hardware Configuration:");
    logInfoP("────────────────────────────────────────────────────────────────────────");

    #ifdef DEVICE_HW_ID
    uint8_t hwIndex = getCurrentHardwareIndex();
    const char* hwName = HardwareMapping::getHardwareName(hwIndex);
    logInfoP("  Mode:             Compile-time (DEVICE_HW_ID defined)");
    logInfoP("  Device HW ID:     0x%04X (%d decimal)", DEVICE_HW_ID, DEVICE_HW_ID);
    logInfoP("  Hardware Index:   %d", hwIndex);
    logInfoP("  Hardware Name:    %s", hwName ? hwName : "Unknown");
    #else
        #ifdef ParamNEO_NeoPixelHardwareSelect
    uint16_t selectedHwId = (uint16_t)ParamNEO_NeoPixelHardwareSelect; // stored selection = DEVICE_HW_ID
    uint8_t hwIndex = getCurrentHardwareIndex();
    const char* hwName = HardwareMapping::getHardwareName(hwIndex);
    logInfoP("  Mode:             ETS Runtime Selection");
    logInfoP("  Selected HW ID:   0x%04X", selectedHwId);
    logInfoP("  Hardware Index:   %d", hwIndex);
    logInfoP("  Hardware Name:    %s", hwName ? hwName : "Unknown");
        #else
    logInfoP("  Mode:             Not configured (no hardware selection)");
        #endif
    #endif
    logInfoP("");

    // ========================================================================
    // Strip Configuration
    // ========================================================================
    logInfoP("▶ Strip Configuration:");
    logInfoP("────────────────────────────────────────────────────────────────────────");
    logInfoP("  Physical Strips:  %d", _physicalStrips.size());
    logInfoP("  Total LEDs:       %d", _totalLeds);
    logInfoP("  Skip First LEDs:  %d (Global)", _skipFirstLeds);
    logInfoP("  Swap Mode:        %d (Global)", _swapMode);

    // Global Power Limiting
    uint8_t globalPowerMode = (uint8_t)ParamNEO_NEOpowerLimitCombined;
    const char* globalPowerModeNames[] = {"Deaktiviert", "Fester Wert", "Pro LED"};
    const char* globalPowerModeName = (globalPowerMode < 3) ? globalPowerModeNames[globalPowerMode] : "Unknown";
    logInfoP("  Global Power:     %d (%s)", globalPowerMode, globalPowerModeName);
    if (globalPowerMode == 1)
    {
        uint16_t maxCurrent = (uint16_t)ParamNEO_NEOcurrentPerChannel;
        logInfoP("    Max Current:    %d mA", maxCurrent);
    }
    else if (globalPowerMode == 2)
    {
        uint8_t currentPerLed = (uint8_t)ParamNEO_NEOcurrentPerLED;
        logInfoP("    Current/LED:    %d mA", currentPerLed);
    }

    // Global Startup Behavior & Start-Farbe
    uint8_t startupBehavior = (uint8_t)ParamNEO_NEOGlobalStartupBehavior;
    const char* startupBehaviorNames[] = {"Aus", "Letzter Zustand", "ETS-Parameterwert"};
    const char* startupBehaviorName = (startupBehavior < 3) ? startupBehaviorNames[startupBehavior] : "Unknown";
    logInfoP("  Startup Behavior: %d (%s)", startupBehavior, startupBehaviorName);
    if (startupBehavior == 2)
    { // ETS-Parameterwert
        uint8_t r = (uint8_t)ParamNEO_NEOGlobalStartupR;
        uint8_t g = (uint8_t)ParamNEO_NEOGlobalStartupG;
        uint8_t b = (uint8_t)ParamNEO_NEOGlobalStartupB;
        uint8_t w = (uint8_t)ParamNEO_NEOGlobalStartupW;
        uint8_t brightness = (uint8_t)ParamNEO_NEOGlobalStartupBrightness;
        uint8_t effect = (uint8_t)ParamNEO_NEOGlobalStartupEffect;
        logInfoP("    Color (RGBW):   R=%d G=%d B=%d W=%d", r, g, b, w);
        logInfoP("    Brightness:     %d", brightness);
        logInfoP("    Effect:         %d", effect);
    }
    logInfoP("");

    // Show each physical strip with ETS and actual values
    for (size_t i = 0; i < _physicalStrips.size(); i++)
    {
        PhysicalStrip* strip = _physicalStrips[i];
        if (!strip) continue;

        // Set _channelIndex to read ETS parameters for this strip
        uint8_t _channelIndex = i;

        logInfoP("  ┌─ Strip %d:", i + 1);
        logInfoP("  │");

        // === ETS Configuration (Raw Parameters) ===
        logInfoP("  │  📋 ETS Configuration:");
        logInfoP("  │    LED Count:        %d", (uint16_t)ParamNEOSTRIP_NEOLength);

        uint8_t ledTypeParam = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const char* ledTypeNames[] = {
            "WS2811/WS2812/WS2812B (GRB)",
            "WS2813 (GRB)",
            "WS2815 (GRB)",
            "SK6812 (GRB)",
            "TM1804 (RGB)",
            "TM1829 (RGB/WRGB)",
            "APA106 (RGB)",
            "UCS1904 (RGB)",
            "SK6812/WS2814 (RGBW)",
            "LPD6803 (RGB SPI)",
            "LPD8806 (GRB SPI)",
            "WS2801 (RGB SPI)",
            "P9813 (RGB SPI)",
            "APA102 (RGB SPI)",
            "SK9822 (RGB SPI)"};
        const char* ledTypeName = (ledTypeParam < 15) ? ledTypeNames[ledTypeParam] : "Unknown";
        logInfoP("  │    LED Type:         %d (%s)", ledTypeParam, ledTypeName);

        uint8_t colorOrderParam = (uint8_t)ParamNEOSTRIP_NEOColourOrder;
        const char* colorOrderNames[] = {"RGB", "RBG", "GRB", "GBR", "BRG", "BGR", "WRGB", "WRBG", "WGRB", "WGBR", "WBRG", "WBGR", "RGBW", "RBGW", "GRBW", "GBRW", "BRGW", "BGRW", "RWGB", "RWBG", "GWRB", "GWBR", "BWRG", "BWGR", "RGBW", "RBWG", "GRBW", "GBWR", "BRWG", "BGWR"};
        const char* colorOrderName = (colorOrderParam < 30) ? colorOrderNames[colorOrderParam] : "Unknown";
        logInfoP("  │    Color Order:      %d (%s)", colorOrderParam, colorOrderName);

        // GPIO Configuration Mode
        bool gpioManual = (bool)ParamNEOSTRIP_NEOGPIOManual;
        logInfoP("  │    GPIO Mode:        %s", gpioManual ? "Manual" : "Hardware-assisted");

        if (gpioManual)
        {
            // Manual mode: show configured pins
            if (isSpiProtocol(strip->getProtocol()))
            {
                logInfoP("  │      MOSI GPIO:      %d", (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO);
                logInfoP("  │      SCK GPIO:       %d", (uint8_t)ParamNEOSTRIP_NEOClockGPIO);

                // SPI Frequency
                bool spiClkManual = (ParamNEOSTRIP_NEOSPICLKMode == 1);
                if (spiClkManual)
                {
                    uint8_t spiClkValue = (uint8_t)ParamNEOSTRIP_NEOSPICLK;
                    const char* spiFreqNames[] = {"1 MHz", "2 MHz", "4 MHz", "8 MHz", "12 MHz", "16 MHz", "20 MHz"};
                    const char* spiFreqName = (spiClkValue < 7) ? spiFreqNames[spiClkValue] : "Unknown";
                    logInfoP("  │      SPI Frequency:  %d (%s)", spiClkValue, spiFreqName);
                }
                else
                {
                    logInfoP("  │      SPI Frequency:  Auto (10 MHz)");
                }
            }
            else
            {
                logInfoP("  │      Data GPIO:      %d", (uint8_t)ParamNEOSTRIP_NEODataGPIO);
            }
        }
        else
        {
            // Hardware-assisted mode: show port selection
            uint8_t hwIndex = getCurrentHardwareIndex();
            uint8_t dataPortSelection = getDataPortSelectionForHardwareIndex(i, hwIndex);

            if (isHardwarePortSelected(dataPortSelection))
            {
                logInfoP("  │      Data Port:      %d (ETS selection)", dataPortSelection);
            }
            else
            {
                logInfoP("  │      Data Port:      Auto (not selected in ETS)");
            }

            if (isSpiProtocol(strip->getProtocol()))
            {
                uint8_t clockPortSelection = getClockPortSelectionForHardwareIndex(i, hwIndex);
                if (isHardwarePortSelected(clockPortSelection))
                {
                    logInfoP("  │      Clock Port:     %d (ETS selection)", clockPortSelection);
                }
                else
                {
                    logInfoP("  │      Clock Port:     Auto (not selected in ETS)");
                }
            }
        }

        // Timing (bitrate): ETS value -> kHz (matches the ETS Timing dropdown)
        uint8_t timingSel = (uint8_t)ParamNEOSTRIP_NEOTiming & 0x0F;
        static const uint16_t timingFreqTbl[16] = {
            800, 960, 640, 680, 720, 760, 840, 880, 920, 750, 765, 770, 775, 780, 785, 790};
        logInfoP("  │    Timing:           value %d = %u kHz", timingSel, timingFreqTbl[timingSel]);

        // Skip First LEDs
        uint16_t skipLeds = (uint16_t)ParamNEOSTRIP_NEOSkipFirstLEDs;
        logInfoP("  │    Skip First LEDs:  %d", skipLeds);

        // Color Correction (Master + Sub-Checkboxen)
        bool colorCalibMaster = (bool)ParamNEOSTRIP_NEOColorCalibrationMaster;
        bool gammaSubCheckbox = (bool)ParamNEOSTRIP_NEOGammaCorrection;
        bool gammaEffectivelyEnabled = colorCalibMaster && gammaSubCheckbox;
        bool whiteBalEffectivelyEnabled = colorCalibMaster;
        logInfoP("  │    Farbkalibrierung: %s", colorCalibMaster ? "Yes" : "No");
        logInfoP("  │      Gamma Corr:    %s (Master:%s, Sub:%s)", gammaEffectivelyEnabled ? "ACTIVE" : "Off", colorCalibMaster ? "✓" : "✗", gammaSubCheckbox ? "✓" : "✗");
        logInfoP("  │      White Balance: %s", whiteBalEffectivelyEnabled ? "ACTIVE" : "Off");

        // Power Limiting
        uint8_t powerMode = (uint8_t)ParamNEOSTRIP_NEOpowerLimitCombined;
        const char* powerModeNames[] = {"Deaktiviert", "Global verwenden", "Fester Wert", "Pro LED"};
        const char* powerModeName = (powerMode < 4) ? powerModeNames[powerMode] : "Unknown";
        logInfoP("  │    Power Limit:      %d (%s)", powerMode, powerModeName);
        if (powerMode == 2)
        {
            uint16_t maxCurrent = (uint16_t)ParamNEOSTRIP_NEOcurrentPerChannel;
            logInfoP("  │      Max Current:    %d mA", maxCurrent);
        }
        else if (powerMode == 3)
        {
            uint8_t currentPerLed = (uint8_t)ParamNEOSTRIP_NEOcurrentPerLED;
            logInfoP("  │      Current/LED:    %d mA", currentPerLed);
        }

        logInfoP("  │");

        // === Actual Hardware (What's running) ===
        logInfoP("  │  ⚙️  Actual Hardware:");
        logInfoP("  │    LEDs:             %d", strip->getLedCount());
        logInfoP("  │    Protocol:         %s", getProtocolName(strip->getProtocol()));
        logInfoP("  │    Color Order:      %s", getColorOrderName(strip->getColorOrder()));
        logInfoP("  │    Data GPIO:        %d", strip->getDataPin());
        if (isSpiProtocol(strip->getProtocol()))
        {
            logInfoP("  │    Clock GPIO:       %d", strip->getClockPin());
        }

        logInfoP("  └──");
    }
    logInfoP("");

    // ========================================================================
    // Segment Configuration
    // ========================================================================
    logInfoP("▶ Segment Configuration:");
    logInfoP("────────────────────────────────────────────────────────────────────────");
    logInfoP("  Number of Segments: %d", _numberOfSegments);
    logInfoP("");

    for (size_t i = 0; i < _segments.size(); i++)
    {
        const auto& seg = _segments[i];
        _channelIndex = i; // Set for macro access

        logInfoP("  ┌─ Segment %d:", i + 1);
        logInfoP("  │  LED Range:     %d - %d (%d LEDs)",
                 seg.startLed, seg.endLed, seg.endLed - seg.startLed + 1);
        logInfoP("  │  Offset:        %d", seg.offset);
        logInfoP("  │  Grouping:      %d", seg.grouping);
        logInfoP("  │  Spacing:       %d", seg.spacing);
        logInfoP("  │  Reverse:       %s", seg.reverseDirection ? "Yes" : "No");
        logInfoP("  │  Mirror:        %s", seg.mirrorEffect ? "Yes" : "No");

        // Show CURRENT effect (from segment), not savedEffectType (from flash)
        uint8_t currentEffectType = seg.segment ? seg.segment->getConfig().effectType : 0;
        const char* effectName = seg.segment && seg.segment->getEffect() ? seg.segment->getEffect()->getName() : "None";
        logInfoP("  │  Effect:        %d (%s)", currentEffectType, effectName);

        // Start-Farbe (aus ETS Config)
        uint32_t startupColorRGB = (uint32_t)ParamNEO_NEOSegmentStartupColor;
        uint8_t startupR = (startupColorRGB >> 16) & 0xFF;
        uint8_t startupG = (startupColorRGB >> 8) & 0xFF;
        uint8_t startupB = startupColorRGB & 0xFF;
        uint8_t startupW = (uint8_t)ParamNEO_NEOSegmentStartupW;
        uint8_t startupBr = (uint8_t)ParamNEO_NEOSegmentStartupBrightness;
        logInfoP("  │  Start-Farbe: R=%d G=%d B=%d W=%d Br=%d",
                 startupR, startupG, startupB, startupW, startupBr);

        if (seg.savedValid)
        {
            logInfoP("  │  Saved State:   R=%d G=%d B=%d WW=%d CW=%d Br=%d",
                     seg.savedR, seg.savedG, seg.savedB, seg.savedWW, seg.savedCW, seg.savedBrightness);
        }

        // HCL Configuration
        uint8_t hclMode = (uint8_t)ParamNEO_NEOHCLMode;
        const char* hclModeNames[] = {"Disabled", "Global LightManager", "Segment LightManager"};
        const char* hclModeName = (hclMode < 3) ? hclModeNames[hclMode] : "Unknown";
        logInfoP("  │  HCL Mode:      %d (%s)", hclMode, hclModeName);

        if (i < _hclTransformContext.segmentConfigs.size())
        {
            const NeoHclSegmentConfig& segCfg = _hclTransformContext.segmentConfigs[i];
            const NeoHclConfig* effectiveConfig = nullptr;

            if (segCfg.hclMode == NeoHclMode::Global)
            {
                effectiveConfig = &_hclTransformContext.globalHclConfig;
                logInfoP("  │    Source:       Global LightManager");
            }
            else if (segCfg.hclMode == NeoHclMode::Custom)
            {
                effectiveConfig = &segCfg.customHclConfig;
                logInfoP("  │    Source:       Segment LightManager");
            }

            if (effectiveConfig)
            {
                logInfoP("  │    Master:       %u (resolved %u)",
                         effectiveConfig->lightManagerMaster,
                         effectiveConfig->resolvedMaster);
                logInfoP("  │    Kelvin:       %u - %u K",
                         effectiveConfig->minKelvin,
                         effectiveConfig->maxKelvin);

                if (effectiveConfig->resolvedMaster > 0)
                {
                    const NeoHclMasterState& state =
                        _hclTransformContext.masterStates[effectiveConfig->resolvedMaster - 1];
                    logInfoP("  │    Current:      %u K", state.kelvin);
                    logInfoP("  │    Blocked:      %s", state.blocked ? "Yes" : "No");
                }
            }
        }

        logInfoP("  └──");
    }
    logInfoP("");

    // ========================================================================
    // Color Correction
    // ========================================================================
    logInfoP("▶ Color Correction:");
    logInfoP("────────────────────────────────────────────────────────────────────────");
    logInfoP("  Gamma Correction:   %s", _gammaCorrectionEnabled ? "Enabled" : "Disabled");
    if (_gammaCorrectionEnabled)
    {
        logInfoP("  Gamma Value:        %.2f", _gammaValue);
    }
    logInfoP("  White Balance:      %s", _whiteBalanceEnabled ? "Enabled" : "Disabled");
    if (_whiteBalanceEnabled)
    {
        logInfoP("  WB Red:             %d", _whiteBalanceRed);
        logInfoP("  WB Green:           %d", _whiteBalanceGreen);
        logInfoP("  WB Blue:            %d", _whiteBalanceBlue);
    }
    logInfoP("  Global Brightness:  %d", _globalBrightness);
    logInfoP("  HCL Mode:           %s", _hclModeEnabled ? "Active" : "Inactive");
    if (_hclModeEnabled)
    {
        logInfoP("  HCL Temperature:    %d K", _hclAppliedKelvin);
    }
    logInfoP("");

    // ========================================================================
    // Effects Configuration
    // ========================================================================
    logInfoP("▶ Effects System:");
    logInfoP("────────────────────────────────────────────────────────────────────────");
    logInfoP("  Effects Enabled:    %s", _effectsEnabled ? "Yes" : "No");
    logInfoP("");

    logInfoP("════════════════════════════════════════════════════════════════════════");
}
#endif

// =============================================================================
// Effects Implementation
// =============================================================================

void NeoPixelBusModule::configureEffects()
{
    // Delegate to EffectConfiguration sub-module
    _effectConfiguration->configureEffects();
}

void NeoPixelBusModule::applyEffectToSegment(Segment* segment, uint8_t effectType)
{
    // Delegate to EffectConfiguration sub-module
    _effectConfiguration->applyEffectToSegment(segment, effectType);
}

void NeoPixelBusModule::setupEffectConfiguration(Segment* segment)
{
    // Delegate to EffectConfiguration sub-module
    _effectConfiguration->setupEffectConfiguration(segment);
}

// =============================================================================
// Power Monitoring Implementation
// =============================================================================

void NeoPixelBusModule::sendPowerMonitoringKOs()
{
    auto* mgr = _neoPixel.getManager();
    if (!mgr) return;

    bool isEnabled = mgr->getPowerManager()->isEnabled();

    // If power management is disabled, send 0 values to ensure KOs are readable
    if (!isEnabled)
    {
        if (KoNEO_PowerCurrentTotal.valueNoSendCompare((uint32_t)0, DPT_Value_2_Ucount))
            KoNEO_PowerCurrentTotal.objectWritten();
        if (KoNEO_PowerLoadTotal.valueNoSendCompare((uint8_t)0, DPT_Scaling))
            KoNEO_PowerLoadTotal.objectWritten();
        // logDebugP("Power management disabled - sent zero values for power KOs");
        return;
    }

    PowerLimitMode mode = mgr->getPowerManager()->getPowerLimitMode();

    // Check for ABL (Auto Brightness Limiting) active warning
    // Uses global power stats to detect when brightness is being limited
    {
        uint32_t ablCheckCurrent, ablCheckLimit;
        uint8_t ablCheckLoad;
        mgr->getGlobalPowerStats(ablCheckCurrent, ablCheckLimit, ablCheckLoad);
        if (ablCheckLoad >= 95 && _activeWarnCode == 0)
        {
            setWarningBlink(NEO_WARN_ABL_ACTIVE); // pulsing Orange (color from warnColorForCode)
        }
    }

    if (mode == PowerLimitMode::GLOBAL)
    {
        // GLOBAL MODE: Send global + per-strip KOs
        uint32_t currentMa, limitMa;
        uint8_t loadPercent;
        mgr->getGlobalPowerStats(currentMa, limitMa, loadPercent);

        // Global KOs
        if (KoNEO_PowerCurrentTotal.valueNoSendCompare(currentMa, DPT_Value_2_Ucount))
            KoNEO_PowerCurrentTotal.objectWritten();

        if (KoNEO_PowerLoadTotal.valueNoSendCompare(loadPercent, DPT_Scaling))
            KoNEO_PowerLoadTotal.objectWritten();

        // Calculate total power in watts (sum of all UseGlobal strips)
        float totalWatts = 0.0f;
        for (size_t i = 0; i < _physicalStrips.size(); i++)
        {
            auto* phys = _physicalStrips[i];
            if (!phys) continue;

            auto* cfg = phys->getConfig();
            if (!cfg || cfg->getPowerLimitMode() != 1) continue; // Only UseGlobal

            uint32_t stripCurrent = mgr->getPowerManager()->calculateStripCurrent(
                phys->getBuffer(), phys->getLedCount(),
                phys->getProtocol(), phys->getColorOrder(), true);

            totalWatts += phys->getPowerWatts(stripCurrent);
        }

        if (KoNEO_PowerWattsTotal.valueNoSendCompare(totalWatts, DPT_Value_Power))
        {
            // logDebugP("Updating KoNEO_PowerWattsTotal: %f W", totalWatts);
            KoNEO_PowerWattsTotal.objectWritten();
        }

        // Per-strip KOs (for ALL strips, regardless of power mode)
        for (size_t i = 0; i < _physicalStrips.size() && i < NEOPIXEL_MAX_PHYSICAL_STRIPS; i++)
        {
            auto* phys = _physicalStrips[i];
            if (!phys) continue;

            auto* cfg = phys->getConfig();
            if (!cfg) continue;

            // CRITICAL: Use cached values from applyPowerLimit() to avoid race condition
            // Direct buffer access is unsafe (show() thread may be writing simultaneously)
            uint32_t stripCurrent = 0;
            uint32_t stripLimit = 0;
            uint8_t stripLoad = 0;
            if (!mgr->getStripPowerStats(phys, stripCurrent, stripLimit, stripLoad))
            {
                // Fallback: calculate from buffer (should rarely happen)
                stripCurrent = mgr->getPowerManager()->calculateStripCurrent(
                    phys->getBuffer(), phys->getLedCount(),
                    phys->getProtocol(), phys->getColorOrder(), true);

                // Calculate load percentage (if limit configured)
                uint16_t limit = cfg->getMaxCurrentMa();
                if (limit > 0)
                    stripLoad = (uint8_t)((stripCurrent * 100) / limit);
            }

            float stripWatts = phys->getPowerWatts(stripCurrent);

            // Set channel index to access correct strip KOs
            uint8_t savedChannelIndex = _channelIndex;
            _channelIndex = i;

            if (KoNEOSTRIP_PowerCurrent.valueNoSendCompare(stripCurrent, DPT_Value_2_Ucount))
            {
                // logDebugP("Updating KoNEOSTRIP_PowerCurrent for strip %u: %u mA", (unsigned)i, stripCurrent);
                KoNEOSTRIP_PowerCurrent.objectWritten();
            }

            if (KoNEOSTRIP_PowerLoad.valueNoSendCompare(stripLoad, DPT_Scaling))
            {
                // logDebugP("Updating KoNEOSTRIP_PowerLoad for strip %u: %u %%", (unsigned)i, stripLoad);
                KoNEOSTRIP_PowerLoad.objectWritten();
            }

            if (KoNEOSTRIP_PowerWatts.valueNoSendCompare(stripWatts, DPT_Value_Power))
            {
                // logDebugP("Updating KoNEOSTRIP_PowerWatts for strip %u: %f W", (unsigned)i, stripWatts);
                KoNEOSTRIP_PowerWatts.objectWritten();
            }

            _channelIndex = savedChannelIndex;
        }
    }
    else if (mode == PowerLimitMode::PER_CHANNEL)
    {
        // PER_CHANNEL MODE: Send global + per-strip KOs
        uint32_t currentMa, limitMa;
        uint8_t loadPercent;

        // Global KOs (sum of all strips)
        mgr->getGlobalPowerStats(currentMa, limitMa, loadPercent);

        if (KoNEO_PowerCurrentTotal.valueNoSendCompare(currentMa, DPT_Value_2_Ucount))
        {
            // logDebugP("Updating KoNEO_PowerCurrentTotal: %u mA", currentMa);
            KoNEO_PowerCurrentTotal.objectWritten();
        }
        if (KoNEO_PowerLoadTotal.valueNoSendCompare(loadPercent, DPT_Scaling))
        {
            // logDebugP("Updating KoNEO_PowerLoadTotal: %u %%", loadPercent);
            KoNEO_PowerLoadTotal.objectWritten();
        }

        // Calculate total power in watts (use cached values from applyPowerLimit)
        float totalWatts = 0.0f;
        for (size_t i = 0; i < _physicalStrips.size(); i++)
        {
            auto* phys = _physicalStrips[i];
            if (!phys) continue;

            // Use cached current from applyPowerLimit (requested current BEFORE ABL scaling)
            uint32_t stripCurrent = 0;
            if (mgr->getStripPowerStats(phys, stripCurrent, limitMa, loadPercent))
            {
                // Got cached value - requested current before ABL scaling
                totalWatts += phys->getPowerWatts(stripCurrent);
            }
            else
            {
                // Fallback: calculate from buffer (shouldn't happen in normal operation)
                stripCurrent = mgr->getPowerManager()->calculateStripCurrent(
                    phys->getBuffer(), phys->getLedCount(),
                    phys->getProtocol(), phys->getColorOrder(), true);
                totalWatts += phys->getPowerWatts(stripCurrent);
            }
        }

        if (KoNEO_PowerWattsTotal.valueNoSendCompare(totalWatts, DPT_Value_Power))
        {
            // logDebugP("Updating KoNEO_PowerWattsTotal: %f W", totalWatts);
            KoNEO_PowerWattsTotal.objectWritten();
        }

        // Per-strip KOs (for ALL strips)
        for (size_t i = 0; i < _physicalStrips.size() && i < NEOPIXEL_MAX_PHYSICAL_STRIPS; i++)
        {
            auto* phys = _physicalStrips[i];
            if (!phys) continue;

            auto* cfg = phys->getConfig();
            if (!cfg) continue;

            // Use cached values from applyPowerLimit (requested current BEFORE ABL scaling)
            uint32_t stripCurrent = 0;
            if (!mgr->getStripPowerStats(phys, currentMa, limitMa, loadPercent))
            {
                // Fallback: calculate from buffer if cache miss
                stripCurrent = mgr->getPowerManager()->calculateStripCurrent(
                    phys->getBuffer(), phys->getLedCount(),
                    phys->getProtocol(), phys->getColorOrder(), true);

                // Fallback load calculation
                uint16_t stripLimit = cfg->getMaxCurrentMa();
                if (stripLimit > 0)
                    loadPercent = (uint8_t)((stripCurrent * 100) / stripLimit);
                else
                    loadPercent = 0;
            }
            else
            {
                // Use cached current (requested BEFORE ABL scaling)
                stripCurrent = currentMa;
            }

            float stripWatts = phys->getPowerWatts(stripCurrent);

            // Set channel index to access correct strip KOs
            uint8_t savedChannelIndex = _channelIndex;
            _channelIndex = i;

            if (KoNEOSTRIP_PowerCurrent.valueNoSendCompare(stripCurrent, DPT_Value_2_Ucount))
            {
                // logDebugP("Updating KoNEOSTRIP_PowerCurrent for strip %u: %u mA", (unsigned)i, stripCurrent);
                KoNEOSTRIP_PowerCurrent.objectWritten();
            }

            if (KoNEOSTRIP_PowerLoad.valueNoSendCompare(loadPercent, DPT_Scaling))
            {
                // logDebugP("Updating KoNEOSTRIP_PowerLoad for strip %u: %u %%", (unsigned)i, loadPercent);
                KoNEOSTRIP_PowerLoad.objectWritten();
            }

            if (KoNEOSTRIP_PowerWatts.valueNoSendCompare(stripWatts, DPT_Value_Power))
            {
                // logDebugP("Updating KoNEOSTRIP_PowerWatts for strip %u: %f W", (unsigned)i, stripWatts);
                KoNEOSTRIP_PowerWatts.objectWritten();
            }

            _channelIndex = savedChannelIndex;
        }
    }
    else if (mode == PowerLimitMode::PER_LED)
    {
        // PER_LED MODE: Send global + per-strip KOs (same as PER_CHANNEL)
        uint32_t currentMa, limitMa;
        uint8_t loadPercent;

        // Global KOs (sum of all strips)
        mgr->getGlobalPowerStats(currentMa, limitMa, loadPercent);

        if (KoNEO_PowerCurrentTotal.valueNoSendCompare(currentMa, DPT_Value_2_Ucount))
        {
            // logDebugP("Updating KoNEO_PowerCurrentTotal: %u mA", currentMa);
            KoNEO_PowerCurrentTotal.objectWritten();
        }

        if (KoNEO_PowerLoadTotal.valueNoSendCompare(loadPercent, DPT_Scaling))
        {
            // logDebugP("Updating KoNEO_PowerLoadTotal: %u %%", loadPercent);
            KoNEO_PowerLoadTotal.objectWritten();
        }

        // Calculate total power in watts (use cached values from applyPowerLimit)
        float totalWatts = 0.0f;
        for (size_t i = 0; i < _physicalStrips.size(); i++)
        {
            auto* phys = _physicalStrips[i];
            if (!phys) continue;

            // Use cached current from applyPowerLimit (requested current BEFORE ABL scaling)
            uint32_t stripCurrent = 0;
            if (mgr->getStripPowerStats(phys, stripCurrent, limitMa, loadPercent))
            {
                // Got cached value - requested current before ABL scaling
                totalWatts += phys->getPowerWatts(stripCurrent);
            }
            else
            {
                // Fallback: calculate from buffer (shouldn't happen in normal operation)
                stripCurrent = mgr->getPowerManager()->calculateStripCurrent(
                    phys->getBuffer(), phys->getLedCount(),
                    phys->getProtocol(), phys->getColorOrder(), true);
                totalWatts += phys->getPowerWatts(stripCurrent);
            }
        }

        if (KoNEO_PowerWattsTotal.valueNoSendCompare(totalWatts, DPT_Value_Power))
        {
            // logDebugP("Updating KoNEO_PowerWattsTotal: %f W", totalWatts);
            KoNEO_PowerWattsTotal.objectWritten();
        }

        // Per-strip KOs (for ALL strips)
        for (size_t i = 0; i < _physicalStrips.size() && i < NEOPIXEL_MAX_PHYSICAL_STRIPS; i++)
        {
            auto* phys = _physicalStrips[i];
            if (!phys) continue;

            auto* cfg = phys->getConfig();
            if (!cfg) continue;

            // Use cached values from applyPowerLimit (requested current BEFORE ABL scaling)
            uint32_t stripCurrent = 0;
            if (!mgr->getStripPowerStats(phys, currentMa, limitMa, loadPercent))
            {
                // Fallback: calculate from buffer if cache miss
                stripCurrent = mgr->getPowerManager()->calculateStripCurrent(
                    phys->getBuffer(), phys->getLedCount(),
                    phys->getProtocol(), phys->getColorOrder(), true);

                // Fallback load calculation
                uint16_t stripLimit = cfg->getMaxCurrentMa();
                if (stripLimit > 0)
                    loadPercent = (uint8_t)((stripCurrent * 100) / stripLimit);
                else
                    loadPercent = 0;
            }
            else
            {
                // Use cached current (requested BEFORE ABL scaling)
                stripCurrent = currentMa;
            }

            float stripWatts = phys->getPowerWatts(stripCurrent);

            // Set channel index to access correct strip KOs
            uint8_t savedChannelIndex = _channelIndex;
            _channelIndex = i;

            if (KoNEOSTRIP_PowerCurrent.valueNoSendCompare(stripCurrent, DPT_Value_2_Ucount))
            {
                // logDebugP("Updating KoNEOSTRIP_PowerCurrent for strip %u: %u mA", (unsigned)i, stripCurrent);
                KoNEOSTRIP_PowerCurrent.objectWritten();
            }

            if (KoNEOSTRIP_PowerLoad.valueNoSendCompare(loadPercent, DPT_Scaling))
            {
                // logDebugP("Updating KoNEOSTRIP_PowerLoad for strip %u: %u %%", (unsigned)i, loadPercent);
                KoNEOSTRIP_PowerLoad.objectWritten();
            }

            if (KoNEOSTRIP_PowerWatts.valueNoSendCompare(stripWatts, DPT_Value_Power))
            {
                // logDebugP("Updating KoNEOSTRIP_PowerWatts for strip %u: %f W", (unsigned)i, stripWatts);
                KoNEOSTRIP_PowerWatts.objectWritten();
            }

            _channelIndex = savedChannelIndex;
        }
    }
}

// =============================================================================
// Blink Code Helpers
// =============================================================================

// Single source of truth: blink code -> human-readable console text.
static const char* errorMsgForCode(uint8_t code)
{
    switch (code)
    {
        case NEO_ERROR_HW_MISMATCH: return "Hardware mismatch (firmware vs ETS) - GPIO mappings ignored";
        case NEO_ERROR_NO_HW_CONFIGURED: return "No hardware selected in ETS (dummy 255)";
        case NEO_ERROR_GPIO_CONFLICT: return "GPIO pin conflict - duplicate pin assignment";
        case NEO_ERROR_STRIP_FAILED: return "Physical strip creation failed";
        case NEO_ERROR_VSTRIP_FAILED: return "Virtual strip creation failed - no LED output";
        default: return "unknown error";
    }
}

static const char* warnMsgForCode(uint8_t code)
{
    switch (code)
    {
        case NEO_WARN_ABL_ACTIVE: return "Auto brightness limiting active (power >= 95% of limit)";
        case NEO_WARN_FLASH_DISCARDED: return "Saved state discarded (config changed) - using defaults";
        case NEO_WARN_UNKNOWN_EFFECT: return "Unknown effect type configured in ETS";
        default: return "unknown warning";
    }
}

void NeoPixelBusModule::setErrorBlink(uint8_t code)
{
    // Only show the most severe (lowest code number) error
    if (_activeErrorCode != 0 && _activeErrorCode <= code) return;
    _activeErrorCode = code;
    // Always log to console first — independent of whether a STATUS LED is mapped on this hardware.
    logErrorP("NeoPixel error: %s (STATUS LED red, %d× blink)", errorMsgForCode(code), code);
    auto* led = openknx.ledFunctions.get(OPENKNX_LEDFUNC_BASE_STATE);
    if (led)
    {
        led->color(OpenKNX::Led::Color::Red);
        led->errorCode(code); // Prio 2 — overrides any active pulsing (Prio 5)
    }
}

OpenKNX::Led::Color NeoPixelBusModule::warnColorForCode(uint8_t code)
{
    // Single source of truth: warning code -> STATUS LED color.
    switch (code)
    {
        case NEO_WARN_ABL_ACTIVE: return OpenKNX::Led::Color::Orange;
        case NEO_WARN_FLASH_DISCARDED: return OpenKNX::Led::Color::Purple;
        case NEO_WARN_UNKNOWN_EFFECT: return OpenKNX::Led::Color::Yellow;
        default: return OpenKNX::Led::Color::Yellow;
    }
}

void NeoPixelBusModule::setWarningBlink(uint8_t code)
{
    OpenKNX::Led::Color color = warnColorForCode(code);
    // Don't touch STATUS LED if an error is already showing (errorCode Prio 2 would win,
    // but the color() call would wrongly change the error's color)
    if (_activeErrorCode != 0) return;
    // Only show the most severe (lowest code number) warning
    if (_activeWarnCode != 0 && _activeWarnCode <= code) return;
    _activeWarnCode = code;
    // Always log to console first — independent of whether a STATUS LED is mapped on this hardware.
    logWarningP("NeoPixel warning: %s (STATUS LED pulsing)", warnMsgForCode(code));
    auto* led = openknx.ledFunctions.get(OPENKNX_LEDFUNC_BASE_STATE);
    if (led)
    {
        led->color(color);
        led->pulsing(); // Prio 5 — smooth breathing to indicate warning
    }
}

void NeoPixelBusModule::clearBlinkCodes()
{
    _activeErrorCode = 0;
    _activeWarnCode = 0;
    auto* led = openknx.ledFunctions.get(OPENKNX_LEDFUNC_BASE_STATE);
    if (led)
    {
        led->errorCode(0);
        led->off();
    }
}

// ============================================================================
// Effektmanager
// ============================================================================

void NeoPixelBusModule::loopEffektManager()
{
    if (!_emData) return; // EM table not allocated (out of memory) -> EM disabled
    for (size_t i = 0; i < _segments.size(); i++)
    {
        auto& cfg = _segments[i];
        if (!cfg.segment) continue;

        // Detect an auto-finish (non-looping cue chain ends inside tick()). OFM's internal finish
        // does LAST (restoreDirectState); override here for OFF/DEFAULT, which OFM cannot resolve
        // (no ETS access). Chaining EM->EM keeps isRunning() true, so only a real chain-end flips
        // the edge — manual stops go through stopEffektManager(), not this path.
        bool wasRunning = cfg.emController.isRunning();
        cfg.emController.tick(cfg.segment, _emData);
        if (wasRunning && !cfg.emController.isRunning())
        {
            if (cfg.emStopReturnMode == EM_STOP_OFF)
                cfg.segment->stop();
            else if (cfg.emStopReturnMode == EM_STOP_DEFAULT)
                applySegmentDefaultState(i);
        }

        // Re-apply any long console cue-text after a cue switch (no-op unless one is set).
        applyCueLongText(cfg);

        // Keep EM status KOs in sync with cue changes and chaining.
        // valueNoSendCompare in sendEmStatusKOs avoids bus traffic on unchanged values.
        sendEmStatusKOs(i);
    }
}

void NeoPixelBusModule::applyCueLongText(SegmentConfig& cfg)
{
    // Fast path: nothing configured → zero overhead in the normal (ETS) case.
    if (_cueLongText.empty()) { cfg.lastCueTextKey = 0xFFFF; return; }

    const uint8_t em  = cfg.emController.activeEmId();
    const uint8_t cue = cfg.emController.activeCueNum();
    const uint16_t key = (em && cue) ? (((uint16_t)em << 8) | cue) : 0;
    if (key == cfg.lastCueTextKey) return; // same cue as last tick → already applied
    cfg.lastCueTextKey = key;
    if (key == 0) return;

    auto it = _cueLongText.find(key);
    if (it == _cueLongText.end()) return; // this cue has no override → keep its 13-char cue text

    Segment* s = cfg.segment;
    Effect* e = s ? s->getEffect() : nullptr;
    if (!e) return;
    for (uint8_t i = 0; i < e->getParameterCount(); ++i)
        if (e->getParameterType(i) == ParameterType::PARAM_STRING)
        {
            e->setParameter(s, i, (uint32_t)(uintptr_t)it->second.c_str());
            break;
        }
}

void NeoPixelBusModule::startEffektManager(size_t segmentIndex, uint8_t emId)
{
    if (segmentIndex >= _segments.size())
    {
        logWarningP("Segment %d: EM %d start ignored (segment out of range)",
                    (int)segmentIndex + 1, (int)emId);
        return;
    }
    auto& cfg = _segments[segmentIndex];
    if (!cfg.segment)
    {
        logWarningP("Segment %d: EM %d start ignored (no segment instance)",
                    (int)segmentIndex + 1, (int)emId);
        return;
    }

    if (emId == EM_NONE)
    {
        stopEffektManager(segmentIndex);
        return;
    }

    if (emId > EM_COUNT)
    {
        logWarningP("Segment %d: EM %d start ignored (valid range 1..%d)",
                    (int)segmentIndex + 1, (int)emId, (int)EM_COUNT);
        return;
    }

    if (!_emData) return; // EM table not allocated (out of memory) -> EM disabled
    const auto& header = _emData[emId - 1].header;
    if (header.isPaused())
    {
        logInfoP("Segment %d: EM %d suspendiert -> Start ignoriert (Konfig/KOs bleiben erhalten)",
                 (int)segmentIndex + 1, (int)emId);
    }
    else if (!header.enabled || header.cueCount == 0)
    {
        logWarningP("Segment %d: EM %d nicht startfaehig (state=%d [0=Deaktiviert,1=Aktiv,2=Suspendiert], cueCount=%d)",
                    (int)segmentIndex + 1,
                    (int)emId,
                    (int)header.enabled,
                    (int)header.cueCount);
    }

    // A normal effect KO interrupts a running EM immediately
    cfg.emController.start(emId, cfg.segment, _emData);
    sendEmStatusKOs(segmentIndex);

    // Suspendiert (paused) ignoriert Start bewusst -> kein Fehler-Log (oben bereits als Info vermerkt)
    if (!header.isPaused() && cfg.emController.activeEmId() != emId)
    {
        logWarningP("Segment %d: EM %d start fehlgeschlagen (state=%d [0=Deaktiviert,1=Aktiv,2=Suspendiert], cueCount=%d, next=%d, loop=%d)",
                    (int)segmentIndex + 1,
                    (int)emId,
                    (int)header.enabled,
                    (int)header.cueCount,
                    (int)header.nextEmId,
                    header.loop ? 1 : 0);
    }
}

void NeoPixelBusModule::stopEffektManager(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (!cfg.segment) return;

    // Honor the per-segment "EM-Stop return" mode (0=last, 1=default, 2=off).
    // OFM handles LAST/OFF directly; DEFAULT falls back to LAST inside OFM, so the
    // OAM applies the ETS default config explicitly here (OFM cannot read ETS).
    uint8_t mode = cfg.emStopReturnMode;
    // Capture BEFORE stop(): an EM-Stop on an idle segment (no EM running) must NOT reset the
    // segment — otherwise it would clobber a manual color/effect/brightness the user just set.
    // Only a genuine running EM triggers the DEFAULT re-apply (mirrors the loopEffektManager edge).
    bool wasRunning = cfg.emController.isRunning();
    // isRunning() is also true for a PAUSED EM (e.g. power-suspended); capture that so a stop
    // landing on a powered-off segment does not paint the ETS default onto an off segment.
    bool wasPowerSuspended = cfg.suspendedByPower;
    cfg.emController.stop(cfg.segment, mode);
    // An explicit stop clears the power-suspend marker: the EM state is now manually controlled,
    // so a later power-on must not "resume" it (the shared flag would otherwise go stale-true).
    cfg.suspendedByPower = false;
    if (wasRunning && !wasPowerSuspended && mode == EM_STOP_DEFAULT)
        applySegmentDefaultState(segmentIndex);
    sendEmStatusKOs(segmentIndex);
}

void NeoPixelBusModule::pauseEffektManager(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (!cfg.segment) return;

    cfg.emController.pause(cfg.segment);
    sendEmStatusKOs(segmentIndex);
}

void NeoPixelBusModule::resumeEffektManager(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (!cfg.segment) return;

    cfg.emController.resume(cfg.segment);
    sendEmStatusKOs(segmentIndex);
}

void NeoPixelBusModule::interruptEmIfRunning(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (!cfg.segment || !cfg.emController.isRunning()) return;
    // Halt the EM without restoring the pre-EM state — the triggering direct KO sets the new visual.
    cfg.emController.stop(cfg.segment, EM_STOP_LEAVE);
    sendEmStatusKOs(segmentIndex);
}

void NeoPixelBusModule::applySegmentDefaultState(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (!cfg.segment) return;

    // "Segment-Standardwerte": re-apply the boot-time ETS default for this segment.
    // Set the channel context so the ParamNEO_ macros resolve for this segment, then restore it.
    uint8_t prevChannel = getChannelIndex();
    setChannelIndex(static_cast<uint8_t>(segmentIndex));

    // Base effect + its effect-specific parameters (mirror of configureEffects()).
    uint8_t effectType = static_cast<uint8_t>(ParamNEO_NEONEOEffectType);
    applyEffectToSegment(cfg.segment, effectType);
    setupEffectConfiguration(cfg.segment);

    // Default color/white (TypeColor RGB = 0x00RRGGBB) — the member wrapper above does NOT
    // load the default color, so apply it explicitly like setupEffectConfiguration(seg, true).
    uint32_t color = ParamNEO_NEOSegmentStartupColor;
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    uint8_t w = static_cast<uint8_t>(ParamNEO_NEOSegmentStartupW);
    cfg.segment->setPrimaryColor(r, g, b, w);

    // Start brightness seeds the Segment (master) layer of the cascade — a full reset to ETS
    // values, so any live dim is intentionally cleared in this mode.
    uint8_t startupBrightness = static_cast<uint8_t>(ParamNEO_NEOSegmentStartupBrightness);
    cfg.segment->setMasterBrightness(startupBrightness);
    cfg.segment->setBrightness(startupBrightness);
    // Keep the pre-global baseline in sync (mirror the boot seed at EffectConfiguration.cpp:51) so a
    // later global-brightness recompute / power-restore does not revert to a stale dimmed value.
    cfg.savedBrightness = startupBrightness;

    cfg.savedEffectType = effectType;
    cfg.savedEffectValid = true;

    setChannelIndex(prevChannel);
}

void NeoPixelBusModule::sendEmStatusKOs(size_t segmentIndex)
{
    // Console bring-up ('neo init') has no ETS KO table — writing KOs would fault. Covers
    // every caller (loopEffektManager/start/stop/triggerCue). _localTestMode is false on a
    // normal ETS-configured boot, so production is unaffected.
    if (_localTestMode) return;
#ifdef NEO_KoEmStatus
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];

    uint8_t oldCh = _channelIndex;
    _channelIndex = (uint8_t)segmentIndex;

    bool changed1 = KoNEO_EmStatus.valueNoSendCompare(cfg.emController.activeEmId(), DPT_Value_1_Ucount);
    if (changed1) KoNEO_EmStatus.objectWritten();

    bool changed2 = KoNEO_EmCueStatus.valueNoSendCompare(cfg.emController.activeCueNum(), DPT_Value_1_Ucount);
    if (changed2) KoNEO_EmCueStatus.objectWritten();

    #ifdef NEO_KoEmRunState
    // 0 = gestoppt, 1 = läuft, 2 = pausiert
    bool changed3 = KoNEO_EmRunState.valueNoSendCompare(cfg.emController.runState(), DPT_Value_1_Ucount);
    if (changed3) KoNEO_EmRunState.objectWritten();
    #endif

    _channelIndex = oldCh;
#endif
}

void NeoPixelBusModule::sendEffectTextStatusKO(size_t segmentIndex)
{
#ifdef NEO_KoEffectTextStatus
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (!cfg.segment) return;

    uint8_t oldCh = _channelIndex;
    _channelIndex = (uint8_t)segmentIndex;

    KoNEO_EffectTextStatus.value(cfg.segment->getConfig().effectText, DPT_String_8859_1);

    _channelIndex = oldCh;
#endif
}
