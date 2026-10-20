#pragma once

#include <stdint.h>

/// @brief What an ON command does with the configured power-on EM.
enum class SegmentPowerOnAction : uint8_t
{
    SwitchOn,       ///< segmentOn(): resume a paused EM or the effect, no restart
    KeepPause,      ///< segment is on already: a manually paused EM stays paused
    DeferPowerOnEm, ///< record ON, start the power-on EM at global ON
    StartPowerOnEm, ///< fresh start of the power-on EM (refused start falls back to SwitchOn)
};

/// @brief Power-on EM and resume of a manual pause only on the off->on edge of the segment's own power.
constexpr SegmentPowerOnAction decideSegmentPowerOnAction(bool wasOff, uint8_t configuredPowerOnEm,
                                                          bool globalSuspended, bool manualEmPause)
{
    if (!wasOff && manualEmPause && !globalSuspended) return SegmentPowerOnAction::KeepPause;
    if (!wasOff || configuredPowerOnEm == 0) return SegmentPowerOnAction::SwitchOn;
    return globalSuspended ? SegmentPowerOnAction::DeferPowerOnEm : SegmentPowerOnAction::StartPowerOnEm;
}

/// @brief What global ON does with a segment that global OFF suspended.
enum class GlobalPowerOnAction : uint8_t
{
    StayOff,        ///< own power is OFF
    StartPowerOnEm, ///< ON arrived while suspended: deferred power-on EM start
    ShowOutput,     ///< resume the EM or the effect
};

constexpr GlobalPowerOnAction decideGlobalPowerOnAction(bool segmentPowerOn, bool pendingPowerOnRestart)
{
    if (!segmentPowerOn) return GlobalPowerOnAction::StayOff;
    return pendingPowerOnRestart ? GlobalPowerOnAction::StartPowerOnEm : GlobalPowerOnAction::ShowOutput;
}

static_assert(decideSegmentPowerOnAction(true, 1, false, false) == SegmentPowerOnAction::StartPowerOnEm,
              "Power-on EM starts on the off->on edge");
static_assert(decideSegmentPowerOnAction(false, 1, false, false) == SegmentPowerOnAction::SwitchOn,
              "ON on a segment that is already on does not restart the power-on EM");
static_assert(decideSegmentPowerOnAction(true, 0, false, false) == SegmentPowerOnAction::SwitchOn,
              "Without a power-on EM an ON resumes the paused EM or the effect");
static_assert(decideSegmentPowerOnAction(true, 1, true, false) == SegmentPowerOnAction::DeferPowerOnEm,
              "Fresh start waits while global power suspends the segment");
static_assert(decideSegmentPowerOnAction(false, 1, true, false) == SegmentPowerOnAction::SwitchOn,
              "ON on a segment that is on but suspended by global power does not defer a restart");
static_assert(decideSegmentPowerOnAction(true, 0, true, false) == SegmentPowerOnAction::SwitchOn,
              "Without a power-on EM an ON under global OFF only records the segment power");
static_assert(decideSegmentPowerOnAction(false, 0, false, true) == SegmentPowerOnAction::KeepPause,
              "ON on a segment that is on already keeps a manually paused EM paused (caller: intent > 0)");
static_assert(decideSegmentPowerOnAction(true, 0, false, true) == SegmentPowerOnAction::SwitchOn,
              "The off->on edge resumes a manually paused EM");
static_assert(decideSegmentPowerOnAction(true, 1, false, true) == SegmentPowerOnAction::StartPowerOnEm,
              "The off->on edge starts the power-on EM also over a manual pause");
static_assert(decideGlobalPowerOnAction(false, true) == GlobalPowerOnAction::StayOff,
              "Global ON does not light a segment whose own power is OFF");
static_assert(decideGlobalPowerOnAction(true, true) == GlobalPowerOnAction::StartPowerOnEm,
              "A deferred power-on EM starts at global ON");
static_assert(decideGlobalPowerOnAction(true, false) == GlobalPowerOnAction::ShowOutput,
              "Global ON resumes a suspended EM or effect");
