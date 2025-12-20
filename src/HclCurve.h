#pragma once

#include <Arduino.h>
#include <string>

#include "OpenKNX.h"
#include "knxprod.h"
#include "Timer.h" // from OFM-LogicModule instead of TimerModule
#include "colorhelper.h"

class HclCurve
{
  public:
    void setup(uint8_t index);
    void loop();

  private:
    const std::string logPrefix();
    const uint8_t channelIndex();
    uint8_t _index = 0;
    bool _isConfigured = false;
    uint8_t _type = 0;
    uint16_t _lastKelvinValue = 0; // For debouncing KO writes
    uint8_t _lastMinute = 255;     // For update rate limiting (255 = invalid)
};