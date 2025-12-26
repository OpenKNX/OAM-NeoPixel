#include "NeoPixelFlashPersistence.h"
#include "NeoPixelModule.h"
#include "Segment.h"

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
            logInfoP("  Color:      R=%3d G=%3d B=%3d W=%3d", state.r, state.g, state.b, state.w);
            logInfoP("  Brightness: %d", state.brightness);
            if (state.effectType > 0)
            {
                logInfoP("  Effect:     Type=%d Speed=%d Intensity=%d", 
                         state.effectType, state.effectSpeed, state.effectIntensity);
            }
            else
            {
                logInfoP("  Effect:     None (solid color)");
            }
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
    uint16_t expectedSize = sizeof(SegmentFlashState);

    for (size_t i = 0; i < segments.size(); i++)
    {
        // Check if we have enough data left
        if (offset + expectedSize > size)
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
        logInfoP("  Color:      R=%3d G=%3d B=%3d W=%3d", state.r, state.g, state.b, state.w);
        logInfoP("  Brightness: %d", state.brightness);
        if (state.effectType > 0)
        {
            logInfoP("  Effect:     Type=%d Speed=%d Intensity=%d", 
                     state.effectType, state.effectSpeed, state.effectIntensity);
        }
        else
        {
            logInfoP("  Effect:     None (solid color)");
        }
#endif
        
        // Store in segment config for later restoration (after startup delay)
        auto& segments = const_cast<std::vector<NeoPixelBusModule::SegmentConfig>&>(_module->getSegments());
        auto& cfg = segments[i];
        
        cfg.savedPower = state.power;
        cfg.savedR = state.r;
        cfg.savedG = state.g;
        cfg.savedB = state.b;
        cfg.savedW = state.w;
        cfg.savedBrightness = state.brightness;
        cfg.savedEffectType = state.effectType;
        // Note: Effect-specific parameters are now loaded from ETS, not from flash
        cfg.savedValid = true;
        cfg.savedEffectValid = (state.effectType > 0);
        cfg.savedLastWasEffect = (state.effectType > 0);
        
#ifdef OPENKNX_DEBUG
        logInfoP("  -> Stored in config for later restoration");
#endif
    }

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
            case 0: // OFF
                seg->setBrightness(0);
                seg->setPrimaryColor(0, 0, 0, 0);
#ifdef OPENKNX_DEBUG
                logInfoP("[Segment %d] APPLIED: OFF (brightness=0)", i);
                restoredCount++;
#endif
                break;

            case 1: // LAST STATE (flash restore)
                if (cfg.savedValid)
                {
                    // Check power state
                    if (cfg.savedPower == 0)
                    {
                        seg->setBrightness(0);
                        seg->setPrimaryColor(0, 0, 0, 0);
#ifdef OPENKNX_DEBUG
                        logInfoP("[Segment %d] RESTORED: Last state was OFF", i);
#endif
                    }
                    else if (cfg.savedLastWasEffect && cfg.savedEffectValid && cfg.savedEffectType > 0)
                    {
                        // Restore effect (parameters loaded from ETS automatically)
                        _module->applyEffectToSegment(seg, cfg.savedEffectType);
                        _module->setupEffectConfiguration(seg);
                        seg->setBrightness(cfg.savedBrightness);
#ifdef OPENKNX_DEBUG
                        logInfoP("[Segment %d] RESTORED: Last effect (Type=%d, Brightness=%d)", 
                                 i, cfg.savedEffectType, cfg.savedBrightness);
#endif
                    }
                    else
                    {
                        // Restore solid color
                        seg->setBrightness(cfg.savedBrightness);
                        seg->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedW);
#ifdef OPENKNX_DEBUG
                        logInfoP("[Segment %d] RESTORED: Last color (R=%d,G=%d,B=%d,W=%d, Brightness=%d)", 
                                 i, cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedW, cfg.savedBrightness);
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
                    seg->setPrimaryColor(0, 0, 0, 0);
                }
                break;

            case 2: // DEFAULT COLOR
                {
                    uint8_t r, g, b, w, brightness, effectType;
                    
                    // Get default color params (segment-specific or global)
                    if (segmentBehavior == 3)
                    {
                        // Segment has own default color
                        r = ParamNEO_NEOSegmentStartupR;
                        g = ParamNEO_NEOSegmentStartupG;
                        b = ParamNEO_NEOSegmentStartupB;
                        w = ParamNEO_NEOSegmentStartupW;
                        brightness = ParamNEO_NEOSegmentStartupBrightness;
                        effectType = ParamNEO_NEOSegmentStartupEffect;
                    }
                    else
                    {
                        // Use global default color
                        r = ParamNEO_NEOGlobalStartupR;
                        g = ParamNEO_NEOGlobalStartupG;
                        b = ParamNEO_NEOGlobalStartupB;
                        w = ParamNEO_NEOGlobalStartupW;
                        brightness = ParamNEO_NEOGlobalStartupBrightness;
                        effectType = ParamNEO_NEOGlobalStartupEffect;
                    }

                    // Apply default color
                    seg->setBrightness(brightness);
                    if (effectType > 0)
                    {
                        _module->applyEffectToSegment(seg, effectType);
#ifdef OPENKNX_DEBUG
                        logInfoP("[Segment %d] APPLIED: Default effect (Type=%d, Brightness=%d)", i, effectType, brightness);
#endif
                    }
                    else
                    {
                        seg->setPrimaryColor(r, g, b, w);
#ifdef OPENKNX_DEBUG
                        logInfoP("[Segment %d] APPLIED: Default color (R=%d,G=%d,B=%d,W=%d, Brightness=%d)", 
                                 i, r, g, b, w, brightness);
#endif
                    }
#ifdef OPENKNX_DEBUG
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
    
    // Save color (use savedR/G/B if valid, otherwise read from segment)
    if (cfg.savedValid)
    {
        state.r = cfg.savedR;
        state.g = cfg.savedG;
        state.b = cfg.savedB;
        state.w = cfg.savedW;
    }
    else
    {
        uint8_t r, g, b;
        VirtualStrip* vstrip = _module->getVirtualStrip();
        if (vstrip && vstrip->getBytesPerLed() == 4)
        {
            uint8_t w;
            seg->getPixel(0, r, g, b, w);
            state.w = w;
        }
        else
        {
            seg->getPixel(0, r, g, b);
            state.w = 0;
        }
        state.r = r;
        state.g = g;
        state.b = b;
    }
    
    // Save brightness
    state.brightness = seg->getBrightness();
    
    // Save effect type only (parameters are always loaded from ETS)
    state.effectType = cfg.savedEffectType;
    // Note: effectSpeed and effectIntensity fields deprecated (always 0)
    state.effectSpeed = 0;
    state.effectIntensity = 0;
    
    return true;
}

void NeoPixelFlashPersistence::restoreSegmentState(uint8_t segmentIndex, const SegmentFlashState& state)
{
    // Note: This is handled in restoreStatesAfterStartup() directly
    // Kept as a placeholder for future refactoring
}
