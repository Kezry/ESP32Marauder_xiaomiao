#line 1 "/home/runner/work/ESP32Marauder_xiaomiao/ESP32Marauder_xiaomiao/esp32_marauder/xiaoLED.h"
#pragma once

#ifndef xiaoLED_H
#define xiaoLED_H

#include "configs.h"
#include "settings.h"

#include <Arduino.h>

#define XIAO_LED_PIN 21

extern Settings settings_obj;

class xiaoLED {

    public:
        void RunSetup();
        void main();
        void attackLED();
        void sniffLED();
        void offLED();
};

#endif  /* xiaoLED_H */