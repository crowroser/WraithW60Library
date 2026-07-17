#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace wraith {

struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    Color() = default;
    Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

    bool operator==(const Color& o) const { return r == o.r && g == o.g && b == o.b; }
    bool operator!=(const Color& o) const { return !(*this == o); }

    static Color fromHSV(uint16_t h, uint8_t s, uint8_t v);
    void toHSV(uint16_t& h, uint8_t& s, uint8_t& v) const;
};

struct KeyDefinition {
    uint8_t index;          // Logical index (0-78)
    uint8_t physicalIndex;  // Physical LED index (0-125)
    const char* name;
    const char* label;
    uint8_t row;
    uint8_t column;
};

class KeyLayout {
public:
    static const std::vector<KeyDefinition>& getStandardLayout();
    static const KeyDefinition* getKeyByName(const std::string& name);
    static const KeyDefinition* getKeyByIndex(uint8_t index);
    static uint8_t getTotalKeys();
    static uint8_t getKeyIndex(uint8_t row, uint8_t column);
};

enum class ZoneType {
    Backlight,
    Underglow,
    PerKey
};

struct ZoneDefinition {
    ZoneType type;
    const char* name;
    const char* displayName;
    uint8_t ledCount;
    bool supportsMode;
    bool supportsColor;
    bool supportsSpeed;
};

class ZoneRegistry {
public:
    static const std::vector<ZoneDefinition>& getZones();
    static const ZoneDefinition* getZone(ZoneType type);
};

} // namespace wraith
