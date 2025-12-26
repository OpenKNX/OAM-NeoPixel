#include "EffectConfiguration.h"
#include "NeoPixelModule.h"
#include "OpenKNX.h"
#include "knxprod.h"

// Include auto-generated effect parameter mappings (if available)
#ifdef EFFECT_PARAMETER_MAPPING_GENERATED
extern void loadEffectParameters(Effect* effect, Segment* segment, uint8_t effectType, uint8_t channelIndex);
#endif

EffectConfiguration::EffectConfiguration(NeoPixelBusModule* module)
    : _module(module)
{
}

void EffectConfiguration::configureEffects()
{
    auto& segments = _module->getSegments();
    auto& neoPixel = _module->getNeoPixel();

    for (size_t i = 0; i < segments.size(); ++i)
    {
        if (segments[i].segment)
        {
            _module->setChannelIndex(i); // Set channel context for parameter access
            uint8_t _channelIndex = i;   // Local variable for ETS parameter macros
            uint8_t effectType = ParamNEO_NEONEOEffectType;
            applyEffectToSegment(segments[i].segment, effectType);
            setupEffectConfiguration(segments[i].segment);
            // Initialize saved effect state from ETS defaults
            segments[i].savedEffectType = effectType;
            segments[i].savedEffectValid = true;
            // Note: Effect-specific parameters are loaded dynamically from ETS

            logInfoP("Segment %zu: Applied effect type %d", i, effectType);
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
        
        // Clear the segment (turn all LEDs off) before starting the effect
        segment->setPrimaryColor(0, 0, 0, 0);

        segment->setEffect(effect);
        logInfoP("Applied effect '%s' (ID: %d) to segment (cleared first)", effect->getName(), effectType);
    }
    else
    {
        logWarningP("Unknown effect type: %d", effectType);
    }
}

Effect* EffectConfiguration::getEffectFromType(uint8_t effectType)
{
    // Map effect type ID to actual effect instances
    // Based on the console effect list:
    switch (effectType)
    {
        case 0: return EffectPool::getSolid();    // Solid Color
        case 1: return EffectPool::getWipe();     // Color Wipe
        case 2: return EffectPool::getRainbow();  // Rainbow
        case 3: return EffectPool::getPride();    // Pride2015
        case 4: return EffectPool::getConfetti(); // Confetti
        case 5: return EffectPool::getJuggle();   // Juggle
        case 6: return EffectPool::getBPM();      // BPM
        case 7: return EffectPool::getCylon();    // Cylon
#ifndef NEOPIXEL_MINIMAL_EFFECTS
        case 8: return EffectPool::getRGBWTest();             // SK6812/RGBW Test
        case 9: return EffectPool::getGarageDoor();           // GarageDoor
        case 10: return EffectPool::getFire();                // Fire
        case 11: return EffectPool::getTheaterChase();        // Theater Chase
        case 12: return EffectPool::getTheaterChaseRainbow(); // Theater Chase Rainbow
        case 13: return EffectPool::getSinelon();             // Sinelon
        case 14: return EffectPool::getTwinkle();             // Twinkle
        case 15: return EffectPool::getSparkle();             // Sparkle
        case 16: return EffectPool::getBreathing();           // Breathing
        case 17: return EffectPool::getStrobe();              // Strobe
        case 18: return EffectPool::getPulse();               // Pulse
        case 19: return EffectPool::getComet();               // Comet
        case 20: return EffectPool::getMeteor();              // Meteor
#endif
        default: return EffectPool::getSolid();
    }
}

void EffectConfiguration::setupEffectConfiguration(Segment* segment)
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

    // Mirror effect (from segment configuration)
    uint8_t _channelIndex = _module->getChannelIndex();  // Local variable for ETS parameter macros
    bool mirrorEffect = ParamNEO_NEOSegmentMirrorEffect;
    if (mirrorEffect)
    {
        logInfoP("Mirror effect enabled for segment");
    }
}
