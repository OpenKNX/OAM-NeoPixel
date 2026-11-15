/**
 * @file EffectTypeMapping.h
 * @brief Effect Type ID to EffectPool Instance Mapping (Auto-Generated)
 *
 * This file contains the mapping between ETS Effect Type IDs and EffectPool singleton instances.
 * The mapping is automatically generated from Effect header files at build time and synchronized
 * with the registration order in EffectPool.cpp and the enumeration in NeoPixel.share.xml.
 *
 * @warning AUTO-GENERATED FILE - DO NOT EDIT MANUALLY
 * @note Auto-generated - do not edit by hand
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
#ifndef NEOPIXEL_DISABLE_WIPE
        case PT_NEOEffectType::Wipe: return EffectPool::getWipe(); // Wipe
#endif
#ifndef NEOPIXEL_DISABLE_RAINBOW
        case PT_NEOEffectType::Rainbow: return EffectPool::getRainbow(); // Rainbow
#endif
#ifndef NEOPIXEL_DISABLE_PRIDE
        case PT_NEOEffectType::Pride: return EffectPool::getPride(); // Pride2015
#endif
#ifndef NEOPIXEL_DISABLE_JUGGLE
        case PT_NEOEffectType::Juggle: return EffectPool::getJuggle(); // Juggle
#endif
#ifndef NEOPIXEL_DISABLE_BPM
        case PT_NEOEffectType::BPM: return EffectPool::getBPM(); // BPM
#endif
#ifndef NEOPIXEL_DISABLE_CYLON
        case PT_NEOEffectType::Cylon: return EffectPool::getCylon(); // Cylon
#endif
#ifndef NEOPIXEL_DISABLE_RGBWTEST
        case PT_NEOEffectType::RGBWTest: return EffectPool::getRGBWTest(); // Test
#endif
#if !defined(NEOPIXEL_DISABLE_FIRE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
        case PT_NEOEffectType::Fire: return EffectPool::getFire(); // Fire
#endif
#if !defined(NEOPIXEL_DISABLE_THEATERCHASE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
        case PT_NEOEffectType::TheaterChase: return EffectPool::getTheaterChase(); // Theater Chase
#endif
#if !defined(NEOPIXEL_DISABLE_SPARKLE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
        case PT_NEOEffectType::Sparkle: return EffectPool::getSparkle(); // Sparkle
#endif
#if !defined(NEOPIXEL_DISABLE_BREATHING) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
        case PT_NEOEffectType::Breathing: return EffectPool::getBreathing(); // Breathing
#endif
#if !defined(NEOPIXEL_DISABLE_STROBE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
        case PT_NEOEffectType::Strobe: return EffectPool::getStrobe(); // Strobe
#endif
#if !defined(NEOPIXEL_DISABLE_COMET) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
        case PT_NEOEffectType::Comet: return EffectPool::getComet(); // Comet
#endif
#if !defined(NEOPIXEL_DISABLE_NOISE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
        case PT_NEOEffectType::Noise: return EffectPool::getNoise(); // Noise
#endif
#if !defined(NEOPIXEL_DISABLE_PALETTE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
        case PT_NEOEffectType::Palette: return EffectPool::getPalette(); // Palette
#endif
#if !defined(NEOPIXEL_DISABLE_LIGHTNING) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
        case PT_NEOEffectType::Lightning: return EffectPool::getLightning(); // Lightning (DE: Blitz)
#endif
#if !defined(NEOPIXEL_DISABLE_GRADIENT) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
        case PT_NEOEffectType::Gradient: return EffectPool::getGradient(); // Gradient
#endif
#if !defined(NEOPIXEL_DISABLE_CANDLE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
        case PT_NEOEffectType::Candle: return EffectPool::getCandle(); // Candle (DE: Kerze)
#endif
#ifndef NEOPIXEL_DISABLE_SCROLLTEXT
        case PT_NEOEffectType::ScrollText: return EffectPool::getScrollText(); // Scroll Text (DE: Laufschrift)
#endif
#ifndef NEOPIXEL_DISABLE_CLOCK2D
        case PT_NEOEffectType::Clock2D: return EffectPool::getClock2D(); // Clock 2D (DE: Uhr 2D)
#endif
#ifndef NEOPIXEL_DISABLE_SNAKE2D
        case PT_NEOEffectType::Snake2D: return EffectPool::getSnake2D(); // Snake 2D
#endif
#ifndef NEOPIXEL_DISABLE_MATRIX2D
        case PT_NEOEffectType::Matrix2D: return EffectPool::getMatrix2D(); // Matrix 2D
#endif
#ifndef NEOPIXEL_DISABLE_TETRIS2D
        case PT_NEOEffectType::Tetris2D: return EffectPool::getTetris2D(); // Tetris 2D
#endif
#ifndef NEOPIXEL_DISABLE_TRON2D
        case PT_NEOEffectType::Tron2D: return EffectPool::getTron2D(); // TRON
#endif
#ifndef NEOPIXEL_DISABLE_STARFIELDWARP2D
        case PT_NEOEffectType::StarfieldWarp2D: return EffectPool::getStarfieldWarp2D(); // Starfield Warp
#endif
#ifndef NEOPIXEL_DISABLE_PLASMANEBULA2D
        case PT_NEOEffectType::PlasmaNebula2D: return EffectPool::getPlasmaNebula2D(); // Plasma Nebula
#endif
#ifndef NEOPIXEL_DISABLE_UFOSWARM2D
        case PT_NEOEffectType::UfoSwarm2D: return EffectPool::getUfoSwarm2D(); // UFO Swarm
#endif
#ifndef NEOPIXEL_DISABLE_GAMEOFLIFE2D
        case PT_NEOEffectType::GameOfLife2D: return EffectPool::getGameOfLife2D(); // Game of Life 2D
#endif
#ifndef NEOPIXEL_DISABLE_DNA2D
        case PT_NEOEffectType::DNA2D: return EffectPool::getDNA2D(); // DNA 2D
#endif
#ifndef NEOPIXEL_DISABLE_AURORA2D
        case PT_NEOEffectType::Aurora2D: return EffectPool::getAurora2D(); // Aurora 2D
#endif
#ifndef NEOPIXEL_DISABLE_LISSAJOUS2D
        case PT_NEOEffectType::Lissajous2D: return EffectPool::getLissajous2D(); // Lissajous 2D
#endif
#ifndef NEOPIXEL_DISABLE_METABALLS2D
        case PT_NEOEffectType::Metaballs2D: return EffectPool::getMetaballs2D(); // Metaballs 2D
#endif
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
    if (!effect) return 0; // nullptr -> Solid

    if (effect == EffectPool::getSolid()) return static_cast<uint8_t>(PT_NEOEffectType::Solid); // Solid
#ifndef NEOPIXEL_DISABLE_WIPE
    if (effect == EffectPool::getWipe()) return static_cast<uint8_t>(PT_NEOEffectType::Wipe); // Wipe
#endif
#ifndef NEOPIXEL_DISABLE_RAINBOW
    if (effect == EffectPool::getRainbow()) return static_cast<uint8_t>(PT_NEOEffectType::Rainbow); // Rainbow
#endif
#ifndef NEOPIXEL_DISABLE_PRIDE
    if (effect == EffectPool::getPride()) return static_cast<uint8_t>(PT_NEOEffectType::Pride); // Pride2015
#endif
#ifndef NEOPIXEL_DISABLE_JUGGLE
    if (effect == EffectPool::getJuggle()) return static_cast<uint8_t>(PT_NEOEffectType::Juggle); // Juggle
#endif
#ifndef NEOPIXEL_DISABLE_BPM
    if (effect == EffectPool::getBPM()) return static_cast<uint8_t>(PT_NEOEffectType::BPM); // BPM
#endif
#ifndef NEOPIXEL_DISABLE_CYLON
    if (effect == EffectPool::getCylon()) return static_cast<uint8_t>(PT_NEOEffectType::Cylon); // Cylon
#endif
#ifndef NEOPIXEL_DISABLE_RGBWTEST
    if (effect == EffectPool::getRGBWTest()) return static_cast<uint8_t>(PT_NEOEffectType::RGBWTest); // Test
#endif
#if !defined(NEOPIXEL_DISABLE_FIRE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
    if (effect == EffectPool::getFire()) return static_cast<uint8_t>(PT_NEOEffectType::Fire); // Fire
#endif
#if !defined(NEOPIXEL_DISABLE_THEATERCHASE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
    if (effect == EffectPool::getTheaterChase()) return static_cast<uint8_t>(PT_NEOEffectType::TheaterChase); // Theater Chase
#endif
#if !defined(NEOPIXEL_DISABLE_SPARKLE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
    if (effect == EffectPool::getSparkle()) return static_cast<uint8_t>(PT_NEOEffectType::Sparkle); // Sparkle
#endif
#if !defined(NEOPIXEL_DISABLE_BREATHING) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
    if (effect == EffectPool::getBreathing()) return static_cast<uint8_t>(PT_NEOEffectType::Breathing); // Breathing
#endif
#if !defined(NEOPIXEL_DISABLE_STROBE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
    if (effect == EffectPool::getStrobe()) return static_cast<uint8_t>(PT_NEOEffectType::Strobe); // Strobe
#endif
#if !defined(NEOPIXEL_DISABLE_COMET) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
    if (effect == EffectPool::getComet()) return static_cast<uint8_t>(PT_NEOEffectType::Comet); // Comet
#endif
#if !defined(NEOPIXEL_DISABLE_NOISE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
    if (effect == EffectPool::getNoise()) return static_cast<uint8_t>(PT_NEOEffectType::Noise); // Noise
#endif
#if !defined(NEOPIXEL_DISABLE_PALETTE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
    if (effect == EffectPool::getPalette()) return static_cast<uint8_t>(PT_NEOEffectType::Palette); // Palette
#endif
#if !defined(NEOPIXEL_DISABLE_LIGHTNING) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
    if (effect == EffectPool::getLightning()) return static_cast<uint8_t>(PT_NEOEffectType::Lightning); // Lightning (DE: Blitz)
#endif
#if !defined(NEOPIXEL_DISABLE_GRADIENT) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
    if (effect == EffectPool::getGradient()) return static_cast<uint8_t>(PT_NEOEffectType::Gradient); // Gradient
#endif
#if !defined(NEOPIXEL_DISABLE_CANDLE) && !defined(NEOPIXEL_MINIMAL_EFFECTS)
    if (effect == EffectPool::getCandle()) return static_cast<uint8_t>(PT_NEOEffectType::Candle); // Candle (DE: Kerze)
#endif
#ifndef NEOPIXEL_DISABLE_SCROLLTEXT
    if (effect == EffectPool::getScrollText()) return static_cast<uint8_t>(PT_NEOEffectType::ScrollText); // Scroll Text (DE: Laufschrift)
#endif
#ifndef NEOPIXEL_DISABLE_CLOCK2D
    if (effect == EffectPool::getClock2D()) return static_cast<uint8_t>(PT_NEOEffectType::Clock2D); // Clock 2D (DE: Uhr 2D)
#endif
#ifndef NEOPIXEL_DISABLE_SNAKE2D
    if (effect == EffectPool::getSnake2D()) return static_cast<uint8_t>(PT_NEOEffectType::Snake2D); // Snake 2D
#endif
#ifndef NEOPIXEL_DISABLE_MATRIX2D
    if (effect == EffectPool::getMatrix2D()) return static_cast<uint8_t>(PT_NEOEffectType::Matrix2D); // Matrix 2D
#endif
#ifndef NEOPIXEL_DISABLE_TETRIS2D
    if (effect == EffectPool::getTetris2D()) return static_cast<uint8_t>(PT_NEOEffectType::Tetris2D); // Tetris 2D
#endif
#ifndef NEOPIXEL_DISABLE_TRON2D
    if (effect == EffectPool::getTron2D()) return static_cast<uint8_t>(PT_NEOEffectType::Tron2D); // TRON
#endif
#ifndef NEOPIXEL_DISABLE_STARFIELDWARP2D
    if (effect == EffectPool::getStarfieldWarp2D()) return static_cast<uint8_t>(PT_NEOEffectType::StarfieldWarp2D); // Starfield Warp
#endif
#ifndef NEOPIXEL_DISABLE_PLASMANEBULA2D
    if (effect == EffectPool::getPlasmaNebula2D()) return static_cast<uint8_t>(PT_NEOEffectType::PlasmaNebula2D); // Plasma Nebula
#endif
#ifndef NEOPIXEL_DISABLE_UFOSWARM2D
    if (effect == EffectPool::getUfoSwarm2D()) return static_cast<uint8_t>(PT_NEOEffectType::UfoSwarm2D); // UFO Swarm
#endif
#ifndef NEOPIXEL_DISABLE_GAMEOFLIFE2D
    if (effect == EffectPool::getGameOfLife2D()) return static_cast<uint8_t>(PT_NEOEffectType::GameOfLife2D); // Game of Life 2D
#endif
#ifndef NEOPIXEL_DISABLE_DNA2D
    if (effect == EffectPool::getDNA2D()) return static_cast<uint8_t>(PT_NEOEffectType::DNA2D); // DNA 2D
#endif
#ifndef NEOPIXEL_DISABLE_AURORA2D
    if (effect == EffectPool::getAurora2D()) return static_cast<uint8_t>(PT_NEOEffectType::Aurora2D); // Aurora 2D
#endif
#ifndef NEOPIXEL_DISABLE_LISSAJOUS2D
    if (effect == EffectPool::getLissajous2D()) return static_cast<uint8_t>(PT_NEOEffectType::Lissajous2D); // Lissajous 2D
#endif
#ifndef NEOPIXEL_DISABLE_METABALLS2D
    if (effect == EffectPool::getMetaballs2D()) return static_cast<uint8_t>(PT_NEOEffectType::Metaballs2D); // Metaballs 2D
#endif

    return 0; // Unknown effect -> fallback to Solid
}
