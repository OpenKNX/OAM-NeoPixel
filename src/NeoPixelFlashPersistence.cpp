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

        // Only "Letzter Zustand" segments take the flash state; the others keep their ETS
        // startup state, so their saved* fields describe what they actually show.
        if (_module->effectiveStartupBehavior(i) != 1) continue;

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

        // Parameter macros use this persistence object's channel index; the module's
        // index is also needed by applyEffectToSegment() below.
        _channelIndex = static_cast<uint8_t>(i);
        _module->setChannelIndex(i);
        uint8_t effectiveBehavior = _module->effectiveStartupBehavior(i);

        // Only restore if mode is "Letzter Zustand" (1)
        if (effectiveBehavior != 1)
        {
#ifdef OPENKNX_DEBUG
            const char* behaviorNames[] = {"Aus", "Letzter Zustand", "An (ETS-Werte)"};
            logInfoP("[Segment %d] SKIPPED: Behavior is '%s', not 'Letzter Zustand'", i, behaviorNames[effectiveBehavior]);
            skippedCount++;
#endif
            continue;
        }

        // Mode is "Letzter Zustand" - try to restore from flash
        if (cfg.savedValid)
        {
            // Own power from flash. Every restore branch ends with segmentOff() for a saved OFF
            // (an EM is paused and resumed by the next ON).
            const uint8_t flashPower = cfg.savedPower;

            // A scene or EM start snapshots the live direct state (EM return "Letzter Zustand",
            // Effektkette freeze): put the saved one on the segment first, not the ETS boot state.
            if (cfg.savedSceneNumber > 0 || cfg.emController.lastEmId() != EM_NONE)
            {
                if (cfg.savedEffectValid) _module->applyEffectToSegment(seg, cfg.savedEffectType);
                seg->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedCW);
            }

            // Check if a scene was active — if so, recall it directly
            if (cfg.savedSceneNumber > 0 && _module->_sceneManager)
            {
                // Seed master from the saved intent BEFORE recall: an EM-action scene starts an EM via
                // recallScene() (rendering master*cue/255), so without this it would resume at the ETS
                // boot-seed brightness instead of the remembered level — the EM branch below seeds the
                // same way. A static scene that carries its own brightness simply overrides this.
                // recallScene() overwrites savedBrightness with the scene value; keep the intent the
                // user set after the recall (e.g. dimmed).
                const uint8_t intentBrightness = cfg.savedBrightness;
                seg->setMasterBrightness(intentBrightness);
                // Recall as OFF: a scene or EM start switches on, a brightness-0 scene leaves it at 0
                // without overwriting the flash state with a snapshot of the boot state.
                cfg.savedPower = 0;
                bool sceneRecalled = _module->_sceneManager->recallScene(i, cfg.savedSceneNumber, seg);
                const bool sceneOn = cfg.savedPower != 0;
                cfg.savedPower = flashPower;
                cfg.emStartedFromOff = false;
                if (sceneRecalled && sceneOn)
                {
                    cfg.savedBrightness = intentBrightness;
                    seg->setMasterBrightness(intentBrightness);
                    if (cfg.emController.isRunning())
                        cfg.emController.reapplyMasterBrightness(seg);
                    else
                        seg->setBrightness(intentBrightness);
                    if (flashPower == 0) _module->segmentOff(i, false);
#ifdef OPENKNX_DEBUG
                    logInfoP("[Segment %d] RESTORED Scene %d from Flash (power=%s)",
                             i, cfg.savedSceneNumber, cfg.savedPower ? "ON" : "OFF");
                    restoredCount++;
#endif
                    continue;
                }
                // Switch-off scene or refused EM start: restore the saved state below. The recall may
                // have stopped the segment; the branches below set the power again.
                if (sceneRecalled) seg->resume();
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
                _module->captureSyncFreeze(i); // Effektkette master: the state the segment returns to after the EM
                cfg.emController.restoreState(seg, _module->_emData);
                if (flashPower == 0) _module->segmentOff(i, false); // paused, the next ON resumes it
#ifdef OPENKNX_DEBUG
                logInfoP("[Segment %d] RESTORED EffektManager %d from Flash", i, cfg.emController.lastEmId());
                restoredCount++;
#endif
                continue; // EM handles the segment, skip normal restore
            }

            // Determine effect source: Flash (if KO-changed) or ETS (base config)
            uint8_t effectType;
            bool effectFromFlash = false;

            // Solid is effect type 0; a colour KO after it must not drop it in favour of the ETS effect
            if (cfg.savedEffectValid)
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
            }
            // If effect from ETS: already loaded in configureEffects(), skip re-apply

            // Restore color from flash
            seg->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedCW);

            // Restore brightness/power state. Drive the MASTER (intent) layer like the boot seed
            // (EffectConfiguration.cpp:48-51) so the restored brightness survives a later global-
            // brightness recompute or EM start (render = master*cue/255). "Aus" keeps the intent
            // and stops the segment, so a subsequent power-on returns to it.
            seg->setMasterBrightness(cfg.savedBrightness);
            seg->setBrightness(cfg.savedBrightness);
            if (flashPower == 0)
            {
                _module->segmentOff(i, false);
#ifdef OPENKNX_DEBUG
                logInfoP("[Segment %d] RESTORED from Flash: OFF (Effect=%d from %s, Color from flash)",
                         i, effectType, effectFromFlash ? "Flash" : "ETS");
#endif
            }
            else
            {
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

    // "Alle LEDs" AUS received during the startup delay: the restore above may have resumed a
    // suspended segment (EM restart, effect enter). Suspend it again before the frame goes out.
    for (size_t i = 0; i < segments.size(); i++)
        if (segments[i].segment && segments[i].globalSuspended) _module->globalSuspendSegment(i);

    // Via the module helper: a direct vstrip->show() bypasses the strip arbitration.
    _module->showVirtualStripIfIdle();
#ifdef OPENKNX_DEBUG
    logInfoP("Hardware update triggered (show() called)");
#endif

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

    // Power = visible: own power ON and not suspended by global power (a segment saved under global
    // OFF restores off). The brightness intent is kept separately for the next power-on.
    state.power = NeoPixelBusModule::isSegmentVisible(cfg) ? 1 : 0;
    state.validFlags |= 0x01;

    // Save color: the live primary colour, which follows every colour path (direct, relative,
    // dimming, presets, scenes) and is kept while the segment is off. The saved snapshot instead
    // while an EM runs (the primary is the cue colour). Never the pixel buffer: it holds the
    // rendered, brightness-scaled output and is blanked by prepareLedShutdown() before Common
    // saves on restart / ETS download.
    if (cfg.savedValid && cfg.emController.isRunning())
    {
        state.r = cfg.savedR;
        state.g = cfg.savedG;
        state.b = cfg.savedB;
        state.ww = cfg.savedWW;
        state.cw = cfg.savedCW;
    }
    else
    {
        const uint32_t primary = seg->getPrimaryColor(); // 0xRRGGBBWW
        const uint8_t bytesPerLed = _module->getVirtualStrip() ? _module->getVirtualStrip()->getBytesPerLed() : 3;
        state.r = (primary >> 24) & 0xFF;
        state.g = (primary >> 16) & 0xFF;
        state.b = (primary >> 8) & 0xFF;
        state.ww = (bytesPerLed >= 4) ? (primary & 0xFF) : 0;
        state.cw = (bytesPerLed == 5) ? seg->getConfig().cw() : 0;
    }
    state.validFlags |= 0x02;

    // Save brightness (always valid): the pre-global intent. The render is cue-scaled under an EM,
    // global-scaled otherwise (_globalBrightness is not persisted and resets to 255 on boot) and 0
    // for a segment that is switched off; the restore feeds the intent back as the master.
    state.brightness = cfg.savedBrightness;
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
