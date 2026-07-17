#include "wraith/WraithW60Modes.h"

namespace wraith {

static const ModeInfo s_allModes[] = {
    { LightingMode::Static,    "static",    "Static",    true,  false, false },
    { LightingMode::Breathe,   "breathe",   "Breathe",   true,  true,  false },
    { LightingMode::Wave,      "wave",      "Wave",      true,  true,  false },
    { LightingMode::Neon,      "neon",      "Neon",      true,  true,  false },
    { LightingMode::Sparkle,   "sparkle",   "Sparkle",   true,  true,  false },
    { LightingMode::Mod6,      "mod6",      "Mode 6",    true,  true,  false },
    { LightingMode::Reactive,  "reactive",  "Reactive",  true,  true,  true  },
    { LightingMode::Mod8,      "mod8",      "Mode 8",    true,  true,  false },
    { LightingMode::Mod9,      "mod9",      "Mode 9",    true,  true,  false },
    { LightingMode::Ripple,    "ripple",    "Ripple",    true,  true,  false },
    { LightingMode::Mod11,     "mod11",     "Mode 11",   true,  true,  false },
    { LightingMode::Mod12,     "mod12",     "Mode 12",   true,  true,  false },
    { LightingMode::Mod14,     "mod14",     "Mode 14",   true,  true,  false },
    { LightingMode::Mod15,     "mod15",     "Mode 15",   true,  true,  false },
    { LightingMode::Mod16,     "mod16",     "Mode 16",   true,  true,  false },
};

static const size_t s_modeCount = sizeof(s_allModes) / sizeof(s_allModes[0]);

const ModeInfo& ModeRegistry::getModeInfo(LightingMode mode) {
    for (size_t i = 0; i < s_modeCount; ++i) {
        if (s_allModes[i].mode == mode) return s_allModes[i];
    }
    return s_allModes[0];
}

const std::vector<ModeInfo>& ModeRegistry::getAllModes() {
    static std::vector<ModeInfo> modes(s_allModes, s_allModes + s_modeCount);
    return modes;
}

ModeInfo ModeRegistry::findByName(const std::string& name) {
    for (size_t i = 0; i < s_modeCount; ++i) {
        if (s_allModes[i].name == name) return s_allModes[i];
    }
    return s_allModes[0];
}

bool ModeRegistry::isValidMode(uint8_t value) {
    for (size_t i = 0; i < s_modeCount; ++i) {
        if (static_cast<uint8_t>(s_allModes[i].mode) == value) return true;
    }
    return false;
}

uint8_t ModeRegistry::toByte(LightingMode mode) {
    return static_cast<uint8_t>(mode);
}

LightingMode ModeRegistry::fromByte(uint8_t value) {
    return static_cast<LightingMode>(value);
}

} // namespace wraith
