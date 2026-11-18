#pragma once

#include <cstdint>

// Own hardware profile, the HW-IDs of the ETS hardware module and the port value resolution
// (OGM-HardwareConfig/generator, HardwareManifest.xml portManualValue / portNoneValue):
// HardwareProfile::signalGpio / relaySignalGpio, PORT_MANUAL (10), PORT_NONE (15), GPIO_NONE (255).
// PORT_MANUAL / PORT_NONE are tested before the port, so a port described on 10 or 15 is never used; ETS offers ports
// 1..10 only (HardwareManifest.xml <Ports max="10">, the generator stops on a board with more).
#include "HardwareProfileData.h"
// GPIO of a port value, hard-wired relays (OGM-HardwareConfig)
#include "HardwareConfig/Helpers/HardwarePorts.h"

#ifndef DEVICE_HW_ID
    #error "The board header defines no DEVICE_HW_ID: the ETS hardware selection cannot be checked"
#endif
// Hardware selection of the generated hardware module NEOHW (HardwareManifest.xml <Output share>). Without it the
// check that keeps every port byte unused while no or another board is selected would not exist.
#ifndef ParamNEOHW_HardwareSelect
    #error "include/knxprod.h lacks ParamNEOHW_HardwareSelect (hardware module NEOHW) - run the producer (scripts/Build-Release.ps1)"
#endif
/// Value of the hardware selection while no board is selected
constexpr uint16_t HW_SELECT_NONE = HardwareProfile::NOT_SELECTED;
static_assert(DEVICE_HW_ID != HW_SELECT_NONE, "DEVICE_HW_ID equals the 'not selected' value of the ETS hardware selection");
// A board missing in the ETS hardware module could never be selected: every port would stay unused
static_assert(HardwareProfile::isKnown(DEVICE_HW_ID), "DEVICE_HW_ID is not in the ETS hardware module - run scripts/Build-Release.ps1 (HardwareManifest.xml)");

/// @brief DEVICE_HW_ID stored by the ETS hardware selection; HW_SELECT_NONE while none is selected
inline uint16_t selectedHardwareId()
{
    return (uint16_t)ParamNEOHW_HardwareSelect;
}

/// @brief true when ETS is configured for the board this firmware is built for. Only then do the port bytes hold
/// values for this board: ETS writes them from the parameters of the selected board, or not at all.
inline bool hardwareSelectionMatches(uint16_t selectedHwId)
{
    return HardwareProfile::selectionIsOwn(selectedHwId);
}

// Port carriers: one parameter per strip on the byte that the port parameters of all boards share (strip union)
#if !defined(ParamNEOSTRIP_NEODataPort) || !defined(ParamNEOSTRIP_NEOClockPort)
    #error "include/knxprod.h lacks the strip port carriers NEO<n>DataPort / NEO<n>ClockPort - run the producer (scripts/Build-Release.ps1)"
#endif

/// @brief DEVICE_NAME for the own DEVICE_HW_ID, nullptr for any other (shown by its ID)
inline const char* hardwareNameForId(uint16_t deviceHwId)
{
#ifdef DEVICE_NAME
    if (deviceHwId == DEVICE_HW_ID) return DEVICE_NAME;
#endif
    return nullptr;
}

/**
 * @brief Data port selection of a strip: 0-9 = hardware port (port number - 1), 10 = manual GPIO, 15 = not selected
 * @note Reads the carrier on the byte of the per-board port parameters; ETS writes it from the selected board's parameter.
 */
inline uint8_t getDataPortSelection(uint8_t stripIndex)
{
    const uint8_t _channelIndex = stripIndex; // used by the NEOSTRIP_ParamCalcIndex macro of knxprod.h
    return (uint8_t)ParamNEOSTRIP_NEODataPort;
}

/// @brief Clock port selection of a strip (SPI LED types), same values as getDataPortSelection
inline uint8_t getClockPortSelection(uint8_t stripIndex)
{
    const uint8_t _channelIndex = stripIndex; // used by the NEOSTRIP_ParamCalcIndex macro of knxprod.h
    return (uint8_t)ParamNEOSTRIP_NEOClockPort;
}
