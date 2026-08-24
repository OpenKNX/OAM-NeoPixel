/**
 * @file EffectTypeMapping.h
 * @brief Effect Type ID to EffectPool Instance Mapping (Auto-Generated)
 *
 * This file contains the mapping between ETS Effect Type IDs and EffectPool singleton instances.
 * The mapping is automatically generated from Effect header files at build time and synchronized
 * with the registration order in EffectPool.cpp and the enumeration in NeoPixel.share.xml.
 *
 * @warning AUTO-GENERATED FILE - DO NOT EDIT MANUALLY
 * @note Generated: 2026-08-21 12:59:19
 * @note Source: Build-EffectParameters.ps1
 *
 * @copyright Copyright (c) 2026 OpenKNX (Licensed under GNU GPL v3.0)
 */

#pragma once

#include "../lib/OFM-NeoPixel/src/effects/EffectPool.h"

/**
 * @brief Maps ETS effect type ID to actual Effect instance
 *
 * This function is synchronized with the Effect Type enumeration in
 * NeoPixel.share.xml and the registration order in EffectPool.cpp
 *
 * @param effectType Effect type ID from ETS (0-32)
 * @return Pointer to Effect instance, or Solid effect for unknown types
 */
inline Effect* getEffectFromType(uint8_t effectType)
{
    switch (static_cast<PT_NEOEffectType>(effectType))
    {
        case PT_NEOEffectType::Solid: return EffectPool::getSolid(); // Solid
        case PT_NEOEffectType::Wipe: return EffectPool::getWipe(); // Wipe
        case PT_NEOEffectType::Rainbow: return EffectPool::getRainbow(); // Rainbow
        case PT_NEOEffectType::Pride: return EffectPool::getPride(); // Pride2015
        case PT_NEOEffectType::Juggle: return EffectPool::getJuggle(); // Juggle
        case PT_NEOEffectType::BPM: return EffectPool::getBPM(); // BPM
        case PT_NEOEffectType::Cylon: return EffectPool::getCylon(); // Cylon
        case PT_NEOEffectType::RGBWTest: return EffectPool::getRGBWTest(); // Test
        case PT_NEOEffectType::Fire: return EffectPool::getFire(); // Fire
        case PT_NEOEffectType::TheaterChase: return EffectPool::getTheaterChase(); // Theater Chase
        case PT_NEOEffectType::Sparkle: return EffectPool::getSparkle(); // Sparkle
        case PT_NEOEffectType::Breathing: return EffectPool::getBreathing(); // Breathing
        case PT_NEOEffectType::Strobe: return EffectPool::getStrobe(); // Strobe
        case PT_NEOEffectType::Comet: return EffectPool::getComet(); // Comet
        case PT_NEOEffectType::Noise: return EffectPool::getNoise(); // Noise
        case PT_NEOEffectType::Palette: return EffectPool::getPalette(); // Palette
        case PT_NEOEffectType::Lightning: return EffectPool::getLightning(); // Lightning (DE: Blitz)
        case PT_NEOEffectType::Gradient: return EffectPool::getGradient(); // Gradient
        case PT_NEOEffectType::Candle: return EffectPool::getCandle(); // Candle (DE: Kerze)
        case PT_NEOEffectType::ScrollText: return EffectPool::getScrollText(); // Scroll Text (DE: Laufschrift)
        case PT_NEOEffectType::Clock2D: return EffectPool::getClock2D(); // Clock 2D (DE: Uhr 2D)
        case PT_NEOEffectType::Snake2D: return EffectPool::getSnake2D(); // Snake 2D
        case PT_NEOEffectType::Matrix2D: return EffectPool::getMatrix2D(); // Matrix 2D
        case PT_NEOEffectType::Tetris2D: return EffectPool::getTetris2D(); // Tetris 2D
        case PT_NEOEffectType::Tron2D: return EffectPool::getTron2D(); // TRON
        case PT_NEOEffectType::StarfieldWarp2D: return EffectPool::getStarfieldWarp2D(); // Starfield Warp
        case PT_NEOEffectType::PlasmaNebula2D: return EffectPool::getPlasmaNebula2D(); // Plasma Nebula
        case PT_NEOEffectType::UfoSwarm2D: return EffectPool::getUfoSwarm2D(); // UFO Swarm
        case PT_NEOEffectType::GameOfLife2D: return EffectPool::getGameOfLife2D(); // Game of Life 2D
        case PT_NEOEffectType::DNA2D: return EffectPool::getDNA2D(); // DNA 2D
        case PT_NEOEffectType::Aurora2D: return EffectPool::getAurora2D(); // Aurora 2D
        case PT_NEOEffectType::Lissajous2D: return EffectPool::getLissajous2D(); // Lissajous 2D
        case PT_NEOEffectType::Metaballs2D: return EffectPool::getMetaballs2D(); // Metaballs 2D
        default: return EffectPool::getSolid(); // Fallback to Solid
    }
}

/**
 * @brief Maps Effect instance pointer back to ETS effect type ID (reverse mapping)
 *
 * This function is synchronized with getEffectFromType() and provides the reverse mapping.
 * Used for power-off snapshots to save the currently active effect type ID.
 *
 * @param effect Pointer to Effect instance
 * @return Effect type ID (0-32), or 0 (Solid) if effect is nullptr or unknown
 */
inline uint8_t getTypeFromEffect(Effect* effect)
{
    return EffectPool::getEffectIndex(effect);
}
