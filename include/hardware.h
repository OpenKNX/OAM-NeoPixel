#pragma once
#include "HardwareConfig.h"

#ifdef ARDUINO_ARCH_RP2040
//     #include "OpenKNXHardware.h"

//     #ifdef BOARD_MASIFI_SENSOR_BREAKOUT
//         // #define OPENKNX_RECOVERY_ON LOW
//         #define PROG_LED_PIN 1
//         #define PROG_LED_PIN_ACTIVE_ON HIGH
//         #define INFO_LED_PIN 3
//         #define INFO_LED_PIN_ACTIVE_ON HIGH
//         #define PROG_BUTTON_PIN 0
//         #define PROG_BUTTON_PIN_INTERRUPT_ON FALLING
//         #define SAVE_INTERRUPT_PIN 22
//         #define KNX_UART_RX_PIN 17
//         #define KNX_UART_TX_PIN 16
//         #define HARDWARE_NAME "MASIFI-SENSOR-BREAKOUT"
//     // #define SmartMF_HardwareVariant_PIN 26
//     // #define SmartMF_HardwareRevision_PIN1 10
//     // #define SmartMF_HardwareRevision_PIN2 11
//     // #define SmartMF_HardwareRevision_PIN3 12
//     #endif

    #ifdef BOARD_PIPICOW
        #define PROG_LED_PIN 32
        #define PROG_LED_PIN_ACTIVE_ON HIGH
    #endif
#endif

#ifdef ARDUINO_ARCH_ESP32
    #ifdef BOARD_ESP32_DEV
        #define PROG_LED_PIN 2
        #define PROG_LED_PIN_ACTIVE_ON HIGH
        // #define PROG_BUTTON_PIN 35
        // #define PROG_BUTTON_PIN_INTERRUPT_ON FALLING
        #define DEVICE_ID "ESP32-DEV"
    #endif
//     #ifdef BOARD_ESP32_WT32_ETH01
//         #define PROG_LED_PIN 2
//         #define PROG_LED_PIN_ACTIVE_ON HIGH
//         #define PROG_BUTTON_PIN 4
//         #define PROG_BUTTON_PIN_INTERRUPT_ON FALLING

//         #define ETH_PHY_TYPE ETH_PHY_LAN8720
//         #define ETH_PHY_ADDR 1
//         #define ETH_PHY_MDC 23
//         #define ETH_PHY_MDIO 18
//         #define ETH_PHY_POWER 16
//         #define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN

//         #define HARDWARE_NAME "ESP32_WT32_ETH01"
//     #endif
#endif
