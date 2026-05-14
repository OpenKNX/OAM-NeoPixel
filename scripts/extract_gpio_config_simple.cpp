/**
 * @file extract_gpio_config_simple.cpp
 * @brief Preprocessor-only GPIO configuration extractor (no compilation required)
 * @author Erkan Çolak
 *
 * This tool uses the C preprocessor to extract hardware GPIO configuration
 * from HardwareConfig.h and outputs KEY=VALUE pairs for PowerShell build scripts.
 *
 * Key advantages over compiled version:
 * - Faster execution (no compilation/linking overhead)
 * - Simpler dependencies (only preprocessor required)
 * - Direct macro expansion output
 * - Supports up to 99 GPIO ports per hardware variant
 *
 * Usage: g++ -E -P -I<include_paths> -D<defines> extract_gpio_config_simple.cpp
 *   -E              : Preprocess only
 *   -P              : Omit line markers
 *   -I<paths>       : Include paths for HardwareConfig.h
 *   -D<defines>     : Hardware-specific defines (e.g., -DFEATURE_PREFIX=NEOPIXEL_HW)
 *
 * Output format: KEY=VALUE pairs for easy PowerShell parsing
 */

// Set default DEVICE_HW_ID if not defined by hardware config
#ifndef DEVICE_HW_ID
    #define DEVICE_HW_ID 0x0000
#endif

#include "../include/hardware.h"

// Macro stringification
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

// Expand macro value (extra layer for full expansion)
#define EXPAND(x) x

// Extract Device Info (DEVICE_ID and DEVICE_NAME are already strings)
#ifdef DEVICE_ID
HW_DEVICE_ID = DEVICE_ID
#else
HW_DEVICE_ID = "Unknown"
#endif

#ifdef DEVICE_NAME
    HW_DEVICE_NAME = DEVICE_NAME
#else
    HW_DEVICE_NAME = "Unknown Device"
#endif

        HW_DEVICE_ID_BIT = EXPAND(DEVICE_HW_ID)

// Build macro names dynamically
#define CONCAT_IMPL(a, b) a##_##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define FEATURE_DEFINE(suffix) CONCAT(FEATURE_PREFIX, suffix)

// Check if GPIO ports are defined
#if defined(NEOPIXEL_HW_GPIO_PORTS) || defined(I2C_HW_GPIO_PORTS) || defined(UART_HW_GPIO_PORTS)
            HW_GPIO_PORTS_DEFINED = 1

    // Output port count
    #if defined(NEOPIXEL_HW_PORT_COUNT) || defined(I2C_HW_PORT_COUNT) || defined(UART_HW_PORT_COUNT)
    HW_GPIO_PORT_COUNT = EXPAND(FEATURE_DEFINE(PORT_COUNT))
    #else
    HW_GPIO_PORT_COUNT = 0
    #endif

    // Port 1
    #if defined(NEOPIXEL_HW_PORT_1_GPIO) || defined(I2C_HW_PORT_1_GPIO) || defined(UART_HW_PORT_1_GPIO)
        HW_GPIO_PORT_1_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_1_GPIO))
        #if defined(NEOPIXEL_HW_PORT_1_LABEL) || defined(I2C_HW_PORT_1_LABEL) || defined(UART_HW_PORT_1_LABEL)
            HW_GPIO_PORT_1_LABEL = TO_STRING(FEATURE_DEFINE(PORT_1_LABEL))
        #else
            HW_GPIO_PORT_1_LABEL = "Port 1"
        #endif
    #endif

    // Port 2
    #if defined(NEOPIXEL_HW_PORT_2_GPIO) || defined(I2C_HW_PORT_2_GPIO) || defined(UART_HW_PORT_2_GPIO)
                HW_GPIO_PORT_2_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_2_GPIO))
        #if defined(NEOPIXEL_HW_PORT_2_LABEL) || defined(I2C_HW_PORT_2_LABEL) || defined(UART_HW_PORT_2_LABEL)
                    HW_GPIO_PORT_2_LABEL = TO_STRING(FEATURE_DEFINE(PORT_2_LABEL))
        #else
        HW_GPIO_PORT_2_LABEL = "Port 2"
        #endif
    #endif

    // Port 3
    #if defined(NEOPIXEL_HW_PORT_3_GPIO) || defined(I2C_HW_PORT_3_GPIO) || defined(UART_HW_PORT_3_GPIO)
                        HW_GPIO_PORT_3_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_3_GPIO))
        #if defined(NEOPIXEL_HW_PORT_3_LABEL) || defined(I2C_HW_PORT_3_LABEL) || defined(UART_HW_PORT_3_LABEL)
                            HW_GPIO_PORT_3_LABEL = TO_STRING(FEATURE_DEFINE(PORT_3_LABEL))
        #else
        HW_GPIO_PORT_3_LABEL = "Port 3"
        #endif
    #endif

    // Port 4
    #if defined(NEOPIXEL_HW_PORT_4_GPIO) || defined(I2C_HW_PORT_4_GPIO) || defined(UART_HW_PORT_4_GPIO)
                                HW_GPIO_PORT_4_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_4_GPIO))
        #if defined(NEOPIXEL_HW_PORT_4_LABEL) || defined(I2C_HW_PORT_4_LABEL) || defined(UART_HW_PORT_4_LABEL)
                                    HW_GPIO_PORT_4_LABEL = TO_STRING(FEATURE_DEFINE(PORT_4_LABEL))
        #else
        HW_GPIO_PORT_4_LABEL = "Port 4"
        #endif
    #endif

    // Port 5
    #if defined(NEOPIXEL_HW_PORT_5_GPIO) || defined(I2C_HW_PORT_5_GPIO) || defined(UART_HW_PORT_5_GPIO)
                                        HW_GPIO_PORT_5_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_5_GPIO))
        #if defined(NEOPIXEL_HW_PORT_5_LABEL) || defined(I2C_HW_PORT_5_LABEL) || defined(UART_HW_PORT_5_LABEL)
                                            HW_GPIO_PORT_5_LABEL = TO_STRING(FEATURE_DEFINE(PORT_5_LABEL))
        #else
        HW_GPIO_PORT_5_LABEL = "Port 5"
        #endif
    #endif

    // Port 6
    #if defined(NEOPIXEL_HW_PORT_6_GPIO) || defined(I2C_HW_PORT_6_GPIO) || defined(UART_HW_PORT_6_GPIO)
                                                HW_GPIO_PORT_6_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_6_GPIO))
        #if defined(NEOPIXEL_HW_PORT_6_LABEL) || defined(I2C_HW_PORT_6_LABEL) || defined(UART_HW_PORT_6_LABEL)
                                                    HW_GPIO_PORT_6_LABEL = TO_STRING(FEATURE_DEFINE(PORT_6_LABEL))
        #else
        HW_GPIO_PORT_6_LABEL = "Port 6"
        #endif
    #endif

    // Port 7
    #if defined(NEOPIXEL_HW_PORT_7_GPIO) || defined(I2C_HW_PORT_7_GPIO) || defined(UART_HW_PORT_7_GPIO)
                                                        HW_GPIO_PORT_7_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_7_GPIO))
        #if defined(NEOPIXEL_HW_PORT_7_LABEL) || defined(I2C_HW_PORT_7_LABEL) || defined(UART_HW_PORT_7_LABEL)
                                                            HW_GPIO_PORT_7_LABEL = TO_STRING(FEATURE_DEFINE(PORT_7_LABEL))
        #else
        HW_GPIO_PORT_7_LABEL = "Port 7"
        #endif
    #endif

    // Port 8
    #if defined(NEOPIXEL_HW_PORT_8_GPIO) || defined(I2C_HW_PORT_8_GPIO) || defined(UART_HW_PORT_8_GPIO)
                                                                HW_GPIO_PORT_8_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_8_GPIO))
        #if defined(NEOPIXEL_HW_PORT_8_LABEL) || defined(I2C_HW_PORT_8_LABEL) || defined(UART_HW_PORT_8_LABEL)
                                                                    HW_GPIO_PORT_8_LABEL = TO_STRING(FEATURE_DEFINE(PORT_8_LABEL))
        #else
        HW_GPIO_PORT_8_LABEL = "Port 8"
        #endif
    #endif

    // Port 9
    #if defined(NEOPIXEL_HW_PORT_9_GPIO) || defined(I2C_HW_PORT_9_GPIO) || defined(UART_HW_PORT_9_GPIO)
                                                                        HW_GPIO_PORT_9_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_9_GPIO))
        #if defined(NEOPIXEL_HW_PORT_9_LABEL) || defined(I2C_HW_PORT_9_LABEL) || defined(UART_HW_PORT_9_LABEL)
                                                                            HW_GPIO_PORT_9_LABEL = TO_STRING(FEATURE_DEFINE(PORT_9_LABEL))
        #else
        HW_GPIO_PORT_9_LABEL = "Port 9"
        #endif
    #endif

    // Port 10
    #if defined(NEOPIXEL_HW_PORT_10_GPIO) || defined(I2C_HW_PORT_10_GPIO) || defined(UART_HW_PORT_10_GPIO)
                                                                                HW_GPIO_PORT_10_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_10_GPIO))
        #if defined(NEOPIXEL_HW_PORT_10_LABEL) || defined(I2C_HW_PORT_10_LABEL) || defined(UART_HW_PORT_10_LABEL)
                                                                                    HW_GPIO_PORT_10_LABEL = TO_STRING(FEATURE_DEFINE(PORT_10_LABEL))
        #else
        HW_GPIO_PORT_10_LABEL = "Port 10"
        #endif
    #endif

    // Port 11
    #if defined(NEOPIXEL_HW_PORT_11_GPIO) || defined(I2C_HW_PORT_11_GPIO) || defined(UART_HW_PORT_11_GPIO)
                                                                                        HW_GPIO_PORT_11_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_11_GPIO))
        #if defined(NEOPIXEL_HW_PORT_11_LABEL) || defined(I2C_HW_PORT_11_LABEL) || defined(UART_HW_PORT_11_LABEL)
                                                                                            HW_GPIO_PORT_11_LABEL = TO_STRING(FEATURE_DEFINE(PORT_11_LABEL))
        #else
        HW_GPIO_PORT_11_LABEL = "Port 11"
        #endif
    #endif

    // Port 12
    #if defined(NEOPIXEL_HW_PORT_12_GPIO) || defined(I2C_HW_PORT_12_GPIO) || defined(UART_HW_PORT_12_GPIO)
                                                                                                HW_GPIO_PORT_12_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_12_GPIO))
        #if defined(NEOPIXEL_HW_PORT_12_LABEL) || defined(I2C_HW_PORT_12_LABEL) || defined(UART_HW_PORT_12_LABEL)
                                                                                                    HW_GPIO_PORT_12_LABEL = TO_STRING(FEATURE_DEFINE(PORT_12_LABEL))
        #else
        HW_GPIO_PORT_12_LABEL = "Port 12"
        #endif
    #endif

    // Port 13
    #if defined(NEOPIXEL_HW_PORT_13_GPIO) || defined(I2C_HW_PORT_13_GPIO) || defined(UART_HW_PORT_13_GPIO)
                                                                                                        HW_GPIO_PORT_13_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_13_GPIO))
        #if defined(NEOPIXEL_HW_PORT_13_LABEL) || defined(I2C_HW_PORT_13_LABEL) || defined(UART_HW_PORT_13_LABEL)
                                                                                                            HW_GPIO_PORT_13_LABEL = TO_STRING(FEATURE_DEFINE(PORT_13_LABEL))
        #else
        HW_GPIO_PORT_13_LABEL = "Port 13"
        #endif
    #endif

    // Port 14
    #if defined(NEOPIXEL_HW_PORT_14_GPIO) || defined(I2C_HW_PORT_14_GPIO) || defined(UART_HW_PORT_14_GPIO)
                                                                                                                HW_GPIO_PORT_14_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_14_GPIO))
        #if defined(NEOPIXEL_HW_PORT_14_LABEL) || defined(I2C_HW_PORT_14_LABEL) || defined(UART_HW_PORT_14_LABEL)
                                                                                                                    HW_GPIO_PORT_14_LABEL = TO_STRING(FEATURE_DEFINE(PORT_14_LABEL))
        #else
        HW_GPIO_PORT_14_LABEL = "Port 14"
        #endif
    #endif

    // Port 15
    #if defined(NEOPIXEL_HW_PORT_15_GPIO) || defined(I2C_HW_PORT_15_GPIO) || defined(UART_HW_PORT_15_GPIO)
                                                                                                                        HW_GPIO_PORT_15_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_15_GPIO))
        #if defined(NEOPIXEL_HW_PORT_15_LABEL) || defined(I2C_HW_PORT_15_LABEL) || defined(UART_HW_PORT_15_LABEL)
                                                                                                                            HW_GPIO_PORT_15_LABEL = TO_STRING(FEATURE_DEFINE(PORT_15_LABEL))
        #else
        HW_GPIO_PORT_15_LABEL = "Port 15"
        #endif
    #endif

    // Port 16
    #if defined(NEOPIXEL_HW_PORT_16_GPIO) || defined(I2C_HW_PORT_16_GPIO) || defined(UART_HW_PORT_16_GPIO)
                                                                                                                                HW_GPIO_PORT_16_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_16_GPIO))
        #if defined(NEOPIXEL_HW_PORT_16_LABEL) || defined(I2C_HW_PORT_16_LABEL) || defined(UART_HW_PORT_16_LABEL)
                                                                                                                                    HW_GPIO_PORT_16_LABEL = TO_STRING(FEATURE_DEFINE(PORT_16_LABEL))
        #else
        HW_GPIO_PORT_16_LABEL = "Port 16"
        #endif
    #endif

    // Port 17
    #if defined(NEOPIXEL_HW_PORT_17_GPIO) || defined(I2C_HW_PORT_17_GPIO) || defined(UART_HW_PORT_17_GPIO)
                                                                                                                                        HW_GPIO_PORT_17_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_17_GPIO))
        #if defined(NEOPIXEL_HW_PORT_17_LABEL) || defined(I2C_HW_PORT_17_LABEL) || defined(UART_HW_PORT_17_LABEL)
                                                                                                                                            HW_GPIO_PORT_17_LABEL = TO_STRING(FEATURE_DEFINE(PORT_17_LABEL))
        #else
        HW_GPIO_PORT_17_LABEL = "Port 17"
        #endif
    #endif

    // Port 18
    #if defined(NEOPIXEL_HW_PORT_18_GPIO) || defined(I2C_HW_PORT_18_GPIO) || defined(UART_HW_PORT_18_GPIO)
                                                                                                                                                HW_GPIO_PORT_18_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_18_GPIO))
        #if defined(NEOPIXEL_HW_PORT_18_LABEL) || defined(I2C_HW_PORT_18_LABEL) || defined(UART_HW_PORT_18_LABEL)
                                                                                                                                                    HW_GPIO_PORT_18_LABEL = TO_STRING(FEATURE_DEFINE(PORT_18_LABEL))
        #else
        HW_GPIO_PORT_18_LABEL = "Port 18"
        #endif
    #endif

    // Port 19
    #if defined(NEOPIXEL_HW_PORT_19_GPIO) || defined(I2C_HW_PORT_19_GPIO) || defined(UART_HW_PORT_19_GPIO)
                                                                                                                                                        HW_GPIO_PORT_19_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_19_GPIO))
        #if defined(NEOPIXEL_HW_PORT_19_LABEL) || defined(I2C_HW_PORT_19_LABEL) || defined(UART_HW_PORT_19_LABEL)
                                                                                                                                                            HW_GPIO_PORT_19_LABEL = TO_STRING(FEATURE_DEFINE(PORT_19_LABEL))
        #else
        HW_GPIO_PORT_19_LABEL = "Port 19"
        #endif
    #endif

    // Port 20
    #if defined(NEOPIXEL_HW_PORT_20_GPIO) || defined(I2C_HW_PORT_20_GPIO) || defined(UART_HW_PORT_20_GPIO)
                                                                                                                                                                HW_GPIO_PORT_20_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_20_GPIO))
        #if defined(NEOPIXEL_HW_PORT_20_LABEL) || defined(I2C_HW_PORT_20_LABEL) || defined(UART_HW_PORT_20_LABEL)
                                                                                                                                                                    HW_GPIO_PORT_20_LABEL = TO_STRING(FEATURE_DEFINE(PORT_20_LABEL))
        #else
        HW_GPIO_PORT_20_LABEL = "Port 20"
        #endif
    #endif

    // Ports 21-99 follow the same pattern...
    // Port 21-30
    #if defined(NEOPIXEL_HW_PORT_21_GPIO) || defined(I2C_HW_PORT_21_GPIO) || defined(UART_HW_PORT_21_GPIO)
                                                                                                                                                                        HW_GPIO_PORT_21_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_21_GPIO))
        #if defined(NEOPIXEL_HW_PORT_21_LABEL) || defined(I2C_HW_PORT_21_LABEL) || defined(UART_HW_PORT_21_LABEL)
                                                                                                                                                                            HW_GPIO_PORT_21_LABEL = TO_STRING(FEATURE_DEFINE(PORT_21_LABEL))
        #else
        HW_GPIO_PORT_21_LABEL = "Port 21"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_22_GPIO) || defined(I2C_HW_PORT_22_GPIO) || defined(UART_HW_PORT_22_GPIO)
                                                                                                                                                                                HW_GPIO_PORT_22_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_22_GPIO))
        #if defined(NEOPIXEL_HW_PORT_22_LABEL) || defined(I2C_HW_PORT_22_LABEL) || defined(UART_HW_PORT_22_LABEL)
                                                                                                                                                                                    HW_GPIO_PORT_22_LABEL = TO_STRING(FEATURE_DEFINE(PORT_22_LABEL))
        #else
        HW_GPIO_PORT_22_LABEL = "Port 22"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_23_GPIO) || defined(I2C_HW_PORT_23_GPIO) || defined(UART_HW_PORT_23_GPIO)
                                                                                                                                                                                        HW_GPIO_PORT_23_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_23_GPIO))
        #if defined(NEOPIXEL_HW_PORT_23_LABEL) || defined(I2C_HW_PORT_23_LABEL) || defined(UART_HW_PORT_23_LABEL)
                                                                                                                                                                                            HW_GPIO_PORT_23_LABEL = TO_STRING(FEATURE_DEFINE(PORT_23_LABEL))
        #else
        HW_GPIO_PORT_23_LABEL = "Port 23"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_24_GPIO) || defined(I2C_HW_PORT_24_GPIO) || defined(UART_HW_PORT_24_GPIO)
                                                                                                                                                                                                HW_GPIO_PORT_24_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_24_GPIO))
        #if defined(NEOPIXEL_HW_PORT_24_LABEL) || defined(I2C_HW_PORT_24_LABEL) || defined(UART_HW_PORT_24_LABEL)
                                                                                                                                                                                                    HW_GPIO_PORT_24_LABEL = TO_STRING(FEATURE_DEFINE(PORT_24_LABEL))
        #else
        HW_GPIO_PORT_24_LABEL = "Port 24"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_25_GPIO) || defined(I2C_HW_PORT_25_GPIO) || defined(UART_HW_PORT_25_GPIO)
                                                                                                                                                                                                        HW_GPIO_PORT_25_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_25_GPIO))
        #if defined(NEOPIXEL_HW_PORT_25_LABEL) || defined(I2C_HW_PORT_25_LABEL) || defined(UART_HW_PORT_25_LABEL)
                                                                                                                                                                                                            HW_GPIO_PORT_25_LABEL = TO_STRING(FEATURE_DEFINE(PORT_25_LABEL))
        #else
        HW_GPIO_PORT_25_LABEL = "Port 25"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_26_GPIO) || defined(I2C_HW_PORT_26_GPIO) || defined(UART_HW_PORT_26_GPIO)
                                                                                                                                                                                                                HW_GPIO_PORT_26_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_26_GPIO))
        #if defined(NEOPIXEL_HW_PORT_26_LABEL) || defined(I2C_HW_PORT_26_LABEL) || defined(UART_HW_PORT_26_LABEL)
                                                                                                                                                                                                                    HW_GPIO_PORT_26_LABEL = TO_STRING(FEATURE_DEFINE(PORT_26_LABEL))
        #else
        HW_GPIO_PORT_26_LABEL = "Port 26"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_27_GPIO) || defined(I2C_HW_PORT_27_GPIO) || defined(UART_HW_PORT_27_GPIO)
                                                                                                                                                                                                                        HW_GPIO_PORT_27_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_27_GPIO))
        #if defined(NEOPIXEL_HW_PORT_27_LABEL) || defined(I2C_HW_PORT_27_LABEL) || defined(UART_HW_PORT_27_LABEL)
                                                                                                                                                                                                                            HW_GPIO_PORT_27_LABEL = TO_STRING(FEATURE_DEFINE(PORT_27_LABEL))
        #else
        HW_GPIO_PORT_27_LABEL = "Port 27"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_28_GPIO) || defined(I2C_HW_PORT_28_GPIO) || defined(UART_HW_PORT_28_GPIO)
                                                                                                                                                                                                                                HW_GPIO_PORT_28_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_28_GPIO))
        #if defined(NEOPIXEL_HW_PORT_28_LABEL) || defined(I2C_HW_PORT_28_LABEL) || defined(UART_HW_PORT_28_LABEL)
                                                                                                                                                                                                                                    HW_GPIO_PORT_28_LABEL = TO_STRING(FEATURE_DEFINE(PORT_28_LABEL))
        #else
        HW_GPIO_PORT_28_LABEL = "Port 28"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_29_GPIO) || defined(I2C_HW_PORT_29_GPIO) || defined(UART_HW_PORT_29_GPIO)
                                                                                                                                                                                                                                        HW_GPIO_PORT_29_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_29_GPIO))
        #if defined(NEOPIXEL_HW_PORT_29_LABEL) || defined(I2C_HW_PORT_29_LABEL) || defined(UART_HW_PORT_29_LABEL)
                                                                                                                                                                                                                                            HW_GPIO_PORT_29_LABEL = TO_STRING(FEATURE_DEFINE(PORT_29_LABEL))
        #else
        HW_GPIO_PORT_29_LABEL = "Port 29"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_30_GPIO) || defined(I2C_HW_PORT_30_GPIO) || defined(UART_HW_PORT_30_GPIO)
                                                                                                                                                                                                                                                HW_GPIO_PORT_30_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_30_GPIO))
        #if defined(NEOPIXEL_HW_PORT_30_LABEL) || defined(I2C_HW_PORT_30_LABEL) || defined(UART_HW_PORT_30_LABEL)
                                                                                                                                                                                                                                                    HW_GPIO_PORT_30_LABEL = TO_STRING(FEATURE_DEFINE(PORT_30_LABEL))
        #else
        HW_GPIO_PORT_30_LABEL = "Port 30"
        #endif
    #endif

    // Port 31-40
    #if defined(NEOPIXEL_HW_PORT_31_GPIO) || defined(I2C_HW_PORT_31_GPIO) || defined(UART_HW_PORT_31_GPIO)
                                                                                                                                                                                                                                                        HW_GPIO_PORT_31_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_31_GPIO))
        #if defined(NEOPIXEL_HW_PORT_31_LABEL) || defined(I2C_HW_PORT_31_LABEL) || defined(UART_HW_PORT_31_LABEL)
                                                                                                                                                                                                                                                            HW_GPIO_PORT_31_LABEL = TO_STRING(FEATURE_DEFINE(PORT_31_LABEL))
        #else
        HW_GPIO_PORT_31_LABEL = "Port 31"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_32_GPIO) || defined(I2C_HW_PORT_32_GPIO) || defined(UART_HW_PORT_32_GPIO)
                                                                                                                                                                                                                                                                HW_GPIO_PORT_32_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_32_GPIO))
        #if defined(NEOPIXEL_HW_PORT_32_LABEL) || defined(I2C_HW_PORT_32_LABEL) || defined(UART_HW_PORT_32_LABEL)
                                                                                                                                                                                                                                                                    HW_GPIO_PORT_32_LABEL = TO_STRING(FEATURE_DEFINE(PORT_32_LABEL))
        #else
        HW_GPIO_PORT_32_LABEL = "Port 32"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_33_GPIO) || defined(I2C_HW_PORT_33_GPIO) || defined(UART_HW_PORT_33_GPIO)
                                                                                                                                                                                                                                                                        HW_GPIO_PORT_33_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_33_GPIO))
        #if defined(NEOPIXEL_HW_PORT_33_LABEL) || defined(I2C_HW_PORT_33_LABEL) || defined(UART_HW_PORT_33_LABEL)
                                                                                                                                                                                                                                                                            HW_GPIO_PORT_33_LABEL = TO_STRING(FEATURE_DEFINE(PORT_33_LABEL))
        #else
        HW_GPIO_PORT_33_LABEL = "Port 33"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_34_GPIO) || defined(I2C_HW_PORT_34_GPIO) || defined(UART_HW_PORT_34_GPIO)
                                                                                                                                                                                                                                                                                HW_GPIO_PORT_34_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_34_GPIO))
        #if defined(NEOPIXEL_HW_PORT_34_LABEL) || defined(I2C_HW_PORT_34_LABEL) || defined(UART_HW_PORT_34_LABEL)
                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_34_LABEL = TO_STRING(FEATURE_DEFINE(PORT_34_LABEL))
        #else
        HW_GPIO_PORT_34_LABEL = "Port 34"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_35_GPIO) || defined(I2C_HW_PORT_35_GPIO) || defined(UART_HW_PORT_35_GPIO)
                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_35_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_35_GPIO))
        #if defined(NEOPIXEL_HW_PORT_35_LABEL) || defined(I2C_HW_PORT_35_LABEL) || defined(UART_HW_PORT_35_LABEL)
                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_35_LABEL = TO_STRING(FEATURE_DEFINE(PORT_35_LABEL))
        #else
        HW_GPIO_PORT_35_LABEL = "Port 35"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_36_GPIO) || defined(I2C_HW_PORT_36_GPIO) || defined(UART_HW_PORT_36_GPIO)
                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_36_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_36_GPIO))
        #if defined(NEOPIXEL_HW_PORT_36_LABEL) || defined(I2C_HW_PORT_36_LABEL) || defined(UART_HW_PORT_36_LABEL)
                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_36_LABEL = TO_STRING(FEATURE_DEFINE(PORT_36_LABEL))
        #else
        HW_GPIO_PORT_36_LABEL = "Port 36"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_37_GPIO) || defined(I2C_HW_PORT_37_GPIO) || defined(UART_HW_PORT_37_GPIO)
                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_37_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_37_GPIO))
        #if defined(NEOPIXEL_HW_PORT_37_LABEL) || defined(I2C_HW_PORT_37_LABEL) || defined(UART_HW_PORT_37_LABEL)
                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_37_LABEL = TO_STRING(FEATURE_DEFINE(PORT_37_LABEL))
        #else
        HW_GPIO_PORT_37_LABEL = "Port 37"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_38_GPIO) || defined(I2C_HW_PORT_38_GPIO) || defined(UART_HW_PORT_38_GPIO)
                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_38_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_38_GPIO))
        #if defined(NEOPIXEL_HW_PORT_38_LABEL) || defined(I2C_HW_PORT_38_LABEL) || defined(UART_HW_PORT_38_LABEL)
                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_38_LABEL = TO_STRING(FEATURE_DEFINE(PORT_38_LABEL))
        #else
        HW_GPIO_PORT_38_LABEL = "Port 38"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_39_GPIO) || defined(I2C_HW_PORT_39_GPIO) || defined(UART_HW_PORT_39_GPIO)
                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_39_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_39_GPIO))
        #if defined(NEOPIXEL_HW_PORT_39_LABEL) || defined(I2C_HW_PORT_39_LABEL) || defined(UART_HW_PORT_39_LABEL)
                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_39_LABEL = TO_STRING(FEATURE_DEFINE(PORT_39_LABEL))
        #else
        HW_GPIO_PORT_39_LABEL = "Port 39"
        #endif
    #endif

    #if defined(NEOPIXEL_HW_PORT_40_GPIO) || defined(I2C_HW_PORT_40_GPIO) || defined(UART_HW_PORT_40_GPIO)
                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_40_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_40_GPIO))
        #if defined(NEOPIXEL_HW_PORT_40_LABEL) || defined(I2C_HW_PORT_40_LABEL) || defined(UART_HW_PORT_40_LABEL)
                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_40_LABEL = TO_STRING(FEATURE_DEFINE(PORT_40_LABEL))
        #else
        HW_GPIO_PORT_40_LABEL = "Port 40"
        #endif
    #endif

    // Port 41
    #if defined(NEOPIXEL_HW_PORT_41_GPIO) || defined(I2C_HW_PORT_41_GPIO) || defined(UART_HW_PORT_41_GPIO)
                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_41_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_41_GPIO))
        #if defined(NEOPIXEL_HW_PORT_41_LABEL) || defined(I2C_HW_PORT_41_LABEL) || defined(UART_HW_PORT_41_LABEL)
                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_41_LABEL = TO_STRING(FEATURE_DEFINE(PORT_41_LABEL))
        #else
        HW_GPIO_PORT_41_LABEL = "Port 41"
        #endif
    #endif

    // Port 42
    #if defined(NEOPIXEL_HW_PORT_42_GPIO) || defined(I2C_HW_PORT_42_GPIO) || defined(UART_HW_PORT_42_GPIO)
                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_42_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_42_GPIO))
        #if defined(NEOPIXEL_HW_PORT_42_LABEL) || defined(I2C_HW_PORT_42_LABEL) || defined(UART_HW_PORT_42_LABEL)
                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_42_LABEL = TO_STRING(FEATURE_DEFINE(PORT_42_LABEL))
        #else
        HW_GPIO_PORT_42_LABEL = "Port 42"
        #endif
    #endif

    // Port 43
    #if defined(NEOPIXEL_HW_PORT_43_GPIO) || defined(I2C_HW_PORT_43_GPIO) || defined(UART_HW_PORT_43_GPIO)
                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_43_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_43_GPIO))
        #if defined(NEOPIXEL_HW_PORT_43_LABEL) || defined(I2C_HW_PORT_43_LABEL) || defined(UART_HW_PORT_43_LABEL)
                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_43_LABEL = TO_STRING(FEATURE_DEFINE(PORT_43_LABEL))
        #else
        HW_GPIO_PORT_43_LABEL = "Port 43"
        #endif
    #endif

    // Port 44
    #if defined(NEOPIXEL_HW_PORT_44_GPIO) || defined(I2C_HW_PORT_44_GPIO) || defined(UART_HW_PORT_44_GPIO)
                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_44_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_44_GPIO))
        #if defined(NEOPIXEL_HW_PORT_44_LABEL) || defined(I2C_HW_PORT_44_LABEL) || defined(UART_HW_PORT_44_LABEL)
                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_44_LABEL = TO_STRING(FEATURE_DEFINE(PORT_44_LABEL))
        #else
        HW_GPIO_PORT_44_LABEL = "Port 44"
        #endif
    #endif

    // Port 45
    #if defined(NEOPIXEL_HW_PORT_45_GPIO) || defined(I2C_HW_PORT_45_GPIO) || defined(UART_HW_PORT_45_GPIO)
                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_45_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_45_GPIO))
        #if defined(NEOPIXEL_HW_PORT_45_LABEL) || defined(I2C_HW_PORT_45_LABEL) || defined(UART_HW_PORT_45_LABEL)
                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_45_LABEL = TO_STRING(FEATURE_DEFINE(PORT_45_LABEL))
        #else
        HW_GPIO_PORT_45_LABEL = "Port 45"
        #endif
    #endif

    // Port 46
    #if defined(NEOPIXEL_HW_PORT_46_GPIO) || defined(I2C_HW_PORT_46_GPIO) || defined(UART_HW_PORT_46_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_46_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_46_GPIO))
        #if defined(NEOPIXEL_HW_PORT_46_LABEL) || defined(I2C_HW_PORT_46_LABEL) || defined(UART_HW_PORT_46_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_46_LABEL = TO_STRING(FEATURE_DEFINE(PORT_46_LABEL))
        #else
        HW_GPIO_PORT_46_LABEL = "Port 46"
        #endif
    #endif

    // Port 47
    #if defined(NEOPIXEL_HW_PORT_47_GPIO) || defined(I2C_HW_PORT_47_GPIO) || defined(UART_HW_PORT_47_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_47_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_47_GPIO))
        #if defined(NEOPIXEL_HW_PORT_47_LABEL) || defined(I2C_HW_PORT_47_LABEL) || defined(UART_HW_PORT_47_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_47_LABEL = TO_STRING(FEATURE_DEFINE(PORT_47_LABEL))
        #else
        HW_GPIO_PORT_47_LABEL = "Port 47"
        #endif
    #endif

    // Port 48
    #if defined(NEOPIXEL_HW_PORT_48_GPIO) || defined(I2C_HW_PORT_48_GPIO) || defined(UART_HW_PORT_48_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_48_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_48_GPIO))
        #if defined(NEOPIXEL_HW_PORT_48_LABEL) || defined(I2C_HW_PORT_48_LABEL) || defined(UART_HW_PORT_48_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_48_LABEL = TO_STRING(FEATURE_DEFINE(PORT_48_LABEL))
        #else
        HW_GPIO_PORT_48_LABEL = "Port 48"
        #endif
    #endif

    // Port 49
    #if defined(NEOPIXEL_HW_PORT_49_GPIO) || defined(I2C_HW_PORT_49_GPIO) || defined(UART_HW_PORT_49_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_49_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_49_GPIO))
        #if defined(NEOPIXEL_HW_PORT_49_LABEL) || defined(I2C_HW_PORT_49_LABEL) || defined(UART_HW_PORT_49_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_49_LABEL = TO_STRING(FEATURE_DEFINE(PORT_49_LABEL))
        #else
        HW_GPIO_PORT_49_LABEL = "Port 49"
        #endif
    #endif

    // Port 50
    #if defined(NEOPIXEL_HW_PORT_50_GPIO) || defined(I2C_HW_PORT_50_GPIO) || defined(UART_HW_PORT_50_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_50_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_50_GPIO))
        #if defined(NEOPIXEL_HW_PORT_50_LABEL) || defined(I2C_HW_PORT_50_LABEL) || defined(UART_HW_PORT_50_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_50_LABEL = TO_STRING(FEATURE_DEFINE(PORT_50_LABEL))
        #else
        HW_GPIO_PORT_50_LABEL = "Port 50"
        #endif
    #endif

    // Port 51
    #if defined(NEOPIXEL_HW_PORT_51_GPIO) || defined(I2C_HW_PORT_51_GPIO) || defined(UART_HW_PORT_51_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_51_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_51_GPIO))
        #if defined(NEOPIXEL_HW_PORT_51_LABEL) || defined(I2C_HW_PORT_51_LABEL) || defined(UART_HW_PORT_51_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_51_LABEL = TO_STRING(FEATURE_DEFINE(PORT_51_LABEL))
        #else
        HW_GPIO_PORT_51_LABEL = "Port 51"
        #endif
    #endif

    // Port 52
    #if defined(NEOPIXEL_HW_PORT_52_GPIO) || defined(I2C_HW_PORT_52_GPIO) || defined(UART_HW_PORT_52_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_52_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_52_GPIO))
        #if defined(NEOPIXEL_HW_PORT_52_LABEL) || defined(I2C_HW_PORT_52_LABEL) || defined(UART_HW_PORT_52_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_52_LABEL = TO_STRING(FEATURE_DEFINE(PORT_52_LABEL))
        #else
        HW_GPIO_PORT_52_LABEL = "Port 52"
        #endif
    #endif

    // Port 53
    #if defined(NEOPIXEL_HW_PORT_53_GPIO) || defined(I2C_HW_PORT_53_GPIO) || defined(UART_HW_PORT_53_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_53_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_53_GPIO))
        #if defined(NEOPIXEL_HW_PORT_53_LABEL) || defined(I2C_HW_PORT_53_LABEL) || defined(UART_HW_PORT_53_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_53_LABEL = TO_STRING(FEATURE_DEFINE(PORT_53_LABEL))
        #else
        HW_GPIO_PORT_53_LABEL = "Port 53"
        #endif
    #endif

    // Port 54
    #if defined(NEOPIXEL_HW_PORT_54_GPIO) || defined(I2C_HW_PORT_54_GPIO) || defined(UART_HW_PORT_54_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_54_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_54_GPIO))
        #if defined(NEOPIXEL_HW_PORT_54_LABEL) || defined(I2C_HW_PORT_54_LABEL) || defined(UART_HW_PORT_54_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_54_LABEL = TO_STRING(FEATURE_DEFINE(PORT_54_LABEL))
        #else
        HW_GPIO_PORT_54_LABEL = "Port 54"
        #endif
    #endif

    // Port 55
    #if defined(NEOPIXEL_HW_PORT_55_GPIO) || defined(I2C_HW_PORT_55_GPIO) || defined(UART_HW_PORT_55_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_55_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_55_GPIO))
        #if defined(NEOPIXEL_HW_PORT_55_LABEL) || defined(I2C_HW_PORT_55_LABEL) || defined(UART_HW_PORT_55_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_55_LABEL = TO_STRING(FEATURE_DEFINE(PORT_55_LABEL))
        #else
        HW_GPIO_PORT_55_LABEL = "Port 55"
        #endif
    #endif

    // Port 56
    #if defined(NEOPIXEL_HW_PORT_56_GPIO) || defined(I2C_HW_PORT_56_GPIO) || defined(UART_HW_PORT_56_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_56_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_56_GPIO))
        #if defined(NEOPIXEL_HW_PORT_56_LABEL) || defined(I2C_HW_PORT_56_LABEL) || defined(UART_HW_PORT_56_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_56_LABEL = TO_STRING(FEATURE_DEFINE(PORT_56_LABEL))
        #else
        HW_GPIO_PORT_56_LABEL = "Port 56"
        #endif
    #endif

    // Port 57
    #if defined(NEOPIXEL_HW_PORT_57_GPIO) || defined(I2C_HW_PORT_57_GPIO) || defined(UART_HW_PORT_57_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_57_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_57_GPIO))
        #if defined(NEOPIXEL_HW_PORT_57_LABEL) || defined(I2C_HW_PORT_57_LABEL) || defined(UART_HW_PORT_57_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_57_LABEL = TO_STRING(FEATURE_DEFINE(PORT_57_LABEL))
        #else
        HW_GPIO_PORT_57_LABEL = "Port 57"
        #endif
    #endif

    // Port 58
    #if defined(NEOPIXEL_HW_PORT_58_GPIO) || defined(I2C_HW_PORT_58_GPIO) || defined(UART_HW_PORT_58_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_58_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_58_GPIO))
        #if defined(NEOPIXEL_HW_PORT_58_LABEL) || defined(I2C_HW_PORT_58_LABEL) || defined(UART_HW_PORT_58_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_58_LABEL = TO_STRING(FEATURE_DEFINE(PORT_58_LABEL))
        #else
        HW_GPIO_PORT_58_LABEL = "Port 58"
        #endif
    #endif

    // Port 59
    #if defined(NEOPIXEL_HW_PORT_59_GPIO) || defined(I2C_HW_PORT_59_GPIO) || defined(UART_HW_PORT_59_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_59_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_59_GPIO))
        #if defined(NEOPIXEL_HW_PORT_59_LABEL) || defined(I2C_HW_PORT_59_LABEL) || defined(UART_HW_PORT_59_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_59_LABEL = TO_STRING(FEATURE_DEFINE(PORT_59_LABEL))
        #else
        HW_GPIO_PORT_59_LABEL = "Port 59"
        #endif
    #endif

    // Port 60
    #if defined(NEOPIXEL_HW_PORT_60_GPIO) || defined(I2C_HW_PORT_60_GPIO) || defined(UART_HW_PORT_60_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_60_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_60_GPIO))
        #if defined(NEOPIXEL_HW_PORT_60_LABEL) || defined(I2C_HW_PORT_60_LABEL) || defined(UART_HW_PORT_60_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_60_LABEL = TO_STRING(FEATURE_DEFINE(PORT_60_LABEL))
        #else
        HW_GPIO_PORT_60_LABEL = "Port 60"
        #endif
    #endif

    // Port 61
    #if defined(NEOPIXEL_HW_PORT_61_GPIO) || defined(I2C_HW_PORT_61_GPIO) || defined(UART_HW_PORT_61_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_61_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_61_GPIO))
        #if defined(NEOPIXEL_HW_PORT_61_LABEL) || defined(I2C_HW_PORT_61_LABEL) || defined(UART_HW_PORT_61_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_61_LABEL = TO_STRING(FEATURE_DEFINE(PORT_61_LABEL))
        #else
        HW_GPIO_PORT_61_LABEL = "Port 61"
        #endif
    #endif

    // Port 62
    #if defined(NEOPIXEL_HW_PORT_62_GPIO) || defined(I2C_HW_PORT_62_GPIO) || defined(UART_HW_PORT_62_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_62_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_62_GPIO))
        #if defined(NEOPIXEL_HW_PORT_62_LABEL) || defined(I2C_HW_PORT_62_LABEL) || defined(UART_HW_PORT_62_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_62_LABEL = TO_STRING(FEATURE_DEFINE(PORT_62_LABEL))
        #else
        HW_GPIO_PORT_62_LABEL = "Port 62"
        #endif
    #endif

    // Port 63
    #if defined(NEOPIXEL_HW_PORT_63_GPIO) || defined(I2C_HW_PORT_63_GPIO) || defined(UART_HW_PORT_63_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_63_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_63_GPIO))
        #if defined(NEOPIXEL_HW_PORT_63_LABEL) || defined(I2C_HW_PORT_63_LABEL) || defined(UART_HW_PORT_63_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_63_LABEL = TO_STRING(FEATURE_DEFINE(PORT_63_LABEL))
        #else
        HW_GPIO_PORT_63_LABEL = "Port 63"
        #endif
    #endif

    // Port 64
    #if defined(NEOPIXEL_HW_PORT_64_GPIO) || defined(I2C_HW_PORT_64_GPIO) || defined(UART_HW_PORT_64_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_64_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_64_GPIO))
        #if defined(NEOPIXEL_HW_PORT_64_LABEL) || defined(I2C_HW_PORT_64_LABEL) || defined(UART_HW_PORT_64_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_64_LABEL = TO_STRING(FEATURE_DEFINE(PORT_64_LABEL))
        #else
        HW_GPIO_PORT_64_LABEL = "Port 64"
        #endif
    #endif

    // Port 65
    #if defined(NEOPIXEL_HW_PORT_65_GPIO) || defined(I2C_HW_PORT_65_GPIO) || defined(UART_HW_PORT_65_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_65_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_65_GPIO))
        #if defined(NEOPIXEL_HW_PORT_65_LABEL) || defined(I2C_HW_PORT_65_LABEL) || defined(UART_HW_PORT_65_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_65_LABEL = TO_STRING(FEATURE_DEFINE(PORT_65_LABEL))
        #else
        HW_GPIO_PORT_65_LABEL = "Port 65"
        #endif
    #endif

    // Port 66
    #if defined(NEOPIXEL_HW_PORT_66_GPIO) || defined(I2C_HW_PORT_66_GPIO) || defined(UART_HW_PORT_66_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_66_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_66_GPIO))
        #if defined(NEOPIXEL_HW_PORT_66_LABEL) || defined(I2C_HW_PORT_66_LABEL) || defined(UART_HW_PORT_66_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_66_LABEL = TO_STRING(FEATURE_DEFINE(PORT_66_LABEL))
        #else
        HW_GPIO_PORT_66_LABEL = "Port 66"
        #endif
    #endif

    // Port 67
    #if defined(NEOPIXEL_HW_PORT_67_GPIO) || defined(I2C_HW_PORT_67_GPIO) || defined(UART_HW_PORT_67_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_67_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_67_GPIO))
        #if defined(NEOPIXEL_HW_PORT_67_LABEL) || defined(I2C_HW_PORT_67_LABEL) || defined(UART_HW_PORT_67_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_67_LABEL = TO_STRING(FEATURE_DEFINE(PORT_67_LABEL))
        #else
        HW_GPIO_PORT_67_LABEL = "Port 67"
        #endif
    #endif

    // Port 68
    #if defined(NEOPIXEL_HW_PORT_68_GPIO) || defined(I2C_HW_PORT_68_GPIO) || defined(UART_HW_PORT_68_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_68_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_68_GPIO))
        #if defined(NEOPIXEL_HW_PORT_68_LABEL) || defined(I2C_HW_PORT_68_LABEL) || defined(UART_HW_PORT_68_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_68_LABEL = TO_STRING(FEATURE_DEFINE(PORT_68_LABEL))
        #else
        HW_GPIO_PORT_68_LABEL = "Port 68"
        #endif
    #endif

    // Port 69
    #if defined(NEOPIXEL_HW_PORT_69_GPIO) || defined(I2C_HW_PORT_69_GPIO) || defined(UART_HW_PORT_69_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_69_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_69_GPIO))
        #if defined(NEOPIXEL_HW_PORT_69_LABEL) || defined(I2C_HW_PORT_69_LABEL) || defined(UART_HW_PORT_69_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_69_LABEL = TO_STRING(FEATURE_DEFINE(PORT_69_LABEL))
        #else
        HW_GPIO_PORT_69_LABEL = "Port 69"
        #endif
    #endif

    // Port 70
    #if defined(NEOPIXEL_HW_PORT_70_GPIO) || defined(I2C_HW_PORT_70_GPIO) || defined(UART_HW_PORT_70_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_70_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_70_GPIO))
        #if defined(NEOPIXEL_HW_PORT_70_LABEL) || defined(I2C_HW_PORT_70_LABEL) || defined(UART_HW_PORT_70_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_70_LABEL = TO_STRING(FEATURE_DEFINE(PORT_70_LABEL))
        #else
        HW_GPIO_PORT_70_LABEL = "Port 70"
        #endif
    #endif

    // Port 71
    #if defined(NEOPIXEL_HW_PORT_71_GPIO) || defined(I2C_HW_PORT_71_GPIO) || defined(UART_HW_PORT_71_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_71_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_71_GPIO))
        #if defined(NEOPIXEL_HW_PORT_71_LABEL) || defined(I2C_HW_PORT_71_LABEL) || defined(UART_HW_PORT_71_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_71_LABEL = TO_STRING(FEATURE_DEFINE(PORT_71_LABEL))
        #else
        HW_GPIO_PORT_71_LABEL = "Port 71"
        #endif
    #endif

    // Port 72
    #if defined(NEOPIXEL_HW_PORT_72_GPIO) || defined(I2C_HW_PORT_72_GPIO) || defined(UART_HW_PORT_72_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_72_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_72_GPIO))
        #if defined(NEOPIXEL_HW_PORT_72_LABEL) || defined(I2C_HW_PORT_72_LABEL) || defined(UART_HW_PORT_72_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_72_LABEL = TO_STRING(FEATURE_DEFINE(PORT_72_LABEL))
        #else
        HW_GPIO_PORT_72_LABEL = "Port 72"
        #endif
    #endif

    // Port 73
    #if defined(NEOPIXEL_HW_PORT_73_GPIO) || defined(I2C_HW_PORT_73_GPIO) || defined(UART_HW_PORT_73_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_73_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_73_GPIO))
        #if defined(NEOPIXEL_HW_PORT_73_LABEL) || defined(I2C_HW_PORT_73_LABEL) || defined(UART_HW_PORT_73_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_73_LABEL = TO_STRING(FEATURE_DEFINE(PORT_73_LABEL))
        #else
        HW_GPIO_PORT_73_LABEL = "Port 73"
        #endif
    #endif

    // Port 74
    #if defined(NEOPIXEL_HW_PORT_74_GPIO) || defined(I2C_HW_PORT_74_GPIO) || defined(UART_HW_PORT_74_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_74_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_74_GPIO))
        #if defined(NEOPIXEL_HW_PORT_74_LABEL) || defined(I2C_HW_PORT_74_LABEL) || defined(UART_HW_PORT_74_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_74_LABEL = TO_STRING(FEATURE_DEFINE(PORT_74_LABEL))
        #else
        HW_GPIO_PORT_74_LABEL = "Port 74"
        #endif
    #endif

    // Port 75
    #if defined(NEOPIXEL_HW_PORT_75_GPIO) || defined(I2C_HW_PORT_75_GPIO) || defined(UART_HW_PORT_75_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_75_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_75_GPIO))
        #if defined(NEOPIXEL_HW_PORT_75_LABEL) || defined(I2C_HW_PORT_75_LABEL) || defined(UART_HW_PORT_75_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_75_LABEL = TO_STRING(FEATURE_DEFINE(PORT_75_LABEL))
        #else
        HW_GPIO_PORT_75_LABEL = "Port 75"
        #endif
    #endif

    // Port 76
    #if defined(NEOPIXEL_HW_PORT_76_GPIO) || defined(I2C_HW_PORT_76_GPIO) || defined(UART_HW_PORT_76_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_76_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_76_GPIO))
        #if defined(NEOPIXEL_HW_PORT_76_LABEL) || defined(I2C_HW_PORT_76_LABEL) || defined(UART_HW_PORT_76_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_76_LABEL = TO_STRING(FEATURE_DEFINE(PORT_76_LABEL))
        #else
        HW_GPIO_PORT_76_LABEL = "Port 76"
        #endif
    #endif

    // Port 77
    #if defined(NEOPIXEL_HW_PORT_77_GPIO) || defined(I2C_HW_PORT_77_GPIO) || defined(UART_HW_PORT_77_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_77_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_77_GPIO))
        #if defined(NEOPIXEL_HW_PORT_77_LABEL) || defined(I2C_HW_PORT_77_LABEL) || defined(UART_HW_PORT_77_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_77_LABEL = TO_STRING(FEATURE_DEFINE(PORT_77_LABEL))
        #else
        HW_GPIO_PORT_77_LABEL = "Port 77"
        #endif
    #endif

    // Port 78
    #if defined(NEOPIXEL_HW_PORT_78_GPIO) || defined(I2C_HW_PORT_78_GPIO) || defined(UART_HW_PORT_78_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_78_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_78_GPIO))
        #if defined(NEOPIXEL_HW_PORT_78_LABEL) || defined(I2C_HW_PORT_78_LABEL) || defined(UART_HW_PORT_78_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_78_LABEL = TO_STRING(FEATURE_DEFINE(PORT_78_LABEL))
        #else
        HW_GPIO_PORT_78_LABEL = "Port 78"
        #endif
    #endif

    // Port 79
    #if defined(NEOPIXEL_HW_PORT_79_GPIO) || defined(I2C_HW_PORT_79_GPIO) || defined(UART_HW_PORT_79_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_79_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_79_GPIO))
        #if defined(NEOPIXEL_HW_PORT_79_LABEL) || defined(I2C_HW_PORT_79_LABEL) || defined(UART_HW_PORT_79_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_79_LABEL = TO_STRING(FEATURE_DEFINE(PORT_79_LABEL))
        #else
        HW_GPIO_PORT_79_LABEL = "Port 79"
        #endif
    #endif

    // Port 80
    #if defined(NEOPIXEL_HW_PORT_80_GPIO) || defined(I2C_HW_PORT_80_GPIO) || defined(UART_HW_PORT_80_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_80_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_80_GPIO))
        #if defined(NEOPIXEL_HW_PORT_80_LABEL) || defined(I2C_HW_PORT_80_LABEL) || defined(UART_HW_PORT_80_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_80_LABEL = TO_STRING(FEATURE_DEFINE(PORT_80_LABEL))
        #else
        HW_GPIO_PORT_80_LABEL = "Port 80"
        #endif
    #endif

    // Port 81
    #if defined(NEOPIXEL_HW_PORT_81_GPIO) || defined(I2C_HW_PORT_81_GPIO) || defined(UART_HW_PORT_81_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_81_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_81_GPIO))
        #if defined(NEOPIXEL_HW_PORT_81_LABEL) || defined(I2C_HW_PORT_81_LABEL) || defined(UART_HW_PORT_81_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_81_LABEL = TO_STRING(FEATURE_DEFINE(PORT_81_LABEL))
        #else
        HW_GPIO_PORT_81_LABEL = "Port 81"
        #endif
    #endif

    // Port 82
    #if defined(NEOPIXEL_HW_PORT_82_GPIO) || defined(I2C_HW_PORT_82_GPIO) || defined(UART_HW_PORT_82_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_82_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_82_GPIO))
        #if defined(NEOPIXEL_HW_PORT_82_LABEL) || defined(I2C_HW_PORT_82_LABEL) || defined(UART_HW_PORT_82_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_82_LABEL = TO_STRING(FEATURE_DEFINE(PORT_82_LABEL))
        #else
        HW_GPIO_PORT_82_LABEL = "Port 82"
        #endif
    #endif

    // Port 83
    #if defined(NEOPIXEL_HW_PORT_83_GPIO) || defined(I2C_HW_PORT_83_GPIO) || defined(UART_HW_PORT_83_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_83_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_83_GPIO))
        #if defined(NEOPIXEL_HW_PORT_83_LABEL) || defined(I2C_HW_PORT_83_LABEL) || defined(UART_HW_PORT_83_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_83_LABEL = TO_STRING(FEATURE_DEFINE(PORT_83_LABEL))
        #else
        HW_GPIO_PORT_83_LABEL = "Port 83"
        #endif
    #endif

    // Port 84
    #if defined(NEOPIXEL_HW_PORT_84_GPIO) || defined(I2C_HW_PORT_84_GPIO) || defined(UART_HW_PORT_84_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_84_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_84_GPIO))
        #if defined(NEOPIXEL_HW_PORT_84_LABEL) || defined(I2C_HW_PORT_84_LABEL) || defined(UART_HW_PORT_84_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_84_LABEL = TO_STRING(FEATURE_DEFINE(PORT_84_LABEL))
        #else
        HW_GPIO_PORT_84_LABEL = "Port 84"
        #endif
    #endif

    // Port 85
    #if defined(NEOPIXEL_HW_PORT_85_GPIO) || defined(I2C_HW_PORT_85_GPIO) || defined(UART_HW_PORT_85_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_85_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_85_GPIO))
        #if defined(NEOPIXEL_HW_PORT_85_LABEL) || defined(I2C_HW_PORT_85_LABEL) || defined(UART_HW_PORT_85_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_85_LABEL = TO_STRING(FEATURE_DEFINE(PORT_85_LABEL))
        #else
        HW_GPIO_PORT_85_LABEL = "Port 85"
        #endif
    #endif

    // Port 86
    #if defined(NEOPIXEL_HW_PORT_86_GPIO) || defined(I2C_HW_PORT_86_GPIO) || defined(UART_HW_PORT_86_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_86_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_86_GPIO))
        #if defined(NEOPIXEL_HW_PORT_86_LABEL) || defined(I2C_HW_PORT_86_LABEL) || defined(UART_HW_PORT_86_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_86_LABEL = TO_STRING(FEATURE_DEFINE(PORT_86_LABEL))
        #else
        HW_GPIO_PORT_86_LABEL = "Port 86"
        #endif
    #endif

    // Port 87
    #if defined(NEOPIXEL_HW_PORT_87_GPIO) || defined(I2C_HW_PORT_87_GPIO) || defined(UART_HW_PORT_87_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_87_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_87_GPIO))
        #if defined(NEOPIXEL_HW_PORT_87_LABEL) || defined(I2C_HW_PORT_87_LABEL) || defined(UART_HW_PORT_87_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_87_LABEL = TO_STRING(FEATURE_DEFINE(PORT_87_LABEL))
        #else
        HW_GPIO_PORT_87_LABEL = "Port 87"
        #endif
    #endif

    // Port 88
    #if defined(NEOPIXEL_HW_PORT_88_GPIO) || defined(I2C_HW_PORT_88_GPIO) || defined(UART_HW_PORT_88_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_88_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_88_GPIO))
        #if defined(NEOPIXEL_HW_PORT_88_LABEL) || defined(I2C_HW_PORT_88_LABEL) || defined(UART_HW_PORT_88_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_88_LABEL = TO_STRING(FEATURE_DEFINE(PORT_88_LABEL))
        #else
        HW_GPIO_PORT_88_LABEL = "Port 88"
        #endif
    #endif

    // Port 89
    #if defined(NEOPIXEL_HW_PORT_89_GPIO) || defined(I2C_HW_PORT_89_GPIO) || defined(UART_HW_PORT_89_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_89_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_89_GPIO))
        #if defined(NEOPIXEL_HW_PORT_89_LABEL) || defined(I2C_HW_PORT_89_LABEL) || defined(UART_HW_PORT_89_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_89_LABEL = TO_STRING(FEATURE_DEFINE(PORT_89_LABEL))
        #else
        HW_GPIO_PORT_89_LABEL = "Port 89"
        #endif
    #endif

    // Port 90
    #if defined(NEOPIXEL_HW_PORT_90_GPIO) || defined(I2C_HW_PORT_90_GPIO) || defined(UART_HW_PORT_90_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_90_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_90_GPIO))
        #if defined(NEOPIXEL_HW_PORT_90_LABEL) || defined(I2C_HW_PORT_90_LABEL) || defined(UART_HW_PORT_90_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_90_LABEL = TO_STRING(FEATURE_DEFINE(PORT_90_LABEL))
        #else
        HW_GPIO_PORT_90_LABEL = "Port 90"
        #endif
    #endif

    // Port 91
    #if defined(NEOPIXEL_HW_PORT_91_GPIO) || defined(I2C_HW_PORT_91_GPIO) || defined(UART_HW_PORT_91_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_91_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_91_GPIO))
        #if defined(NEOPIXEL_HW_PORT_91_LABEL) || defined(I2C_HW_PORT_91_LABEL) || defined(UART_HW_PORT_91_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_91_LABEL = TO_STRING(FEATURE_DEFINE(PORT_91_LABEL))
        #else
        HW_GPIO_PORT_91_LABEL = "Port 91"
        #endif
    #endif

    // Port 92
    #if defined(NEOPIXEL_HW_PORT_92_GPIO) || defined(I2C_HW_PORT_92_GPIO) || defined(UART_HW_PORT_92_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_92_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_92_GPIO))
        #if defined(NEOPIXEL_HW_PORT_92_LABEL) || defined(I2C_HW_PORT_92_LABEL) || defined(UART_HW_PORT_92_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_92_LABEL = TO_STRING(FEATURE_DEFINE(PORT_92_LABEL))
        #else
        HW_GPIO_PORT_92_LABEL = "Port 92"
        #endif
    #endif

    // Port 93
    #if defined(NEOPIXEL_HW_PORT_93_GPIO) || defined(I2C_HW_PORT_93_GPIO) || defined(UART_HW_PORT_93_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_93_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_93_GPIO))
        #if defined(NEOPIXEL_HW_PORT_93_LABEL) || defined(I2C_HW_PORT_93_LABEL) || defined(UART_HW_PORT_93_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_93_LABEL = TO_STRING(FEATURE_DEFINE(PORT_93_LABEL))
        #else
        HW_GPIO_PORT_93_LABEL = "Port 93"
        #endif
    #endif

    // Port 94
    #if defined(NEOPIXEL_HW_PORT_94_GPIO) || defined(I2C_HW_PORT_94_GPIO) || defined(UART_HW_PORT_94_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_94_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_94_GPIO))
        #if defined(NEOPIXEL_HW_PORT_94_LABEL) || defined(I2C_HW_PORT_94_LABEL) || defined(UART_HW_PORT_94_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_94_LABEL = TO_STRING(FEATURE_DEFINE(PORT_94_LABEL))
        #else
        HW_GPIO_PORT_94_LABEL = "Port 94"
        #endif
    #endif

    // Port 95
    #if defined(NEOPIXEL_HW_PORT_95_GPIO) || defined(I2C_HW_PORT_95_GPIO) || defined(UART_HW_PORT_95_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_95_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_95_GPIO))
        #if defined(NEOPIXEL_HW_PORT_95_LABEL) || defined(I2C_HW_PORT_95_LABEL) || defined(UART_HW_PORT_95_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_95_LABEL = TO_STRING(FEATURE_DEFINE(PORT_95_LABEL))
        #else
        HW_GPIO_PORT_95_LABEL = "Port 95"
        #endif
    #endif

    // Port 96
    #if defined(NEOPIXEL_HW_PORT_96_GPIO) || defined(I2C_HW_PORT_96_GPIO) || defined(UART_HW_PORT_96_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_96_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_96_GPIO))
        #if defined(NEOPIXEL_HW_PORT_96_LABEL) || defined(I2C_HW_PORT_96_LABEL) || defined(UART_HW_PORT_96_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_96_LABEL = TO_STRING(FEATURE_DEFINE(PORT_96_LABEL))
        #else
        HW_GPIO_PORT_96_LABEL = "Port 96"
        #endif
    #endif

    // Port 97
    #if defined(NEOPIXEL_HW_PORT_97_GPIO) || defined(I2C_HW_PORT_97_GPIO) || defined(UART_HW_PORT_97_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_97_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_97_GPIO))
        #if defined(NEOPIXEL_HW_PORT_97_LABEL) || defined(I2C_HW_PORT_97_LABEL) || defined(UART_HW_PORT_97_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_97_LABEL = TO_STRING(FEATURE_DEFINE(PORT_97_LABEL))
        #else
        HW_GPIO_PORT_97_LABEL = "Port 97"
        #endif
    #endif

    // Port 98
    #if defined(NEOPIXEL_HW_PORT_98_GPIO) || defined(I2C_HW_PORT_98_GPIO) || defined(UART_HW_PORT_98_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_GPIO_PORT_98_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_98_GPIO))
        #if defined(NEOPIXEL_HW_PORT_98_LABEL) || defined(I2C_HW_PORT_98_LABEL) || defined(UART_HW_PORT_98_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_GPIO_PORT_98_LABEL = TO_STRING(FEATURE_DEFINE(PORT_98_LABEL))
        #else
        HW_GPIO_PORT_98_LABEL = "Port 98"
        #endif
    #endif

    // Port 99
    #if defined(NEOPIXEL_HW_PORT_99_GPIO) || defined(I2C_HW_PORT_99_GPIO) || defined(UART_HW_PORT_99_GPIO)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        HW_GPIO_PORT_99_GPIO_DATA = EXPAND(FEATURE_DEFINE(PORT_99_GPIO))
        #if defined(NEOPIXEL_HW_PORT_99_LABEL) || defined(I2C_HW_PORT_99_LABEL) || defined(UART_HW_PORT_99_LABEL)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            HW_GPIO_PORT_99_LABEL = TO_STRING(FEATURE_DEFINE(PORT_99_LABEL))
        #else
        HW_GPIO_PORT_99_LABEL = "Port 99"
        #endif
    #endif

#else
        HW_GPIO_PORTS_DEFINED = 0 HW_GPIO_PORT_COUNT = 0
#endif

// Extract Network capability
#ifdef NEOPIXEL_HW_HAS_NETWORK
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                HW_HAS_NETWORK = EXPAND(NEOPIXEL_HW_HAS_NETWORK)
#else
    HW_HAS_NETWORK = 0
#endif

// External Relays Support (0=None, 1-4=Count, default=2 if not defined)
#ifdef NEOPIXEL_EXT_RELAIS
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    HW_EXT_RELAIS = EXPAND(NEOPIXEL_EXT_RELAIS)
#else
    HW_EXT_RELAIS = 0
#endif
