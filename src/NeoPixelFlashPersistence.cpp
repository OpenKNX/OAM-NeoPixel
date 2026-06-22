#include "NeoPixelFlashPersistence.h"
#include "NeoPixelModule.h"
#include "SceneManager.h"
#include "Segment.h"
#include <algorithm>

namespace
{
    constexpr uint8_t kSceneFlashVersion = 0x01;
    constexpr uint8_t kSceneFlashSignature[4] = {'N', 'S', 'C', 'N'};
} // namespace

NeoPixelFlashPersistence::NeoPixelFlashPersistence(NeoPixelBusModule* module)
    : _module(module)
{
}

void NeoPixelFlashPersistence::invalidateForEtsDownload()
{
    _invalidateForEtsDownload = true;
    logInfoP("ETS parameter download detected - persisted runtime and scene snapshots will be cleared");
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
    size += sizeof(SceneFlashHeader) + sizeof(SceneSegmentFlashState) * numSegments;

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

    const bool invalidateForEtsDownload = _invalidateForEtsDownload;

#ifdef OPENKNX_DEBUG
    logInfoP("========================================");
    logInfoP(invalidateForEtsDownload ? "FLASH WRITE: Clearing persisted LED states for ETS download" : "FLASH WRITE: Saving LED states to flash");
    logInfoP("Segments to save: %d", segments.size());
    logInfoP("========================================");
#else
    if (invalidateForEtsDownload)
    {
        logInfoP("Clearing persisted LED states for ETS download (%d segments)", segments.size());
    }
    else
    {
        logInfoP("Writing LED states to flash (%d segments)", segments.size());
    }
#endif

    // Use OGM-Common flash write helpers
    for (size_t i = 0; i < segments.size(); i++)
    {
        if (invalidateForEtsDownload)
        {
            SegmentFlashState emptyState = {};
            openknx.flash.write((uint8_t*)&emptyState, sizeof(SegmentFlashState));
#ifdef OPENKNX_DEBUG
            logInfoP("[Segment %d] SAVED: Cleared persisted runtime state for ETS download", i);
#endif
            continue;
        }

        SegmentFlashState state;
        if (saveSegmentState(i, state))
        {
            // Write state structure to flash (14 bytes)
            openknx.flash.write((uint8_t*)&state, sizeof(SegmentFlashState));

#ifdef OPENKNX_DEBUG
            logInfoP("[Segment %d] SAVED (v%d, flags=0x%02X):", i, state.version, state.validFlags);
            if (state.validFlags & 0x01) logInfoP("  Power:      %s", state.power ? "ON" : "OFF");
            if (state.validFlags & 0x02) logInfoP("  Colors:     P=(%3d,%3d,%3d,WW=%3d,CW=%3d) S=(%3d,%3d,%3d,WW=%3d,CW=%3d)",
                                                  state.r, state.g, state.b, state.ww, state.cw,
                                                  state.secondaryR, state.secondaryG, state.secondaryB, state.secondaryWW, state.secondaryCW);
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
        if (!invalidateForEtsDownload)
        {
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
        }

        openknx.flash.write((uint8_t*)&relayState, sizeof(RelayFlashState));
    }

    writeSceneStatesToFlash(invalidateForEtsDownload);

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
    uint16_t expectedLegacyTotalSize = expectedSegmentSize;
    if (NeoPixelBusModule::kMaxExternalRelays > 0)
    {
        expectedLegacyTotalSize += sizeof(RelayFlashState);
    }
    const uint16_t expectedSceneSize = sizeof(SceneFlashHeader) + sizeof(SceneSegmentFlashState) * segments.size();
    const uint16_t expectedTotalSize = expectedLegacyTotalSize + expectedSceneSize;
    if (size != expectedSegmentSize && size != expectedLegacyTotalSize && size != expectedTotalSize)
    {
        logWarningP("========================================");
        logWarningP("FLASH SIZE MISMATCH - Configuration changed!");
        logWarningP("Flash data: %d bytes (%d segments)", size, size / sizeof(SegmentFlashState));
        logWarningP("Current config: %d bytes (%d segments)", expectedSegmentSize, segments.size());
        logWarningP("Ignoring flash data to prevent corruption");
        logWarningP("Segments will use default startup behavior");
        logWarningP("========================================");
        _module->setWarningBlink(NEO_WARN_FLASH_DISCARDED); // pulsing Purple (color from warnColorForCode)
        return;
    }

    // Validate FORMAT VERSION: a firmware update may keep the same size but change
    // the meaning of the bytes. The version byte is the first byte of every record
    // (also stamped on empty/invalid segments), so data[0] is reliable here.
    if (data[0] != FLASH_FORMAT_VERSION)
    {
        logWarningP("========================================");
        logWarningP("FLASH FORMAT VERSION MISMATCH - firmware updated!");
        logWarningP("Flash format v%d, firmware expects v%d", data[0], FLASH_FORMAT_VERSION);
        logWarningP("Ignoring flash data to prevent corruption");
        logWarningP("Segments will use default startup behavior");
        logWarningP("========================================");
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
        if (state.validFlags & 0x02) logInfoP("  Colors:     P=(%3d,%3d,%3d,WW=%3d,CW=%3d) S=(%3d,%3d,%3d,WW=%3d,CW=%3d)",
                                              state.r, state.g, state.b, state.ww, state.cw,
                                              state.secondaryR, state.secondaryG, state.secondaryB, state.secondaryWW, state.secondaryCW);
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
            cfg.savedSecondaryR = state.secondaryR;
            cfg.savedSecondaryG = state.secondaryG;
            cfg.savedSecondaryB = state.secondaryB;
            cfg.savedSecondaryWW = state.secondaryWW;
            cfg.savedSecondaryCW = state.secondaryCW;
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
    if (size >= expectedLegacyTotalSize && NeoPixelBusModule::kMaxExternalRelays > 0)
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
    if (size >= expectedLegacyTotalSize && NeoPixelBusModule::kMaxExternalRelays > 0)
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

    if (size == expectedTotalSize)
    {
        readSceneStatesFromFlash(data, size, expectedLegacyTotalSize, segments.size());
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
            VirtualStrip* virtualStrip = _module->getVirtualStrip();
            const uint8_t bytesPerLed = virtualStrip ? virtualStrip->getBytesPerLed() : 3;
            if (bytesPerLed == 5)
            {
                seg->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedCW);
                seg->setSecondaryColor(cfg.savedSecondaryR, cfg.savedSecondaryG, cfg.savedSecondaryB, cfg.savedSecondaryWW, cfg.savedSecondaryCW);
            }
            else
            {
                seg->setPrimaryColor(cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW);
                seg->setSecondaryColor(cfg.savedSecondaryR, cfg.savedSecondaryG, cfg.savedSecondaryB, cfg.savedSecondaryWW);
            }

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
                logInfoP("[Segment %d] RESTORED from Flash: ON (Effect=%d from %s, Primary=R:%d,G:%d,B:%d,WW:%d,CW:%d, Secondary=R:%d,G:%d,B:%d,WW:%d,CW:%d, Brightness=%d)",
                         i, effectType, effectFromFlash ? "Flash" : "ETS",
                         cfg.savedR, cfg.savedG, cfg.savedB, cfg.savedWW, cfg.savedCW,
                         cfg.savedSecondaryR, cfg.savedSecondaryG, cfg.savedSecondaryB, cfg.savedSecondaryWW, cfg.savedSecondaryCW,
                         cfg.savedBrightness);
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

    // Save color (use saved values if available, otherwise read from the segment config)
    if (cfg.savedValid)
    {
        state.r = cfg.savedR;
        state.g = cfg.savedG;
        state.b = cfg.savedB;
        state.ww = cfg.savedWW;
        state.cw = cfg.savedCW;
        state.secondaryR = cfg.savedSecondaryR;
        state.secondaryG = cfg.savedSecondaryG;
        state.secondaryB = cfg.savedSecondaryB;
        state.secondaryWW = cfg.savedSecondaryWW;
        state.secondaryCW = cfg.savedSecondaryCW;
        state.validFlags |= 0x02;
    }
    else
    {
        const EffectConfig& config = seg->getConfig();
        state.r = config.r();
        state.g = config.g();
        state.b = config.b();
        state.ww = config.ww();
        state.cw = config.cw();
        state.secondaryR = config.r2();
        state.secondaryG = config.g2();
        state.secondaryB = config.b2();
        state.secondaryWW = config.ww2();
        state.secondaryCW = config.cw2();
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

    // reserved[0] = last active scene number (0 = no scene)
    state.reserved[0] = cfg.savedSceneNumber;
    // reserved[1] = last active Effektmanager ID (0 = none)
    cfg.emController.saveState();
    state.reserved[1] = cfg.emController.lastEmId();
    state.reserved[2] = 0;

    return true;
}

void NeoPixelFlashPersistence::writeSceneStatesToFlash(bool invalidateSnapshot)
{
    if (_module == nullptr)
    {
        return;
    }

    const auto& segments = _module->getSegments();

    SceneFlashHeader header = {};
    if (!invalidateSnapshot)
    {
        memcpy(header.signature, kSceneFlashSignature, sizeof(header.signature));
        header.version = kSceneFlashVersion;
        header.segmentCount = static_cast<uint8_t>(segments.size());
        header.maxScenes = SceneManager::MAX_SCENES;
        header.sceneSize = SceneManager::SCENE_SIZE;
    }
    openknx.flash.write((uint8_t*)&header, sizeof(header));

    SceneSegmentFlashState segmentState = {};
    for (size_t segmentIndex = 0; segmentIndex < segments.size(); ++segmentIndex)
    {
        memset(&segmentState, 0, sizeof(segmentState));
        if (!invalidateSnapshot)
        {
            uint8_t length = 0;
            if (_module->_sceneManager != nullptr && _module->_sceneManager->exportScenes(static_cast<uint8_t>(segmentIndex), segmentState.payload, length))
            {
                openknx.flash.write((uint8_t*)&segmentState, sizeof(segmentState));
                continue;
            }

            segmentState.payload[0] = 0;
        }

        openknx.flash.write((uint8_t*)&segmentState, sizeof(segmentState));
    }
}

bool NeoPixelFlashPersistence::readSceneStatesFromFlash(const uint8_t* data, uint16_t size, uint16_t offset, uint16_t segmentCount)
{
    if (_module == nullptr || _module->_sceneManager == nullptr)
    {
        return false;
    }

    const uint16_t requiredSize = static_cast<uint16_t>(offset + sizeof(SceneFlashHeader) + sizeof(SceneSegmentFlashState) * segmentCount);
    if (data == nullptr || size < requiredSize)
    {
        return false;
    }

    SceneFlashHeader header = {};
    memcpy(&header, data + offset, sizeof(header));
    offset += sizeof(header);

    const SceneFlashHeader emptyHeader = {};
    if (memcmp(&header, &emptyHeader, sizeof(header)) == 0)
    {
        logInfoP("Scene flash snapshot is empty after ETS download; using ETS scene configuration");
        return false;
    }

    if (memcmp(header.signature, kSceneFlashSignature, sizeof(header.signature)) != 0 ||
        header.version != kSceneFlashVersion ||
        header.segmentCount != segmentCount ||
        header.maxScenes != SceneManager::MAX_SCENES ||
        header.sceneSize != SceneManager::SCENE_SIZE)
    {
        logWarningP("Scene flash snapshot ignored: incompatible header");
        return false;
    }

    for (uint8_t segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex)
    {
        const SceneSegmentFlashState* segmentState = reinterpret_cast<const SceneSegmentFlashState*>(data + offset);
        offset += sizeof(SceneSegmentFlashState);

        const uint8_t sceneCount = segmentState->payload[0];
        const uint16_t payloadLength = static_cast<uint16_t>(1 + std::min<uint8_t>(sceneCount, SceneManager::MAX_SCENES) * SceneManager::SCENE_SIZE);
        if (!_module->_sceneManager->importScenes(segmentIndex, segmentState->payload, static_cast<uint8_t>(payloadLength)))
        {
            logWarningP("Scene flash snapshot for segment %d could not be restored", segmentIndex);
        }
    }

    logInfoP("Scene flash snapshot restored for %d segment(s)", segmentCount);
    return true;
}

void NeoPixelFlashPersistence::restoreSegmentState(uint8_t segmentIndex, const SegmentFlashState& state)
{
    // Note: This is handled in restoreStatesAfterStartup() directly
    // Kept as a placeholder for future refactoring
}
