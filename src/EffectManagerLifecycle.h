#pragma once

#include <stdint.h>

enum class EmPowerOnAction : uint8_t
{
    RestoreNormalState,
    KeepSuspended,
    Resume,
    DeferFreshRestart,
    FreshRestart,
};

constexpr EmPowerOnAction decideEmPowerOnAction(uint8_t configuredPowerOnEm,
                                                 bool suspendedByPower,
                                                 bool globalPowerOn)
{
    if (configuredPowerOnEm != 0)
        return globalPowerOn ? EmPowerOnAction::FreshRestart
                             : EmPowerOnAction::DeferFreshRestart;

    if (suspendedByPower)
        return globalPowerOn ? EmPowerOnAction::Resume
                             : EmPowerOnAction::KeepSuspended;

    return EmPowerOnAction::RestoreNormalState;
}

static_assert(decideEmPowerOnAction(1, true, true) == EmPowerOnAction::FreshRestart,
              "Power-On EM must take priority over resume");
static_assert(decideEmPowerOnAction(0, true, true) == EmPowerOnAction::Resume,
              "Manual EM must resume when no Power-On EM is configured");
static_assert(decideEmPowerOnAction(0, false, true) == EmPowerOnAction::RestoreNormalState,
              "Normal state must be restored when no EM lifecycle applies");
static_assert(decideEmPowerOnAction(1, true, false) == EmPowerOnAction::DeferFreshRestart,
              "Fresh restart must wait while global power is off");
