#include "EffectConfiguration.h"
#include "NeoPixelModule.h"
#include "OpenKNX.h"
#include "knxprod.h"

// Include auto-generated effect parameter mappings (if available)
#if defined(__has_include)
    #if __has_include("EffectParameterMapping.h")
        #define EFFECT_PARAMETER_MAPPING_GENERATED
        #include "EffectParameterMapping.h"
    #endif
#endif

// Include auto-generated effect type mapping
#include "EffectTypeMapping.h"

EffectConfiguration::EffectConfiguration(NeoPixelBusModule* module)
    : _module(module)
{
}

void EffectConfiguration::configureEffects()
{
    auto& segments = _module->getSegments();
    auto& neoPixel = _module->getNeoPixel();

    // Read global startup behavior (used as default for segments)
    _module->setChannelIndex(0);
    uint8_t globalBehavior = ParamNEO_NEOGlobalStartupBehavior; // 0=Aus, 1=Letzter Zustand, 2=ETS-Parameterwert

    for (size_t i = 0; i < segments.size(); ++i)
    {
        if (segments[i].segment)
        {
            _module->setChannelIndex(i); // Set channel context for parameter access
            uint8_t _channelIndex = i;   // Local variable for ETS parameter macros

            // 1. Load ETS configuration (Effect + Parameters)
            uint8_t effectType = ParamNEO_NEONEOEffectType;
            applyEffectToSegment(segments[i].segment, effectType);
            setupEffectConfiguration(segments[i].segment, true); // loadDefaultColor = true at startup

            // Initialize saved effect state from ETS defaults
            segments[i].savedEffectType = effectType;
            segments[i].savedEffectValid = true;

            // 2. Determine startup behavior (segment can override global)
            uint8_t segmentBehavior = ParamNEO_NEOSegmentStartupBehavior;
            uint8_t effectiveBehavior;

            if (segmentBehavior == 0)
            {
                // Use global setting
                effectiveBehavior = globalBehavior;
            }
            else
            {
                // Segment-specific setting (subtract 1: 1=Aus->0, 2=Letzter->1, 3=ETS->2)
                effectiveBehavior = segmentBehavior - 1;
            }

            // 3. Apply startup behavior
            const char* behaviorNames[] = {"Aus", "Letzter Zustand", "ETS-Parameterwert"};

            switch (effectiveBehavior)
            {
                case 0: // Ausgeschaltet
                    segments[i].segment->setBrightness(0);
                    logInfoP("Segment %zu: ETS-Config geladen, LEDs AUS (Behavior: %s)", i, behaviorNames[0]);
                    break;

                case 1: // Letzter Zustand (Flash)
                    // Flash restore will be attempted in processAfterStartupDelay()
                    // If no flash data exists, the ETS config (already loaded above) will remain
                    logInfoP("Segment %zu: ETS-Config geladen, Flash-Restore später (Behavior: %s)", i, behaviorNames[1]);
                    break;

                case 2: // ETS-Parameterwert (DEFAULT)
                    // ETS configuration already loaded above - nothing more to do!
                    logInfoP("Segment %zu: ETS-Config geladen und aktiv (Behavior: %s, Effect: %d)", i, behaviorNames[2], effectType);
                    break;
            }
        }
    }

    // Read global UpdateSpeed parameter
    _module->setChannelIndex(0); // Use first segment for global settings
    uint8_t updateSpeedParam = ParamNEO_NEOUpdateSpeed;
    UpdateSpeed speed = UpdateSpeed::NORMAL;
    const char* speedName = "Normal";

    // Safely map parameter value to UpdateSpeed enum
    switch (updateSpeedParam)
    {
        case 100:
            speed = UpdateSpeed::SLOW;
            speedName = "Slow (10 FPS)";
            break;
        case 50:
            speed = UpdateSpeed::NORMAL;
            speedName = "Normal (20 FPS)";
            break;
        case 33:
            speed = UpdateSpeed::FAST;
            speedName = "Fast (30 FPS)";
            break;
        case 20:
            speed = UpdateSpeed::MAX;
            speedName = "Max (50 FPS)";
            break;
        case 12:
            speed = UpdateSpeed::EXTREME;
            speedName = "Extreme (80 FPS)";
            break;
        case 4:
            speed = UpdateSpeed::LUDICROUS;
            speedName = "Ludicrous (120 FPS)";
            break;
        case 0:
            speed = UpdateSpeed::FTL;
            speedName = "FTL (240 FPS)";
            break;
        default:
            logInfoP("Invalid UpdateSpeed parameter %d, using Normal", updateSpeedParam);
            speed = UpdateSpeed::NORMAL;
            speedName = "Normal (20 FPS) [fallback]";
            break;
    }

    // ALWAYS enable auto-update (even without effects) so KO color changes are rendered
    neoPixel.setAutoUpdate(true);
    neoPixel.setUpdateSpeed(speed);

    logInfoP("Effect auto-update enabled at %s", speedName);
}

void EffectConfiguration::applyEffectToSegment(Segment* segment, uint8_t effectType)
{
    if (!segment) return;

    Effect* effect = getEffectFromType(effectType);
    if (effect != nullptr)
    {
        // Store effect type in config for parameter loading
        segment->getConfig().effectType = effectType;

        // Don't clear the segment - preserve current color!
        // Effects that generate their own colors (Rainbow, Fire) will override anyway
        // Effects that need a color (Wipe, Theater Chase) will use the primary color

        segment->setEffect(effect);

        // Check if we should reset to default color on effect change
        // This is controlled by the ETS parameter "Bei Effektwechsel Farbe auf Standard zurücksetzen"
        uint8_t _channelIndex = _module->getChannelIndex();
        bool resetToDefault = ParamNEO_NEOSegmentResetColorOnEffectChange;

        // Load effect-specific parameters from ETS after setting the effect
        // If resetToDefault is true, also reload the default color from ETS
        setupEffectConfiguration(segment, resetToDefault);

        logInfoP("Applied effect '%s' (ID: %d) to segment %s",
                 effect->getName(), effectType,
                 resetToDefault ? "(with color reset)" : "(keeping current color)");
    }
    else
    {
        logWarningP("Unknown effect type: %d", effectType);
    }
}

// getEffectFromType() is now auto-generated in EffectTypeMapping.h by Build-EffectParameters.ps1
// This ensures effect IDs stay synchronized with ETS XML and EffectPool.cpp registration order

void EffectConfiguration::setupEffectConfiguration(Segment* segment, bool loadDefaultColor)
{
    if (!segment) return;

    // Load effect-specific parameters from ETS (AUTO-GENERATED by Build-EffectParameters.ps1)
    // This replaces the old generic speed/intensity/option parameters with effect-specific ones
    // (e.g., WipeSpeed, CylonEyeSize, FireCooling, etc.)
    Effect* effect = segment->getEffect();
    if (effect)
    {
#ifdef EFFECT_PARAMETER_MAPPING_GENERATED
        uint8_t channelIndex = _module->getChannelIndex();
        loadEffectParameters(effect, segment, segment->getConfig().effectType, channelIndex);
        logInfoP("Effect '%s' configured with %d parameter(s)",
                 effect->getName(), effect->getParameterCount());
#else
        logDebugP("Effect '%s' loaded (no parameters - run Build-EffectParameters.ps1 to generate)",
                  effect->getName());
#endif
    }

    uint8_t _channelIndex = _module->getChannelIndex(); // Local variable for ETS parameter macros

    // Load default color from ETS ONLY at startup
    // When switching effects, keep the current color set by user
    if (loadDefaultColor)
    {
        // TypeColor RGB format: 0x00RRGGBB (24-bit)
        uint32_t color = ParamNEO_NEOSegmentStartupColor;
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        uint8_t w = ParamNEO_NEOSegmentStartupW;
        segment->setPrimaryColor(r, g, b, w);
        logInfoP("Loaded default color from ETS: R=%d G=%d B=%d W=%d (0x%06X)", r, g, b, w, color);
    }

    // Mirror effect (from segment configuration)
    bool mirrorEffect = ParamNEO_NEOSegmentMirrorEffect;
    if (mirrorEffect)
    {
        logInfoP("Mirror effect enabled for segment");
    }
}
