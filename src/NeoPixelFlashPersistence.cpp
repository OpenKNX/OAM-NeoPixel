#include "NeoPixelFlashPersistence.h"
#include "NeoPixelModule.h"
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
            // Write empty state if segment invalid
            SegmentFlashState emptyState = {};
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
        logWarningP("========================================");
        logWarningP("FLASH SIZE MISMATCH - Configuration changed!");
        logWarningP("Flash data: %d bytes (%d segments)", size, size / sizeof(SegmentFlashState));
        logWarningP("Current config: %d bytes (%d segments)", expectedSegmentSize, segments.size());
        logWarningP("Ignoring flash data to prevent corruption");
        logWarningP("Segments will use default startup behavior");
        logWarningP("========================================");
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
        auto& segments = const_cast<std::vector<NeoPixelBusModule::SegmentConfig>&>(_module->getSegments());
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

    const auto& segments = _module->getSegments();
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
        const auto& cfg = segments[i];
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
                effectType = ParamNEO_NEONEOEffectType;
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

            // Restore brightness/power state
            if (cfg.savedPower == 0)
            {
                seg->setBrightness(0);
#ifdef OPENKNX_DEBUG
                logInfoP("[Segment %d] RESTORED from Flash: OFF (Effect=%d from %s, Color from flash)",
                         i, effectType, effectFromFlash ? "Flash" : "ETS");
#endif
            }
            else
            {
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
                digitalWrite(pin, state ? HIGH : LOW);
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

    const auto& segments = _module->getSegments();
    if (segmentIndex >= segments.size()) return false;

    const auto& cfg = segments[segmentIndex];
    Segment* seg = cfg.segment;

    if (!seg)
    {
        // No segment - save empty state
        memset(&state, 0, sizeof(SegmentFlashState));
        return false;
    }

    // Initialize structure version and flags
    state.version = 0x01;
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
        uint8_t r, g, b;
        VirtualStrip* vstrip = _module->getVirtualStrip();
        if (vstrip && vstrip->getBytesPerLed() == 5)
        {
            // RGBCCT (5 bytes per LED)
            uint8_t ww, cw;
            seg->getPixel(0, r, g, b, ww, cw);
            state.ww = ww;
            state.cw = cw;
        }
        else if (vstrip && vstrip->getBytesPerLed() == 4)
        {
            // RGBW (4 bytes per LED)
            uint8_t w;
            seg->getPixel(0, r, g, b, w);
            state.ww = w;
            state.cw = 0;
        }
        else
        {
            // RGB (3 bytes per LED)
            seg->getPixel(0, r, g, b);
            state.ww = 0;
            state.cw = 0;
        }
        state.r = r;
        state.g = g;
        state.b = b;
        state.validFlags |= 0x02;
    }

    // Save brightness (always valid)
    state.brightness = seg->getBrightness();
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

    // Reserved for future KO parameters (HSV, CCT, effect params)
    state.reserved[0] = 0;
    state.reserved[1] = 0;
    state.reserved[2] = 0;

    return true;
}

void NeoPixelFlashPersistence::restoreSegmentState(uint8_t segmentIndex, const SegmentFlashState& state)
{
    // Note: This is handled in restoreStatesAfterStartup() directly
    // Kept as a placeholder for future refactoring
}
