/**
 * @file EffectParameterMapping.h
 * @brief Effect Parameter Loading from ETS Configuration (Auto-Generated)
 *
 * This file contains the mapping logic to load effect-specific parameters from ETS
 * configuration (EEPROM) and apply them to Effect instances. Parameters are automatically
 * generated from Effect header files and synchronized with NeoPixel.Effects.*.xml definitions.
 *
 * @warning AUTO-GENERATED FILE - DO NOT EDIT MANUALLY
 * @note Auto-generated - do not edit by hand
 * @note Source: Build-EffectParameters.ps1
 *
 * @copyright Copyright (c) 2026 OpenKNX (Licensed under GNU GPL v3.0)
 */

#ifndef EFFECT_PARAMETER_MAPPING_H
#define EFFECT_PARAMETER_MAPPING_H

#pragma once

#include "knxprod.h"
#include "../lib/OFM-NeoPixel/src/effects/Effect.h"
#include "../lib/OFM-NeoPixel/src/Segment.h"

/**
 * @brief Load effect-specific parameters from EEPROM and apply to effect instance
 * 
 * This function reads effect-specific parameters that were automatically generated
 * from Effect*.h headers via Build-EffectParameters.ps1. Parameters are stored in
 * EEPROM at offset 14+ within each segment memory block.
 * 
 * @param effect Effect instance to configure
 * @param segment Segment instance (for setParameter context)
 * @param effectID Effect type ID (0-21)
 * @param channelIndex Channel/Segment index (0-15) for parameter access
 */
inline void loadEffectParameters(Effect* effect, Segment* segment, uint8_t effectID, uint8_t channelIndex)
{
    if (!effect || !segment) return;
    
    uint8_t paramCount = effect->getParameterCount();
    if (paramCount == 0) return;  // No parameters to load
    
    // Set channel index for ParamNEO_* macro expansion
    uint8_t _channelIndex = channelIndex;
    
    // Map effectID to parameter loading code
    // Macro names match OpenKNXproducer format: ParamNEO_NEO{EffectName}{ParamName}
    // (automatically derived from Parameter Name attribute in XML)
    
    switch (static_cast<PT_NEOEffectType>(effectID))
    {
        case PT_NEOEffectType::Wipe:  // Wipe Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOWipeSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOWipeDirection);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOWipeLoop);
            break;
            
        case PT_NEOEffectType::Rainbow:  // Rainbow Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEORainbowSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEORainbowDelta);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEORainbowSaturation);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEORainbowDensity);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEORainbowMode);
            break;
            
        case PT_NEOEffectType::Pride:  // Pride2015 Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOPride2015Speed);
            break;
            
        case PT_NEOEffectType::Juggle:  // Juggle Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOJuggleSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOJuggleNumDots);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOJuggleFadeSpeed);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOJuggleHueOffset);
            break;
            
        case PT_NEOEffectType::BPM:  // BPM Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOBPMBPM);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOBPMHue);
            break;
            
        case PT_NEOEffectType::Cylon:  // Cylon Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOCylonSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOCylonHue);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOCylonEyeSize);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOCylonFadeAmount);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEOCylonDirection);
            if (paramCount >= 6) effect->setParameter(segment, 5, ParamNEO_NEOCylonMode);
            break;
            
        case PT_NEOEffectType::RGBWTest:  // Test Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOTestPhaseDuration);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOTestMode);
            break;
            
        case PT_NEOEffectType::Fire:  // Fire Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOFireSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOFireCooling);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOFireSparking);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOFireReverseDirection);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEOFireBlueFireMode);
            break;
            
        case PT_NEOEffectType::TheaterChase:  // Theater Chase Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOTheaterChaseSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOTheaterChaseSpacing);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOTheaterChaseDotSize);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOTheaterChaseColorMode);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEOTheaterChaseColorSpeed);
            if (paramCount >= 6) effect->setParameter(segment, 5, ParamNEO_NEOTheaterChaseBounce);
            break;
            
        case PT_NEOEffectType::Sparkle:  // Sparkle Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOSparkleSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOSparkleFadeRate);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOSparkleSparkleCount);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOSparkleProbability);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEOSparkleWhiteOnly);
            if (paramCount >= 6) effect->setParameter(segment, 5, ParamNEO_NEOSparkleBurstMode);
            if (paramCount >= 7) effect->setParameter(segment, 6, ParamNEO_NEOSparkleMode);
            break;
            
        case PT_NEOEffectType::Breathing:  // Breathing Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOBreathingSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOBreathingMinBrightness);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOBreathingCurve);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOBreathingRainbowMode);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEOBreathingWaveform);
            break;
            
        case PT_NEOEffectType::Strobe:  // Strobe Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOStrobeSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOStrobeOnRatio);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOStrobeMinBrightness);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOStrobeRandomTiming);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEOStrobeRainbowStrobe);
            break;
            
        case PT_NEOEffectType::Comet:  // Comet Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOCometSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOCometFadeRate);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOCometTailLength);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOCometBounceMode);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEOCometRainbowMode);
            if (paramCount >= 6) effect->setParameter(segment, 5, ParamNEO_NEOCometMode);
            break;
            
        case PT_NEOEffectType::Noise:  // Noise Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEONoiseSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEONoiseScale);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEONoiseSaturation);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEONoiseHueOffset);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEONoisePalette);
            break;
            
        case PT_NEOEffectType::Palette:  // Palette Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOPaletteSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOPalettePalette);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOPaletteBlend);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOPaletteSpacing);
            break;
            
        case PT_NEOEffectType::Lightning:  // Blitz Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOBlitzSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOBlitzWidth);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOBlitzDecay);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOBlitzHue);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEOBlitzIntensity);
            break;
            
        case PT_NEOEffectType::Gradient:  // Gradient Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOGradientSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOGradientStartHue);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOGradientEndHue);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOGradientSaturation);
            break;
            
        case PT_NEOEffectType::Candle:  // Kerze Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOKerzeSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOKerzeIntensity);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOKerzeZones);
            break;
            
        case PT_NEOEffectType::ScrollText:  // Laufschrift Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOLaufschriftSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOLaufschriftGap);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOLaufschriftLoop);
            if (paramCount >= 4) effect->setParameter(segment, 3, (uint32_t)(uintptr_t)ParamNEO_NEOLaufschriftText);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEOLaufschriftFont);
            break;
            
        case PT_NEOEffectType::Clock2D:  // Uhr 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOUhr2DViewMode);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOUhr2DShowSeconds);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOUhr2DBlinkColon);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOUhr2DColourHue);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEOUhr2DDateMode);
            if (paramCount >= 6) effect->setParameter(segment, 5, ParamNEO_NEOUhr2DDateFormat);
            if (paramCount >= 7) effect->setParameter(segment, 6, ParamNEO_NEOUhr2DDateHue);
            if (paramCount >= 8) effect->setParameter(segment, 7, ParamNEO_NEOUhr2DSwitchSec);
            if (paramCount >= 9) effect->setParameter(segment, 8, ParamNEO_NEOUhr2DScrollSpeed);
            if (paramCount >= 10) effect->setParameter(segment, 9, ParamNEO_NEOUhr2DFont);
            break;
            
        case PT_NEOEffectType::Snake2D:  // Snake 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOSnake2DSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOSnake2DHeadHue);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOSnake2DBodyMode);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOSnake2DBodyHue);
            break;
            
        case PT_NEOEffectType::Matrix2D:  // Matrix 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOMatrix2DSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOMatrix2DDensity);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOMatrix2DPalette);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOMatrix2DGlitch);
            break;
            
        case PT_NEOEffectType::Tetris2D:  // Tetris 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOTetris2DSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOTetris2DBgBrightness);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOTetris2DGhostPiece);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOTetris2DColorMode);
            if (paramCount >= 5) effect->setParameter(segment, 4, ParamNEO_NEOTetris2DFlashLines);
            if (paramCount >= 6) effect->setParameter(segment, 5, ParamNEO_NEOTetris2DAutoPlay);
            break;
            
        case PT_NEOEffectType::Tron2D:  // TRON Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOTRONSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOTRONHue);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOTRONGridSpacing);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOTRONGlow);
            break;
            
        case PT_NEOEffectType::StarfieldWarp2D:  // Starfield Warp Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOStarfieldWarpSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOStarfieldWarpDensity);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOStarfieldWarpColorMode);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOStarfieldWarpWarpPulse);
            break;
            
        case PT_NEOEffectType::PlasmaNebula2D:  // Plasma Nebula Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOPlasmaNebulaSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOPlasmaNebulaSaturation);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOPlasmaNebulaContrast);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOPlasmaNebulaPaletteShift);
            break;
            
        case PT_NEOEffectType::UfoSwarm2D:  // UFO Swarm Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOUFOSwarmSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOUFOSwarmCount);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOUFOSwarmHue);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOUFOSwarmBeam);
            break;
            
        case PT_NEOEffectType::GameOfLife2D:  // Game of Life 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOGameofLife2DSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOGameofLife2DHue);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOGameofLife2DColorMode);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOGameofLife2DSpawnDensity);
            break;
            
        case PT_NEOEffectType::DNA2D:  // DNA 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEODNA2DSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEODNA2DHue);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEODNA2DTwist);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEODNA2DRungSpacing);
            break;
            
        case PT_NEOEffectType::Aurora2D:  // Aurora 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOAurora2DSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOAurora2DHue);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOAurora2DScale);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOAurora2DIntensity);
            break;
            
        case PT_NEOEffectType::Lissajous2D:  // Lissajous 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOLissajous2DSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOLissajous2DHue);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOLissajous2DFreqA);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOLissajous2DFreqB);
            break;
            
        case PT_NEOEffectType::Metaballs2D:  // Metaballs 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, ParamNEO_NEOMetaballs2DSpeed);
            if (paramCount >= 2) effect->setParameter(segment, 1, ParamNEO_NEOMetaballs2DHue);
            if (paramCount >= 3) effect->setParameter(segment, 2, ParamNEO_NEOMetaballs2DBlobCount);
            if (paramCount >= 4) effect->setParameter(segment, 3, ParamNEO_NEOMetaballs2DContrast);
            break;
            
        default:
            // No effect-specific parameters to load
            break;
    }
}

// ETS suffixes share the exact ordering used by loadEffectParameters().
inline const char* effectParameterEtsName(uint8_t effectID, uint8_t index)
{
    switch (static_cast<PT_NEOEffectType>(effectID))
    {
        case PT_NEOEffectType::Wipe:
            switch (index)
            {
                case 0: return "WipeSpeed";
                case 1: return "WipeDirection";
                case 2: return "WipeLoop";
                default: return nullptr;
            }
        case PT_NEOEffectType::Rainbow:
            switch (index)
            {
                case 0: return "RainbowSpeed";
                case 1: return "RainbowDelta";
                case 2: return "RainbowSaturation";
                case 3: return "RainbowDensity";
                case 4: return "RainbowMode";
                default: return nullptr;
            }
        case PT_NEOEffectType::Pride:
            switch (index)
            {
                case 0: return "Pride2015Speed";
                default: return nullptr;
            }
        case PT_NEOEffectType::Juggle:
            switch (index)
            {
                case 0: return "JuggleSpeed";
                case 1: return "JuggleNumDots";
                case 2: return "JuggleFadeSpeed";
                case 3: return "JuggleHueOffset";
                default: return nullptr;
            }
        case PT_NEOEffectType::BPM:
            switch (index)
            {
                case 0: return "BPMBPM";
                case 1: return "BPMHue";
                default: return nullptr;
            }
        case PT_NEOEffectType::Cylon:
            switch (index)
            {
                case 0: return "CylonSpeed";
                case 1: return "CylonHue";
                case 2: return "CylonEyeSize";
                case 3: return "CylonFadeAmount";
                case 4: return "CylonDirection";
                case 5: return "CylonMode";
                default: return nullptr;
            }
        case PT_NEOEffectType::RGBWTest:
            switch (index)
            {
                case 0: return "TestPhaseDuration";
                case 1: return "TestMode";
                default: return nullptr;
            }
        case PT_NEOEffectType::Fire:
            switch (index)
            {
                case 0: return "FireSpeed";
                case 1: return "FireCooling";
                case 2: return "FireSparking";
                case 3: return "FireReverseDirection";
                case 4: return "FireBlueFireMode";
                default: return nullptr;
            }
        case PT_NEOEffectType::TheaterChase:
            switch (index)
            {
                case 0: return "TheaterChaseSpeed";
                case 1: return "TheaterChaseSpacing";
                case 2: return "TheaterChaseDotSize";
                case 3: return "TheaterChaseColorMode";
                case 4: return "TheaterChaseColorSpeed";
                case 5: return "TheaterChaseBounce";
                default: return nullptr;
            }
        case PT_NEOEffectType::Sparkle:
            switch (index)
            {
                case 0: return "SparkleSpeed";
                case 1: return "SparkleFadeRate";
                case 2: return "SparkleSparkleCount";
                case 3: return "SparkleProbability";
                case 4: return "SparkleWhiteOnly";
                case 5: return "SparkleBurstMode";
                case 6: return "SparkleMode";
                default: return nullptr;
            }
        case PT_NEOEffectType::Breathing:
            switch (index)
            {
                case 0: return "BreathingSpeed";
                case 1: return "BreathingMinBrightness";
                case 2: return "BreathingCurve";
                case 3: return "BreathingRainbowMode";
                case 4: return "BreathingWaveform";
                default: return nullptr;
            }
        case PT_NEOEffectType::Strobe:
            switch (index)
            {
                case 0: return "StrobeSpeed";
                case 1: return "StrobeOnRatio";
                case 2: return "StrobeMinBrightness";
                case 3: return "StrobeRandomTiming";
                case 4: return "StrobeRainbowStrobe";
                default: return nullptr;
            }
        case PT_NEOEffectType::Comet:
            switch (index)
            {
                case 0: return "CometSpeed";
                case 1: return "CometFadeRate";
                case 2: return "CometTailLength";
                case 3: return "CometBounceMode";
                case 4: return "CometRainbowMode";
                case 5: return "CometMode";
                default: return nullptr;
            }
        case PT_NEOEffectType::Noise:
            switch (index)
            {
                case 0: return "NoiseSpeed";
                case 1: return "NoiseScale";
                case 2: return "NoiseSaturation";
                case 3: return "NoiseHueOffset";
                case 4: return "NoisePalette";
                default: return nullptr;
            }
        case PT_NEOEffectType::Palette:
            switch (index)
            {
                case 0: return "PaletteSpeed";
                case 1: return "PalettePalette";
                case 2: return "PaletteBlend";
                case 3: return "PaletteSpacing";
                default: return nullptr;
            }
        case PT_NEOEffectType::Lightning:
            switch (index)
            {
                case 0: return "BlitzSpeed";
                case 1: return "BlitzWidth";
                case 2: return "BlitzDecay";
                case 3: return "BlitzHue";
                case 4: return "BlitzIntensity";
                default: return nullptr;
            }
        case PT_NEOEffectType::Gradient:
            switch (index)
            {
                case 0: return "GradientSpeed";
                case 1: return "GradientStartHue";
                case 2: return "GradientEndHue";
                case 3: return "GradientSaturation";
                default: return nullptr;
            }
        case PT_NEOEffectType::Candle:
            switch (index)
            {
                case 0: return "KerzeSpeed";
                case 1: return "KerzeIntensity";
                case 2: return "KerzeZones";
                default: return nullptr;
            }
        case PT_NEOEffectType::ScrollText:
            switch (index)
            {
                case 0: return "LaufschriftSpeed";
                case 1: return "LaufschriftGap";
                case 2: return "LaufschriftLoop";
                case 3: return "LaufschriftText";
                case 4: return "LaufschriftFont";
                default: return nullptr;
            }
        case PT_NEOEffectType::Clock2D:
            switch (index)
            {
                case 0: return "Uhr2DViewMode";
                case 1: return "Uhr2DShowSeconds";
                case 2: return "Uhr2DBlinkColon";
                case 3: return "Uhr2DColourHue";
                case 4: return "Uhr2DDateMode";
                case 5: return "Uhr2DDateFormat";
                case 6: return "Uhr2DDateHue";
                case 7: return "Uhr2DSwitchSec";
                case 8: return "Uhr2DScrollSpeed";
                case 9: return "Uhr2DFont";
                default: return nullptr;
            }
        case PT_NEOEffectType::Snake2D:
            switch (index)
            {
                case 0: return "Snake2DSpeed";
                case 1: return "Snake2DHeadHue";
                case 2: return "Snake2DBodyMode";
                case 3: return "Snake2DBodyHue";
                default: return nullptr;
            }
        case PT_NEOEffectType::Matrix2D:
            switch (index)
            {
                case 0: return "Matrix2DSpeed";
                case 1: return "Matrix2DDensity";
                case 2: return "Matrix2DPalette";
                case 3: return "Matrix2DGlitch";
                default: return nullptr;
            }
        case PT_NEOEffectType::Tetris2D:
            switch (index)
            {
                case 0: return "Tetris2DSpeed";
                case 1: return "Tetris2DBgBrightness";
                case 2: return "Tetris2DGhostPiece";
                case 3: return "Tetris2DColorMode";
                case 4: return "Tetris2DFlashLines";
                case 5: return "Tetris2DAutoPlay";
                default: return nullptr;
            }
        case PT_NEOEffectType::Tron2D:
            switch (index)
            {
                case 0: return "TRONSpeed";
                case 1: return "TRONHue";
                case 2: return "TRONGridSpacing";
                case 3: return "TRONGlow";
                default: return nullptr;
            }
        case PT_NEOEffectType::StarfieldWarp2D:
            switch (index)
            {
                case 0: return "StarfieldWarpSpeed";
                case 1: return "StarfieldWarpDensity";
                case 2: return "StarfieldWarpColorMode";
                case 3: return "StarfieldWarpWarpPulse";
                default: return nullptr;
            }
        case PT_NEOEffectType::PlasmaNebula2D:
            switch (index)
            {
                case 0: return "PlasmaNebulaSpeed";
                case 1: return "PlasmaNebulaSaturation";
                case 2: return "PlasmaNebulaContrast";
                case 3: return "PlasmaNebulaPaletteShift";
                default: return nullptr;
            }
        case PT_NEOEffectType::UfoSwarm2D:
            switch (index)
            {
                case 0: return "UFOSwarmSpeed";
                case 1: return "UFOSwarmCount";
                case 2: return "UFOSwarmHue";
                case 3: return "UFOSwarmBeam";
                default: return nullptr;
            }
        case PT_NEOEffectType::GameOfLife2D:
            switch (index)
            {
                case 0: return "GameofLife2DSpeed";
                case 1: return "GameofLife2DHue";
                case 2: return "GameofLife2DColorMode";
                case 3: return "GameofLife2DSpawnDensity";
                default: return nullptr;
            }
        case PT_NEOEffectType::DNA2D:
            switch (index)
            {
                case 0: return "DNA2DSpeed";
                case 1: return "DNA2DHue";
                case 2: return "DNA2DTwist";
                case 3: return "DNA2DRungSpacing";
                default: return nullptr;
            }
        case PT_NEOEffectType::Aurora2D:
            switch (index)
            {
                case 0: return "Aurora2DSpeed";
                case 1: return "Aurora2DHue";
                case 2: return "Aurora2DScale";
                case 3: return "Aurora2DIntensity";
                default: return nullptr;
            }
        case PT_NEOEffectType::Lissajous2D:
            switch (index)
            {
                case 0: return "Lissajous2DSpeed";
                case 1: return "Lissajous2DHue";
                case 2: return "Lissajous2DFreqA";
                case 3: return "Lissajous2DFreqB";
                default: return nullptr;
            }
        case PT_NEOEffectType::Metaballs2D:
            switch (index)
            {
                case 0: return "Metaballs2DSpeed";
                case 1: return "Metaballs2DHue";
                case 2: return "Metaballs2DBlobCount";
                case 3: return "Metaballs2DContrast";
                default: return nullptr;
            }
        default: return nullptr;
    }
}


/**
 * @brief Load effect-specific parameters from a scene slot in EEPROM
 *
 * Reads effect-specific parameters from a scene slot. Parameters are stored
 * at SCENE_DATA_START + sceneIndex * SCENE_SIZE + 12 within each segment's Union.
 *
 * @param effect Effect instance to configure
 * @param segment Segment instance (for setParameter context)
 * @param effectID Effect type ID (0-28)
 * @param channelIndex Channel/Segment index (0-15)
 * @param sceneIndex Scene index (0-14)
 * @note Auto-generated - do not edit by hand
 */
inline void loadSceneEffectParameters(Effect* effect, Segment* segment, uint8_t effectID, uint8_t channelIndex, uint8_t sceneIndex)
{
    if (!effect || !segment) return;
    
    uint8_t paramCount = effect->getParameterCount();
    if (paramCount == 0) return;
    
    // Compute base address for this scene's effect parameters
    // Scene data layout: 36 bytes per scene, effect params at offset +12
    static constexpr uint16_t SCENE_DATA_START = 160;
    static constexpr uint8_t SCENE_SIZE = 36;
    static constexpr uint8_t SCENE_EFFECT_PARAM_OFFSET = 12;
    static constexpr uint8_t SCENE_EFFECT_TEXT_OFFSET = 22;
    static constexpr uint8_t SCENE_EFFECT_TEXT_SIZE = 14;
    
    uint16_t sceneEffectParamBase = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize
                                  + SCENE_DATA_START + sceneIndex * SCENE_SIZE + SCENE_EFFECT_PARAM_OFFSET;
    
    // STRING parameters are stored in the dedicated effectText field (offset +22, 14 bytes)
    uint16_t sceneEffectTextBase = NEO_ParamBlockOffset + channelIndex * NEO_ParamBlockSize
                                 + SCENE_DATA_START + sceneIndex * SCENE_SIZE + SCENE_EFFECT_TEXT_OFFSET;
    char sceneEffectText[SCENE_EFFECT_TEXT_SIZE + 1];
    {
        const uint8_t* textData = knx.paramData(sceneEffectTextBase);
        if (textData) { memcpy(sceneEffectText, textData, SCENE_EFFECT_TEXT_SIZE); }
        else { sceneEffectText[0] = 0; }
        sceneEffectText[SCENE_EFFECT_TEXT_SIZE] = 0;
    }
    (void)sceneEffectText;
    
    switch (static_cast<PT_NEOEffectType>(effectID))
    {
        case PT_NEOEffectType::Wipe:  // Wipe Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            break;
            
        case PT_NEOEffectType::Rainbow:  // Rainbow Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            break;
            
        case PT_NEOEffectType::Pride:  // Pride2015 Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            break;
            
        case PT_NEOEffectType::Juggle:  // Juggle Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::BPM:  // BPM Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            break;
            
        case PT_NEOEffectType::Cylon:  // Cylon Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            if (paramCount >= 6) effect->setParameter(segment, 5, knx.paramByte(sceneEffectParamBase + 5));
            break;
            
        case PT_NEOEffectType::RGBWTest:  // Test Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            break;
            
        case PT_NEOEffectType::Fire:  // Fire Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            break;
            
        case PT_NEOEffectType::TheaterChase:  // Theater Chase Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            if (paramCount >= 6) effect->setParameter(segment, 5, knx.paramByte(sceneEffectParamBase + 5));
            break;
            
        case PT_NEOEffectType::Sparkle:  // Sparkle Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            if (paramCount >= 6) effect->setParameter(segment, 5, knx.paramByte(sceneEffectParamBase + 5));
            if (paramCount >= 7) effect->setParameter(segment, 6, knx.paramByte(sceneEffectParamBase + 6));
            break;
            
        case PT_NEOEffectType::Breathing:  // Breathing Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            break;
            
        case PT_NEOEffectType::Strobe:  // Strobe Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            break;
            
        case PT_NEOEffectType::Comet:  // Comet Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            if (paramCount >= 6) effect->setParameter(segment, 5, knx.paramByte(sceneEffectParamBase + 5));
            break;
            
        case PT_NEOEffectType::Noise:  // Noise Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            break;
            
        case PT_NEOEffectType::Palette:  // Palette Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::Lightning:  // Blitz Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            break;
            
        case PT_NEOEffectType::Gradient:  // Gradient Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::Candle:  // Kerze Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            break;
            
        case PT_NEOEffectType::ScrollText:  // Laufschrift Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, (uint32_t)(uintptr_t)sceneEffectText);
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            break;
            
        case PT_NEOEffectType::Clock2D:  // Uhr 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            if (paramCount >= 6) effect->setParameter(segment, 5, knx.paramByte(sceneEffectParamBase + 5));
            if (paramCount >= 7) effect->setParameter(segment, 6, knx.paramByte(sceneEffectParamBase + 6));
            if (paramCount >= 8) effect->setParameter(segment, 7, knx.paramByte(sceneEffectParamBase + 7));
            if (paramCount >= 9) effect->setParameter(segment, 8, knx.paramByte(sceneEffectParamBase + 8));
            if (paramCount >= 10) effect->setParameter(segment, 9, knx.paramByte(sceneEffectParamBase + 9));
            break;
            
        case PT_NEOEffectType::Snake2D:  // Snake 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::Matrix2D:  // Matrix 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::Tetris2D:  // Tetris 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            if (paramCount >= 5) effect->setParameter(segment, 4, knx.paramByte(sceneEffectParamBase + 4));
            if (paramCount >= 6) effect->setParameter(segment, 5, knx.paramByte(sceneEffectParamBase + 5));
            break;
            
        case PT_NEOEffectType::Tron2D:  // TRON Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::StarfieldWarp2D:  // Starfield Warp Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::PlasmaNebula2D:  // Plasma Nebula Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::UfoSwarm2D:  // UFO Swarm Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::GameOfLife2D:  // Game of Life 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::DNA2D:  // DNA 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::Aurora2D:  // Aurora 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::Lissajous2D:  // Lissajous 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        case PT_NEOEffectType::Metaballs2D:  // Metaballs 2D Effect
            if (paramCount >= 1) effect->setParameter(segment, 0, knx.paramByte(sceneEffectParamBase + 0));
            if (paramCount >= 2) effect->setParameter(segment, 1, knx.paramByte(sceneEffectParamBase + 1));
            if (paramCount >= 3) effect->setParameter(segment, 2, knx.paramByte(sceneEffectParamBase + 2));
            if (paramCount >= 4) effect->setParameter(segment, 3, knx.paramByte(sceneEffectParamBase + 3));
            break;
            
        default:
            break;
    }
}

#endif // EFFECT_PARAMETER_MAPPING_H

