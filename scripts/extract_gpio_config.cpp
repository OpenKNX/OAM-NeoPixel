/**
 * @file extract_gpio_config.cpp
 * @brief Build-time C++ tool to extract hardware GPIO configuration via preprocessor
 * @author Erkan Çolak
 * 
 * This tool reads hardware.h defines and outputs them as KEY=VALUE pairs
 * for consumption by PowerShell build scripts.
 * 
 * Usage: ./extract_gpio_config [feature_prefix]
 *   feature_prefix: Default is "NEOPIXEL_HW" (e.g., for other features: "I2C_HW", "UART_HW")
 */

#include <iostream>
#include <cstdio>

// Include hardware config to expand GPIO macros (OKNXHW_OPENKNXIAO_D5 -> 7)
#include "HardwareConfig.h"

// Macro stringification
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

// Feature prefix (can be overridden at compile time)
#ifndef FEATURE_PREFIX
#define FEATURE_PREFIX NEOPIXEL_HW
#endif

// Build macro names dynamically (two-level expansion required for ## to work with macros)
#define CONCAT_IMPL(a, b) a##_##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define FEATURE_DEFINE(suffix) CONCAT(FEATURE_PREFIX, suffix)

int main(int argc, char* argv[])
{
    // Allow runtime override of feature prefix (for multi-purpose use)
    const char* featurePrefix = (argc > 1) ? argv[1] : TO_STRING(FEATURE_PREFIX);
    
    std::cout << "# GPIO Configuration Extractor" << std::endl;
    std::cout << "# Feature Prefix: " << featurePrefix << std::endl;
    std::cout << "# ========================================" << std::endl;

    // Feature detection: Check if GPIO ports are defined for this feature
    // Note: We use the expanded macro name directly since #ifdef can't use FEATURE_DEFINE()
    #if defined(NEOPIXEL_HW_GPIO_PORTS) || defined(I2C_HW_GPIO_PORTS) || defined(UART_HW_GPIO_PORTS)
        std::cout << "HW_GPIO_PORTS_DEFINED=1" << std::endl;
        
        // Port count - use FEATURE_DEFINE which works in value context
        #if defined(NEOPIXEL_HW_PORT_COUNT) || defined(I2C_HW_PORT_COUNT) || defined(UART_HW_PORT_COUNT)
            std::cout << "HW_GPIO_PORT_COUNT=" << FEATURE_DEFINE(PORT_COUNT) << std::endl;
        #else
            std::cout << "HW_GPIO_PORT_COUNT=0" << std::endl;
        #endif
        
        // Port 1
        #if defined(NEOPIXEL_HW_PORT_1_GPIO) || defined(I2C_HW_PORT_1_GPIO) || defined(UART_HW_PORT_1_GPIO)
            std::cout << "HW_GPIO_PORT_1_GPIO_DATA=" << FEATURE_DEFINE(PORT_1_GPIO) << std::endl;
            #if defined(NEOPIXEL_HW_PORT_1_LABEL) || defined(I2C_HW_PORT_1_LABEL) || defined(UART_HW_PORT_1_LABEL)
                std::cout << "HW_GPIO_PORT_1_LABEL=" << TO_STRING(FEATURE_DEFINE(PORT_1_LABEL)) << std::endl;
            #else
                std::cout << "HW_GPIO_PORT_1_LABEL=Port 1" << std::endl;
            #endif
        #endif
        
        // Port 2
        #if defined(NEOPIXEL_HW_PORT_2_GPIO) || defined(I2C_HW_PORT_2_GPIO) || defined(UART_HW_PORT_2_GPIO)
            std::cout << "HW_GPIO_PORT_2_GPIO_DATA=" << FEATURE_DEFINE(PORT_2_GPIO) << std::endl;
            #if defined(NEOPIXEL_HW_PORT_2_LABEL) || defined(I2C_HW_PORT_2_LABEL) || defined(UART_HW_PORT_2_LABEL)
                std::cout << "HW_GPIO_PORT_2_LABEL=" << TO_STRING(FEATURE_DEFINE(PORT_2_LABEL)) << std::endl;
            #else
                std::cout << "HW_GPIO_PORT_2_LABEL=Port 2" << std::endl;
            #endif
        #endif
        
        // Port 3
        #if defined(NEOPIXEL_HW_PORT_3_GPIO) || defined(I2C_HW_PORT_3_GPIO) || defined(UART_HW_PORT_3_GPIO)
            std::cout << "HW_GPIO_PORT_3_GPIO_DATA=" << FEATURE_DEFINE(PORT_3_GPIO) << std::endl;
            #if defined(NEOPIXEL_HW_PORT_3_LABEL) || defined(I2C_HW_PORT_3_LABEL) || defined(UART_HW_PORT_3_LABEL)
                std::cout << "HW_GPIO_PORT_3_LABEL=" << TO_STRING(FEATURE_DEFINE(PORT_3_LABEL)) << std::endl;
            #else
                std::cout << "HW_GPIO_PORT_3_LABEL=Port 3" << std::endl;
            #endif
        #endif
        
        // Port 4
        #if defined(NEOPIXEL_HW_PORT_4_GPIO) || defined(I2C_HW_PORT_4_GPIO) || defined(UART_HW_PORT_4_GPIO)
            std::cout << "HW_GPIO_PORT_4_GPIO_DATA=" << FEATURE_DEFINE(PORT_4_GPIO) << std::endl;
            #if defined(NEOPIXEL_HW_PORT_4_LABEL) || defined(I2C_HW_PORT_4_LABEL) || defined(UART_HW_PORT_4_LABEL)
                std::cout << "HW_GPIO_PORT_4_LABEL=" << TO_STRING(FEATURE_DEFINE(PORT_4_LABEL)) << std::endl;
            #else
                std::cout << "HW_GPIO_PORT_4_LABEL=Port 4" << std::endl;
            #endif
        #endif
        
        // Port 5
        #if defined(NEOPIXEL_HW_PORT_5_GPIO) || defined(I2C_HW_PORT_5_GPIO) || defined(UART_HW_PORT_5_GPIO)
            std::cout << "HW_GPIO_PORT_5_GPIO_DATA=" << FEATURE_DEFINE(PORT_5_GPIO) << std::endl;
            #if defined(NEOPIXEL_HW_PORT_5_LABEL) || defined(I2C_HW_PORT_5_LABEL) || defined(UART_HW_PORT_5_LABEL)
                std::cout << "HW_GPIO_PORT_5_LABEL=" << TO_STRING(FEATURE_DEFINE(PORT_5_LABEL)) << std::endl;
            #else
                std::cout << "HW_GPIO_PORT_5_LABEL=Port 5" << std::endl;
            #endif
        #endif
        
        // Port 6
        #if defined(NEOPIXEL_HW_PORT_6_GPIO) || defined(I2C_HW_PORT_6_GPIO) || defined(UART_HW_PORT_6_GPIO)
            std::cout << "HW_GPIO_PORT_6_GPIO_DATA=" << FEATURE_DEFINE(PORT_6_GPIO) << std::endl;
            #if defined(NEOPIXEL_HW_PORT_6_LABEL) || defined(I2C_HW_PORT_6_LABEL) || defined(UART_HW_PORT_6_LABEL)
                std::cout << "HW_GPIO_PORT_6_LABEL=" << TO_STRING(FEATURE_DEFINE(PORT_6_LABEL)) << std::endl;
            #else
                std::cout << "HW_GPIO_PORT_6_LABEL=Port 6" << std::endl;
            #endif
        #endif
        
        // Port 7
        #if defined(NEOPIXEL_HW_PORT_7_GPIO) || defined(I2C_HW_PORT_7_GPIO) || defined(UART_HW_PORT_7_GPIO)
            std::cout << "HW_GPIO_PORT_7_GPIO_DATA=" << FEATURE_DEFINE(PORT_7_GPIO) << std::endl;
            #if defined(NEOPIXEL_HW_PORT_7_LABEL) || defined(I2C_HW_PORT_7_LABEL) || defined(UART_HW_PORT_7_LABEL)
                std::cout << "HW_GPIO_PORT_7_LABEL=" << TO_STRING(FEATURE_DEFINE(PORT_7_LABEL)) << std::endl;
            #else
                std::cout << "HW_GPIO_PORT_7_LABEL=Port 7" << std::endl;
            #endif
        #endif
        
    #else
        // Feature not available on this hardware
        std::cout << "HW_GPIO_PORTS_DEFINED=0" << std::endl;
        std::cout << "HW_GPIO_PORT_COUNT=0" << std::endl;
    #endif
    
    std::cout << "# ========================================" << std::endl;
    std::cout << "# Extraction complete" << std::endl;
    
    return 0;
}
