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
            // Write state structure to flash (10 bytes)
            openknx.flash.write((uint8_t*)&state, sizeof(SegmentFlashState));

#ifdef OPENKNX_DEBUG
            logInfoP("[Segment %d] SAVED:", i);
            logInfoP("  Power:      %s (%d)", state.power ? "ON" : "OFF", state.power);
            logInfoP("  Color:      R=%3d G=%3d B=%3d WW=%3d CW=%3d", state.r, state.g, state.b, state.ww, state.cw);
            logInfoP("  Brightness: %d", state.brightness);
            logInfoP("  Note:       Effect type/params from ETS (not saved)");
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
        logInfoP("[Segment %d] LOADED from flash:", i);
        logInfoP("  Power:      %s (%d)", state.power ? "ON" : "OFF", state.power);
        logInfoP("  Color:      R=%3d G=%3d B=%3d WW=%3d CW=%3d", state.r, state.g, state.b, state.ww, state.cw);
        logInfoP("  Brightness: %d", state.brightness);
        logInfoP("  Note:       Effect type/params from ETS (not flash)");
#endif

        // Store in segment config for later restoration (after startup delay)
        auto& segments = const_cast<std::vector<NeoPixelBusModule::SegmentConfig>&>(_module->getSegments());
        auto& cfg = segments[i];

        cfg.savedPower = state.power;
        cfg.savedR = state.r;
        cfg.savedG = state.g;
        cfg.savedB = state.b;
        cfg.savedWW = state.ww;
        cfg.savedCW = state.cw;
        cfg.savedBrightness = state.brightness;
        
        // Phase 1: Effect type NOT restored from flash - always use ETS configuration
        // savedEffectType will be set from ETS in restoreStatesAfterStartup()
        cfg.savedValid = true;
        
        // Phase 2: Reserved fields will contain KO-changed effect parameter flags
        // For now, mark as invalid so ETS effect config is used
        cfg.savedEffectValid = false;
        cfg.savedLastWasEffect = false;

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
    // Statistics counters (only used in debug mode)
    int restoredCount = 0;
    int skippedCount = 0;

    logInfoP("========================================");
    logInfoP("RESTORE: Applying LED states based on startup behavior");
    logInfoP("Segments to process: %d", segments.size());
    logInfoP("========================================");
#else
    logInfoP("Restoring LED states after startup delay...");
#endif

    // TEMPORARILY DISABLED FOR TESTING - Get global startup behavior
    uint8_t globalBehavior = ParamNEO_NEOGlobalStartupBehavior; // 0=Off, 1=Last, 2=Default

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

        // Get segment startup behavior (0=Use Global, 1=Off, 2=Last, 3=Default)
        // Set _channelIndex for ParamNEO_NEO* macros to work
        _channelIndex = i;
        uint8_t segmentBehavior = ParamNEO_NEOSegmentStartupBehavior;

        // Determine effective behavior (segment override or global)
        uint8_t effectiveBehavior;
        if (segmentBehavior == 0)
        {
            // Use global setting
            effectiveBehavior = globalBehavior; // 0=Off, 1=Last, 2=Default
        }
        else
        {
            // Segment-specific setting (subtract 1: 1=Off->0, 2=Last->1, 3=Default->2)
            effectiveBehavior = segmentBehavior - 1;
        }

#ifdef OPENKNX_DEBUG
        const char* behaviorNames[] = {"OFF", "LAST", "DEFAULT"};
        logInfoP("[Segment %d] Behavior: %s (segment=%d, global=%d)",
                 i, behaviorNames[effectiveBehavior], segmentBehavior, globalBehavior);
#endif

        // Apply behavior
        switch (effectiveBehavior)
        {
            case 0: // OFF - Keep configuration but set power off
            {
                // Phase 1: Effect type always from ETS, color/brightness from flash
                uint8_t etsEffectType = ParamNEO_NEONEOEffectType;
                
                // Apply effect from ETS
                _module->applyEffectToSegment(seg, etsEffectType);
                if (etsEffectType > 0)
                {
                    _module->setupEffectConfiguration(seg);
                }
                
                // Restore color if available, otherwise use black
                if (cfg.savedValid)
                {
                    seg->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedCW);
                    // Store the saved brightness internally but set display to 0
                    // The segment will remember this brightness for when power is restored
                    seg->setBrightness(0);
#ifdef OPENKNX_DEBUG
                    logInfoP("[Segment %d] APPLIED: OFF (Effect=%d from ETS, Color=R:%d,G:%d,B:%d,WW:%d,CW:%d from flash, brightness=0, saved=%d)",
                             i, etsEffectType, cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedCW, cfg.savedBrightness);
#endif
                }
                else
                {
                    // No saved data - use black
                    seg->setBrightness(0);
                    seg->setPrimaryColor(0, 0, 0, 0, 0);
#ifdef OPENKNX_DEBUG
                    logInfoP("[Segment %d] APPLIED: OFF (Effect=%d from ETS, no saved color, brightness=0)", i, etsEffectType);
#endif
                }
#ifdef OPENKNX_DEBUG
                restoredCount++;
#endif
            }
            break;

            case 1: // LAST STATE (flash restore)
                if (cfg.savedValid)
                {
                    // Phase 1: Load effect type from ETS (always current config)
                    uint8_t etsEffectType = ParamNEO_NEONEOEffectType;
                    
                    // Apply effect from ETS (not flash!)
                    _module->applyEffectToSegment(seg, etsEffectType);
                    if (etsEffectType > 0)
                    {
                        _module->setupEffectConfiguration(seg);
                    }
                    
                    // Restore color and brightness from flash
                    seg->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedCW);
                    
                    // Check power state
                    if (cfg.savedPower == 0)
                    {
                        seg->setBrightness(0); // Power off
#ifdef OPENKNX_DEBUG
                        logInfoP("[Segment %d] RESTORED: Last state OFF (Effect=%d from ETS, Color from flash, Brightness=0)",
                                 i, etsEffectType);
#endif
                    }
                    else
                    {
                        seg->setBrightness(cfg.savedBrightness);
#ifdef OPENKNX_DEBUG
                        logInfoP("[Segment %d] RESTORED: Last state ON (Effect=%d from ETS, Color=R:%d,G:%d,B:%d,WW:%d,CW:%d from flash, Brightness=%d)",
                                 i, etsEffectType, cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedCW, cfg.savedBrightness);
#endif
                    }
#ifdef OPENKNX_DEBUG
                    restoredCount++;
#endif
                }
                else
                {
#ifdef OPENKNX_DEBUG
                    logInfoP("[Segment %d] WARNING: Last state requested but no valid flash data - keeping OFF", i);
                    skippedCount++;
#endif
                    seg->setBrightness(0);
                    seg->setPrimaryColor(0, 0, 0, 0, 0);
                }
                break;

            case 2: // DEFAULT COLOR (Standard-Farbe)
            {
                uint8_t r, g, b, ww, cw, brightness, effectType;

                // Get default color params (segment-specific or global)
                if (segmentBehavior == 3)
                {
                    // Segment has own default color - _channelIndex must be set correctly for macros
                    logDebugP("[Segment %d] Reading segment-specific startup params with _channelIndex=%d", i, _channelIndex);
                    r = ParamNEO_NEOSegmentStartupR;
                    g = ParamNEO_NEOSegmentStartupG;
                    b = ParamNEO_NEOSegmentStartupB;
                    ww = ParamNEO_NEOSegmentStartupW; // TODO: Add separate WW/CW params if needed
                    cw = 0;                           // Default to 0 for now
                    brightness = ParamNEO_NEOSegmentStartupBrightness;
                    effectType = ParamNEO_NEOSegmentStartupEffect;
                    logDebugP("[Segment %d] Read from ETS: R=%d,G=%d,B=%d,WW=%d,CW=%d,Br=%d,Eff=%d",
                              i, r, g, b, ww, cw, brightness, effectType);
                }
                else
                {
                    // Use global default color
                    logDebugP("[Segment %d] Reading GLOBAL startup params (segmentBehavior=%d)", i, segmentBehavior);
                    r = ParamNEO_NEOGlobalStartupR;
                    g = ParamNEO_NEOGlobalStartupG;
                    b = ParamNEO_NEOGlobalStartupB;
                    ww = ParamNEO_NEOGlobalStartupW; // TODO: Add separate WW/CW params if needed
                    cw = 0;                          // Default to 0 for now
                    brightness = ParamNEO_NEOGlobalStartupBrightness;
                    effectType = ParamNEO_NEOGlobalStartupEffect;
                    logDebugP("[Segment %d] Read from ETS: R=%d,G=%d,B=%d,WW=%d,CW=%d,Br=%d,Eff=%d",
                              i, r, g, b, ww, cw, brightness, effectType);
                }

                // Apply effect FIRST (this clears color), then color, then brightness
                _module->applyEffectToSegment(seg, effectType);

                if (effectType > 0)
                {
                    // Load effect parameters from ETS for active effects
                    _module->setupEffectConfiguration(seg);
                }

                // Always set color (effects use primary color, Solid needs it)
                seg->setPrimaryColor(r, g, b, ww, cw);
                // Set brightness LAST
                seg->setBrightness(brightness);

#ifdef OPENKNX_DEBUG
                if (effectType > 0)
                {
                    logInfoP("[Segment %d] APPLIED: Default effect (Type=%d, Color=R:%d,G:%d,B:%d,WW:%d,CW:%d, Brightness=%d)",
                             i, effectType, r, g, b, ww, cw, brightness);
                }
                else
                {
                    logInfoP("[Segment %d] APPLIED: Default color (R=%d,G=%d,B=%d,WW=%d,CW=%d, Brightness=%d)",
                             i, r, g, b, ww, cw, brightness);
                }
                restoredCount++;
#endif
            }
            break;
        }
    }

    // Trigger update to hardware
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
            _module->setRelayOutput(i, state);
        }
    }

#ifdef OPENKNX_DEBUG
    logInfoP("========================================");
    logInfoP("RESTORE: Complete");
    logInfoP("  Applied:  %d segments", restoredCount);
    logInfoP("  Skipped:  %d segments", skippedCount);
    logInfoP("========================================");
#else
    logInfoP("LED state restoration complete");
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

    // Determine power state (brightness > 0 = on)
    state.power = (seg->getBrightness() > 0) ? 1 : 0;

    // Save color (use savedR/G/B/WW/CW if valid, otherwise read from segment)
    if (cfg.savedValid)
    {
        state.r = cfg.savedR;
        state.g = cfg.savedG;
        state.b = cfg.savedB;
        state.ww = cfg.savedWW;
        state.cw = cfg.savedCW;
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
    }

    // Save brightness
    state.brightness = seg->getBrightness();

    // Phase 1: Effect type always comes from ETS, not from flash
    // Phase 2: Reserved fields will store KO-changed effect parameters
    state.reserved1 = 0;
    state.reserved2 = 0;
    state.reserved3 = 0;

    return true;
}

void NeoPixelFlashPersistence::restoreSegmentState(uint8_t segmentIndex, const SegmentFlashState& state)
{
    // Note: This is handled in restoreStatesAfterStartup() directly
    // Kept as a placeholder for future refactoring
}
