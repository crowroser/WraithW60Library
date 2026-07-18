#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "WraithW60Leds.h"
#include "WraithW60Modes.h"

namespace wraith {

struct DeviceState {
    LightingMode backlightMode = LightingMode::Static;
    Color backlightColor;
    uint8_t backlightSpeed = 0x80;

    LightingMode underglowMode = LightingMode::Static;
    Color underglowColor;
    uint8_t underglowSpeed = 0x80;

    Color powerBarColor;

    bool socdEnabled = false;

    uint8_t rtUpThreshold = 0x55;
    uint8_t rtDownThreshold = 0x19;

    std::vector<Color> perKeyColors;
};

struct DeviceConfig {
    uint16_t vid = 0x2E3C;
    uint16_t pid = 0xC365;
    uint8_t interface = 1;
    uint8_t endpoint = 0x04;
    uint32_t timeoutMs = 1000;
    bool autoReconnect = true;
    uint8_t retryCount = 3;
};

struct WraithW60DeviceInfo {
    uint16_t vid = 0x2E3C;
    uint16_t pid = 0xC365;
    std::string serial;
    std::string manufacturer = "Wraith";
    std::string product = "W60 Keyboard";
    std::string path;
    bool isOpen = false;
};

} // namespace wraith
