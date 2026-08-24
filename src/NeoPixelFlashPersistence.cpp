#include "NeoPixelFlashPersistence.h"
#include "NeoPixelModule.h"
#include "SceneManager.h"
#include "Segment.h"
#include <algorithm>

NeoPixelFlashPersistence::NeoPixelFlashPersistence(NeoPixelBusModule* module)
    : _module(module)
{
}

// ============================================================================
// Flash Size Calculation
// ============================================================================

uint16_t NeoPixelFlashPersistence::calculateFlashSize() const
{
    if (!_module) return 0;

    // Calculate based on actual configured segment count
    uint16_t numSegments = _module->getNumberOfSegments();
    if (numSegments == 0) numSegments = 1; // Minimum 1 segment

    uint16_t size = sizeof(SegmentFlashState) * numSegments;
    if (NeoPixelBusModule::kMaxExternalRelays > 0)
    {
        size += sizeof(RelayFlashState);
    }

    logInfoP("Flash size calculated: %d bytes for %d segments", size, numSegments);
    return size;
}

// ============================================================================
// Write to Flash
// ============================================================================

void NeoPixelFlashPersistence::writeToFlash()
{
    if (!_module)
    {
        logWarningP("writeToFlash: No module reference");
        return;
    }

    const auto& segments = _module->getSegments();
    if (segments.empty())
    {
        logWarningP("writeToFlash: No segments configured");
        return;
    }

#ifdef OPENKNX_DEBUG
    logInfoP("========================================");
    logInfoP("FLASH WRITE: Saving LED states to flash");
    logInfoP("Segments to save: %d", segments.size());
    logInfoP("========================================");
#else
    logInfoP("Writing LED states to flash (%d segments)", segments.size());
#endif

    // Use OGM-Common flash write helpers
    for (size_t i = 0; i < segments.size(); i++)
    {
        SegmentFlashState state;
        if (saveSegmentState(i, state))
        {
            // Write state structure to flash (14 bytes)
            openknx.flash.write((uint8_t*)&state, sizeof(SegmentFlashState));

#ifdef OPENKNX_DEBUG
            logInfoP("[Segment %d] SAVED (v%d, flags=0x%02X):", i, state.version, state.validFlags);
            if (state.validFlags & 0x01) logInfoP("  Power:      %s", state.power ? "ON" : "OFF");
            if (state.validFlags & 0x02) logInfoP("  Color:      R=%3d G=%3d B=%3d WW=%3d CW=%3d", state.r, state.g, state.b, state.ww, state.cw);
            if (state.validFlags & 0x04) logInfoP("  Brightness: %d", state.brightness);
            if (state.validFlags & 0x08) logInfoP("  Effect:     Type=%d (valid=%d, lastWasEffect=%d)", state.effectType,
                                                  (state.effectFlags & 0x01) ? 1 : 0, (state.effectFlags & 0x02) ? 1 : 0);
#endif
        }
        else
        {
            // Write empty state if segment invalid (still stamp the format version
            // so the read-side version check never false-trips on an invalid seg 0)
            SegmentFlashState emptyState = {};
            emptyState.version = FLASH_FORMAT_VERSION;
            openknx.flash.write((uint8_t*)&emptyState, sizeof(SegmentFlashState));
#ifdef OPENKNX_DEBUG
            logInfoP("[Segment %d] SAVED: Empty state (segment invalid)", i);
#endif
        }
    }

    if (NeoPixelBusModule::kMaxExternalRelays > 0)
    {
        RelayFlashState relayState = {};
        relayState.count = _module->_relayCount;
        relayState.statesMask = 0;
        relayState.signature = 0xA5;

        uint8_t maxRelays = std::min<uint8_t>(_module->_relayCount, NeoPixelBusModule::kMaxExternalRelays);
        for (uint8_t i = 0; i < maxRelays; ++i)
        {
            if (_module->_relayStates[i])
            {
                relayState.statesMask |= static_cast<uint8_t>(1u << i);
            }
        }

        openknx.flash.write((uint8_t*)&relayState, sizeof(RelayFlashState));
    }

#ifdef OPENKNX_DEBUG
    logInfoP("========================================");
    logInfoP("FLASH WRITE: Complete - %d segments saved", segments.size());
    logInfoP("========================================");
#else
    logInfoP("Flash write complete");
#endif
}

// ============================================================================
// Read from Flash
// ============================================================================

void NeoPixelFlashPersistence::readFromFlash(const uint8_t* data, uint16_t size)
{
    if (!_module)
    {
        logWarningP("readFromFlash: No module reference");
        return;
    }

    const auto& segments = _module->getSegments();
    if (segments.empty())
    {
        logWarningP("readFromFlash: No segments, deferring restore");
        return;
    }

    if (size == 0)
    {
        logInfoP("readFromFlash: No saved data (first boot)");
        return;
    }

    // CRITICAL: Validate flash data size matches current segment configuration
    // This prevents corruption when ETS changes segment count
    uint16_t expectedSegmentSize = sizeof(SegmentFlashState) * segments.size();
    uint16_t expectedTotalSize = expectedSegmentSize;
    if (NeoPixelBusModule::kMaxExternalRelays > 0)
    {
        expectedTotalSize += sizeof(RelayFlashState);
    }
    if (size != expectedSegmentSize && size != expectedTotalSize)
    {
        logWarningP("Flash size mismatch (%d B vs %d B for %d seg) - ignoring saved data, using ETS defaults",
                    size, expectedSegmentSize, (int)segments.size());
        _module->setWarningBlink(NEO_WARN_FLASH_DISCARDED); // pulsing Purple (color from warnColorForCode)
        return;
    }

    // Validate FORMAT VERSION: a firmware update may keep the same size but change
    // the meaning of the bytes. The version byte is the first byte of every record
    // (also stamped on empty/invalid segments), so data[0] is reliable here.
    if (data[0] != FLASH_FORMAT_VERSION)
    {
        logWarningP("Flash format v%d != expected v%d (firmware updated) - ignoring saved data, using ETS defaults",
                    data[0], FLASH_FORMAT_VERSION);
        _module->setWarningBlink(NEO_WARN_FLASH_DISCARDED); // pulsing Purple
        return;
    }

#ifdef OPENKNX_DEBUG
    logInfoP("========================================");
    logInfoP("FLASH READ: Loading LED states from flash");
    logInfoP("Flash data size: %d bytes", size);
    logInfoP("Available segments: %d", segments.size());
    logInfoP("Expected size per segment: %d bytes", sizeof(SegmentFlashState));
    logInfoP("========================================");
#else
    logInfoP("Reading LED states from flash (%d bytes, %d segments)", size, segments.size());
#endif

    uint16_t offset = 0;

    for (size_t i = 0; i < segments.size(); i++)
    {
        // Check if we have enough data left
        if (offset + sizeof(SegmentFlashState) > size)
        {
            logWarningP("Flash data incomplete for segment %d, skipping remaining segments", i);
            break;
        }

        SegmentFlashState state;
        memcpy(&state, data + offset, sizeof(SegmentFlashState));
        offset += sizeof(SegmentFlashState);

#ifdef OPENKNX_DEBUG
        logInfoP("[Segment %d] LOADED from flash (v%d, flags=0x%02X):", i, state.version, state.validFlags);
        if (state.validFlags & 0x01) logInfoP("  Power:      %s", state.power ? "ON" : "OFF");
        if (state.validFlags & 0x02) logInfoP("  Color:      R=%3d G=%3d B=%3d WW=%3d CW=%3d", state.r, state.g, state.b, state.ww, state.cw);
        if (state.validFlags & 0x04) logInfoP("  Brightness: %d", state.brightness);
        if (state.validFlags & 0x08) logInfoP("  Effect:     Type=%d (valid=%d, lastWasEffect=%d)", state.effectType,
                                              (state.effectFlags & 0x01) ? 1 : 0, (state.effectFlags & 0x02) ? 1 : 0);
#endif

        // Store in segment config for later restoration (after startup delay)
        auto& segments = _module->getSegments();
        auto& cfg = segments[i];

        // Only restore values that were changed via KO (validFlags set)
        if (state.validFlags & 0x01) cfg.savedPower = state.power;
        if (state.validFlags & 0x02)
        {
            cfg.savedR = state.r;
            cfg.savedG = state.g;
            cfg.savedB = state.b;
            cfg.savedWW = state.ww;
            cfg.savedCW = state.cw;
        }
        if (state.validFlags & 0x04) cfg.savedBrightness = state.brightness;

        // NEW: Restore effect state from flash (KO-changed effect)
        if (state.validFlags & 0x08) cfg.savedEffectType = state.effectType;
        if (state.validFlags & 0x10)
        {
            cfg.savedEffectValid = (state.effectFlags & 0x01) != 0;
            cfg.savedLastWasEffect = (state.effectFlags & 0x02) != 0;
        }

        // Restore last active scene number from reserved[0]
        cfg.savedSceneNumber = state.reserved[0];
        // Restore last active Effektmanager ID from reserved[1]
        cfg.emController.setLastEmId(state.reserved[1]);

        // Mark data as valid if ANY flag is set
        cfg.savedValid = (state.validFlags != 0);

#ifdef OPENKNX_DEBUG
        logInfoP("  -> Stored in config for later restoration");
#endif
    }

#ifdef OPENKNX_DEBUG
    if (size == expectedTotalSize && NeoPixelBusModule::kMaxExternalRelays > 0)
    {
        RelayFlashState relayState;
        memcpy(&relayState, data + expectedSegmentSize, sizeof(RelayFlashState));
        _relayFlashState = relayState;
        _relayFlashValid = (relayState.signature == 0xA5);
        logInfoP("Relay flash loaded: count=%d mask=0x%02X valid=%d",
                 relayState.count, relayState.statesMask, _relayFlashValid ? 1 : 0);
    }
    else
    {
        _relayFlashValid = false;
    }
#else
    if (size == expectedTotalSize && NeoPixelBusModule::kMaxExternalRelays > 0)
    {
        RelayFlashState relayState;
        memcpy(&relayState, data + expectedSegmentSize, sizeof(RelayFlashState));
        _relayFlashState = relayState;
        _relayFlashValid = (relayState.signature == 0xA5);
    }
    else
    {
        _relayFlashValid = false;
    }
#endif

#ifdef OPENKNX_DEBUG
    logInfoP("========================================");
    logInfoP("FLASH READ: Complete - %d bytes processed", offset);
    logInfoP("States will be applied after startup delay");
    logInfoP("========================================");
#else
    logInfoP("Flash read complete: %d bytes", offset);
#endif
}

// ============================================================================
// Restore States After Startup
// ============================================================================

void NeoPixelFlashPersistence::restoreStatesAfterStartup()
{
    if (!_module)
    {
        logWarningP("restoreStatesAfterStartup: No module reference");
        return;
    }

    auto& segments = _module->getSegments();
    if (segments.empty())
    {
        logWarningP("restoreStatesAfterStartup: No segments");
        return;
    }

#ifdef OPENKNX_DEBUG
    int restoredCount = 0;
    int skippedCount = 0;

    logInfoP("========================================");
    logInfoP("FLASH RESTORE: Checking segments for 'Letzter Zustand' mode");
    logInfoP("========================================");
#else
    logInfoP("Flash restore: Checking for segments with 'Letzter Zustand' mode...");
#endif

    // Get global startup behavior
    uint8_t globalBehavior = ParamNEO_NEOGlobalStartupBehavior; // 0=Aus, 1=Letzter Zustand, 2=ETS-Parameterwert

    for (size_t i = 0; i < segments.size(); i++)
    {
        auto& cfg = segments[i];
        Segment* seg = cfg.segment;

        if (!seg)
        {
#ifdef OPENKNX_DEBUG
            logInfoP("[Segment %d] SKIPPED: No segment pointer", i);
            skippedCount++;
#endif
            continue;
        }

        // Set channel index for ETS parameter access
        _module->setChannelIndex(i);
        uint8_t segmentBehavior = ParamNEO_NEOSegmentStartupBehavior;

        // Determine effective behavior
        uint8_t effectiveBehavior;
        if (segmentBehavior == 0)
        {
            effectiveBehavior = globalBehavior; // Use global
        }
        else
        {
            effectiveBehavior = segmentBehavior - 1; // 1=Aus->0, 2=Letzter->1, 3=ETS->2
        }

        // Only restore if mode is "Letzter Zustand" (1)
        if (effectiveBehavior != 1)
        {
#ifdef OPENKNX_DEBUG
            const char* behaviorNames[] = {"Aus", "Letzter Zustand", "ETS-Parameterwert"};
            logInfoP("[Segment %d] SKIPPED: Behavior is '%s', not 'Letzter Zustand'", i, behaviorNames[effectiveBehavior]);
            skippedCount++;
#endif
            continue;
        }

        // Mode is "Letzter Zustand" - try to restore from flash
        if (cfg.savedValid)
        {
            // Check if a scene was active — if so, recall it directly
            if (cfg.savedSceneNumber > 0 && _module->_sceneManager)
            {
                // Seed master from the saved intent BEFORE recall: an EM-action scene starts an EM via
                // recallScene() (rendering master*cue/255), so without this it would resume at the ETS
                // boot-seed brightness instead of the remembered level — the EM branch below seeds the
                // same way. A static scene that carries its own brightness simply overrides this.
                seg->setMasterBrightness(cfg.savedBrightness);
                bool sceneRecalled = _module->_sceneManager->recallScene(i, cfg.savedSceneNumber, seg);
                if (sceneRecalled)
                {
                    if (cfg.savedPower == 0) seg->setBrightness(0);
#ifdef OPENKNX_DEBUG
                    logInfoP("[Segment %d] RESTORED Scene %d from Flash (power=%s)",
                             i, cfg.savedSceneNumber, cfg.savedPower ? "ON" : "OFF");
                    restoredCount++;
#endif
                    continue;
                }
            }

            // Restore Effektmanager if one was active
            if (cfg.emController.lastEmId() != EM_NONE)
            {
                // Seed the master (intent) brightness so the EM renders cue at the remembered level
                // (render = master*cue/255). restoreState()->start() reads getMasterBrightness() but
                // does not set it, so without this the EM would resume at the ETS boot-seed brightness.
                // Unconditional (no >0 guard): a saved 0 is an intentional "dimmed dark" state and
                // must be honoured, matching the non-EM restore branch below. savedBrightness is always
                // a valid value here (flash field is always written / boot-seeded).
                seg->setMasterBrightness(cfg.savedBrightness);
                cfg.emController.restoreState(seg, _module->_emData);
#ifdef OPENKNX_DEBUG
                logInfoP("[Segment %d] RESTORED EffektManager %d from Flash", i, cfg.emController.lastEmId());
                restoredCount++;
#endif
                continue; // EM handles the segment, skip normal restore
            }

            // Determine effect source: Flash (if KO-changed) or ETS (base config)
            uint8_t effectType;
            bool effectFromFlash = false;

            if (cfg.savedEffectValid && (cfg.savedEffectType > 0 || cfg.savedLastWasEffect))
            {
                // Effect was changed via KO → restore from flash
                effectType = cfg.savedEffectType;
                effectFromFlash = true;
            }
            else
            {
                // No KO-change → use ETS base config (already loaded in configureEffects)
                effectType = static_cast<uint8_t>(ParamNEO_NEONEOEffectType);
            }

            // Apply effect (from flash or ETS)
            if (effectFromFlash)
            {
                _module->applyEffectToSegment(seg, effectType);
                if (effectType > 0)
                {
                    _module->setChannelIndex(i);
                    _module->setupEffectConfiguration(seg);
                }
            }
            // If effect from ETS: already loaded in configureEffects(), skip re-apply

            // Restore color from flash
            seg->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedCW);

            // Restore brightness/power state. Drive the MASTER (intent) layer like the boot seed
            // (EffectConfiguration.cpp:48-51) so the restored brightness survives a later global-
            // brightness recompute or EM start (render = master*cue/255). "Aus" keeps master but
            // renders 0, so a subsequent power-on dims correctly.
            if (cfg.savedPower == 0)
            {
                seg->setMasterBrightness(cfg.savedBrightness);
                seg->setBrightness(0);
#ifdef OPENKNX_DEBUG
                logInfoP("[Segment %d] RESTORED from Flash: OFF (Effect=%d from %s, Color from flash)",
                         i, effectType, effectFromFlash ? "Flash" : "ETS");
#endif
            }
            else
            {
                seg->setMasterBrightness(cfg.savedBrightness);
                seg->setBrightness(cfg.savedBrightness);
#ifdef OPENKNX_DEBUG
                logInfoP("[Segment %d] RESTORED from Flash: ON (Effect=%d from %s, Color=R:%d,G:%d,B:%d,WW:%d,CW:%d, Brightness=%d)",
                         i, effectType, effectFromFlash ? "Flash" : "ETS",
                         cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedCW, cfg.savedBrightness);
#endif
            }
#ifdef OPENKNX_DEBUG
            restoredCount++;
#endif
        }
        else
        {
            // No flash data available - keep ETS config from configureEffects()
#ifdef OPENKNX_DEBUG
            logInfoP("[Segment %d] No flash data - keeping ETS config from configureEffects()", i);
            skippedCount++;
#endif
        }
    }

    // Trigger hardware update
    VirtualStrip* vstrip = _module->getVirtualStrip();
    if (vstrip)
    {
        vstrip->show();
#ifdef OPENKNX_DEBUG
        logInfoP("Hardware update triggered (show() called)");
#endif
    }

    if (_relayFlashValid && _module->_relayCount > 0)
    {
        uint8_t maxRelays = std::min<uint8_t>(_module->_relayCount, NeoPixelBusModule::kMaxExternalRelays);
        uint8_t restoreCount = std::min<uint8_t>(_relayFlashState.count, maxRelays);

        for (uint8_t i = 0; i < restoreCount; ++i)
        {
            bool state = (_relayFlashState.statesMask & static_cast<uint8_t>(1u << i)) != 0;
            // Direct GPIO control without updating lastOffTime (post-reset, no min-off protection needed)
            uint8_t pin = _module->_relayPins[i];
            if (pin != 255)
            {
                bool physicalState = _module->_relayInverted[i] ? !state : state;
                digitalWrite(pin, physicalState ? HIGH : LOW);
                _module->_relayStates[i] = state;
                logInfoP("Relay %d restored to %s (GPIO %d)", i + 1, state ? "ON" : "OFF", pin);
            }
        }
    }

#ifdef OPENKNX_DEBUG
    logInfoP("========================================");
    logInfoP("FLASH RESTORE: Complete");
    logInfoP("  Restored: %d segments", restoredCount);
    logInfoP("  Skipped:  %d segments", skippedCount);
    logInfoP("========================================");
#else
    logInfoP("Flash restore complete");
#endif
}

// ============================================================================
// Private Helper Functions
// ============================================================================

bool NeoPixelFlashPersistence::saveSegmentState(uint8_t segmentIndex, SegmentFlashState& state)
{
    if (!_module) return false;

    auto& segments = _module->getSegments();
    if (segmentIndex >= segments.size()) return false;

    auto& cfg = segments[segmentIndex];
    Segment* seg = cfg.segment;

    if (!seg)
    {
        // No segment - save empty state
        memset(&state, 0, sizeof(SegmentFlashState));
        return false;
    }

    // Initialize structure version and flags
    state.version = FLASH_FORMAT_VERSION;
    state.validFlags = 0;

    // Save power state (always valid)
    state.power = (seg->getBrightness() > 0) ? 1 : 0;
    state.validFlags |= 0x01;

    // Save color (use savedR/G/B/WW/CW if valid, otherwise read from segment)
    if (cfg.savedValid)
    {
        state.r = cfg.savedR;
        state.g = cfg.savedG;
        state.b = cfg.savedB;
        state.ww = cfg.savedWW;
        state.cw = cfg.savedCW;
        state.validFlags |= 0x02;
    }
    else
    {
        uint8_t r = 0, g = 0, b = 0, ww = 0, cw = 0;
        const uint16_t firstPixel = seg->isVirtualBand() ? seg->getVirtualOffset() : 0;
        bool colorRead = false;
        VirtualStrip* vstrip = _module->getVirtualStrip();
        if (vstrip && vstrip->hasDualWhiteChannel())
        {
            // RGBCCT (5 bytes per LED)
            colorRead = seg->getPixel(firstPixel, r, g, b, ww, cw);
        }
        else if (vstrip && vstrip->hasWhiteChannel())
        {
            // RGBW (4 bytes per LED)
            colorRead = seg->getPixel(firstPixel, r, g, b, ww);
        }
        else
        {
            // RGB (3 bytes per LED)
            colorRead = seg->getPixel(firstPixel, r, g, b);
        }

        if (colorRead)
        {
            state.r = r;
            state.g = g;
            state.b = b;
            state.ww = ww;
            state.cw = cw;
            state.validFlags |= 0x02;
        }
    }

    // Save brightness (always valid). For a RUNNING EM, getBrightness() is the cue-scaled render
    // (master*cue/255); persist the MASTER intent instead, because the restore path
    // (restoreStatesAfterStartup EM branch) feeds savedBrightness back in as the master — storing
    // render would double-scale the EM on every reboot. For non-EM segments persist the render as
    // before (master==render in the normal case, and scene/preset paths set render directly).
    // For a RUNNING EM, getBrightness() is the cue-scaled render and getMasterBrightness() is the
    // global-scaled master — persist cfg.savedBrightness, the PRE-global intent (the same baseline
    // ColorManagement uses), so the restore feeds master back without cue- or global-double-scaling
    // (_globalBrightness is not persisted and resets to 255 on boot). Non-EM persists the render as
    // before (scene/preset/direct set the render layer directly).
    state.brightness = cfg.emController.isRunning() ? cfg.savedBrightness : seg->getBrightness();
    state.validFlags |= 0x04;

    // NEW: Save effect state if changed via KO
    if (cfg.savedEffectValid || cfg.savedLastWasEffect)
    {
        state.effectType = cfg.savedEffectType;
        state.effectFlags = (cfg.savedEffectValid ? 0x01 : 0) | (cfg.savedLastWasEffect ? 0x02 : 0);
        state.validFlags |= 0x08 | 0x10; // Mark effect fields as valid
    }
    else
    {
        state.effectType = 0;
        state.effectFlags = 0;
    }

    // reserved[0] = last active scene number (0 = no scene)
    state.reserved[0] = cfg.savedSceneNumber;
    // reserved[1] = last active Effektmanager ID (0 = none)
    cfg.emController.saveState();
    state.reserved[1] = cfg.emController.lastEmId();
    state.reserved[2] = 0;

    return true;
}

void NeoPixelFlashPersistence::restoreSegmentState(uint8_t segmentIndex, const SegmentFlashState& state)
{
    // Note: This is handled in restoreStatesAfterStartup() directly
    // Kept as a placeholder for future refactoring
}
