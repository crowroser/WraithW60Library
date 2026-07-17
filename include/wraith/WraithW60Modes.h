#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace wraith {

enum class LightingMode : uint8_t {
    Static = 0x00,
    Breathe = 0x01,
    Wave = 0x02,
    Neon = 0x03,
    Sparkle = 0x04,
    Mod6 = 0x06,
    Reactive = 0x07,
    Mod8 = 0x08,
    Mod9 = 0x09,
    Ripple = 0x0A,
    Mod11 = 0x0B,
    Mod12 = 0x0C,
    Mod14 = 0x0E,
    Mod15 = 0x0F,
    Mod16 = 0x10
};

struct ModeInfo {
    LightingMode mode;
    const char* name;
    const char* displayName;
    bool supportsColor;
    bool supportsSpeed;
    bool isReactive;
};

class ModeRegistry {
public:
    static const ModeInfo& getModeInfo(LightingMode mode);
    static const std::vector<ModeInfo>& getAllModes();
    static ModeInfo findByName(const std::string& name);
    static bool isValidMode(uint8_t value);
    static uint8_t toByte(LightingMode mode);
    static LightingMode fromByte(uint8_t value);
};

} // namespace wraith
