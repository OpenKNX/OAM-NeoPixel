#include "NeoPixelModule.h"
#include "NeoPixelStudio.h"
#include "EffectManagerLifecycle.h"
#include "ColorManagement.h"
#include "EffectConfiguration.h"
#include "HardwareMappingLogic.h"
#include "NeoPixelTimingSelection.h"
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
#include "SerialTimingProfile.h"
#include "colorhelper.h"
#include "NeoPixelWeb.h"
#include "knxprod.h"
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <new>
#include <string>
#include <vector>
#ifdef ARDUINO_ARCH_ESP32
    #include <esp_system.h>
#endif
// #define NEOPIXEL_MODULE_TEST_ENV
#ifdef NEOPIXEL_MODULE_TEST_ENV
    #include "test/test.h"
#endif

static uint8_t resolveHclMaster(uint8_t requestedMaster, uint8_t availableMasterCount)
{
    if (requestedMaster == 0 || requestedMaster > availableMasterCount)
    {
        return 0;
    }

    return requestedMaster;
}

NeoPixelBusModule openknxNeoPixelModule;

#ifdef ARDUINO_ARCH_ESP32
static void logEsp32ResetReason()
{
    const esp_reset_reason_t reason = esp_reset_reason();
    const char* name = "unknown/other";
    switch (reason)
    {
        case ESP_RST_POWERON: name = "power-on"; break;
        case ESP_RST_EXT: name = "external reset"; break;
        case ESP_RST_SW: name = "software restart"; break;
        case ESP_RST_PANIC: name = "panic/exception"; break;
        case ESP_RST_INT_WDT: name = "interrupt watchdog"; break;
        case ESP_RST_TASK_WDT: name = "task watchdog"; break;
        case ESP_RST_WDT: name = "other watchdog"; break;
        case ESP_RST_DEEPSLEEP: name = "deep-sleep wake"; break;
        case ESP_RST_BROWNOUT: name = "brownout"; break;
        case ESP_RST_SDIO: name = "SDIO reset"; break;
        default: break;
    }
    logInfo("NeoPixel", "ESP32 reset reason: %d (%s), uptime: %lu ms",
            static_cast<int>(reason), name, static_cast<unsigned long>(millis()));
}
#endif

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
#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    delete _studio;
#endif
    delete _flashPersistence;
    delete _effectConfiguration;
    delete _colorManagement;
    delete _segmentController;
    delete _sceneManager;
    delete[] _emData;
}

void NeoPixelBusModule::setup(bool configured)
{
#if defined(NEOPIXEL_WEB) && !defined(NEOPIXEL_MODULE_TEST_ENV)
    const uint32_t setupStartMs = millis(); // Common starts the startup delay before the module setups
#endif
#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    if (!_studio) _studio = new (std::nothrow) NeoPixelStudio(*this);
    if (_studio) _studio->setup(configured);
#endif
#ifdef ARDUINO_ARCH_ESP32
    logEsp32ResetReason();
#endif
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
    _turnOffBeforeRestart = true; // no ETS parameter in the test env; keep the shutdown hooks live
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

        // paramByte() returns 0 once the tables are unloaded, so the hooks cannot read it.
        _turnOffBeforeRestart = ParamNEO_NEOTurnOffBeforeRestart;
        // Initialize all physical strips (PIO, DMA, etc.) - MUST be after configureFromETS()
        // which creates the PhysicalStrip objects, but BEFORE any show() calls
        _neoPixel.setup(configured);

        // Setup HCL pixel transformation context after strips and segments are available.
        updateHclTransformContext(); // Setup HCL pixel transformation callback

        // Now that hardware is initialized, clear LEDs if requested during configuration
        if (_clearLedsAfterSetup && _virtualStrip)
        {
            showVirtualStripIfIdle();
            logInfoP("LEDs cleared - will restore saved state after startup delay");
            _clearLedsAfterSetup = false;
        }

        // Initialize HCL status KOs from the current LightManager snapshot.
        refreshHclMasterStateCache();
        // Power monitoring KOs go out from loop(), after the startup delay
    }
    else
    {
        logInfoP("NeoPixelBusModule setup skipped - not configured");
        // Initialize GPIOs early to prevent floating states and send LED RESET signals
        // NOTE: This only sets pins LOW - LEDs retain their color until proper protocol
        //       data is sent (happens in configureFromETS() after strip creation)
        initializeGpioPins();
    }
    #ifdef NEOPIXEL_WEB
    // Routes also when unconfigured (the page then shows a hint); the webserver starts later from the network loop
    neoPixelWeb.setup(configured, setupStartMs);
    #endif
#endif
}

void NeoPixelBusModule::loop(bool configured)
{
    // Ahead of the 'configured' gate: during an ETS download this is the only path still
    // running. Skip rendering meanwhile, or a fresh frame re-lights the strips.
    if (_ledShutdownActive)
    {
        pumpLedShutdown();
        return;
    }

    // _localTestMode ('neo init') lets the loop run without an ETS download for bench tests.
    if ((!configured && !_localTestMode) || !_initialized) return;

#ifdef NEOPIXEL_WEB
    if (!_localTestMode) neoPixelWeb.loop(); // status page: snapshot and queued commands, ahead of the startup gate
#endif

    // Ahead of the off-refresh branch, which returns before the render path: a Power-OFF
    // that arrived mid-frame blanked only the virtual buffer.
    if (_virtualShowPending) showVirtualStripIfIdle();

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
                const char* compiledHwName = hardwareNameForId(DEVICE_HW_ID);
                logErrorP("*** No hardware configured! Use 'Hardware auto-detect' button in ETS or manually select: %s (ID: 0x%04X)",
                          compiledHwName ? compiledHwName : "Unknown", DEVICE_HW_ID);
            }
            else
            {
                // Hardware configured but mismatched - compare the stable DEVICE_HW_IDs directly
                const char* compiledHwName = hardwareNameForId(DEVICE_HW_ID);
                const char* etsHwName = hardwareNameForId(selectedHwId);
                logErrorP("!!! HW MISMATCH: Firmware=%s (ID 0x%04X) vs ETS=%s (ID 0x%04X) - Update ETS config!",
                          compiledHwName ? compiledHwName : "Unknown", DEVICE_HW_ID,
                          etsHwName ? etsHwName : "other hardware", selectedHwId);
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

        // KO-learned scenes live in RAM until a flash save: a regular save as soon as the write
        // protection of OGM-Common allows it (same condition as Flash::Default::save), none if
        // another save (cyclic, save object, ...) has run since the learn
        if (_sceneSavePending)
        {
            const uint32_t lastWrite = openknx.flash.lastWrite();
            if (lastWrite != _sceneLearnLastWrite)
            {
                _sceneSavePending = false;
            }
            else if (lastWrite == 0 || delayCheck(lastWrite, FLASH_DATA_WRITE_LIMIT))
            {
                _sceneSavePending = false;
                openknx.flash.save();
            }
        }
    }

    // Do not render the ETS startup defaults before flash persistence has restored the
    // selected "last state" brightness/effect in processAfterStartupDelay(). Otherwise
    // those defaults are transmitted for the startup-delay window and the strip can
    // visibly start at full brightness before changing to its saved brightness.
    // Local console test mode remains usable without a configured KNX application.
    if (!_localTestMode && !openknx.afterStartupDelay()) return;

    // Telegrams of the startup delay, a few per pass (non-blocking). While the backlog is processed
    // (bounded number of passes) no frame, status or sync telegram goes out, so no intermediate state
    // becomes visible; telegrams arriving later stay queued behind it to keep their order.
    if (_deferredKoHead < _deferredKoCount)
    {
        replayDeferredKos(kDeferredKoPerLoop);
        if (_drainGate && _drainPassesLeft > 0) --_drainPassesLeft;
        if (_deferredKoHead >= _deferredKoCount || _drainPassesLeft == 0) _drainGate = false;
        if (_drainGate) return;
    }
    else
    {
        _drainGate = false;
    }

#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    if (_studio) _studio->loop();
#endif

    // Effektkette: slave watchdog + master change detection / heartbeat (after the restore)
    if (!_localTestMode)
    {
        loopSyncWatchdog();
        loopSyncMaster();
    }

    // Effektmanager: tick all active sequencers (no-op in local test mode — EMs are ETS-only)
    loopEffektManager();

    // Paced status telegrams (startup send, bulk changes); also while global power is off
    if (!_localTestMode) processStatusQueue();

    // DPT 3.007 start/stop dimming; also for locked segments that stay lit under global OFF
    // (global OFF cancels the brightness ramp of the segments it suspends)
    processActiveDimming();

    // Global OFF suspends every unlocked segment. With no segment left visible, skip rendering;
    // the data line would then stay silent, which makes some clone chips start their own demo
    // pattern, so give the engine a tick to re-send on the strips configured for it.
    if (!_globalPowerOn && !anySegmentVisible())
    {
        _neoPixel.loopOffRefresh();
        _renderPaused = true;
        clearAblWarning(); // nothing is rendered, so nothing is limited
        if (!_localTestMode && _statusSeeded) loopPowerMonitoring(true); // reports 0 while dark
        return;
    }

    // Rendering resumes after the startup delay or a dark period: restart the frame clock, otherwise
    // the first frame gets the whole dark time as delta and effects that catch up in a loop (Game of
    // Life, Snake, Matrix …) iterate for every missed interval in one pass.
    if (_renderPaused)
    {
        _renderPaused = false;
        if (_neoPixel.getAutoUpdate()) _neoPixel.setAutoUpdate(true);
    }

    // Call library loop() for auto-update timer and effect processing
    _neoPixel.loop(configured);

    // Power monitoring KOs (KO writes — skip without ETS config); first values after the startup
    // restore, changes below the thresholds of sendPowerKo() are not sent
    if (!_localTestMode && _statusSeeded) loopPowerMonitoring(false);
}

void NeoPixelBusModule::loopPowerMonitoring(bool dark)
{
    unsigned long now = millis();
    if (now - _lastPowerMonitoringMs >= _powerMonitoringIntervalMs)
    {
        _lastPowerMonitoringMs = now;
        sendPowerMonitoringKOs(dark);
    }
    drainPowerKos();
}

void NeoPixelBusModule::showVirtualStripIfIdle()
{
    if (!_virtualStrip) return;
    if (_drainGate)
    {
        _virtualShowPending = true; // startup backlog in progress: shown once it is processed
        return;
    }

    NeoPixelManager* manager = _neoPixel.getManager();

    // Phase alone is not enough: an abandoned frame leaves a strip transmitting, and
    // loopOffRefresh() starts one outside the machine. syncToPhysical() would skip those.
    if (manager && (manager->isFrameInFlight() || manager->isAnyBusy()))
    {
        _virtualShowPending = true; // deferred, never dropped - loop() retries
        return;
    }

    _virtualShowPending = false;
    _virtualStrip->show();
}

uint16_t NeoPixelBusModule::relayMinOffSeconds(uint8_t relayIndex) const
{
    switch (relayIndex)
    {
#if defined(ParamNEO_NEOExternalRelay1MinOffTime)
        case 0: return (uint16_t)ParamNEO_NEOExternalRelay1MinOffTime;
#endif
#if defined(ParamNEO_NEOExternalRelay2MinOffTime)
        case 1: return (uint16_t)ParamNEO_NEOExternalRelay2MinOffTime;
#endif
#if defined(ParamNEO_NEOExternalRelay3MinOffTime)
        case 2: return (uint16_t)ParamNEO_NEOExternalRelay3MinOffTime;
#endif
#if defined(ParamNEO_NEOExternalRelay4MinOffTime)
        case 3: return (uint16_t)ParamNEO_NEOExternalRelay4MinOffTime;
#endif
        default: return 0;
    }
}

bool NeoPixelBusModule::relayIntendedState(uint8_t relayIndex) const
{
    if (relayIndex >= kRelayStorageSize) return false;
    return _relayTimers[relayIndex].pendingChange ? _relayTimers[relayIndex].targetState : _relayStates[relayIndex];
}

/**
 * @brief Record the intended relay states, then switch off the relays set to "Aus"
 *
 * The flash save that follows the shutdown stores the recorded states, not the OFF state set
 * here. The record is used by that save only: Common restarts right after it, or the shutdown
 * came from a table unload, after which knx.configured() stays false until the reboot
 * (BauSystemBDevice::configured) and flash.save() returns early. A second shutdown (the restart
 * that ends an ETS download) keeps the first record; its save is skipped for the same reason.
 */
void NeoPixelBusModule::shutdownRelays()
{
    for (uint8_t i = 0; i < _relayCount && i < kRelayStorageSize; ++i)
    {
        const uint8_t bit = static_cast<uint8_t>(1u << i);
        if (!(_relayHeldMask & bit))
        {
            if (relayIntendedState(i))
                _relayMaskBeforeShutdown |= bit;
            else
                _relayMaskBeforeShutdown &= static_cast<uint8_t>(~bit);
            _relayHeldMask |= bit;
        }

        // Value 0 (not written by ETS, defensive) keeps the behaviour of the LED option, as before these parameters
        const bool keep = _relayBeforeRestart[i] == kRelayBeforeKeep ||
                          (_relayBeforeRestart[i] != kRelayBeforeOff && !_turnOffBeforeRestart);
        if (keep) continue; // "unveraendert": the restart follows before a pending change could run

        _relayTimers[i].pendingChange = false; // a delayed or deferred ON must not re-close it
        setRelayOutput(i, false);
    }
}

/**
 * @brief Switch off the relays set to "Aus", stop effects and blank the logical buffer
 * @return false when the ETS option for the LEDs is off or the module never came up
 *
 * A relay follows its "Vor Upload/Neustart" value: 1 (Aus) and 2 (unveraendert) are independent
 * of the LED option, raw value 0 (not written by ETS, defensive) follows the LED option. Blanks the
 * VirtualStrip only - syncAll() rebuilds the physical buffers from it. Those are cleared per
 * strip at the point of transmission, where the driver is provably idle: every driver silently
 * skips clear() while a transfer runs.
 */
bool NeoPixelBusModule::prepareLedShutdown()
{
    shutdownRelays();

    if (!_turnOffBeforeRestart)
    {
        logInfoP("LED shutdown before restart is disabled in ETS");
        return false;
    }

    logInfoP("Turning off all LEDs before restart/programming");

    if (!_initialized) return false;

    // Stop all effects so nothing repaints the buffers behind us
    for (auto& segConfig : _segments)
    {
        if (segConfig.segment && segConfig.segment->hasEffect())
        {
            segConfig.segment->clearEffect();
        }
    }

    if (_virtualStrip) _virtualStrip->clear();
    return true;
}

void NeoPixelBusModule::processBeforeRestart()
{
    if (!prepareLedShutdown()) return;
    _ledShutdownActive = false;

    NeoPixelManager* manager = _neoPixel.getManager();
    if (!manager) return;

    // Blocking on purpose: the reboot follows immediately and must not cut the dark frame.
    // Retire an in-flight frame first - showAll() would resume that one and mark the parallel
    // strips as sent without re-transmitting them, leaving them lit through the reboot.
    manager->flushFrame();

    const uint32_t stripCount = manager->getStripCount();
    for (uint32_t i = 0; i < stripCount; i++)
    {
        PhysicalStrip* strip = manager->getStrip(i);
        if (!strip || !strip->isInitialized()) continue;

        // Frame-derived deadline, so a timeout means a wedge: clear() then no-ops and the
        // strip keeps its lit frame through the reboot.
        if (!strip->waitForTransfer(strip->getTransferTimeoutMs()))
            logWarningP("Strip %u did not finish - it may stay lit through the restart", (unsigned)i);

        strip->clear();
    }

    manager->showAll();
    manager->flushFrame(); // showAll() only starts the frame now - the reboot must not cut it
}

void NeoPixelBusModule::processBeforeTablesUnload()
{
#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    if (_studio) _studio->invalidateSaved();
#endif
    // An ETS download follows, not a restart: the KNX stack keeps running, so arm the
    // sequencer instead of blocking loop() with a synchronous showAll().
    if (!prepareLedShutdown()) return;

    _ledShutdownActive = true;
    _ledShutdownIndex = 0;
    _ledShutdownStartMs = millis();
}

/**
 * @brief Transmit the blanked buffers without stalling loop()
 *
 * One strip per pass, and only while nothing is on the wire - that preserves the arbitration
 * showAll() enforces without its blocking waitForTransfer(). A pass costs a frame start on the
 * asynchronous drivers; the synchronous fallbacks (PIO without DMA, ESP32 hardware SPI) still
 * cost a full frame inside show().
 */
void NeoPixelBusModule::pumpLedShutdown()
{
    NeoPixelManager* manager = _neoPixel.getManager();
    if (!manager)
    {
        _ledShutdownActive = false;
        return;
    }

    // A frame started by the render path still owns the strip buffers. Let the manager's own
    // state machine finish it - one step per pass, no waiting - before blanking anything.
    if (manager->isFrameInFlight())
    {
        manager->showAll();
        return;
    }

    const uint32_t stripCount = manager->getStripCount();

    // Skip strips that never came up; init() keeps the device running on a partial set
    while (_ledShutdownIndex < stripCount)
    {
        PhysicalStrip* candidate = manager->getStrip(_ledShutdownIndex);
        if (candidate && candidate->isInitialized()) break;
        _ledShutdownIndex++;
    }

    if (_ledShutdownIndex >= stripCount)
    {
        _ledShutdownActive = false;
        return;
    }

    // Never overlap frames - poll again on the next pass instead of waiting here
    for (uint32_t i = 0; i < stripCount; i++)
    {
        PhysicalStrip* busyCheck = manager->getStrip(i);
        if (!busyCheck || !busyCheck->isInitialized() || !busyCheck->isBusy()) continue;

        // The budget is per strip: the timer restarts on every start below, so a long chain of
        // strips cannot trip it. Checked here and not earlier so it can only fire while a strip
        // really is busy, and so the log names that strip - not the next one waiting to be sent.
        if ((uint32_t)(millis() - _ledShutdownStartMs) >= kLedShutdownTimeoutMs)
        {
            logWarningP("LED shutdown aborted - strip %u still busy after %ums",
                        (unsigned)i, (unsigned)kLedShutdownTimeoutMs);
            _ledShutdownActive = false;
        }
        return;
    }

    PhysicalStrip* strip = manager->getStrip(_ledShutdownIndex);

    // Safe here and only here: the scan above proved no transfer is running, and the PIO
    // drivers refuse to blank a buffer while it is being sent.
    strip->clear();

    // No markFrameSent(): that records a frame as confirmed on the wire, which it is not
    // until the transfer completes. Leaving the strip dirty keeps a later showAll() retrying.
    if (!strip->show())
        logWarningP("LED shutdown frame rejected by strip %u", (unsigned)_ledShutdownIndex);

    _ledShutdownIndex++;
    _ledShutdownStartMs = millis();
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
#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    // Commissioning patterns are temporary and must never become reboot state.
    if (_studio) _studio->stopTest();
#endif
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
        if (_flashPersistence) _flashPersistence->restoreRelaysAfterStartup();
        seedStatus();
        _startupRestoreDone = true; // loop() processes the telegrams of the startup delay
        _drainGate = _deferredKoCount > 0;
        _drainPassesLeft = (kDeferredKoMax + kDeferredKoPerLoop - 1) / kDeferredKoPerLoop;
        logInfoP("Startup complete - ETS configuration active");
        return;
    }

    // Restore last runtime state ("Letzter Zustand") from flash onto the live segments; must run
    // here so the restored effect/colour/brightness become the ACTIVE state (reported by ETS
    // sync-back readback). Only acts on segments set to "Letzter Zustand"; StartupEM below is
    // evaluated afterwards and can override it (priority EM > Effekt > Solid).
#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    if (_studio) _studio->restoreSaved(true);
#endif
    if (_flashPersistence) _flashPersistence->restoreStatesAfterStartup();
#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    if (_studio) _studio->restoreSaved(false);
#endif
    if (_flashPersistence) _flashPersistence->restoreRelaysAfterStartup();

    uint8_t previousChannel = _channelIndex;
    for (size_t i = 0; i < _segments.size(); ++i)
    {
        auto& cfg = _segments[i];
        if (!cfg.segment) continue;

        uint8_t startupEmId = configuredBootEm(i);

        // Effektkette slave: the watchdog counts from the end of the startup delay
        if (cfg.syncMode == 2)
        {
            cfg.lastSyncMs = millis();
            cfg.lastRxPower = 0xFF;
        }

        if (startupEmId == EM_NONE) continue;

        // Also on an "Aus" segment: the boot EM switches it on (its stop switches off again)
        startEffektManagerFresh(i, startupEmId);

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

    seedStatus();
    // loop() now processes the telegrams of the startup delay on top of the restored state
    _startupRestoreDone = true;
    _drainGate = _deferredKoCount > 0;
    _drainPassesLeft = (kDeferredKoMax + kDeferredKoPerLoop - 1) / kDeferredKoPerLoop;
    logInfoP("Startup complete - ETS configuration active");
}

/**
 * @brief Queue a NeoPixel input telegram with its value (startup delay, or while the queue drains).
 * @return false if the KO is not queued (foreign KO, output, or a dimming telegram of the delay)
 */
bool NeoPixelBusModule::deferKo(GroupObject& ko, uint16_t koNumber)
{
    // NeoPixel input KOs: the module block (400..; the strip block from 500 has no inputs that are
    // processed) and the segment blocks up to the last channel
    const bool segmentKo = koNumber >= NEO_KoBlockOffset && koNumber < NEO_KoBlockOffset + NEO_ChannelCount * NEO_KoBlockSize;
#ifdef NEOSTRIP_KoOffset
    const bool moduleKo = koNumber >= NEO_KoPower && koNumber < NEOSTRIP_KoOffset;
#else
    const bool moduleKo = koNumber >= NEO_KoPower && koNumber < NEO_KoBlockOffset;
#endif
    if (!(segmentKo || moduleKo) || !ko.writeEnable()) return false;
    const int koIndex = segmentKo ? (int)((koNumber - NEO_KoBlockOffset) % NEO_KoBlockSize) : -1;
    const size_t channel = segmentKo ? (size_t)((koNumber - NEO_KoBlockOffset) / NEO_KoBlockSize) : 0;

    // DPT 3.007 start/stop dimming has no meaning without its timing: drop it during the delay
    if (!_startupRestoreDone &&
        (koIndex == NEO_KoBriRel || koIndex == NEO_KoRRel || koIndex == NEO_KoGRel || koIndex == NEO_KoBRel ||
         koIndex == NEO_KoWRel || koIndex == NEO_KoWWRel || koIndex == NEO_KoCWRel || koIndex == NEO_KoHRel ||
         koIndex == NEO_KoSRel || koIndex == NEO_KoVRel))
        return false;

#ifdef NEO_KoSyncChain
    // A master's own sync telegram comes back through this callback: nothing to replay
    if (koIndex == NEO_KoSyncChain && (channel >= _segments.size() || _segments[channel].syncMode != 2)) return false;
    // The master repeats an unchanged state every few seconds: an identical sync telegram with no
    // telegram for this segment or the module in between has no effect when processed twice, so the
    // older copy is dropped and the newest one queued (an overflow drop then cannot lose it).
    if (koIndex == NEO_KoSyncChain)
    {
        const uint16_t blockStart = (uint16_t)(NEO_KoBlockOffset + channel * NEO_KoBlockSize);
        const size_t size = std::min<size_t>(ko.valueSize(), kDeferredKoValueMax);
        for (int i = (int)_deferredKoCount - 1; i >= (int)_deferredKoHead; --i)
        {
            const uint16_t nr = _deferredKo[i].number;
            if (nr == koNumber)
            {
                if (_deferredKo[i].size == size && memcmp(_deferredKo[i].value, ko.valueRef(), size) == 0)
                {
                    memmove(&_deferredKo[i], &_deferredKo[i + 1], (size_t)(_deferredKoCount - i - 1) * sizeof(_deferredKo[0]));
                    --_deferredKoCount;
                }
                break;
            }
            if (nr < NEO_KoBlockOffset || (nr >= blockStart && nr < blockStart + NEO_KoBlockSize)) break;
        }
    }
#endif

    // A relay is independent of the other KOs: only its newest value counts (no click sequence, and
    // the min-off time cannot swallow the final ON when the queue is processed at once)
#ifdef NEO_KoExternalRelay1
    if (koNumber >= NEO_KoExternalRelay1 && koNumber < NEO_KoExternalRelay1 + kMaxExternalRelays)
    {
        for (uint8_t i = _deferredKoHead; i < _deferredKoCount; ++i)
        {
            if (_deferredKo[i].number != koNumber) continue;
            memmove(&_deferredKo[i], &_deferredKo[i + 1], (size_t)(_deferredKoCount - i - 1) * sizeof(_deferredKo[0]));
            --_deferredKoCount;
            break;
        }
    }
#endif

    if (_deferredKoCount >= kDeferredKoMax && _deferredKoHead > 0)
    {
        // Compact: processed entries make room
        memmove(&_deferredKo[0], &_deferredKo[_deferredKoHead], (size_t)(_deferredKoCount - _deferredKoHead) * sizeof(_deferredKo[0]));
        _deferredKoCount -= _deferredKoHead;
        _deferredKoHead = 0;
    }
    if (_deferredKoCount >= kDeferredKoMax)
    {
        // Full: the oldest entry is dropped (order of the rest is kept)
        if (!_deferredKoOverflow) logWarningP("Startup delay: more than %d KO updates, the oldest are dropped", kDeferredKoMax);
        _deferredKoOverflow = true;
        memmove(&_deferredKo[0], &_deferredKo[1], (size_t)(_deferredKoCount - 1) * sizeof(_deferredKo[0]));
        --_deferredKoCount;
    }
    DeferredKo& entry = _deferredKo[_deferredKoCount++];
    entry.number = koNumber;
    entry.size = (uint8_t)std::min<size_t>(ko.valueSize(), kDeferredKoValueMax);
    memcpy(entry.value, ko.valueRef(), entry.size);
    return true;
}

/**
 * @brief Process up to maxCount queued telegrams, oldest first, each with its own value.
 */
void NeoPixelBusModule::replayDeferredKos(uint8_t maxCount)
{
    if (_deferredKoHead == 0 && _deferredKoCount > 0)
        logInfoP("Startup delay: processing %d KO update(s) received meanwhile", _deferredKoCount);
    _replayingDeferred = true;
    for (uint8_t done = 0; done < maxCount && _deferredKoHead < _deferredKoCount; ++done)
    {
        const DeferredKo& entry = _deferredKo[_deferredKoHead++];
        GroupObject& ko = knx.getGroupObject(entry.number);
        if (entry.size != ko.valueSize()) continue;
        // The GO buffer is also the knx send buffer (and may already hold a newer value):
        // process the queued value, then put the current content back.
        uint8_t current[kDeferredKoValueMax];
        memcpy(current, ko.valueRef(), entry.size);
        memcpy(ko.valueRef(), entry.value, entry.size);
        processInputKo(ko);
        memcpy(ko.valueRef(), current, entry.size);
    }
    _replayingDeferred = false;
    if (_deferredKoHead >= _deferredKoCount) _deferredKoHead = _deferredKoCount = 0;
}

// ============================================================================
// Status objects
// ============================================================================

/**
 * @brief Write one status KO in the given mode.
 * @return true if a telegram was requested.
 * The knx stack answers no A_GroupValue_Read for a KO that was never written (GroupObject::readEnable()).
 */
bool NeoPixelBusModule::writeStatus(GroupObject& ko, const KNXValue& value, const Dpt& type, StatusMode mode)
{
    if (mode == StatusMode::Seed && !ko.initialized())
    {
        ko.valueNoSend(value, type);
        return false;
    }
    if (mode == StatusMode::Always)
    {
        if (!ko.valueNoSend(value, type)) return false;
        ko.objectWritten();
        return true;
    }
    if (!ko.valueNoSendCompare(value, type)) return false;
    ko.objectWritten();
    return true;
}

/**
 * @brief Any LED of the segment is not black in the virtual buffer (what the strips show).
 */
bool NeoPixelBusModule::segmentShowsLight(Segment* seg)
{
    VirtualStrip* strip = seg ? seg->getVirtualStrip() : nullptr;
    if (!strip) return false;
    for (uint32_t i = seg->getStartLed(); i <= seg->getEndLed(); i++)
    {
        uint8_t r = 0, g = 0, b = 0, ww = 0, cw = 0;
        if (strip->getPixel((uint16_t)i, r, g, b, ww, cw) && (r | g | b | ww | cw)) return true;
    }
    return false;
}

/**
 * @brief Segment output is on: own power on, not suspended by global power, and an effect set. A
 *        paused segment is not redrawn, so its buffer is what the LEDs show; a running segment is on
 *        while its brightness is > 0 (EM: master, the render dips to 0 in cue fades).
 */
bool NeoPixelBusModule::isSegmentLit(SegmentConfig& cfg)
{
    Segment* seg = cfg.segment;
    if (!isSegmentVisible(cfg) || !seg || !seg->getEffect()) return false;
    if (seg->isPaused()) return segmentShowsLight(seg);
    if (cfg.emController.isRunning()) return seg->getMasterBrightness() > 0;
    return seg->getBrightness() > 0;
}

bool NeoPixelBusModule::syncModuleStatusItem(uint8_t item, StatusMode mode)
{
    if (_localTestMode) return false;
    if (!_statusSeeded && mode != StatusMode::Seed) return false;

    switch (item)
    {
        case kModPower:
            return writeStatus(KoNEO_PowerState, _globalPowerOn, DPT_Switch, mode);
        case kModBrightness:
            // Inverse of the KO scaling (percent * 255 / 100): every percent written reads back unchanged.
            return writeStatus(KoNEO_BrightnessState, (uint8_t)((_globalBrightness * 100 + 127) / 255), DPT_Scaling, mode);
        case kModRelay1:
        case kModRelay2:
        case kModRelay3:
        case kModRelay4:
        {
            const uint8_t relay = item - kModRelay1;
            // Same guards as setRelayOutput(): a relay without a usable pin has no state.
            if (relay >= kRelayStorageSize || relay >= _relayCount || _relayPins[relay] == 255) return false;
            const bool state = _relayStates[relay];
            switch (relay)
            {
#if defined(NEO_KoExternalRelay1State)
                case 0: return writeStatus(KoNEO_ExternalRelay1State, state, DPT_Switch, mode);
#endif
#if defined(NEO_KoExternalRelay2State)
                case 1: return writeStatus(KoNEO_ExternalRelay2State, state, DPT_Switch, mode);
#endif
#if defined(NEO_KoExternalRelay3State)
                case 2: return writeStatus(KoNEO_ExternalRelay3State, state, DPT_Switch, mode);
#endif
#if defined(NEO_KoExternalRelay4State)
                case 3: return writeStatus(KoNEO_ExternalRelay4State, state, DPT_Switch, mode);
#endif
                default: return false;
            }
        }
        case kModHclGlobal:
#ifdef NEO_KoHCLGlobalState
            // 0 K = no HCL: the send-all reports it only as a change.
            return writeStatus(KoNEO_HCLGlobalState, _hclGlobalStatusKelvin, Dpt(7, 600),
                               (mode == StatusMode::Always && _hclGlobalStatusKelvin == 0) ? StatusMode::OnChange : mode);
#else
            return false;
#endif
        default:
            return false;
    }
}

bool NeoPixelBusModule::syncSegmentStatusItem(size_t segmentIndex, uint8_t item, StatusMode mode)
{
    if (_localTestMode) return false;
    if (!_statusSeeded && mode != StatusMode::Seed) return false;
    // KO macros beyond the configured channel table would address foreign KOs (no bounds check in knx).
    if (segmentIndex >= _segments.size() || segmentIndex >= kStatusSegmentMax) return false;
    SegmentConfig& cfg = _segments[segmentIndex];
    Segment* seg = cfg.segment;
    if (!seg) return false;

    // The primary colour is kept while the segment is off, so it is the colour in every state.
    const uint32_t primary = seg->getPrimaryColor(); // 0xRRGGBBWW
    const uint8_t r = (primary >> 24) & 0xFF;
    const uint8_t g = (primary >> 16) & 0xFF;
    const uint8_t b = (primary >> 8) & 0xFF;
    uint8_t w = primary & 0xFF;
    // RGBCCT: a white command is split into WW + CW; report the combined white.
    if (_virtualStrip && _virtualStrip->hasDualWhiteChannel()) w = (uint8_t)std::min<uint16_t>(255, (uint16_t)w + seg->getConfig().cw());
    const uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;

    const uint8_t prevChannel = _channelIndex;
    _channelIndex = static_cast<uint8_t>(segmentIndex);
    bool sent = false;
    switch (item)
    {
        case kSegPower:
            sent = writeStatus(KoNEO_SegmentPowerState, isSegmentLit(cfg), DPT_Switch, mode);
            break;
        case kSegBrightness:
            // Actual dimming value: 0 % while the segment is off or suspended by global power;
            // at least 1 % for an intent > 0 (dimming up from off starts at 1/255).
            sent = writeStatus(KoNEO_SegmentBrightnessState,
                               (uint8_t)(!isSegmentVisible(cfg) ? 0
                                         : std::max<int>(cfg.savedBrightness ? 1 : 0, (cfg.savedBrightness * 100 + 127) / 255)),
                               DPT_Scaling, mode);
            break;
        case kSegFx:
            sent = writeStatus(KoNEO_FxState, getTypeFromEffect(seg->getEffect()), DPT_Value_1_Ucount, mode);
            break;
        case kSegRgb:
            sent = writeStatus(KoNEO_RGBState, rgb, DPT_Colour_RGB, mode);
            break;
        case kSegRgbw:
            sent = writeStatus(KoNEO_RGBWState, ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | w, DPT_Colour_RGBW, mode);
            break;
        case kSegHsv:
            // The HSV status object is DPT 232.600 and has always reported the resulting RGB colour.
            sent = writeStatus(KoNEO_HSVState, rgb, DPT_Colour_RGB, mode);
            break;
        case kSegHcl:
            // 0 K = no HCL: the send-all reports it only as a change.
            sent = writeStatus(KoNEO_HCLState, _hclSegmentStatusKelvin[segmentIndex], Dpt(7, 600),
                               (mode == StatusMode::Always && _hclSegmentStatusKelvin[segmentIndex] == 0) ? StatusMode::OnChange : mode);
            break;
        case kSegEffectText:
#ifdef NEO_KoEffectTextStatus
            // The KO carries the first 14 characters; append chunks are confirmed by sendEffectTextStatusKO().
            sent = writeStatus(KoNEO_EffectTextStatus, seg->getConfig().effectText, DPT_String_8859_1, mode);
#endif
            break;
        case kSegEmStatus:
#ifdef NEO_KoEmStatus
            sent = writeStatus(KoNEO_EmStatus, cfg.emController.activeEmId(), DPT_Value_1_Ucount, mode);
#endif
            break;
        case kSegEmCue:
#ifdef NEO_KoEmCueStatus
            sent = writeStatus(KoNEO_EmCueStatus, cfg.emController.activeCueNum(), DPT_Value_1_Ucount, mode);
#endif
            break;
        case kSegEmRunState:
#ifdef NEO_KoEmRunState
            // 0 = gestoppt, 1 = läuft, 2 = pausiert
            sent = writeStatus(KoNEO_EmRunState, cfg.emController.runState(), DPT_Value_1_Ucount, mode);
#endif
            break;
        case kSegCct:
            // Kelvin is not derivable from the colour and not persisted: only after a CCT command.
            if (cfg.statusCctKelvin != 0)
                sent = writeStatus(KoNEO_CCTState, cfg.statusCctKelvin, Dpt(7, 600), mode);
            break;
        default:
            break;
    }
    _channelIndex = prevChannel;
    return sent;
}

void NeoPixelBusModule::syncModuleStatus(StatusMode mode)
{
    for (uint8_t item = 0; item < kModuleStatusCount; item++)
        syncModuleStatusItem(item, mode);
}

void NeoPixelBusModule::syncSegmentStatus(size_t segmentIndex, StatusMode mode)
{
    for (uint8_t item = 0; item < kSegmentStatusCount; item++)
        syncSegmentStatusItem(segmentIndex, item, mode);
}

uint8_t NeoPixelBusModule::statusScopeCount() const
{
    return 1 + static_cast<uint8_t>(std::min<size_t>(_segments.size(), kStatusSegmentMax));
}

void NeoPixelBusModule::seedStatus()
{
    if (_localTestMode) return;
    syncModuleStatus(StatusMode::Seed);
    for (size_t i = 0; i < _segments.size(); i++)
        syncSegmentStatus(i, StatusMode::Seed);
    _statusSeeded = true;

    // "Status nach Neustart senden": every status once, paced by processStatusQueue().
    if (ParamNEO_NEOStatusSendOnStartup)
    {
        for (uint8_t scope = 0; scope < statusScopeCount(); scope++)
            queueStatus(scope, true);
    }
}

void NeoPixelBusModule::queueStatus(uint8_t scope, bool always)
{
    if (_localTestMode || scope >= statusScopeCount()) return;
    const uint8_t itemCount = (scope == 0) ? (uint8_t)kModuleStatusCount : (uint8_t)kSegmentStatusCount;
    const uint16_t items = (uint16_t)((1u << itemCount) - 1);
    // Bits only: an item already handled is not re-sent unless queued again.
    if (always)
        _statusQueueAlways[scope] |= items;
    else
        _statusQueueChange[scope] |= items;
}

void NeoPixelBusModule::queueStatusItem(uint8_t scope, uint8_t item, bool always)
{
    if (_localTestMode || scope >= statusScopeCount() || item >= 16) return;
    const uint16_t bit = (uint16_t)(1u << item);
    if (always)
        _statusQueueAlways[scope] |= bit;
    else
        _statusQueueChange[scope] |= bit;
}

void NeoPixelBusModule::queueAllSegmentStatus(bool always)
{
    for (uint8_t scope = 1; scope < statusScopeCount(); scope++)
        queueStatus(scope, always);
}

void NeoPixelBusModule::statusAfterCommand(uint8_t scope)
{
    // Through the credit-limited queue: one group telegram can address many segments at once.
    // A command during the startup delay is confirmed with the full status after the seed.
    queueStatus(scope, !_statusSeeded || _replayingDeferred);
}

void NeoPixelBusModule::processStatusQueue()
{
    if (!_statusSeeded) return;

    // Credit budget: kStatusBurst telegrams at once, then one per kStatusPaceMs. Keeps the TP
    // transmit queue far below its limit even when a group telegram addresses every segment.
    const uint32_t now = millis();
    if (_statusCredits >= kStatusBurst)
    {
        _statusCreditMs = now;
    }
    else
    {
        const uint32_t refill = (now - _statusCreditMs) / kStatusPaceMs;
        if (refill > 0)
        {
            _statusCredits = (uint8_t)std::min<uint32_t>(kStatusBurst, _statusCredits + refill);
            _statusCreditMs += refill * kStatusPaceMs;
        }
    }
    if (_statusCredits == 0) return;

    // Changes (commands, EM, global) and the startup send-all take turns per telegram, so a steady
    // stream of changes cannot starve the send-all and the send-all delays a change by one telegram.
    while (_statusCredits > 0)
    {
        const bool alwaysFirst = _statusAlwaysTurn;
        bool sent = alwaysFirst ? serveStatusQueue(_statusQueueAlways, true, _statusAlwaysScope, _statusAlwaysItem)
                                : serveStatusQueue(_statusQueueChange, false, _statusChangeScope, _statusChangeItem);
        if (!sent)
            sent = alwaysFirst ? serveStatusQueue(_statusQueueChange, false, _statusChangeScope, _statusChangeItem)
                               : serveStatusQueue(_statusQueueAlways, true, _statusAlwaysScope, _statusAlwaysItem);
        if (!sent) return;
        _statusCredits--;
        _statusAlwaysTurn = !alwaysFirst;
    }
}

/**
 * @brief Walk one queue (at most one lap) up to the first item that requests a telegram.
 * @return true if a telegram was requested; unchanged OnChange items are cleared on the way.
 */
bool NeoPixelBusModule::serveStatusQueue(uint16_t* queue, bool always, uint8_t& cursorScope, uint8_t& cursorItem)
{
    const uint8_t scopes = statusScopeCount();
    bool pending = false;
    for (uint8_t scope = 0; scope < scopes && !pending; scope++)
        pending = queue[scope] != 0;
    if (!pending) return false;

    const uint16_t steps = (uint16_t)scopes * (kSegmentStatusCount + 1);
    for (uint16_t step = 0; step < steps; step++)
    {
        if (cursorScope >= scopes) cursorScope = 0;
        const uint8_t scope = cursorScope;
        const uint8_t itemCount = (scope == 0) ? (uint8_t)kModuleStatusCount : (uint8_t)kSegmentStatusCount;
        if (cursorItem >= itemCount)
        {
            cursorItem = 0;
            cursorScope = (uint8_t)(scope + 1);
            continue;
        }
        const uint8_t item = cursorItem++;
        const uint16_t bit = (uint16_t)(1u << item);
        if ((queue[scope] & bit) == 0) continue;
        // An item in both queues is sent once, with its value.
        const bool sendValue = always || (_statusQueueAlways[scope] & bit) != 0;
        _statusQueueAlways[scope] &= (uint16_t)~bit;
        _statusQueueChange[scope] &= (uint16_t)~bit;

        const StatusMode mode = sendValue ? StatusMode::Always : StatusMode::OnChange;
        const bool sent = (scope == 0) ? syncModuleStatusItem(item, mode) : syncSegmentStatusItem(scope - 1, item, mode);
        if (sent) return true;
    }
    return false;
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

    const bool wasOn = _relayStates[relayIndex];
    _relayStates[relayIndex] = state;
    bool physicalState = _relayInverted[relayIndex] ? !state : state;
    digitalWrite(pin, physicalState ? HIGH : LOW);

    // Minimum off-time counts from the ON->OFF transition; a repeated OFF does not restart it
    if (!state && wasOn)
    {
        _relayTimers[relayIndex].lastOffTime = millis();
    }

    // Send status feedback to KNX bus
    queueStatus(0, false);

    logDebugP("Relay %d set to %s (GPIO %d)", relayIndex + 1, state ? "ON" : "OFF", pin);
}

void NeoPixelBusModule::scheduleRelayChange(uint8_t relayIndex, bool targetState)
{
    if (relayIndex >= kRelayStorageSize) return;
    if (_relayCount == 0 || relayIndex >= _relayCount) return;

    // A command makes this relay's live state the one to save (no save follows a shutdown today)
    _relayHeldMask &= static_cast<uint8_t>(~(1u << relayIndex));

    // Remaining minimum off-time; an ON is deferred until it has elapsed (only when turning ON)
    unsigned long minOffRemainingMs = 0;
    if (targetState && !_relayStates[relayIndex])
    {
        const uint16_t minOffSeconds = relayMinOffSeconds(relayIndex);

        if (minOffSeconds > 0 && _relayTimers[relayIndex].lastOffTime != 0)
        {
            unsigned long now = millis();
            unsigned long minOffMillis = minOffSeconds * 1000UL;

            // Handle millis() overflow with signed comparison (same as processRelayTimers)
            long elapsed = (long)(now - _relayTimers[relayIndex].lastOffTime);
            if (elapsed >= 0 && (unsigned long)elapsed < minOffMillis)
            {
                minOffRemainingMs = minOffMillis - (unsigned long)elapsed;
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

    // Cancel any pending timer for this relay: the latest command wins
    _relayTimers[relayIndex].pendingChange = false;

    // The ON waits for the switch delay and for the rest of the minimum off-time, whichever is longer
    const unsigned long waitMs = std::max<unsigned long>(delaySeconds * 1000UL, minOffRemainingMs);
    if (waitMs == 0)
    {
        // No delay - execute immediately
        setRelayOutput(relayIndex, targetState);
    }
    else
    {
        // Schedule delayed execution
        _relayTimers[relayIndex].targetState = targetState;
        _relayTimers[relayIndex].pendingChange = true;
        _relayTimers[relayIndex].triggerTime = millis() + waitMs;
        if (minOffRemainingMs > delaySeconds * 1000UL)
            logInfoP("Relay %d ON deferred %lus until the min off-time has elapsed",
                     relayIndex + 1, (minOffRemainingMs + 999) / 1000);
        else
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

    // During the startup delay queue NeoPixel input telegrams: they are processed after the flash
    // restore, the boot EM and the status seed (replayDeferredKos from loop), so the restore cannot
    // undo them. Telegrams arriving while the queue drains line up behind it.
    if (!_localTestMode && !_replayingDeferred && (!_startupRestoreDone || _deferredKoHead < _deferredKoCount))
    {
        if (deferKo(ko, koNumber) || !_startupRestoreDone) return;
    }

    // ── Effektmanager KOs (NEOEM module, KoOffset=1240, BlockSize=4) ─────
#ifdef NEO_KoEmStart
    {
        int emChannel = NEO_KoCalcChannel(koNumber);
        // NEO_KoCalcIndex is channel-relative (uses _channelIndex); point it at THIS KO's channel
        // first, else a stale _channelIndex makes the range-check fail for every segment except the
        // residual one -> EM control KOs for all other segments would be silently dropped.
        if (emChannel >= 0) setChannelIndex(emChannel);
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
            // Guard: an out-of-range EM channel (invalid/stale KO index) must not index _segments[].
            if ((size_t)emChannel >= _segments.size())
                return;
            // EM Stop is a trigger: only 1 stops; a 0 carries no command (no stop, no slave override).
            if (emKoIndex == NEO_KoEmStop && !(bool)ko.value(DPT_Switch))
                return;
            // Own status telegrams come back through this handler; a master's SyncChain is its own output.
            bool statusKo = emKoIndex == NEO_KoEmStatus || emKoIndex == NEO_KoEmCueStatus;
    #ifdef NEO_KoEmRunState
            statusKo = statusKo || emKoIndex == NEO_KoEmRunState;
    #endif
    #ifdef NEO_KoEffectTextStatus
            statusKo = statusKo || emKoIndex == NEO_KoEffectTextStatus;
    #endif
            bool syncKo = false;
    #ifdef NEO_KoSyncChain
            syncKo = emKoIndex == NEO_KoSyncChain;
    #endif
            // Effektkette slave: a local controlling KO overrides the master (policy "Lokal hat Vorrang")
            if (!statusKo && !syncKo && _segments[emChannel].syncMode == 2) _segments[emChannel].localOverride = true;
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
                    const bool emWasRunning = _segments[emChannel].emController.isRunning();
                    stopEffektManager((size_t)emChannel);
                    // A stopped EM-action scene is not replayed at boot; a static scene marker stays.
                    if (emWasRunning && _sceneManager &&
                        _sceneManager->isEmActionScene((uint8_t)emChannel, _segments[emChannel].savedSceneNumber))
                        _segments[emChannel].savedSceneNumber = 0;
                    break;
                }
    #ifdef NEO_KoEmPause
                case NEO_KoEmPause:
                {
                    bool doPause = ko.value(DPT_Switch); // 1 = Pause, 0 = Resume
                    logInfoP("Segment %d: EffektManager %s", emChannel, doPause ? "Pause" : "Resume");
                    SegmentConfig& cfgP = _segments[emChannel];
                    if (doPause)
                        pauseEffektManager((size_t)emChannel);
                    else if (cfgP.emController.isPaused() && !isSegmentVisible(cfgP))
                        cfgP.suspendedByPower = true; // dark segment: the next ON resumes it
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
            // Status from the resulting state; skip own status telegrams and the Effektkette sync of a master.
            if (!statusKo && !(syncKo && _segments[emChannel].syncMode != 2))
                statusAfterCommand(static_cast<uint8_t>(1 + emChannel));
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

        // Per segment, so a locked segment keeps running (lock has priority over global power).
        if (powerState == false)
        {
            bool suspended = false;
            for (size_t i = 0; i < _segments.size(); i++)
            {
                if (!_segments[i].segment || _segments[i].locked) continue;
                globalSuspendSegment(i);
                suspended = true;
            }
            if (suspended) showVirtualStripIfIdle(); // one frame for every cleared segment
        }
        else
        {
            for (size_t i = 0; i < _segments.size(); i++)
            {
                if (!_segments[i].segment || _segments[i].locked || !_segments[i].globalSuspended) continue;
                globalResumeSegment(i);
            }
        }

        // Status: module now; every segment's power status follows global power (paced)
        statusAfterCommand(0);
        if (_statusSeeded) queueAllSegmentStatus(false); // segments changed with the global value
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

        // Status: module now; a segment can go dark at low global brightness (paced)
        statusAfterCommand(0);
        if (_statusSeeded) queueAllSegmentStatus(false); // segments changed with the global value
        return;
    }

#if defined(NEO_KoExternalRelay1)
    if (koNumber == NEO_KoExternalRelay1)
    {
        bool state = ko.value(DPT_Switch);
        scheduleRelayChange(0, state);
        statusAfterCommand(0);
        return;
    }
#endif

#if defined(NEO_KoExternalRelay2)
    if (koNumber == NEO_KoExternalRelay2)
    {
        bool state = ko.value(DPT_Switch);
        scheduleRelayChange(1, state);
        statusAfterCommand(0);
        return;
    }
#endif

#if defined(NEO_KoExternalRelay3)
    if (koNumber == NEO_KoExternalRelay3)
    {
        bool state = ko.value(DPT_Switch);
        scheduleRelayChange(2, state);
        statusAfterCommand(0);
        return;
    }
#endif

#if defined(NEO_KoExternalRelay4)
    if (koNumber == NEO_KoExternalRelay4)
    {
        bool state = ko.value(DPT_Switch);
        scheduleRelayChange(3, state);
        statusAfterCommand(0);
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

        // An unknown or disabled effect ID maps to Solid: ignore it before it takes over an EM,
        // is saved or synced as that ID.
        if (koIndex == NEO_KoFx)
        {
            const uint8_t fxId = ko.value(DPT_Value_1_Ucount);
            if (getTypeFromEffect(getEffectFromType(fxId)) != fxId)
            {
                logWarningP("Segment %d: effect ID %d unknown - ignored", channel, fxId);
                _channelIndex = oldChannelIndex;
                return;
            }
        }

        // A scene number not configured on this segment (recall or learn) changes nothing: ignore it
        // before it overrides an Effektkette master.
        if (koIndex == NEO_KoScene && _sceneManager)
        {
            const uint8_t sceneNumber = (uint8_t)((ko.valueRef()[0] & 0x3F) + 1);
            if (sceneNumber > _sceneManager->getSceneCount((uint8_t)channel))
            {
                logInfoP("Segment %d: scene %d not configured - ignored", channel, sceneNumber);
                _channelIndex = oldChannelIndex;
                return;
            }
        }

        const bool segmentStatusKo = koIndex == NEO_KoSegmentPowerState || koIndex == NEO_KoSegmentBrightnessState ||
                                     koIndex == NEO_KoCCTState || koIndex == NEO_KoFxState || koIndex == NEO_KoSceneState ||
                                     koIndex == NEO_KoRGBState || koIndex == NEO_KoHSVState || koIndex == NEO_KoRGBWState ||
                                     koIndex == NEO_KoHCLState;

        // A DPT 3.007 stop telegram (step code 0, button released) carries no command: no takeover, no override.
        const bool dpt3Stop = (koIndex == NEO_KoRRel || koIndex == NEO_KoGRel || koIndex == NEO_KoBRel ||
                               koIndex == NEO_KoWRel || koIndex == NEO_KoWWRel || koIndex == NEO_KoCWRel ||
                               koIndex == NEO_KoHRel || koIndex == NEO_KoSRel || koIndex == NEO_KoVRel ||
                               koIndex == NEO_KoFxRel || koIndex == NEO_KoBriRel || koIndex == NEO_KoCueRel) &&
                              (ko.valueRef()[0] & 0x07) == 0;

        // Effektkette slave: a local controlling KO overrides the master (policy "Lokal hat Vorrang")
        if (!segmentStatusKo && !dpt3Stop && koIndex != NEO_KoLock && _segments[channel].syncMode == 2)
            _segments[channel].localOverride = true;

        // A direct manual command takes over from a running EM on this segment: halt the EM
        // (without restoring pre-EM state — the command below sets the new visual). Excluded:
        // read/status KOs, Scene recall (interrupts the EM itself), segment power (pauses/resumes
        // the EM), brightness (dims the EM, not stops it), and Cue +/- / Lock (EM/segment control,
        // not a visual takeover).
        if (!segmentStatusKo && !dpt3Stop && koIndex != NEO_KoScene && koIndex != NEO_KoSegmentPower &&
            koIndex != NEO_KoSegmentBrightness && koIndex != NEO_KoBriRel &&
            koIndex != NEO_KoCueRel && koIndex != NEO_KoLock)
        {
            interruptEmIfRunning((size_t)channel);
            // A direct visual command replaces an earlier scene ("Letzter Zustand" restores the scene first).
            _segments[channel].savedSceneNumber = 0;
            // Colour commands outside H/S/V make the stored H/S/V stale (base for H/S/V dimming).
            if (koIndex != NEO_KoFx && koIndex != NEO_KoFxRel)
                _segments[channel].hsvTracked = koIndex == NEO_KoH || koIndex == NEO_KoS || koIndex == NEO_KoV ||
                                                koIndex == NEO_KoHSV || koIndex == NEO_KoHRel || koIndex == NEO_KoSRel ||
                                                koIndex == NEO_KoVRel || koIndex == NEO_KoHSVRel;
        }

        switch (koIndex)
        {
            case NEO_KoR:
            case NEO_KoG:
            case NEO_KoB:
            {
                // One colour channel: the other channels and the white stay as they are.
                const uint8_t value = ko.value(DPT_Value_1_Ucount); // 5.010
                const EffectConfig& pc = targetSegment->getConfig();
                uint8_t r = pc.r(), g = pc.g(), b = pc.b();
                if (koIndex == NEO_KoR) r = value;
                else if (koIndex == NEO_KoG) g = value;
                else b = value;
                targetSegment->setPrimaryColor(r, g, b, pc.ww(), pc.cw());
                logInfoP("Segment %d: R=%d G=%d B=%d", channel, r, g, b);
                break;
            }

            case NEO_KoW:
            {
                uint8_t white = ko.value(DPT_Value_1_Ucount); // 5.010
                logInfoP("Segment %d White: %d", channel, white);

                SegmentConfig& cfg = _segments[channel];


                // Update saved values - preserve existing R, G, B
                if (!cfg.savedValid)
                {
                    cfg.savedR = 0;
                    cfg.savedG = 0;
                    cfg.savedB = 0;
                }
                cfg.savedWW = white;
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                {
                    const EffectConfig& pcW = targetSegment->getConfig();
                    const uint8_t r = pcW.r(), g = pcW.g(), b = pcW.b();
                    // A generic white command is neutral white on an RGBCCT strip (WW + CW)
                    setPrimaryColorOneWhite(targetSegment, r, g, b, white);
                    logInfoP("Segment %d: Updated primary color (White=%d)", channel, white);
                }

                break;
            }

            case NEO_KoCCT:
            {
                // The ETS object is DPST-7-600 (unsigned Kelvin), not a
                // two-byte floating-point temperature value.
                uint16_t cct = ko.value(Dpt(7, 600));
                logInfoP("Segment %d CCT: %dK", channel, cct);

                SegmentConfig& cfg = _segments[channel];

                uint8_t r = 0;
                uint8_t g = 0;
                uint8_t b = 0;
                uint8_t ww = 0;
                uint8_t cw = 0;
                if (_virtualStrip && _virtualStrip->hasDualWhiteChannel())
                {
                    // A true RGBCCT strip expresses colour temperature with
                    // its warm and cool dies.  Their sum remains 255; segment
                    // brightness controls the resulting light level.
                    ProtocolHelper::kelvinToWWCW(cct, ww, cw);
                }
                else
                {
                    // RGB and RGBW strips cannot represent independent warm
                    // and cool white channels.
                    ColorHelper::kelvinToRGB(cct, r, g, b);
                }
                targetSegment->setPrimaryColor(r, g, b, ww, cw);

                // Save color for effect restore
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                cfg.savedWW = ww;
                cfg.savedCW = cw;
                cfg.savedValid = true;
                cfg.savedLastWasEffect = false;
                cfg.statusCctKelvin = cct; // reported by the CCT status item
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


                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, targetSegment->getConfig().ww(), targetSegment->getConfig().cw());
                logInfoP("Segment %d: Updated primary color (Hue)", channel);

                // Store and persist HSV values
                cfg.currentH = hue;
                cfg.currentS = s;
                cfg.currentV = v;

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


                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, targetSegment->getConfig().ww(), targetSegment->getConfig().cw());
                logInfoP("Segment %d: Updated primary color (Saturation)", channel);

                // Store HSV values
                cfg.currentH = h;
                cfg.currentS = saturation;
                cfg.currentV = v;

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


                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, targetSegment->getConfig().ww(), targetSegment->getConfig().cw());
                logInfoP("Segment %d: Updated primary color (Value)", channel);

                // Store HSV values
                cfg.currentH = h;
                cfg.currentS = s;
                cfg.currentV = value;

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


                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, targetSegment->getConfig().ww(), targetSegment->getConfig().cw());
                logInfoP("Segment %d: Updated primary color (RGB)", channel);

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


                // Update saved values - preserve W if it was already set
                cfg.savedR = r;
                cfg.savedG = g;
                cfg.savedB = b;
                if (!cfg.savedValid)
                {
                    cfg.savedWW = 0;
                }
                cfg.savedValid = true;

                // Always update primary color so running effects can use it immediately
                targetSegment->setPrimaryColor(r, g, b, targetSegment->getConfig().ww(), targetSegment->getConfig().cw());
                logInfoP("Segment %d: Updated primary color (HSV)", channel);

                // Base of the single H/S/V objects and of H/S/V dimming (hsvTracked is set above)
                cfg.currentH = h;
                cfg.currentS = s;
                cfg.currentV = v;

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
                            if (cfgQ.emController.triggerCue(target, targetSegment, _emData))
                                hideIfNotVisible((size_t)channel);
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
                SegmentConfig& cfgL = _segments[channel];
                const bool wasLocked = cfgL.locked;
                cfgL.locked = lock;
                logInfoP("Segment %d: Sperre %s", channel, lock ? "EIN" : "AUS");
                // Locked: a running dim ramp stops at its current value (its stop telegram is gated)
                if (lock && cfgL.activeDimming != SegmentConfig::NONE)
                {
                    cfgL.activeDimming = SegmentConfig::NONE;
                    cfgL.dimmingStepCode = 0;
                    cfgL.dimmingRemaining = -1;
                    queueStatus(static_cast<uint8_t>(1 + channel), false); // report the dimmed result
                }
                // Released: catch up with the global power state skipped while locked.
                if (wasLocked && !lock)
                {
                    if (!_globalPowerOn && !cfgL.globalSuspended)
                    {
                        globalSuspendSegment((size_t)channel);
                        showVirtualStripIfIdle();
                    }
                    else if (_globalPowerOn && cfgL.globalSuspended)
                    {
                        globalResumeSegment((size_t)channel);
                    }
                }
                break;
            }

            case NEO_KoFxRel:
            {
                // "Effekt schalten" (DPT 3.7), 4-bit dim control: bit3 = direction (1=next,
                // 0=previous), bits0-2 = step (0 = stop/no-op). Steps to the neighbouring effect ID;
                // IDs without an effect (unknown or disabled) are skipped.
                uint8_t raw = ko.value(Dpt(5, 10));
                if ((raw & 0x07) != 0)
                {
                    const uint8_t cur = getTypeFromEffect(targetSegment->getEffect());
                    uint8_t next = cur;
                    // Same validity rule as the Fx object: the ID must map to its own effect
                    for (int id = (raw & 0x08) ? cur + 1 : cur - 1; id >= 0 && id <= 254; id += (raw & 0x08) ? 1 : -1)
                    {
                        if (getTypeFromEffect(getEffectFromType((uint8_t)id)) == (uint8_t)id)
                        {
                            next = (uint8_t)id;
                            break;
                        }
                    }
                    if (next != cur)
                    {
                        logInfoP("Segment %d Effekt schalten: %d -> %d", channel, cur, next);
                        applyEffectToSegment(targetSegment, next);
                        hideIfNotVisible((size_t)channel); // off: recorded, shown at the next ON
                        SegmentConfig& cfgFx = _segments[channel];
                        cfgFx.savedEffectType = next;
                        // Solid is the valid effect ID 0, not the absence of an
                        // effect.  Keep it restorable just like every animated
                        // effect selected through this KO.
                        cfgFx.savedEffectValid = true;
                        cfgFx.savedLastWasEffect = true;

                        // Effektkette: a master propagates the stepped effect to its slaves (like NEO_KoFx).
                        if (cfgFx.syncMode == 1) sendSyncTelegram((size_t)channel);

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

                // The effect keeps the live primary colour (the direct colour; saved* mirrors it).
                if (cfg.savedValid)
                {
                    if (effect == 0)
                    {
                        // For Solid effect, also restore brightness — scale the pre-global intent by
                        // the live global factor, else a restore while globally dimmed overshoots to full.
                        // An effect command is no switch-on: an intent of 0 stays dark.
                        uint8_t eff = (uint8_t)((uint16_t)cfg.savedBrightness * getGlobalBrightness() / 255);
                        targetSegment->setMasterBrightness(eff);
                        targetSegment->setBrightness(eff);
                    }
                    logInfoP("Segment %d: effect %d keeps the colour", channel, effect);
                }
                else
                {
                    // No saved color - use the segment ETS start colour
                    uint32_t color = ParamNEO_NEOSegmentStartupColor;
                    uint8_t r = (color >> 16) & 0xFF;
                    uint8_t g = (color >> 8) & 0xFF;
                    uint8_t b = color & 0xFF;
                    uint8_t w = ParamNEO_NEOSegmentStartupW;

                    setPrimaryColorOneWhite(targetSegment, r, g, b, w);

                    if (effect == 0)
                    {
                        // For Solid effect, also apply the brightness intent (boot-seeded from the ETS start
                        // brightness), scaled by the live global factor like the saved-colour branch.
                        uint8_t brightness = (uint8_t)((uint16_t)cfg.savedBrightness * getGlobalBrightness() / 255);
                        targetSegment->setMasterBrightness(brightness);
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
                // Off: recorded, shown at the next ON. Otherwise the auto-update renders it.
                hideIfNotVisible((size_t)channel);
                cfg.savedEffectType = effect;
                // Effect 0 is Solid.  It has a real Effect instance and must
                // not be treated as an invalid/no-effect state, otherwise a
                // later restore or effect-chain handoff loses the selection.
                cfg.savedEffectValid = true;
                cfg.savedLastWasEffect = true;
                // Note: Effect-specific parameters are now stored in ETS, no need to snapshot

                // Effektkette: Master sends sync telegram to all slaves
                if (cfg.syncMode == 1) sendSyncTelegram((size_t)channel);

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
                    const SceneManager::SceneData before = _sceneManager->sceneData(channel, sceneNumber);
                    bool stored = _sceneManager->storeScene(channel, sceneNumber, targetSegment, true); // KO learn: cut a long text
                    if (stored)
                    {
                        logInfoP("Segment %d: Scene %d stored successfully", channel, sceneNumber);
                        // An unchanged scene needs no flash write (a cyclic learn telegram)
                        if (_sceneManager->sceneData(channel, sceneNumber) != before && !_sceneSavePending)
                        {
                            _sceneSavePending = true;
                            _sceneLearnLastWrite = openknx.flash.lastWrite();
                        }
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
                        _segments[channel].hsvTracked = false;
                        // Save scene number for flash persistence
                        _segments[channel].savedSceneNumber = sceneNumber;
                    }
                    else
                    {
                        // Not applied (unconfigured numbers are rejected above): the state stays
                        logInfoP("Segment %d: scene %d not applied", channel, sceneNumber);
                    }
                }
                break;
            }

            case NEO_KoSegmentPower:
            {
                bool power = ko.value(DPT_Switch);
                // An EM on the segment is paused by OFF and resumed by ON (not interrupted).
                if (power)
                    segmentPowerOn((size_t)channel);
                else
                    segmentOff((size_t)channel, true);
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

                // KNX dimmer: 0 switches off and keeps the intent, so the next ON returns to it.
                if (brightness == 0)
                {
                    segmentOff((size_t)channel, true);
                    break;
                }

                // Persist desired brightness for power-restore (before global scaling)
                cfgB.savedBrightness = brightness;

                // A value > 0 switches an off segment on (incl. the power-on EM edge) at this intent.
                if (cfgB.savedPower == 0)
                {
                    segmentPowerOn((size_t)channel);
                    break;
                }

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

                break;
            }

            // Warm White and Cool White control
            case NEO_KoWW:
            {
                uint8_t ww = ko.value(DPT_Value_1_Ucount); // 5.010
                // logDebugP("Segment %d Warm White: %d", channel, ww);

                // This KO selects warm white directly. Clear RGB and cool white
                // so stale values cannot turn a direct WW command into a mix.
                targetSegment->setPrimaryColor(0, 0, 0, ww, 0);

                // Keep the Solid restore state in sync with the live colour.
                SegmentConfig& cfg = _segments[channel];
                cfg.savedR = 0;
                cfg.savedG = 0;
                cfg.savedB = 0;
                cfg.savedWW = ww;
                cfg.savedCW = 0;
                cfg.savedValid = true;
                cfg.savedLastWasEffect = false;
                break;
            }

            case NEO_KoCW:
            {
                uint8_t cw = ko.value(DPT_Value_1_Ucount); // 5.010
                // logDebugP("Segment %d Cool White: %d", channel, cw);

                // This KO selects cool white directly. Clear RGB and warm white
                // so stale values cannot turn a direct CW command into a mix.
                targetSegment->setPrimaryColor(0, 0, 0, 0, cw);

                // Keep the Solid restore state in sync with the live colour.
                SegmentConfig& cfg = _segments[channel];
                cfg.savedR = 0;
                cfg.savedG = 0;
                cfg.savedB = 0;
                cfg.savedWW = 0;
                cfg.savedCW = cw;
                cfg.savedValid = true;
                cfg.savedLastWasEffect = false;
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

                // Store in config and apply (one white value, split on RGBCCT)
                setPrimaryColorOneWhite(targetSegment, r, g, b, w);

                // Persist desired static color for power-restore
                {
                    SegmentConfig& cfg = _segments[channel];
                    cfg.savedR = r;
                    cfg.savedG = g;
                    cfg.savedB = b;
                    cfg.savedWW = w;
                    cfg.savedValid = true;
                    cfg.savedLastWasEffect = false;
                }

                break;
            }

            // Relative controls (DPT 3.007 - relative dimming)
            case NEO_KoBriRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                SegmentConfig& cfgRel = _segments[channel];
                // KNX dimmer on an off segment: dimming up switches on from the lowest level, down is
                // ignored. Under global OFF no ramp runs: dimming up only records the ON.
                if ((rel & 0x07) != 0 && (cfgRel.savedPower == 0 || cfgRel.globalSuspended))
                {
                    const bool up = (rel & 0x08) != 0;
                    if (cfgRel.globalSuspended)
                    {
                        if (up && cfgRel.savedPower == 0) segmentPowerOn((size_t)channel);
                        break;
                    }
                    if (!up) break;
                    cfgRel.savedBrightness = 1;
                    segmentPowerOn((size_t)channel);
                }
                _segmentController->startStopDimming(channel, SegmentConfig::BRIGHTNESS, rel);
                break;
            }

            case NEO_KoRRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                _segmentController->startStopDimming(channel, SegmentConfig::RED, rel);
                break;
            }

            case NEO_KoGRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                _segmentController->startStopDimming(channel, SegmentConfig::GREEN, rel);
                break;
            }

            case NEO_KoBRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                _segmentController->startStopDimming(channel, SegmentConfig::BLUE, rel);
                break;
            }

            case NEO_KoWRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                _segmentController->startStopDimming(channel, SegmentConfig::WHITE, rel);
                break;
            }

            case NEO_KoWWRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                _segmentController->startStopDimming(channel, SegmentConfig::WARM_WHITE, rel);
                break;
            }

            case NEO_KoCWRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                _segmentController->startStopDimming(channel, SegmentConfig::COOL_WHITE, rel);
                break;
            }

            case NEO_KoHRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                _segmentController->startStopDimming(channel, SegmentConfig::HUE, rel);
                break;
            }

            case NEO_KoSRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                _segmentController->startStopDimming(channel, SegmentConfig::SATURATION, rel);
                break;
            }

            case NEO_KoVRel:
            {
                uint8_t rel = ko.value(Dpt(5, 10));
                _segmentController->startStopDimming(channel, SegmentConfig::VALUE, rel);
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

                // Base on the stored H/S/V while it describes the primary colour (see processActiveDimming)
                SegmentConfig& cfgHsv = _segments[channel];
                uint8_t currentH = cfgHsv.currentH, currentS = cfgHsv.currentS, currentV = cfgHsv.currentV;
                {
                    uint8_t cr, cg, cb;
                    ColorHelper::hsvToRGB(currentH, currentS, currentV, cr, cg, cb);
                    if (!cfgHsv.hsvTracked || cr != currentR || cg != currentG || cb != currentB)
                        ColorHelper::rgbToHSV(currentR, currentG, currentB, currentH, currentS, currentV);
                }

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
                cfgHsv.currentH = (uint8_t)newH;
                cfgHsv.currentS = (uint8_t)newS;
                cfgHsv.currentV = (uint8_t)newV;
                cfgHsv.hsvTracked = true;
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
                // One white value: WW + CW on RGBCCT, as the W object and the W status define it
                const uint8_t currentWW = targetSegment->getConfig().ww(), currentCW = targetSegment->getConfig().cw();
                const bool dualWhite = _virtualStrip && _virtualStrip->hasDualWhiteChannel();
                uint8_t currentW = dualWhite ? (uint8_t)std::min<uint16_t>(255, (uint16_t)currentWW + currentCW) : currentWW;

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

                // Update config and apply. RGBCCT: the white step scales WW + CW and keeps their ratio
                // (from black split like the W object).
                if (dualWhite)
                {
                    const int16_t sum = (int16_t)currentWW + currentCW;
                    // Upper end like the white ramp: 255, or above it as far as no channel clips
                    const int16_t maxCh = std::max<int16_t>(currentWW, currentCW);
                    const int16_t upper = (sum > 255) ? std::max<int16_t>(sum, (int16_t)(255 * sum / maxCh)) : 255;
                    const int16_t newSum = constrain((int16_t)(sum + ((int16_t)wRel - 128)), 0, upper);
                    uint8_t newWW, newCW;
                    if (sum == 0)
                    {
                        newWW = (uint8_t)(newSum / 2);
                        newCW = (uint8_t)(newSum - newSum / 2);
                    }
                    else
                    {
                        newWW = (uint8_t)std::min<int16_t>(255, (int16_t)(((int32_t)currentWW * newSum + sum / 2) / sum));
                        newCW = (uint8_t)std::min<int16_t>(255, (int16_t)(newSum - newWW));
                    }
                    targetSegment->setPrimaryColor((uint8_t)newR, (uint8_t)newG, (uint8_t)newB, newWW, newCW);
                    logInfoP("Segment %d RGBW Relative: R=%d G=%d B=%d WW=%d CW=%d", channel, newR, newG, newB, newWW, newCW);
                }
                else
                {
                    targetSegment->setPrimaryColor((uint8_t)newR, (uint8_t)newG, (uint8_t)newB, (uint8_t)newW, 0);
                    logInfoP("Segment %d RGBW Relative: R=%d G=%d B=%d W=%d", channel, newR, newG, newB, newW);
                }
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

        // Status from the resulting state. Own status telegrams come back through this handler
        // (knx class callback) and must not trigger another sync.
        if (koIndex != NEO_KoSegmentPowerState && koIndex != NEO_KoSegmentBrightnessState &&
            koIndex != NEO_KoCCTState && koIndex != NEO_KoFxState && koIndex != NEO_KoSceneState &&
            koIndex != NEO_KoRGBState && koIndex != NEO_KoHSVState && koIndex != NEO_KoRGBWState &&
            koIndex != NEO_KoHCLState)
        {
            // saved* mirrors the live colour after every command (no-op while an EM runs)
            captureDirectColor((size_t)channel);
            statusAfterCommand(static_cast<uint8_t>(1 + channel));
        }

        // Auto-update cycle will render changes
    }
}

// ============================================================================
// Effektkette — Sync functions
// ============================================================================

/**
 * @brief Compose the 9-byte sync telegram of a master segment.
 *
 * Payload layout:
 *   Byte 0: Flags  (bit0 = power on/off)
 *   Byte 1: EffectType
 *   Byte 2: Speed
 *   Byte 3: R (primary colour)
 *   Byte 4: G
 *   Byte 5: B
 *   Byte 6: W  (warm white / single white)
 *   Byte 7: Brightness intent (before the global factor)
 *   Byte 8: CW (cool white, RGBCCT)
 *
 * While the master's own EM runs, bytes 1-6 and 8 carry the direct state frozen at the EM start
 * (without a freeze: saved effect, else the live one, saved colour, live speed); the power bit stays live.
 */
void NeoPixelBusModule::composeSyncPayload(size_t segmentIndex, uint8_t* payload)
{
    auto& cfg = _segments[segmentIndex];
    if (cfg.emController.isRunning())
    {
        if (cfg.syncFreezeValid)
        {
            memcpy(payload, cfg.syncFreezePayload, sizeof(cfg.syncFreezePayload));
        }
        else
        {
            // No capture: the console switched the segment to master while its EM runs.
            const auto& ecfg = cfg.segment->getConfig();
            payload[1] = cfg.savedEffectValid ? cfg.savedEffectType : ecfg.effectType;
            payload[2] = ecfg.speed;
            payload[3] = cfg.savedR;
            payload[4] = cfg.savedG;
            payload[5] = cfg.savedB;
            payload[6] = cfg.savedWW;
            payload[8] = (_virtualStrip && _virtualStrip->hasDualWhiteChannel()) ? cfg.savedCW : 0; // CW only on RGBCCT
        }
    }
    else
    {
        composeLiveSyncPayload(segmentIndex, payload);
    }
    payload[0] = isSegmentVisible(cfg) ? 0x01 : 0x00; // power
    payload[7] = cfg.savedBrightness;
}

void NeoPixelBusModule::composeLiveSyncPayload(size_t segmentIndex, uint8_t* payload)
{
    auto& cfg = _segments[segmentIndex];
    const auto& ecfg = cfg.segment->getConfig();
    payload[0] = isSegmentVisible(cfg) ? 0x01 : 0x00;
    payload[1] = ecfg.effectType;
    payload[2] = ecfg.speed;
    payload[3] = ecfg.r();
    payload[4] = ecfg.g();
    payload[5] = ecfg.b();
    payload[6] = ecfg.ww();
    payload[7] = cfg.savedBrightness;
    payload[8] = (_virtualStrip && _virtualStrip->hasDualWhiteChannel()) ? ecfg.cw() : 0; // CW only on RGBCCT
}

void NeoPixelBusModule::captureSyncFreeze(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (cfg.syncMode != 1 || !cfg.segment) return;
    composeLiveSyncPayload(segmentIndex, cfg.syncFreezePayload);
    cfg.syncFreezeValid = true;
}

void NeoPixelBusModule::sendSyncTelegram(size_t segmentIndex)
{
#ifdef NEO_KoSyncChain
    // KO macros beyond the configured channel table would address foreign KOs (no bounds check in knx).
    if (segmentIndex >= _segments.size() || segmentIndex >= NEO_ChannelCount) return;
    auto& cfg = _segments[segmentIndex];
    if (cfg.syncMode != 1 || !cfg.segment) return; // only masters send
    if (_drainGate) return; // startup backlog: loopSyncMaster() sends the final state afterwards

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
 *
 * Effect, colour, speed and brightness first, then the power bit. Policy 0 applies the master's
 * power as a level on every telegram, policy 1 only on a change (a local command overrides).
 */
void NeoPixelBusModule::receiveSyncTelegram(size_t segmentIndex,
                                            const uint8_t* payload, uint8_t len)
{
    if (segmentIndex >= _segments.size() || len < 9) return;
    auto& cfg = _segments[segmentIndex];
    if (cfg.syncMode != 2 || !cfg.segment) return; // only slaves receive

    // Any telegram proves the master is alive, also when it is not applied.
    cfg.lastSyncMs = millis();
    cfg.lastSyncRxMs = cfg.lastSyncMs | 1;
    memcpy(cfg.lastRxPayload, payload, sizeof(cfg.lastRxPayload)); // len >= 9 checked above
    cfg.rxPayloadValid = true;

    if (cfg.locked) return;
    // The slave's own EM (running, paused or power-suspended) owns the segment.
    if (cfg.emController.isRunning()) return;
    // Policy "Lokal hat Vorrang": a local command keeps the slave until reboot.
    if (cfg.syncOverridePolicy == 1 && cfg.localOverride) return;

    bool powerOn = (payload[0] & 0x01) != 0;
    uint8_t effectId = payload[1];
    uint8_t speed = payload[2];
    uint8_t r = payload[3], g = payload[4], b = payload[5];
    uint8_t w = payload[6], brightness = payload[7], cw = payload[8];
    // A slave without RGBCCT shows the master's white as one value (WW + CW, as the W status)
    if (!(_virtualStrip && _virtualStrip->hasDualWhiteChannel()))
    {
        w = (uint8_t)std::min<uint16_t>(255, (uint16_t)w + cw);
        cw = 0;
    }

    // Apply colour + speed to segment config
    auto& ecfg = cfg.segment->getConfig();
    ecfg.speed = speed;
    cfg.segment->setPrimaryColor(r, g, b, w, cw);

    // Apply effect only when it actually changed (re-applying resets the effect each telegram)
    bool effectChanged = !cfg.segment->getEffect() || getTypeFromEffect(cfg.segment->getEffect()) != effectId;
    if (effectChanged)
    {
        uint8_t oldCh = _channelIndex;
        _channelIndex = (uint8_t)segmentIndex;
        applyEffectToSegment(cfg.segment, effectId);
        if (cfg.segment->getEffect() && getTypeFromEffect(cfg.segment->getEffect()) == effectId)
        {
            cfg.savedEffectType = effectId;
            cfg.savedEffectValid = true;
            cfg.savedLastWasEffect = true;
        }
        // Effect selection loads either the configured ETS parameters or built-in defaults,
        // both of which include speed. The synchronized value is authoritative here.
        ecfg.speed = speed;
        // Colour was set before the effect clear — re-apply it
        cfg.segment->setPrimaryColor(r, g, b, w, cw);
        _channelIndex = oldCh;
        hideIfNotVisible(segmentIndex);
    }
    captureDirectColor(segmentIndex);

    // Brightness byte = master intent; the slave's own global factor applies on top.
    const uint8_t effective = (uint8_t)((uint16_t)brightness * _globalBrightness / 255);
    cfg.savedBrightness = brightness;
    cfg.segment->setMasterBrightness(effective);
    cfg.segment->setBrightness(effective);

    const bool applyPower = cfg.syncOverridePolicy == 0 || cfg.lastRxPower != (powerOn ? 1 : 0);
    cfg.lastRxPower = powerOn ? 1 : 0;
    if (applyPower)
    {
        if (powerOn && (cfg.savedPower == 0 || (!cfg.globalSuspended && cfg.segment->isPaused())))
        {
            segmentOn(segmentIndex);
            // segmentOn() lifts a 0 intent to full; the slave follows the master's value.
            cfg.savedBrightness = brightness;
            cfg.segment->setMasterBrightness(effective);
            cfg.segment->setBrightness(effective);
        }
        else if (!powerOn && (cfg.savedPower != 0 || !cfg.segment->isPaused()))
        {
            segmentOff(segmentIndex, true);
        }
    }

    logInfoP("Effektkette [%zu]: received sync power=%d effect=%d speed=%d bri=%d%s",
             segmentIndex, powerOn ? 1 : 0, effectId, speed, brightness, effectChanged ? " (effect applied)" : "");
}

/**
 * @brief Called every loop() iteration after the startup delay — master segments send a sync
 *        telegram on a state change (power, effect, speed, colour, brightness), at most one per
 *        kSyncMinGapMs, and repeat an unchanged payload every kSyncHeartbeatMs so a slave
 *        watchdog only fires on a real master loss.
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

        const bool changed = !cfg.syncPayloadValid || memcmp(payload, cfg.lastSyncPayload, sizeof(payload)) != 0;
        const uint32_t sinceLast = now - cfg.lastSyncSentMs;
        if (changed ? (cfg.syncPayloadValid && sinceLast < kSyncMinGapMs) : (sinceLast < kSyncHeartbeatMs))
            continue; // rate limit / heartbeat not due — retry next loop

        sendSyncTelegram(i);
    }
#endif
}

/**
 * @brief Called every loop() iteration after the startup delay — switches a slave off when its
 *        master stays silent longer than the configured timeout.
 */
void NeoPixelBusModule::loopSyncWatchdog()
{
    uint32_t now = millis();
    for (size_t i = 0; i < _segments.size(); i++)
    {
        auto& cfg = _segments[i];
        if (cfg.syncMode != 2 || !cfg.segment) continue; // only slaves
        if (cfg.syncTimeoutSteps == 0) continue;         // no timeout configured
        if (cfg.lastSyncMs == 0) continue;               // not armed (fired already)

        uint32_t timeoutMs = (uint32_t)cfg.syncTimeoutSteps * 10000u;
        if (now - cfg.lastSyncMs <= timeoutMs) continue;
        // Locked, own EM, or local override keeps the segment; fire once that ends.
        if (cfg.locked || cfg.emController.isRunning() || (cfg.syncOverridePolicy == 1 && cfg.localOverride)) continue;

        segmentOff(i, true);
        cfg.lastRxPower = 0xFF; // the next telegram applies its power again
        cfg.lastSyncMs = 0;     // reset so we don't keep clearing every frame
        queueStatus(static_cast<uint8_t>(1 + i), false);
        logInfoP("Effektkette slave timeout — segment turned off");
    }
}

// Console: show help entries by delegating to core NeoPixel module
void NeoPixelBusModule::showHelp()
{
    // Print the 'neo' command group header via core module
    _neoPixel.showHelp();
#ifdef ARDUINO_ARCH_ESP32
    openknx.console.printHelpLine("neo resetreason", "Show ESP32 reset cause and current uptime (read-only)");
#endif
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
    // A console command can change any segment; the queue reports what actually changed.
    if (_statusSeeded && command.rfind("neo", 0) == 0)
    {
        queueStatus(0, false);
        queueAllSegmentStatus(false);
    }
#ifdef ARDUINO_ARCH_ESP32
    if (command == "neo resetreason")
    {
        logEsp32ResetReason();
        return true;
    }
#endif
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
            _warnCodeBeforeAbl = 0;
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
            _warnCodeBeforeAbl = 0;
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
            _warnCodeBeforeAbl = 0;
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
    out.startupEm = (uint8_t)ParamNEO_NEOStartupEMAfterBoot;
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
            const bool emWasRunning = _segments[seg].emController.isRunning();
            stopEffektManager(seg);
            // A stopped EM-action scene is not replayed at boot; a static scene marker stays.
            if (emWasRunning && _sceneManager && _sceneManager->isEmActionScene((uint8_t)seg, _segments[seg].savedSceneNumber))
                _segments[seg].savedSceneNumber = 0;
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
            hideIfNotVisible(seg); // dark segment: the cue shows at the next ON
            sendEmStatusKOs(seg);
            return true;
        }
        case NEO_CHAIN_SET:
        {
            if (arg2 < 0 || arg2 > 2) return false;
            if (arg2 != 0 && seg >= NEO_ChannelCount)
            {
                openknxNeoPixelConsolePrintf("Segment %d has no Effektkette object (segments 0-%d)", (int)seg, NEO_ChannelCount - 1);
                return false;
            }
            auto& cfg = _segments[seg];
            cfg.syncMode = (uint8_t)arg2;
            cfg.localOverride = false;
            cfg.lastRxPower = 0xFF; // the next telegram applies its power again
            if (arg2 != 2) cfg.lastSyncMs = 0;
            return true;
        }
        case NEO_CHAIN_OVERRIDE:
        {
            if (arg2 != 0 && arg2 != 1) return false;
            _segments[seg].localOverride = (arg2 == 1);
            _segments[seg].lastRxPower = 0xFF;
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
    // Join the global power state: under global OFF the new segment starts suspended.
    cfg.globalSuspended = !_globalPowerOn;
    if (cfg.globalSuspended) seg->stop();
    _segments.push_back(cfg);
    const size_t idx = _segments.size() - 1;
    openknxNeoPixelConsolePrintf("bind: manager seg %d -> OAM segment %u  (now: neo em start %u <em>)",
                                 managerSegIdx, (unsigned)idx, (unsigned)idx);
    return (int)idx;
}

// FunctionProperty: Handle ETS online functions (hardware detection)
bool NeoPixelBusModule::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t* data, uint8_t* resultData, uint8_t& resultLength)
{
    // Not our hardware-detection object/property? Silently decline so another module can handle it
    // (e.g. the FileTransfer server on ObjectIndex 159). Do NOT log here: FunctionProperty is hit at
    // flood rate during an FTC upload, and a log PER FRAME stalls the blocking USB-CDC -> the loop
    // misses its watchdog feed -> the KNeoPix reboots mid-upload (same class as the router's TP print
    // storm). A non-matching FunctionProperty is normal dispatch, not an error.
    if (objectIndex != NEOPIXEL_FUNCTION_OBJECT_INDEX || propertyId != NEOPIXEL_FUNCTION_PROPERTY_ID)
        return false;

#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    if (length && data && data[0] == 0x20)
    {
        if (_studio && _initialized && openknx.afterStartupDelay()) return _studio->readback(length, data, resultData, resultLength);
        resultData[0] = 1;
        resultLength = 1;
        return true;
    }
#endif
    if (!data || length != 1 || data[0] != 0)
    {
        resultData[0] = 5;
        resultLength = 1;
        return true;
    }
    logDebugP("==> processFunctionProperty (hardware detection): ObjectIndex=%d, PropertyId=%d, Length=%d", objectIndex, propertyId, length);

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

uint16_t NeoPixelBusModule::configuredStripLength(uint8_t index) const
{
    if (_liveLedLayout && index < 8) return _liveStripLengths[index];
    const uint8_t _channelIndex = index;
    return ParamNEOSTRIP_NEOLength;
}

uint16_t NeoPixelBusModule::configuredStripSkip(uint8_t index) const
{
    if (_liveLedLayout && index < 8) return _liveStripSkips[index];
    const uint8_t _channelIndex = index;
    return ParamNEOSTRIP_NEOSkipFirstLEDs;
}

void NeoPixelBusModule::configureFromETS(bool physicalOnly)
{
    // Initialize the OFM-NeoPixel module first
    if (!_neoPixel.getManager()) _neoPixel.init();

// Log hardware configuration and check for mismatch
#ifdef DEVICE_HW_ID
    uint8_t hwIndex = getCurrentHardwareIndex();
    const char* hwName = hardwareNameForId(DEVICE_HW_ID);
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
        const char* compiledHwName = hardwareNameForId(DEVICE_HW_ID);
        const char* etsHwName = hardwareNameForId(selectedHwId);
        logErrorP("!!!! HARDWARE MISMATCH !!!!");
        logErrorP("  Compiled for:  %s (ID: 0x%04X)",
                  compiledHwName ? compiledHwName : "Unknown", DEVICE_HW_ID);
        logErrorP("  ETS configured: %s (ID: 0x%04X)",
                  etsHwName ? etsHwName : (selectedHwId == 255 ? "no hardware selected" : "other hardware"), selectedHwId);
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
    const char* hwName = hardwareNameForId(selectedHwId);
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

    // Helper lambda to check if a pin is already used
    auto isPinUsed = [&usedPins](uint8_t pin) {
        return std::find(usedPins.begin(), usedPins.end(), pin) != usedPins.end();
    };

    // CRITICAL SAFETY GUARD: Pins reserved by the KNX/BCU subsystem must NEVER be driven by a LED strip.
    // On KNeoPix XIAO hardware KNX_UART_TX_PIN == GPIO0 and KNX_UART_RX_PIN == GPIO1. A manual data
    // GPIO 0 (or any other KNX pin) would otherwise let a strip take over the KNX TX line,
    // which kills the TPUart bus initialization (endless "Try Initialize", never "BCU connected").
    // This check rejects such strips regardless of the configuration mode (manual / hardware port / auto).
    // The buttons and status LEDs of the board are reserved as well (a value < 0 = not present,
    // > 255 = IO expander pin, not a GPIO).
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
#if defined(PROG_BUTTON_PIN)
        if ((int)(PROG_BUTTON_PIN) >= 0 && (int)(PROG_BUTTON_PIN) <= 255 && pin == (uint8_t)(PROG_BUTTON_PIN)) return true;
#endif
#if defined(FUNC1_BUTTON_PIN)
        if ((int)(FUNC1_BUTTON_PIN) >= 0 && (int)(FUNC1_BUTTON_PIN) <= 255 && pin == (uint8_t)(FUNC1_BUTTON_PIN)) return true;
#endif
#if defined(FUNC2_BUTTON_PIN)
        if ((int)(FUNC2_BUTTON_PIN) >= 0 && (int)(FUNC2_BUTTON_PIN) <= 255 && pin == (uint8_t)(FUNC2_BUTTON_PIN)) return true;
#endif
#if defined(FUNC3_BUTTON_PIN)
        if ((int)(FUNC3_BUTTON_PIN) >= 0 && (int)(FUNC3_BUTTON_PIN) <= 255 && pin == (uint8_t)(FUNC3_BUTTON_PIN)) return true;
#endif
#if defined(PROG_LED_PIN)
        if ((int)(PROG_LED_PIN) >= 0 && (int)(PROG_LED_PIN) <= 255 && pin == (uint8_t)(PROG_LED_PIN)) return true;
#endif
#if defined(INFO_LED_PIN)
        if ((int)(INFO_LED_PIN) >= 0 && (int)(INFO_LED_PIN) <= 255 && pin == (uint8_t)(INFO_LED_PIN)) return true;
#endif
#if defined(INFO1_LED_PIN)
        if ((int)(INFO1_LED_PIN) >= 0 && (int)(INFO1_LED_PIN) <= 255 && pin == (uint8_t)(INFO1_LED_PIN)) return true;
#endif
#if defined(INFO2_LED_PIN)
        if ((int)(INFO2_LED_PIN) >= 0 && (int)(INFO2_LED_PIN) <= 255 && pin == (uint8_t)(INFO2_LED_PIN)) return true;
#endif
#if defined(INFO3_LED_PIN)
        if ((int)(INFO3_LED_PIN) >= 0 && (int)(INFO3_LED_PIN) <= 255 && pin == (uint8_t)(INFO3_LED_PIN)) return true;
#endif
#if defined(OPENKNX_SERIALLED_PIN)
        if ((int)(OPENKNX_SERIALLED_PIN) >= 0 && (int)(OPENKNX_SERIALLED_PIN) <= 255 && pin == (uint8_t)(OPENKNX_SERIALLED_PIN)) return true;
#endif
#if defined(OPENKNX_SERIALLED_POWER_PIN)
        if ((int)(OPENKNX_SERIALLED_POWER_PIN) >= 0 && (int)(OPENKNX_SERIALLED_POWER_PIN) <= 255 && pin == (uint8_t)(OPENKNX_SERIALLED_POWER_PIN)) return true;
#endif
#if defined(NEOPIX_LED_PIN)
        if ((int)(NEOPIX_LED_PIN) >= 0 && (int)(NEOPIX_LED_PIN) <= 255 && pin == (uint8_t)(NEOPIX_LED_PIN)) return true;
#endif
#if defined(NEOPIX_LED_POWER_PIN)
        if ((int)(NEOPIX_LED_POWER_PIN) >= 0 && (int)(NEOPIX_LED_POWER_PIN) <= 255 && pin == (uint8_t)(NEOPIX_LED_POWER_PIN)) return true;
#endif
        return false;
    };

    // GPIO of one strip signal from its ETS port selection (0-9 port, 10 manual, else not selected);
    // 255 = no GPIO, the strip stays off. log = false for the pre-scan, which runs before the build.
    auto resolveStripGpio = [&](uint8_t stripIdx, const char* signal, uint8_t portSelection, uint8_t manualGpio, bool log) -> uint8_t {
        switch (classifyPortSelection(portSelection))
        {
            case PortSelectionKind::Manual:
                return manualGpio;
            case PortSelectionKind::HardwarePort:
            {
                if (_hwConfigMismatch)
                {
                    if (log) logErrorP("Strip %d: %s port ignored - hardware mismatch, select the GPIO manually", stripIdx, signal);
                    return 255;
                }
                const uint8_t gpio = mapPortIndexToGpio(portSelection);
                if (gpio == 255 && log) logErrorP("Strip %d: %s port %d does not exist on this hardware", stripIdx, signal, portSelection + 1);
                return gpio;
            }
            default:
                if (log) logWarningP("Strip %d: no %s port selected in ETS (value %d) - strip disabled", stripIdx, signal, portSelection);
                return 255;
        }
    };

    // Configure external relays (max derived from knxprod.h) and reserve their GPIO pins
    if (!physicalOnly)
    {
        _relayCount = std::min<uint8_t>(kMaxExternalRelays, ParamNEO_NEOExternalRelayCount);
        for (uint8_t i = 0; i < kRelayStorageSize; ++i)
        {
            _relayPins[i] = 255;
            _relayStates[i] = false;
            _relayInverted[i] = false;
            _relayBeforeRestart[i] = kRelayBeforeAsLeds;
            _relayStartup[i] = kRelayStartupLast;
        }
        _relayHeldMask = 0;

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

                    if (isReservedPin(gpio))
                    {
                        logErrorP("Relay %d: REFUSED - GPIO %d is a reserved KNX/button/LED pin", i + 1, gpio);
                        _relayPins[i] = 255;
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

    // Read output logic (invert) setting per relay; needed before the first write below
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

    // Restart behaviour per relay; cached because the parameters read 0 once the tables unload.
    // A knxprod.h without them is stale: fail the build instead of silently using value 0.
    #if defined(ParamNEO_NEOExternalRelay1BeforeRestart) && defined(ParamNEO_NEOExternalRelay1AfterRestart)
                    if (i == 0) { _relayBeforeRestart[i] = (uint8_t)ParamNEO_NEOExternalRelay1BeforeRestart; _relayStartup[i] = (uint8_t)ParamNEO_NEOExternalRelay1AfterRestart; }
    #elif defined(NEO_KoExternalRelay1)
        #error "knxprod.h lacks the restart parameters of relay 1 - regenerate it (Build-HardwareConfig.ps1, OpenKNXproducer)"
    #endif
    #if defined(ParamNEO_NEOExternalRelay2BeforeRestart) && defined(ParamNEO_NEOExternalRelay2AfterRestart)
                    if (i == 1) { _relayBeforeRestart[i] = (uint8_t)ParamNEO_NEOExternalRelay2BeforeRestart; _relayStartup[i] = (uint8_t)ParamNEO_NEOExternalRelay2AfterRestart; }
    #elif defined(NEO_KoExternalRelay2)
        #error "knxprod.h lacks the restart parameters of relay 2 - regenerate it (Build-HardwareConfig.ps1, OpenKNXproducer)"
    #endif
    #if defined(ParamNEO_NEOExternalRelay3BeforeRestart) && defined(ParamNEO_NEOExternalRelay3AfterRestart)
                    if (i == 2) { _relayBeforeRestart[i] = (uint8_t)ParamNEO_NEOExternalRelay3BeforeRestart; _relayStartup[i] = (uint8_t)ParamNEO_NEOExternalRelay3AfterRestart; }
    #elif defined(NEO_KoExternalRelay3)
        #error "knxprod.h lacks the restart parameters of relay 3 - regenerate it (Build-HardwareConfig.ps1, OpenKNXproducer)"
    #endif
    #if defined(ParamNEO_NEOExternalRelay4BeforeRestart) && defined(ParamNEO_NEOExternalRelay4AfterRestart)
                    if (i == 3) { _relayBeforeRestart[i] = (uint8_t)ParamNEO_NEOExternalRelay4BeforeRestart; _relayStartup[i] = (uint8_t)ParamNEO_NEOExternalRelay4AfterRestart; }
    #elif defined(NEO_KoExternalRelay4)
        #error "knxprod.h lacks the restart parameters of relay 4 - regenerate it (Build-HardwareConfig.ps1, OpenKNXproducer)"
    #endif
                    // Value 3 is not offered: treat it like 0
                    if (_relayBeforeRestart[i] > kRelayBeforeKeep) _relayBeforeRestart[i] = kRelayBeforeAsLeds;
                    if (_relayStartup[i] > kRelayStartupOn) _relayStartup[i] = kRelayStartupLast;
                    logInfoP("Relay %d: before restart %s, after restart %s", i + 1,
                             _relayBeforeRestart[i] == kRelayBeforeKeep ? "unchanged" : (_relayBeforeRestart[i] == kRelayBeforeOff ? "off" : "as LED option"),
                             _relayStartup[i] == kRelayStartupOff ? "off" : (_relayStartup[i] == kRelayStartupOn ? "on" : "last state"));

                    pinMode(gpio, OUTPUT);
                    digitalWrite(gpio, _relayInverted[i] ? HIGH : LOW); // OFF until the startup restore
                    _relayStates[i] = false;
                    // OFF from here: the minimum off-time of an ON (restore or object) counts from setup
                    const unsigned long offSince = millis();
                    _relayTimers[i].lastOffTime = offSince ? offSince : 1;
                    logInfoP("Relay %d: Initial state OFF", i + 1);
                }
            }
        }

    }
    else
    {
        for (uint8_t i = 0; i < _relayCount; ++i)
            if (_relayPins[i] != 255) usedPins.push_back(_relayPins[i]);
    }

    // Determine number of strips from ETS (match the runtime strip limit).
    const uint8_t maxStrips = std::max<uint8_t>(1, std::min<uint8_t>(NEOPIXEL_MAX_PHYSICAL_STRIPS, ParamNEO_NEONumberOfLEDStrips));
    _totalLeds = 0;

    // CRITICAL: Delete old PhysicalStrip objects before clearing vector!
    // This prevents memory leaks and deregisters old DMA handlers
    for (auto* strip : _physicalStrips)
    {
        if (strip) delete strip;
    }
    _physicalStrips.clear();
    _physicalStripEtsIds.clear();

    // Clear virtual strip configuration
    _virtualStripConfiguration.clear();

    // PRE-SCAN: mark the GPIOs of all strips (hardware port or manual) as used before any strip is built,
    // so a pin conflict between strips is reported
    logInfoP("Pre-scanning %d strips for their GPIOs...", maxStrips);
    for (uint8_t i = 0; i < maxStrips; ++i)
    {
        // CRITICAL: Set module _channelIndex so ParamNEOSTRIP_* macros read from correct strip
        setChannelIndex(i);
        logInfoP("PRE-SCAN Strip %d: Set _channelIndex = %d", i, _channelIndex);

        if (physicalOnly && _livePinKnown[i])
        {
            usedPins.push_back(_liveDataPins[i]);
            if (_liveClockPins[i] != 255) usedPins.push_back(_liveClockPins[i]);
            continue;
        }

        const uint16_t pixels = configuredStripLength(_channelIndex);
        logInfoP("PRE-SCAN Strip %d: Read pixels = %d (from offset %d)", i, pixels, NEOSTRIP_ParamBlockOffset + _channelIndex * NEOSTRIP_ParamBlockSize + NEOSTRIP_NEOLength);
        if (pixels == 0) continue; // Skip strips with 0 LEDs

        const uint8_t ledTypeParam = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const LedProtocol proto = StripConfiguration::mapProtocol(ledTypeParam);
        // Reserve the strip's GPIOs (hardware port or manual) before any strip is built
        auto preMark = [&](const char* signal, uint8_t gpio) {
            if (gpio == 255) return;
            if (!isPinUsed(gpio))
            {
                usedPins.push_back(gpio);
                logInfoP("Strip %d: Pre-marked %s GPIO %d as used", i, signal, gpio);
            }
            else
            {
                logWarningP("Strip %d: %s GPIO %d already marked as used!", i, signal, gpio);
            }
        };
        const uint8_t dataPortSelection = getDataPortSelectionForHardwareIndex(i);
        if (isSpiProtocol(proto))
        {
            preMark("SCK", resolveStripGpio(i, "clock", getClockPortSelectionForHardwareIndex(i), (uint8_t)ParamNEOSTRIP_NEOClockGPIO, false));
            preMark("MOSI", resolveStripGpio(i, "data", dataPortSelection, (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO, false));
        }
        else
        {
            preMark("Data", resolveStripGpio(i, "data", dataPortSelection, (uint8_t)ParamNEOSTRIP_NEODataGPIO, false));
        }
    }
    logInfoP("Pre-scan complete: %d GPIO pins marked as used", (int)usedPins.size());

    // 2) Create all physical strips in configuration order
    for (uint8_t i = 0; i < maxStrips; ++i)
    {
        // Set channel index for strip parameter access
        // ParamNEOSTRIP_* macros use _channelIndex to calculate correct EEPROM address
        _channelIndex = i;

        const uint8_t ledTypeParam = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const LedProtocol proto = StripConfiguration::mapProtocol(ledTypeParam);

        // Get color order from ETS setting for all protocols
        ColorOrder order = StripConfiguration::mapColorOrder((uint8_t)ParamNEOSTRIP_NEOColourOrder);
        // "Chip-Vorgabe": the driver knows the native order per protocol - TM1814 for
        // example ships W first. Without this ETS always overwrites it.
        if (order == ColorOrder::NONE)
            order = ProtocolHelper::getColorOrder(proto);
        const uint8_t ledType = (uint8_t)ParamNEOSTRIP_NEOLEDType;
        const uint8_t dataGpio = (uint8_t)ParamNEOSTRIP_NEODataGPIO;
        const uint16_t pixels = configuredStripLength(_channelIndex);

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

            if (physicalOnly && _livePinKnown[i])
            {
                mosiGpio = _liveDataPins[i];
                sckGpio = _liveClockPins[i];
            }
            else
            {
                // Data Port -> MOSI, Clock Port -> SCK; each "Manuell" uses its own manual GPIO parameter
                mosiGpio = resolveStripGpio(i, "data", getDataPortSelectionForHardwareIndex(i), (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO, true);
                sckGpio = resolveStripGpio(i, "clock", getClockPortSelectionForHardwareIndex(i), (uint8_t)ParamNEOSTRIP_NEOClockGPIO, true);
                if (mosiGpio == 255 || sckGpio == 255) continue; // strip stays off, logged above
                logInfoP("Strip %d: SPI pins MOSI:%d, SCK:%d", i, mosiGpio, sckGpio);
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
                logErrorP("Strip %d: REFUSED - SPI GPIO(s) collide with reserved KNX/button/LED pins (MOSI=%d, SCK=%d). Strip disabled.",
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
                    uint16_t skipLeds = configuredStripSkip(_channelIndex);

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
                     spiFrequency, classifyPortSelection(getDataPortSelectionForHardwareIndex(i)) == PortSelectionKind::Manual ? " (Manual)" : " (Port)");
        }
        else
        {
            // 1-Wire protocols use Data GPIO
            uint8_t dataGpioPin;

            if (physicalOnly && _livePinKnown[i]) dataGpioPin = _liveDataPins[i];
            else
            {
                dataGpioPin = resolveStripGpio(i, "data", getDataPortSelectionForHardwareIndex(i), dataGpio, true);
                if (dataGpioPin == 255) continue; // strip stays off, logged above
                logInfoP("Strip %d: Data GPIO %d", i, dataGpioPin);
            }

            // SAFETY: never drive KNX/system-reserved pins (e.g. GPIO0/1 = KNX TX/RX on KNeoPix)
            if (isReservedPin(dataGpioPin))
            {
                logErrorP("Strip %d: REFUSED - Data GPIO %d collides with a reserved KNX/button/LED pin. Strip disabled.",
                          i, dataGpioPin);
                continue; // Skip this strip completely
            }

            phys = _neoPixel.addStrip(dataGpioPin, pixels, proto, order);
            logInfoP("1-Wire Strip %d: %d LEDs, GPIO=%d, Protocol=%s, ColorOrder=%s%s",
                     i, pixels, dataGpioPin, getProtocolName(proto), getColorOrderName(order),
                     classifyPortSelection(getDataPortSelectionForHardwareIndex(i)) == PortSelectionKind::Manual ? " (Manual)" : " (Port)");
            if (phys)
            {
                auto* cfg = phys->getConfig();
                if (cfg)
                {
                    uint16_t skipLeds = configuredStripSkip(_channelIndex);
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
            // The ETS voltage is part of the strip's power model; applying it
            // here (the active configuration path) keeps current limiting and
            // monitoring independent of the legacy StripConfiguration copy.
            const uint8_t voltageParam = (uint8_t)ParamNEOSTRIP_NEOVoltage;
            const uint8_t voltage = voltageParam == 1 ? 12 : voltageParam == 2 ? 24 : 5;
            phys->setVoltage(voltage);
            logInfoP("Strip %d: Supply voltage set to %u V", i, voltage);

            _totalLeds += pixels;
            _physicalStrips.push_back(phys);
            _physicalStripEtsIds.push_back(i);

            // ETS "Timing": value 0 means "use the chip profile" and leaves the driver to
            // resolve the protocol itself. Any other value is an expert bit-rate override
            // that SCALES that profile, keeping the chip's pulse ratios.
            const uint8_t timingSel = (uint8_t)ParamNEOSTRIP_NEOTiming & 0x0F;

            if (NeoPixelTimingSelection::isProtocolDefault(timingSel))
            {
                const SerialTiming::Profile prof = SerialTiming::profileFor(proto);
                if (prof.t1h > 0)
                    logInfoP("Strip %d: Timing = chip profile (T0H=%u ns, T1H=%u ns, latch=%u us)",
                             i, prof.t0h, prof.t1h, (unsigned)prof.resetUs);
            }
            else
            {
                const uint16_t freqKhz = NeoPixelTimingSelection::bitrateKhz(timingSel);
                SerialTiming::Profile base = SerialTiming::profileFor(proto);
                if (base.t1h == 0) base = SerialTiming::profileFor(LedProtocol::WS2812B);
                const SerialTiming::Profile bent = SerialTiming::scaledTo(base, (uint32_t)freqKhz * 1000UL);
                if (phys->setCustomTiming(bent.t0h, bent.t0l, bent.t1h, bent.t1l, bent.resetUs))
                    logInfoP("Strip %d: Timing override = %u kHz (T0H=%u ns, T1H=%u ns)",
                             i, freqKhz, bent.t0h, bent.t1h);
            }

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

                // Per-strip channel swap. Was read into a module-wide field that nothing
                // applied, so the last strip's value would have decided for all of them.
                cfg->setChannelSwap((uint8_t)ParamNEOSTRIP_NEOSwap & 0x07);

                // 0 keeps the chip profile; 1 and 2 force normal or inverted, which is the
                // case for an inverting level shifter the profile cannot know about.
                cfg->setSignalPolarity((uint8_t)ParamNEOSTRIP_NEOSignalPolarity & 0x03);
                cfg->setOffRefresh((bool)ParamNEOSTRIP_NEOOffRefresh);
                cfg->setWhiteMode((uint8_t)ParamNEOSTRIP_NEOWhiteMode & 0x01);

                // Per-strip automatic brightness limiting. ETS offers these alongside the
                // limit itself, but only the unused twin in StripConfiguration.cpp read them,
                // so the values shown in ETS never reached the strip.
                if (powerMode == 2 || powerMode == 3)
                {
                    uint8_t autoBrLimit = (uint8_t)ParamNEOSTRIP_NEOautoBrightnessLimit;
                    uint8_t threshold = (uint8_t)ParamNEOSTRIP_NEOpowerLimitThreshold;
                    uint8_t slewRate = (uint8_t)ParamNEOSTRIP_NEOablSlewRatePercent;
                    cfg->setAutoBrightnessLimit(autoBrLimit);
                    cfg->setPowerLimitThreshold(threshold);
                    cfg->setAblSlewRate(slewRate);
                    logInfoP("Strip %d: ABL limit %d%%, threshold %d%%, slew %d%%",
                             i, autoBrLimit, threshold, slewRate);
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

    if (physicalOnly) return;

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
            //   - Mode "Aus": own power OFF, segment stopped
            //   - Mode "Letzter Zustand": Will be restored from flash in processAfterStartupDelay()
            //   - Mode "An (ETS-Werte)": Active with ETS configuration
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
        if (physStripIndex == 0) continue;
        const auto slot = std::find(_physicalStripEtsIds.begin(), _physicalStripEtsIds.end(), physStripIndex - 1);
        if (slot == _physicalStripEtsIds.end()) continue; // disabled or failed ETS slot
        physStripIndex = static_cast<uint8_t>(slot - _physicalStripEtsIds.begin());

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
        _channelIndex = _physicalStripEtsIds[config.physicalStripIndex];
        uint16_t skipLeds = configuredStripSkip(_channelIndex);
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
        case LedProtocol::WS2811_400KHZ: return "WS2811/WS2812 400kHz";
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

/**
 * @brief LED current profile used to measure the strips: the ETS "Strom pro LED" split per channel
 *        count, or the default profile of the strip type.
 * @note Also used while global limiting is disabled, so a strip with its own limit is measured the same way.
 */
LedCurrentProfile NeoPixelBusModule::powerMeasureProfile(uint8_t currentPerLed, bool perLedMode) const
{
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

    if (currentPerLed > 0 && !perLedMode)
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
    return profile;
}

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

        // Hand the raw ETS value to the manager: it splits it per protocol, so a
        // 4-channel and a 5-channel strip on the same device are each measured right.
        powerManager->setCurrentPerLedMa(currentPerLed);
        const LedCurrentProfile profile = powerMeasureProfile(currentPerLed, mode == PowerLimitMode::PER_LED);
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
        // Strips with their own limit are still limited: measure them like with global limiting
        const uint8_t currentPerLed = (uint8_t)ParamNEO_NEOcurrentPerLED;
        powerManager->setCurrentPerLedMa(currentPerLed);
        powerManager->setLedProfile(powerMeasureProfile(currentPerLed, false));
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
    _skipFirstLeds = configuredStripSkip(_channelIndex);
    if (_skipFirstLeds > 0)
    {
        logInfoP("Skip first LEDs configured: %d LEDs (handled by OFM-NeoPixel)", _skipFirstLeds);
    }
}

// Map ETS swap parameter to swap mode
uint8_t NeoPixelBusModule::mapSwapMode(uint8_t paramValue)
{
    // Values follow the NEOSwap enum in NeoPixel.share.xml:
    // 0 = none, 1 = W and B, 2 = W and G, 3 = W and R, 4 = WW and CW.
    // Applied per strip via PhysicalStripConfig::setChannelSwap().
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
    uint16_t etsStartLed = _liveLedLayout ? _liveSegmentStarts[segmentIndex] : ParamNEO_NEOSegmentStart;
    uint16_t etsEndLed = _liveLedLayout ? _liveSegmentEnds[segmentIndex] : ParamNEO_NEOSegmentEnd;

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
    // "Mein Offset" is a slave parameter (hidden for a master, whose band starts at 0)
    config.virtualOffset = (config.syncMode == 2) ? (uint16_t)ParamNEO_NEOVirtualOffset : 0;
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

        // The segment covers Start..End; "Offset" only rotates the pixels inside it (setOffset)
        uint16_t effectiveStart = config.startLed;
        uint16_t effectiveEnd = config.endLed;

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

        // Apply offset: rotate the pixel mapping inside the segment with wrap-around
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
        // A channel not active in the LightManager channel selection has no master; its
        // getCurrentValue() is the 4000 K default, so it stays kelvin 0 / blocked here.
        if (masterNum <= masterCount && HCL::masterManager.getMaster(masterNum) != nullptr)
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

        logDebugP("HCL master %u: kelvin=%u brightness=%u blocked=%d kelvinRgb=(%u,%u,%u)",
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

    // Warn once per master that an HCL source references but the LightManager does not run.
    static uint16_t warnedInactiveMasters = 0;
    auto warnInactiveMaster = [this](uint8_t master) {
        if (master == 0 || master > 16) return;
        const uint16_t bit = static_cast<uint16_t>(1u << (master - 1));
        if ((warnedInactiveMasters & bit) != 0) return;
        if (HCL::masterManager.getMaster(master) != nullptr) return;
        warnedInactiveMasters |= bit;
        logWarningP("HCL: LightManager channel %u is not active (channel selection) - no HCL applied", master);
    };
    warnInactiveMaster(_hclTransformContext.globalHclConfig.resolvedMaster);
    for (const NeoHclSegmentConfig& segCfg : _hclTransformContext.segmentConfigs)
    {
        if (segCfg.hclMode == NeoHclMode::Custom) warnInactiveMaster(segCfg.customHclConfig.resolvedMaster);
    }

    uint16_t globalKelvin = 0;
    if (_hclTransformContext.globalHclConfig.resolvedMaster > 0)
    {
        globalKelvin = _hclTransformContext.masterStates[_hclTransformContext.globalHclConfig.resolvedMaster - 1].kelvin;
    }

    // Global HCL Kelvin has its own status KO (407)
    if (globalKelvin != _hclGlobalStatusKelvin)
    {
        _hclGlobalStatusKelvin = globalKelvin;
        queueStatus(0, false);
    }

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

        if (index < kStatusSegmentMax && segmentKelvin != _hclSegmentStatusKelvin[index])
        {
            _hclSegmentStatusKelvin[index] = segmentKelvin;
            queueStatus(static_cast<uint8_t>(1 + index), false);
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

        // Physical range: getLength() is the grouped length or the Effektkette band length
        const uint16_t startLed = seg->getStartLed();
        const uint32_t endLed = (uint32_t)startLed + seg->getPhysicalLength();

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

#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    if (_studio && _studio->_testSegment >= 0)
    {
        _virtualStrip->setPixelTransformCallback([](uint16_t pixel, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t* ww, uint8_t* cw, void* context) {
            auto& module = *static_cast<NeoPixelBusModule*>(context);
            if (!module._studio->transformTestPixel(pixel, r, g, b, ww, cw))
                HclPixelTransform::Callback(pixel, r, g, b, ww, cw, &module._hclTransformContext);
        }, this);
    }
    else
#endif
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
    const char* hwName = hardwareNameForId(DEVICE_HW_ID);
    logInfoP("  Mode:             Compile-time (DEVICE_HW_ID defined)");
    logInfoP("  Device HW ID:     0x%04X (%d decimal)", DEVICE_HW_ID, DEVICE_HW_ID);
    logInfoP("  Hardware Index:   %d", hwIndex);
    logInfoP("  Hardware Name:    %s", hwName ? hwName : "Unknown");
    #else
        #ifdef ParamNEO_NeoPixelHardwareSelect
    uint16_t selectedHwId = (uint16_t)ParamNEO_NeoPixelHardwareSelect; // stored selection = DEVICE_HW_ID
    uint8_t hwIndex = getCurrentHardwareIndex();
    const char* hwName = hardwareNameForId(selectedHwId);
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

    // Global Startup Behavior (segments use their own ETS configuration for "An")
    uint8_t startupBehavior = (uint8_t)ParamNEO_NEOGlobalStartupBehavior;
    const char* startupBehaviorNames[] = {"Aus", "Letzter Zustand", "An (ETS-Werte)"};
    const char* startupBehaviorName = (startupBehavior < 3) ? startupBehaviorNames[startupBehavior] : "Unknown";
    logInfoP("  Startup Behavior: %d (%s)", startupBehavior, startupBehaviorName);
    logInfoP("");

    // Show each physical strip with ETS and actual values
    for (size_t i = 0; i < _physicalStrips.size(); i++)
    {
        PhysicalStrip* strip = _physicalStrips[i];
        if (!strip) continue;

        // Set _channelIndex to read ETS parameters for this strip
        uint8_t _channelIndex = _physicalStripEtsIds[i];

        logInfoP("  ┌─ Strip %d:", i + 1);
        logInfoP("  │");

        // === ETS Configuration (Raw Parameters) ===
        logInfoP("  │  📋 ETS Configuration:");
        logInfoP("  │    LED Count:        %d", configuredStripLength(_channelIndex));

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

        // GPIO selection per signal: ETS port value 0-9 = hardware port, 10 = manual GPIO, else not selected
        auto showPort = [&](const char* label, uint8_t portSelection, uint8_t manualGpio) {
            switch (classifyPortSelection(portSelection))
            {
                case PortSelectionKind::HardwarePort:
                    logInfoP("  │    %-18s port %d -> GPIO %d", label, portSelection + 1, mapPortIndexToGpio(portSelection));
                    break;
                case PortSelectionKind::Manual:
                    logInfoP("  │    %-18s manual GPIO %d", label, manualGpio);
                    break;
                default:
                    logInfoP("  │    %-18s not selected (value %d)", label, portSelection);
                    break;
            }
        };
        if (isSpiProtocol(strip->getProtocol()))
        {
            showPort("Data (MOSI):", getDataPortSelectionForHardwareIndex(_channelIndex), (uint8_t)ParamNEOSTRIP_NEOSPIMOSIGPIO);
            showPort("Clock (SCK):", getClockPortSelectionForHardwareIndex(_channelIndex), (uint8_t)ParamNEOSTRIP_NEOClockGPIO);
            if (ParamNEOSTRIP_NEOSPICLKMode == 1)
            {
                const uint8_t spiClkValue = (uint8_t)ParamNEOSTRIP_NEOSPICLK;
                const char* spiFreqNames[] = {"1 MHz", "2 MHz", "4 MHz", "8 MHz", "12 MHz", "16 MHz", "20 MHz"};
                logInfoP("  │    SPI Frequency:    %d (%s)", spiClkValue, spiClkValue < 7 ? spiFreqNames[spiClkValue] : "10 MHz");
            }
            else
            {
                logInfoP("  │    SPI Frequency:    Auto (10 MHz)");
            }
        }
        else
        {
            showPort("Data:", getDataPortSelectionForHardwareIndex(_channelIndex), (uint8_t)ParamNEOSTRIP_NEODataGPIO);
        }

        // Timing (bitrate): ETS value -> kHz, from the single persisted-value mapping.
        uint8_t timingSel = (uint8_t)ParamNEOSTRIP_NEOTiming & 0x0F;
        logInfoP("  │    Timing:           value %d = %u kHz", timingSel,
                 NeoPixelTimingSelection::bitrateKhz(timingSel));

        // Skip First LEDs
        uint16_t skipLeds = configuredStripSkip(_channelIndex);
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

/** @brief Load in percent of the limit, clamped to 100 (DPT 5.001 range). */
static uint8_t loadPercentClamped(uint32_t currentMa, uint32_t limitMa)
{
    if (limitMa == 0) return 0;
    const uint32_t pct = (currentMa * 100) / limitMa;
    return (uint8_t)(pct > 100 ? 100 : pct);
}

/**
 * @brief End an active ABL warning; the warning it took over is shown again (errors stay untouched).
 */
void NeoPixelBusModule::clearAblWarning()
{
    // Drop a latch from before the pause, so resuming does not report a stale limit
    if (auto* mgr = _neoPixel.getManager()) (void)mgr->takePowerLimitSeen();
    if (_activeWarnCode != NEO_WARN_ABL_ACTIVE) return;
    // The warning the ABL warning took over (if any) is shown again
    _activeWarnCode = _warnCodeBeforeAbl;
    _warnCodeBeforeAbl = 0;
    logInfoP("NeoPixel warning cleared: power limiting no longer active");
    auto* led = openknx.ledFunctions.get(OPENKNX_LEDFUNC_BASE_STATE);
    if (!led || _activeErrorCode != 0) return;
    if (_activeWarnCode != 0)
    {
        led->color(warnColorForCode(_activeWarnCode));
        led->pulsing();
    }
    else
    {
        led->off();
    }
}

void NeoPixelBusModule::sendPowerMonitoringKOs(bool dark)
{
    auto* mgr = _neoPixel.getManager();
    if (!mgr) return;

    // A round that ran out of status credits continues at its next KO, so no KO is starved
    _powerKoStart = (_powerKoCount > 0 && _powerKoDone < _powerKoCount)
                        ? (uint8_t)((_powerKoStart + _powerKoDone) % _powerKoCount)
                        : 0;
    _powerKoCount = 0;
    _powerKoDone = 0;
    _powerKoDark = dark;

    bool isEnabled = mgr->getPowerManager()->isEnabled();

    // ABL warning: on if any frame since the last sample was limited, off after a sample interval without
    if (mgr->takePowerLimitSeen())
    {
        // ABL is the most severe warning: it takes over a boot warning, which returns when ABL ends
        if (_activeWarnCode != NEO_WARN_ABL_ACTIVE)
        {
            const uint8_t before = _activeWarnCode;
            setWarningBlink(NEO_WARN_ABL_ACTIVE); // pulsing Orange (color from warnColorForCode)
            if (_activeWarnCode == NEO_WARN_ABL_ACTIVE) _warnCodeBeforeAbl = before;
        }
    }
    else
    {
        clearAblWarning();
    }

    // Global limiting disabled: the totals report 0; strips with their own limit are still limited
    // by the manager and report their objects
    if (!isEnabled)
    {
        queuePowerKo(KoNEO_PowerCurrentTotal, (uint32_t)0, DPT_UElCurrentmA, powerCurrentMinDelta(0));
        queuePowerKo(KoNEO_PowerLoadTotal, (uint8_t)0, DPT_Scaling, kPowerLoadMinDelta);
        for (size_t i = 0; i < _physicalStrips.size() && i < NEOPIXEL_MAX_PHYSICAL_STRIPS; i++)
        {
            auto* phys = _physicalStrips[i];
            auto* cfg = phys ? phys->getConfig() : nullptr;
            if (!cfg) continue;
            const uint8_t stripMode = cfg->getPowerLimitMode();
            if (stripMode != 2 && stripMode != 3) continue; // objects exist only for an own limit

            uint32_t stripCurrent = 0;
            uint32_t stripLimit = 0;
            uint8_t stripLoad = 0;
            if (!mgr->getStripPowerStats(phys, stripCurrent, stripLimit, stripLoad) && !_powerKoDark) continue; // not measured yet

            const uint8_t savedChannelIndex = _channelIndex;
            _channelIndex = i;
            queuePowerKo(KoNEOSTRIP_PowerCurrent, stripCurrent, DPT_UElCurrentmA, powerCurrentMinDelta(stripLimit));
            queuePowerKo(KoNEOSTRIP_PowerLoad, stripLoad, DPT_Scaling, kPowerLoadMinDelta);
            queuePowerKo(KoNEOSTRIP_PowerWatts, phys->getPowerWatts(stripCurrent), DPT_Value_Power, kPowerWattsMinDelta);
            _channelIndex = savedChannelIndex;
        }
        return;
    }

    PowerLimitMode mode = mgr->getPowerManager()->getPowerLimitMode();

    if (mode == PowerLimitMode::GLOBAL)
    {
        // GLOBAL MODE: Send global + per-strip KOs
        uint32_t currentMa, limitMa;
        uint8_t loadPercent;
        mgr->getGlobalPowerStats(currentMa, limitMa, loadPercent);

        // Global KOs
        queuePowerKo(KoNEO_PowerCurrentTotal, currentMa, DPT_UElCurrentmA, powerCurrentMinDelta(limitMa));
        queuePowerKo(KoNEO_PowerLoadTotal, loadPercent, DPT_Scaling, kPowerLoadMinDelta);

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

        queuePowerKo(KoNEO_PowerWattsTotal, totalWatts, DPT_Value_Power, kPowerWattsMinDelta);

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
                stripLimit = limit;
                if (limit > 0)
                    stripLoad = loadPercentClamped(stripCurrent, limit);
            }

            float stripWatts = phys->getPowerWatts(stripCurrent);

            // Set channel index to access correct strip KOs
            uint8_t savedChannelIndex = _channelIndex;
            _channelIndex = i;

            queuePowerKo(KoNEOSTRIP_PowerCurrent, stripCurrent, DPT_UElCurrentmA, powerCurrentMinDelta(stripLimit));
            queuePowerKo(KoNEOSTRIP_PowerLoad, stripLoad, DPT_Scaling, kPowerLoadMinDelta);
            queuePowerKo(KoNEOSTRIP_PowerWatts, stripWatts, DPT_Value_Power, kPowerWattsMinDelta);

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

        queuePowerKo(KoNEO_PowerCurrentTotal, currentMa, DPT_UElCurrentmA, powerCurrentMinDelta(limitMa));
        queuePowerKo(KoNEO_PowerLoadTotal, loadPercent, DPT_Scaling, kPowerLoadMinDelta);

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

        queuePowerKo(KoNEO_PowerWattsTotal, totalWatts, DPT_Value_Power, kPowerWattsMinDelta);

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
                limitMa = stripLimit;
                if (stripLimit > 0)
                    loadPercent = loadPercentClamped(stripCurrent, stripLimit);
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

            queuePowerKo(KoNEOSTRIP_PowerCurrent, stripCurrent, DPT_UElCurrentmA, powerCurrentMinDelta(limitMa));
            queuePowerKo(KoNEOSTRIP_PowerLoad, loadPercent, DPT_Scaling, kPowerLoadMinDelta);
            queuePowerKo(KoNEOSTRIP_PowerWatts, stripWatts, DPT_Value_Power, kPowerWattsMinDelta);

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

        queuePowerKo(KoNEO_PowerCurrentTotal, currentMa, DPT_UElCurrentmA, powerCurrentMinDelta(limitMa));
        queuePowerKo(KoNEO_PowerLoadTotal, loadPercent, DPT_Scaling, kPowerLoadMinDelta);

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

        queuePowerKo(KoNEO_PowerWattsTotal, totalWatts, DPT_Value_Power, kPowerWattsMinDelta);

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
                limitMa = stripLimit;
                if (stripLimit > 0)
                    loadPercent = loadPercentClamped(stripCurrent, stripLimit);
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

            queuePowerKo(KoNEOSTRIP_PowerCurrent, stripCurrent, DPT_UElCurrentmA, powerCurrentMinDelta(limitMa));
            queuePowerKo(KoNEOSTRIP_PowerLoad, loadPercent, DPT_Scaling, kPowerLoadMinDelta);
            queuePowerKo(KoNEOSTRIP_PowerWatts, stripWatts, DPT_Value_Power, kPowerWattsMinDelta);

            _channelIndex = savedChannelIndex;
        }
    }
}

double NeoPixelBusModule::powerCurrentMinDelta(uint32_t limitMa)
{
    return std::max<double>(50.0, limitMa * 0.02);
}

void NeoPixelBusModule::queuePowerKo(GroupObject& ko, double value, const Dpt& type, double minDelta)
{
    if (_powerKoCount >= kPowerKoMax) return;
    PowerKoEntry& e = _powerKos[_powerKoCount++];
    e.ko = &ko;
    e.value = _powerKoDark ? 0.0 : value;
    e.type = type;
    e.minDelta = minDelta;
}

void NeoPixelBusModule::drainPowerKos()
{
    if (_powerKoCount == 0) return;
    if (_powerKoStart >= _powerKoCount) _powerKoStart = 0;
    while (_powerKoDone < _powerKoCount)
    {
        if (_statusCredits == 0) return; // continues in the next loop pass
        const PowerKoEntry& e = _powerKos[(_powerKoStart + _powerKoDone) % _powerKoCount];
        sendPowerKo(*e.ko, powerKoValue(e.value, e.type), e.type, e.minDelta);
        _powerKoDone++;
    }
}

KNXValue NeoPixelBusModule::powerKoValue(double value, const Dpt& type)
{
    if (type.mainGroup == 14) return KNXValue((float)value);   // DPT 14.056 W
    if (type.mainGroup == 7) return KNXValue((uint32_t)(value > 65535.0 ? 65535.0 : value)); // DPT 7.012 mA, 2 bytes
    return KNXValue((uint8_t)value);                           // DPT 5.001 %
}

bool NeoPixelBusModule::sendPowerKo(GroupObject& ko, const KNXValue& value, const Dpt& type, double minDelta)
{
    const double next = (double)value;
    if (ko.initialized())
    {
        const double last = (double)ko.value(type);
        const double delta = next > last ? next - last : last - next;
        if (delta == 0.0 || (delta < minDelta && next != 0.0)) return false;
    }
    // Shares the credit budget of the status queue; a value without credit goes out next round
    if (_statusCredits == 0) return false;
    if (!ko.valueNoSend(value, type)) return false;
    ko.objectWritten();
    _statusCredits--;
    return true;
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
        case NEO_WARN_ABL_ACTIVE: return "Power limiting active (brightness reduced)";
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
    // Only show the most severe (lowest code number) warning; a warning raised during ABL shows after it
    if (_activeWarnCode == NEO_WARN_ABL_ACTIVE && code > NEO_WARN_ABL_ACTIVE &&
        (_warnCodeBeforeAbl == 0 || code < _warnCodeBeforeAbl))
        _warnCodeBeforeAbl = code;
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
    _warnCodeBeforeAbl = 0;
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
        // does LAST (restoreDirectState); finishEmStop() applies OFF/DEFAULT on top, which OFM
        // cannot resolve (no ETS access). Chaining EM->EM keeps isRunning() true, so only a real
        // chain-end flips the edge — manual stops go through stopEffektManager(), not this path.
        bool wasRunning = cfg.emController.isRunning();
        cfg.emController.tick(cfg.segment, _emData);
        if (wasRunning && !cfg.emController.isRunning())
        {
            // A finished one-shot EM-action scene is not replayed at boot; a static scene marker stays.
            if (_sceneManager && _sceneManager->isEmActionScene((uint8_t)i, cfg.savedSceneNumber))
                cfg.savedSceneNumber = 0;
            finishEmStop(i, cfg.emStopReturnMode);
        }

        // Re-apply any long console cue-text after a cue switch (no-op unless one is set).
        applyCueLongText(cfg);

        // Keep EM status KOs in sync with cue changes and chaining.
        // sendEmStatusKOs() queues the segment only when the EM state changed.
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

uint8_t NeoPixelBusModule::configuredBootEm(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return EM_NONE;
    const uint8_t previousChannel = _channelIndex;
    _channelIndex = static_cast<uint8_t>(segmentIndex);
    const uint8_t emId = static_cast<uint8_t>(ParamNEO_NEOStartupEMAfterBoot);
    _channelIndex = previousChannel;
    return emId;
}

uint8_t NeoPixelBusModule::configuredPowerOnEm(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return EM_NONE;
    const uint8_t previousChannel = _channelIndex;
    _channelIndex = static_cast<uint8_t>(segmentIndex);
    const uint8_t emId = static_cast<uint8_t>(ParamNEO_NEOStartupEMOnPowerOn);
    _channelIndex = previousChannel;
    return emId;
}

void NeoPixelBusModule::startEffektManager(size_t segmentIndex, uint8_t emId)
{
    startEffektManagerFresh(segmentIndex, emId);
}

void NeoPixelBusModule::startEffektManagerFresh(size_t segmentIndex, uint8_t emId)
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

    // Effektkette master: keep sending the direct state while the EM runs.
    const bool wasRunning = cfg.emController.isRunning();
    if (!wasRunning) captureSyncFreeze(segmentIndex);

    // The first cue renders and enters its effect at master x cue: master = (intent, 0 -> full)
    // x global before the restart; a refused start keeps the old master.
    const uint8_t previousMaster = cfg.segment->getMasterBrightness();
    const bool liftedIntent = cfg.savedBrightness == 0;
    const uint8_t startIntent = liftedIntent ? 255 : cfg.savedBrightness;
    cfg.segment->setMasterBrightness((uint8_t)((uint16_t)startIntent * _globalBrightness / 255));

    // A normal effect KO interrupts a running EM immediately
    cfg.emController.restart(emId, cfg.segment, _emData);

    if (cfg.emController.activeEmId() != emId)
    {
        cfg.segment->setMasterBrightness(previousMaster);
    }
    else
    {
        // An EM start switches the segment on; started from OFF, its stop/finish switches off again.
        const bool fromOff = cfg.savedPower == 0;
        cfg.emStartedFromOff = wasRunning ? (cfg.emStartedFromOff || fromOff) : fromOff;
        cfg.savedPower = 1;
        cfg.pendingPowerOnRestart = false;
        cfg.suspendedByPower = false;
        cfg.hiddenEffectChange = false; // the first cue was entered just now
        cfg.savedBrightness = startIntent;
        // A colour ramp would keep changing the cue colour; a brightness ramp dims the EM and stays,
        // unless the start lifted intent 0 (the ramp would dim it again, like applyBrightnessIntent).
        if (cfg.activeDimming != SegmentConfig::NONE &&
            (cfg.activeDimming != SegmentConfig::BRIGHTNESS || liftedIntent))
        {
            cfg.activeDimming = SegmentConfig::NONE;
            cfg.dimmingStepCode = 0;
            cfg.dimmingRemaining = -1;
        }
        if (cfg.globalSuspended)
        {
            // Dark until global ON resumes it.
            pauseEffektManager(segmentIndex);
            cfg.suspendedByPower = true;
            hideIfNotVisible(segmentIndex);
        }
    }
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

    // Honor the per-segment "EM-Stop return" mode (0=last, 1=default, 2=off). OFM restores the
    // direct state (LAST) underneath every mode; finishEmStop() applies DEFAULT (OFM cannot read
    // ETS) and switches the segment off for OFF.
    const uint8_t mode = cfg.emStopReturnMode;
    // Capture BEFORE stop(): an EM-Stop on an idle segment (no EM running) must NOT reset the
    // segment — otherwise it would clobber a manual color/effect/brightness the user just set.
    // isRunning() is also true for a paused EM.
    const bool wasRunning = cfg.emController.isRunning();
    cfg.emController.stop(cfg.segment, EM_STOP_LAST);
    if (wasRunning) finishEmStop(segmentIndex, mode);
    sendEmStatusKOs(segmentIndex);
}

/**
 * @brief Segment state after an EM stopped or finished; OFM already restored the direct state.
 * OFF, an EM started on a switched-off segment, or own power OFF end dark; otherwise the effect
 * restarts (dark until global ON while suspended by global power).
 */
void NeoPixelBusModule::finishEmStop(size_t segmentIndex, uint8_t mode)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (!cfg.segment) return;

    cfg.syncFreezeValid = false;
    if (mode == EM_STOP_DEFAULT) applySegmentDefaultState(segmentIndex);

    if (mode == EM_STOP_OFF || cfg.emStartedFromOff || cfg.savedPower == 0)
    {
        segmentOff(segmentIndex, true);
    }
    else if (cfg.globalSuspended)
    {
        cfg.segment->stop(); // re-entered at global ON
    }
    else
    {
        cfg.segment->resume();
        reEnterEffect(segmentIndex);
    }
    cfg.emStartedFromOff = false;
    cfg.suspendedByPower = false;
    cfg.pendingPowerOnRestart = false;
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
    // The cue's colour and effect stay: they are the segment's direct state from now on.
    captureDirectColor(segmentIndex);
    // A direct effect renders at the master (intent x global), like the "Letzter Zustand" return.
    cfg.segment->setBrightness(cfg.segment->getMasterBrightness());
    Effect* effect = cfg.segment->getEffect();
    cfg.savedEffectType = getTypeFromEffect(effect);
    cfg.savedEffectValid = effect != nullptr;
    cfg.savedLastWasEffect = cfg.savedEffectValid;
    if (effect) cfg.segment->getConfig().effectType = cfg.savedEffectType; // a cue does not set it (sync byte 1)
    cfg.suspendedByPower = false;
    cfg.emStartedFromOff = false;
    cfg.pendingPowerOnRestart = false;
    cfg.syncFreezeValid = false;
    // A manually paused EM froze the segment; the direct command shows on a visible segment.
    // A dark segment stays stopped: the command is recorded and shown at the next ON.
    if (isSegmentVisible(cfg) && cfg.segment->isPaused())
    {
        cfg.segment->resume();
        reEnterEffect(segmentIndex);
    }
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

    // Default color/white (TypeColor RGB = 0x00RRGGBB) — the member wrapper above does NOT
    // load the default color, so apply it explicitly like setupEffectConfiguration(seg, true).
    applyStartupColors(cfg.segment);
    captureDirectColor(segmentIndex);

    // Start brightness seeds the pre-global intent; render then respects the live global factor
    // (like the boot path / applyGlobalBrightness), else the segment would ignore an active global dim.
    uint8_t startupBrightness = static_cast<uint8_t>(ParamNEO_NEOSegmentStartupBrightness);
    cfg.savedBrightness = startupBrightness; // pre-global baseline
    uint8_t effectiveBrightness = (uint8_t)((uint16_t)startupBrightness * getGlobalBrightness() / 255);
    cfg.segment->setMasterBrightness(effectiveBrightness);
    cfg.segment->setBrightness(effectiveBrightness);

    cfg.savedEffectType = effectType;
    cfg.savedEffectValid = true;

    setChannelIndex(prevChannel);
}

// ============================================================================
// Segment power model
// ============================================================================

bool NeoPixelBusModule::anySegmentVisible() const
{
    for (const auto& cfg : _segments)
        if (cfg.segment && isSegmentVisible(cfg)) return true;
    return false;
}

/**
 * @brief Own power OFF: pause an EM on the segment (the next ON resumes it), stop and clear it.
 * @param show push the cleared buffer now; skipped while global power suspends the segment
 */
void NeoPixelBusModule::segmentOff(size_t segmentIndex, bool show)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    Segment* seg = cfg.segment;
    if (!seg) return;

    const bool emRunning = cfg.emController.isRunning();
    if (emRunning)
    {
        if (!cfg.emController.isPaused()) pauseEffektManager(segmentIndex);
        cfg.suspendedByPower = true;
    }
    cfg.pendingPowerOnRestart = false;
    // A brightness ramp would change the intent while dark
    if (cfg.activeDimming == SegmentConfig::BRIGHTNESS)
    {
        cfg.activeDimming = SegmentConfig::NONE;
        cfg.dimmingStepCode = 0;
        cfg.dimmingRemaining = -1;
    }

    // Direct state for power-on and flash; under an EM the segment shows the cue instead.
    if (!emRunning && cfg.savedPower != 0)
    {
        captureDirectColor(segmentIndex);
        Effect* effect = seg->getEffect();
        cfg.savedEffectType = getTypeFromEffect(effect);
        cfg.savedEffectValid = effect != nullptr;
        cfg.savedLastWasEffect = cfg.savedEffectValid;
    }

    cfg.savedPower = 0;
    seg->stop(); // also when already off: a direct-state restore may have resumed it
    if (show && !cfg.globalSuspended) showVirtualStripIfIdle();
}

/**
 * @brief Mirror the live primary colour into saved* (base of the absolute colour KOs, Fx and flash).
 * @note While an EM runs, saved* keeps the direct state from before the EM.
 */
void NeoPixelBusModule::captureDirectColor(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (!cfg.segment || cfg.emController.isRunning()) return;
    const uint32_t primary = cfg.segment->getPrimaryColor(); // 0xRRGGBBWW
    cfg.savedR = (primary >> 24) & 0xFF;
    cfg.savedG = (primary >> 16) & 0xFF;
    cfg.savedB = (primary >> 8) & 0xFF;
    cfg.savedWW = primary & 0xFF;
    cfg.savedCW = cfg.segment->getConfig().cw(); // RGBCCT: the 5th channel is not in primary
    const auto& secondary = cfg.segment->getConfig();
    cfg.savedSecondaryR = secondary.r2(); cfg.savedSecondaryG = secondary.g2(); cfg.savedSecondaryB = secondary.b2();
    cfg.savedSecondaryWW = secondary.ww2(); cfg.savedSecondaryCW = secondary.cw2();
    cfg.savedValid = true;
}

/**
 * @brief Primary colour with one white value. On an RGBCCT buffer W is split like the W object
 *        (warm half, cool rest); otherwise CW is cleared, so no earlier CW stays active.
 */
void NeoPixelBusModule::applyStartupColors(Segment* segment)
{
    const uint32_t primary = ParamNEO_NEOSegmentStartupColor;
    const uint8_t white = ParamNEO_NEOSegmentStartupW;
#ifdef ParamNEO_NEOSegmentStartupSeparateWhite
    if (ParamNEO_NEOSegmentStartupSeparateWhite)
        segment->setPrimaryColor((primary >> 16) & 255, (primary >> 8) & 255, primary & 255, white, ParamNEO_NEOSegmentStartupCW);
    else
#endif
        setPrimaryColorOneWhite(segment, (primary >> 16) & 255, (primary >> 8) & 255, primary & 255, white);
#ifdef ParamNEO_NEOSegmentStartupSecondaryColor
    const uint32_t secondary = ParamNEO_NEOSegmentStartupSecondaryColor;
    segment->setSecondaryColor((secondary >> 16) & 255, (secondary >> 8) & 255, secondary & 255, ParamNEO_NEOSegmentStartupSecondaryWW, ParamNEO_NEOSegmentStartupSecondaryCW);
#endif
}

void NeoPixelBusModule::setPrimaryColorOneWhite(Segment* seg, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    if (!seg) return;
    const VirtualStrip* vstrip = seg->getVirtualStrip();
    if (vstrip && vstrip->hasDualWhiteChannel())
        seg->setPrimaryColor(r, g, b, w / 2, w - w / 2);
    else
        seg->setPrimaryColor(r, g, b, w, 0);
}

/**
 * @brief Own power ON without the power-on EM edge. Dark while global power suspends the segment.
 */
void NeoPixelBusModule::segmentOn(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size() || !_segments[segmentIndex].segment) return;
    auto& cfg = _segments[segmentIndex];
    cfg.emStartedFromOff = false;
    cfg.savedPower = 1;
    if (cfg.globalSuspended) return;
    showSegmentOutput(segmentIndex);
}

/**
 * @brief ON command: the configured power-on EM starts fresh on the off->on edge, else segmentOn().
 */
void NeoPixelBusModule::segmentPowerOn(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size() || !_segments[segmentIndex].segment) return;
    auto& cfg = _segments[segmentIndex];
    const uint8_t powerOnEm = configuredPowerOnEm(segmentIndex);
    // At intent 0 ON does not keep the pause: SwitchOn resumes the EM at intent 255 (x global).
    const bool manualEmPause = cfg.emController.isRunning() && cfg.emController.isPaused() && !cfg.suspendedByPower &&
                               cfg.savedBrightness != 0;
    switch (decideSegmentPowerOnAction(cfg.savedPower == 0, powerOnEm, cfg.globalSuspended, manualEmPause))
    {
        case SegmentPowerOnAction::KeepPause:
            cfg.emStartedFromOff = false;
            return;
        case SegmentPowerOnAction::DeferPowerOnEm:
            cfg.savedPower = 1;
            cfg.emStartedFromOff = false;
            cfg.pendingPowerOnRestart = true;
            return;
        case SegmentPowerOnAction::StartPowerOnEm:
            cfg.savedPower = 1;
            cfg.emStartedFromOff = false;
            if (startPowerOnEm(segmentIndex, powerOnEm)) return;
            break; // refused: switch on without it
        case SegmentPowerOnAction::SwitchOn:
            break;
    }
    segmentOn(segmentIndex);
}

/**
 * @brief Make a segment with power ON visible: resume its EM (manual or power pause) or restart
 *        its effect at the brightness intent. A segment that runs already keeps running.
 */
void NeoPixelBusModule::showSegmentOutput(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    Segment* seg = cfg.segment;
    if (!seg) return;

    if (cfg.emController.isRunning())
    {
        // Segment or global OFF cleared the segment (stop()); a cue effect that has finished its
        // one-shot phase (e.g. Wipe) would not paint again.
        const bool clearedByPower = cfg.suspendedByPower;
        if (cfg.emController.isPaused())
        {
            cfg.suspendedByPower = false;
            applyBrightnessIntent(segmentIndex); // resume() renders the cue at the new master
            resumeEffektManager(segmentIndex);
        }
        else
        {
            if (seg->isPaused()) seg->resume();
            if (cfg.savedBrightness == 0) applyBrightnessIntent(segmentIndex);
        }
        // A cleared cue is entered again (the cue timer and fade position are not touched).
        if ((cfg.hiddenEffectChange || clearedByPower) && seg->getEffect()) reEnterEffect(segmentIndex);
        cfg.hiddenEffectChange = false;
        return;
    }

    if (seg->isPaused())
    {
        applyBrightnessIntent(segmentIndex); // before onEnter(): Wipe paints its background once
        seg->resume();
        reEnterEffect(segmentIndex);
    }
    else if (cfg.savedBrightness == 0)
    {
        applyBrightnessIntent(segmentIndex);
    }
    cfg.hiddenEffectChange = false;
}

/**
 * @brief A static scene or preset was applied: switch on, then re-enter the new effect.
 * The caller has set colour, effect and the brightness intent.
 */
void NeoPixelBusModule::sceneSwitchOn(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size() || !_segments[segmentIndex].segment) return;
    auto& cfg = _segments[segmentIndex];
    cfg.savedPower = 1;
    cfg.emStartedFromOff = false;
    cfg.pendingPowerOnRestart = false;
    cfg.suspendedByPower = false;
    if (cfg.globalSuspended)
    {
        hideIfNotVisible(segmentIndex); // shown at global ON
        return;
    }
    cfg.segment->resume();
    reEnterEffect(segmentIndex);
}

bool NeoPixelBusModule::startPowerOnEm(size_t segmentIndex, uint8_t emId)
{
    if (segmentIndex >= _segments.size()) return false;
    auto& cfg = _segments[segmentIndex];
    cfg.pendingPowerOnRestart = false;
    startEffektManagerFresh(segmentIndex, emId);
    if (cfg.emController.activeEmId() != emId) return false;
    cfg.savedSceneNumber = 0;
    return true;
}

/**
 * @brief Restart the segment's current effect: Wipe repaints its background, finished effects
 *        run again. Without an effect: the saved effect, else the ETS effect.
 */
void NeoPixelBusModule::reEnterEffect(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    Segment* seg = cfg.segment;
    if (!seg) return;

    const uint8_t prevChannel = _channelIndex;
    _channelIndex = static_cast<uint8_t>(segmentIndex);
    Effect* effect = seg->getEffect();
    if (effect)
    {
        seg->resetEffectState();
        seg->enterEffect();
    }
    else if (segmentIndex < kStatusSegmentMax) // ETS parameters exist only for the KO channels
    {
        applyEffectToSegment(seg, cfg.savedEffectValid ? cfg.savedEffectType
                                                       : static_cast<uint8_t>(ParamNEO_NEONEOEffectType));
    }
    _channelIndex = prevChannel;
    cfg.hiddenEffectChange = false;
}

/**
 * @brief master = (intent, 0 -> full and stored) x global; render = master, or EM re-derive.
 */
void NeoPixelBusModule::applyBrightnessIntent(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    Segment* seg = cfg.segment;
    if (!seg) return;

    if (cfg.savedBrightness == 0)
    {
        cfg.savedBrightness = 255;
        // A running brightness ramp would dim the lifted intent again
        if (cfg.activeDimming == SegmentConfig::BRIGHTNESS)
        {
            cfg.activeDimming = SegmentConfig::NONE;
            cfg.dimmingStepCode = 0;
            cfg.dimmingRemaining = -1;
        }
    }
    const uint8_t effective = (uint8_t)((uint16_t)cfg.savedBrightness * _globalBrightness / 255);
    seg->setMasterBrightness(effective);
    if (cfg.emController.isRunning())
        cfg.emController.reapplyMasterBrightness(seg);
    else
        seg->setBrightness(effective);
}

/**
 * @brief After an effect or cue change on a segment that is not visible: clear what onEnter()
 *        painted; the next ON re-enters the effect.
 */
void NeoPixelBusModule::hideIfNotVisible(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (!cfg.segment || isSegmentVisible(cfg)) return;
    cfg.segment->clear();
    cfg.hiddenEffectChange = true;
}

void NeoPixelBusModule::globalSuspendSegment(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (!cfg.segment) return;

    cfg.globalSuspended = true;
    if (cfg.emController.isRunning())
    {
        if (!cfg.emController.isPaused()) pauseEffektManager(segmentIndex);
        cfg.suspendedByPower = true; // also a manual pause: global ON resumes it
    }
    // A brightness ramp would change the intent while dark
    if (cfg.activeDimming == SegmentConfig::BRIGHTNESS)
    {
        cfg.activeDimming = SegmentConfig::NONE;
        cfg.dimmingStepCode = 0;
        cfg.dimmingRemaining = -1;
    }
    cfg.segment->stop();
}

void NeoPixelBusModule::globalResumeSegment(size_t segmentIndex)
{
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    if (!cfg.segment) return;

    cfg.globalSuspended = false;
    switch (decideGlobalPowerOnAction(cfg.savedPower != 0, cfg.pendingPowerOnRestart))
    {
        case GlobalPowerOnAction::StayOff:
            return;
        case GlobalPowerOnAction::StartPowerOnEm:
        {
            const uint8_t powerOnEm = configuredPowerOnEm(segmentIndex);
            if (powerOnEm != EM_NONE && startPowerOnEm(segmentIndex, powerOnEm)) return;
            cfg.pendingPowerOnRestart = false;
            segmentOn(segmentIndex); // refused: switch on without it
            return;
        }
        case GlobalPowerOnAction::ShowOutput:
            showSegmentOutput(segmentIndex);
            return;
    }
}

uint8_t NeoPixelBusModule::effectiveStartupBehavior(size_t segmentIndex)
{
    uint8_t prevChannel = getChannelIndex();
    setChannelIndex(static_cast<uint8_t>(segmentIndex));
    // Segment: 0=Global verwenden, 1=Aus, 2=Letzter Zustand, 3=An (ETS-Werte)
    uint8_t segmentBehavior = ParamNEO_NEOSegmentStartupBehavior;
    uint8_t behavior = (segmentBehavior == 0) ? ParamNEO_NEOGlobalStartupBehavior : segmentBehavior - 1;
    setChannelIndex(prevChannel);
    return behavior > 2 ? 2 : behavior;
}

void NeoPixelBusModule::sendEmStatusKOs(size_t segmentIndex)
{
    // Called every loop pass: queue the segment status only when the EM state changed (start,
    // stop, cue, pause, chaining, auto-finish). A cue change also changes colour/effect/brightness.
    if (segmentIndex >= _segments.size()) return;
    auto& cfg = _segments[segmentIndex];
    const uint32_t key = ((uint32_t)cfg.emController.activeEmId() << 16) |
                         ((uint32_t)cfg.emController.activeCueNum() << 8) | cfg.emController.runState();
    if (key == cfg.statusEmKey) return;
    cfg.statusEmKey = key;
    queueStatus(static_cast<uint8_t>(1 + segmentIndex), false);
}

void NeoPixelBusModule::sendEffectTextStatusKO(size_t segmentIndex)
{
#ifdef NEO_KoEffectTextStatus
    // Every text/append write is confirmed with the current text (also when the first 14
    // characters repeat), paced like every status. Before the seed the EM hook queues the scope.
    if (!_statusSeeded || segmentIndex >= _segments.size()) return;
    queueStatusItem(static_cast<uint8_t>(1 + segmentIndex), kSegEffectText, true);
#endif
}
