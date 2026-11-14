#pragma once

#include <cstdint>

// Own hardware profile and the HW-IDs of the ETS hardware module (OGM-HardwareConfig/generator, HardwareProfile.xml)
#include "HardwareProfileData.h"
// GPIO of a port value (OGM-HardwareConfig)
#include "HardwareConfig/Helpers/HardwarePorts.h"

// Port selection values of the ETS "Daten Port" / "Clock Port" / relay port parameters
constexpr uint8_t MAX_HARDWARE_PORT_INDEX = 9;
constexpr uint8_t MANUAL_GPIO_PORT_SELECTION = 10;
constexpr uint8_t DUMMY_GPIO_PORT_SELECTION = 15;
// ports 1..10 fit below the manual value 10; a board with more ports needs another value layout (manifest <Ports max="10">)
#if defined(HW_PORT_COUNT) && (HW_PORT_COUNT) > 10
    #error "NeoPixel stores hardware ports 1..10 (value 10 = manual GPIO): the board describes more ports"
#endif

#ifndef DEVICE_HW_ID
    #error "The board header defines no DEVICE_HW_ID: the ETS hardware selection cannot be checked"
#endif
// Hardware selection of the generated hardware module NEOHW (HardwareProfile.xml <Output share>). Without it the
// check that keeps every port byte unused while no or another board is selected would not exist.
#ifndef ParamNEOHW_HardwareSelect
    #error "include/knxprod.h lacks ParamNEOHW_HardwareSelect (hardware module NEOHW) - run the producer (scripts/Build-Release.ps1)"
#endif
/// Value of the hardware selection while no board is selected
constexpr uint16_t HW_SELECT_NONE = HardwareProfile::NOT_SELECTED;
static_assert(DEVICE_HW_ID != HW_SELECT_NONE, "DEVICE_HW_ID equals the 'not selected' value of the ETS hardware selection");
// A board missing in the ETS hardware module could never be selected: every port would stay unused
static_assert(HardwareProfile::isKnown(DEVICE_HW_ID), "DEVICE_HW_ID is not in the ETS hardware module - run scripts/Build-Release.ps1 (HardwareProfile.xml)");

/// @brief DEVICE_HW_ID stored by the ETS hardware selection; HW_SELECT_NONE while none is selected
inline uint16_t selectedHardwareId()
{
    return (uint16_t)ParamNEOHW_HardwareSelect;
}

/// @brief true when ETS is configured for the board this firmware is built for. Only then do the port bytes hold
/// values for this board: ETS writes them from the parameters of the selected board, or not at all.
inline bool hardwareSelectionMatches(uint16_t selectedHwId)
{
    return selectedHwId == DEVICE_HW_ID;
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

/**
 * @brief Check if hardware-assisted GPIO mode is active
 *
 * Returns true if a valid hardware port is selected (not manual, not dummy).
 *
 * @param portSelection Port-selection value from getDataPortSelection() or getClockPortSelection()
 * @return true if hardware port is selected, false if manual (10) or dummy (15)
 */
inline bool isHardwarePortSelected(uint8_t portSelection)
{
    return portSelection <= MAX_HARDWARE_PORT_INDEX;
}

/// What a strip port selection value (ETS "Daten Port" / "Clock Port") selects
enum class PortSelectionKind : uint8_t
{
    HardwarePort, ///< 0-9: port of this hardware (hwPortGpio)
    Manual,       ///< 10: the manual GPIO parameter of the strip
    NotSelected   ///< 15 ("please select") or any other value
};

/**
 * @brief Classify a strip port selection value
 *
 * The value alone decides.
 */
inline PortSelectionKind classifyPortSelection(uint8_t portSelection)
{
    if (portSelection <= MAX_HARDWARE_PORT_INDEX) return PortSelectionKind::HardwarePort;
    if (portSelection == MANUAL_GPIO_PORT_SELECTION) return PortSelectionKind::Manual;
    return PortSelectionKind::NotSelected;
}

/**
 * @brief GPIO of a strip signal from its port selection value; 255 = no GPIO (the strip stays off)
 * @param hwMatches hardwareSelectionMatches(): otherwise no value of the port byte is used, neither a port nor
 *                  "manual" (an unwritten byte reads 0 = port 1)
 */
inline uint8_t portSelectionGpio(uint8_t portSelection, uint8_t manualGpio, bool hwMatches)
{
    if (!hwMatches) return 255;
    switch (classifyPortSelection(portSelection))
    {
        case PortSelectionKind::HardwarePort: return hwPortGpio(portSelection);
        case PortSelectionKind::Manual: return manualGpio;
        default: return 255;
    }
}
