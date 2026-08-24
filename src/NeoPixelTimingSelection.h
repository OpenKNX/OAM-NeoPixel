/**
 * @file NeoPixelTimingSelection.h
 * @brief One authoritative mapping for the ETS 4-bit one-wire timing selection.
 *
 * Values are deliberately indexed by their persisted ETS value, not display
 * order.  Keep every historic value so existing projects remain compatible.
 * Value zero is the protocol default; every other value is an expert override.
 */
#pragma once

#include <stdint.h>

namespace NeoPixelTimingSelection
{
constexpr uint8_t kSelectionCount = 16;

// ETS value -> requested bit rate in kHz. The XML enum and generated
// PT_NEOTiming names are compile-time checked at the call sites.
constexpr uint16_t kBitrateKhz[kSelectionCount] = {
    800, 960, 640, 680, 720, 760, 840, 880,
    920, 750, 765, 770, 775, 780, 785, 790,
};

constexpr uint8_t sanitize(uint8_t selection)
{
    return selection < kSelectionCount ? selection : 0;
}

constexpr bool isProtocolDefault(uint8_t selection)
{
    return sanitize(selection) == 0;
}

constexpr bool isLegacyFineOverride(uint8_t selection)
{
    const uint8_t value = sanitize(selection);
    return value >= 9 && value <= 15;
}

constexpr uint16_t bitrateKhz(uint8_t selection)
{
    return kBitrateKhz[sanitize(selection)];
}


static_assert(kBitrateKhz[0] == 800, "ETS value 0 must remain the protocol default");
static_assert(kBitrateKhz[15] == 790, "historic ETS timing values must remain stable");
} // namespace NeoPixelTimingSelection
