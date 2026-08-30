/**
 * @file NeoPixelTimingSelection.h
 * @brief Authoritative mapping for the persisted ETS 1-wire timing selection.
 */
#pragma once

#include <stdint.h>

namespace NeoPixelTimingSelection
{
constexpr uint8_t kSelectionCount = 16;

// Indexed by ETS value, rather than display order, to retain compatibility.
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

constexpr uint16_t bitrateKhz(uint8_t selection)
{
    return kBitrateKhz[sanitize(selection)];
}

static_assert(kBitrateKhz[0] == 800, "ETS value 0 must remain the protocol default");
static_assert(kBitrateKhz[15] == 790, "historic ETS timing values must remain stable");
} // namespace NeoPixelTimingSelection
