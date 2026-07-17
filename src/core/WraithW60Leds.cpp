#include "wraith/WraithW60Leds.h"

namespace wraith {

// Physical LED index mapping for Wraith W60 (Turkish QWERTY layout)
// Verified via hardware testing. Gaps = underglow/indicator LEDs.
static const KeyDefinition s_standardLayout[] = {
    // Row 0: Number row
    {  0, 22, "esc",         "Esc",     0, 0 },
    {  1, 23, "1",           "1",       0, 1 },
    {  2, 24, "2",           "2",       0, 2 },
    {  3, 25, "3",           "3",       0, 3 },
    {  4, 26, "4",           "4",       0, 4 },
    {  5, 27, "5",           "5",       0, 5 },
    {  6, 28, "6",           "6",       0, 6 },
    {  7, 29, "7",           "7",       0, 7 },
    {  8, 30, "8",           "8",       0, 8 },
    {  9, 31, "9",           "9",       0, 9 },
    { 10, 32, "0",           "0",       0, 10 },
    { 11, 33, "minus",       "-",       0, 11 },
    { 12, 34, "equal",       "=",       0, 12 },
    { 13, 36, "backspace",   "Bksp",    0, 13 },
    // Row 1: QWERTY row
    { 14, 44, "tab",         "Tab",     1, 0 },
    { 15, 45, "q",           "Q",       1, 1 },
    { 16, 46, "w",           "W",       1, 2 },
    { 17, 47, "e",           "E",       1, 3 },
    { 18, 48, "r",           "R",       1, 4 },
    { 19, 49, "t",           "T",       1, 5 },
    { 20, 50, "y",           "Y",       1, 6 },
    { 21, 51, "u",           "U",       1, 7 },
    { 22, 52, "i",           "I",       1, 8 },
    { 23, 53, "o",           "O",       1, 9 },
    { 24, 54, "p",           "P",       1, 10 },
    { 25, 55, "g",           "Ð",       1, 11 },
    { 26, 56, "u",           "Ü",       1, 12 },
    // Row 2: Home row
    { 27, 66, "capslock",    "Caps",    2, 0 },
    { 28, 68, "a",           "A",       2, 1 },
    { 29, 69, "s",           "S",       2, 2 },
    { 30, 70, "d",           "D",       2, 3 },
    { 31, 71, "f",           "F",       2, 4 },
    { 32, 72, "g",           "G",       2, 5 },
    { 33, 73, "h",           "H",       2, 6 },
    { 34, 74, "j",           "J",       2, 7 },
    { 35, 75, "k",           "K",       2, 8 },
    { 36, 76, "l",           "L",       2, 9 },
    { 37, 77, "semicolon",   "Þ",       2, 10 },
    { 38, 78, "quote",       "Ý",       2, 11 },
    { 39, 80, "enter",       "Enter",   2, 12 },
    // Row 3: Shift row
    { 40, 88, "lshift",      "LShift",  3, 0 },
    { 41, 90, "z",           "Z",       3, 1 },
    { 42, 91, "x",           "X",       3, 2 },
    { 43, 92, "c",           "C",       3, 3 },
    { 44, 93, "v",           "V",       3, 4 },
    { 45, 94, "b",           "B",       3, 5 },
    { 46, 95, "n",           "N",       3, 6 },
    { 47, 96, "m",           "M",       3, 7 },
    { 48, 97, "comma",       "Ö",       3, 8 },
    { 49, 98, "period",      "Ç",       3, 9 },
    { 50, 99, "slash",       "/",       3, 10 },
    { 51, 100, "rshift",     "RShift",  3, 11 },
    // Row 4: Bottom row
    { 52, 110, "lctrl",      "LCtrl",   4, 0 },
    { 53, 111, "lwin",       "Win",     4, 1 },
    { 54, 112, "lalt",       "LAlt",    4, 2 },
    { 55, 116, "space",      "Space",   4, 3 },
    { 56, 119, "ralt",       "RAlt",    4, 4 },
    { 57, 120, "menu",       "Menu",    4, 5 },
    { 58, 121, "rctrl",      "RCtrl",   4, 6 },
    { 59, 122, "fn",         "Fn",      4, 7 },
};

static const size_t s_layoutCount = sizeof(s_standardLayout) / sizeof(s_standardLayout[0]);
static const ZoneDefinition s_zones[] = {
    { ZoneType::Backlight, "backlight", "Backlight", 1,   true, true, false },
    { ZoneType::Underglow, "underglow", "Underglow", 1,   true, true, false },
    { ZoneType::PerKey,    "per-key",   "Per-Key",   126, false, true, false },
};

static const size_t s_zoneCount = sizeof(s_zones) / sizeof(s_zones[0]);

static const ZoneDefinition s_backlightZone = s_zones[0];
static const ZoneDefinition s_underglowZone = s_zones[1];
static const ZoneDefinition s_perKeyZone = s_zones[2];

Color Color::fromHSV(uint16_t h, uint8_t s, uint8_t v) {
    if (s == 0) return Color(v, v, v);

    h %= 360;
    uint8_t region = h / 60;
    uint8_t remainder = (h % 60) * 255 / 60;
    uint8_t p = v * (255 - s) / 255;
    uint8_t q = v * (255 - s * remainder / 255) / 255;
    uint8_t t = v * (255 - s * (255 - remainder) / 255) / 255;

    switch (region) {
        case 0:  return Color(v, t, p);
        case 1:  return Color(q, v, p);
        case 2:  return Color(p, v, t);
        case 3:  return Color(p, q, v);
        case 4:  return Color(t, p, v);
        default: return Color(v, p, q);
    }
}

void Color::toHSV(uint16_t& h, uint8_t& s, uint8_t& v) const {
    uint8_t max = (r > g) ? (r > b ? r : b) : (g > b ? g : b);
    uint8_t min = (r < g) ? (r < b ? r : b) : (g < b ? g : b);
    uint8_t delta = max - min;

    v = max;
    s = (max == 0) ? 0 : (delta * 255 / max);
    h = 0;

    if (delta != 0) {
        if (max == r)      h = 60 * (g - b) / delta;
        else if (max == g) h = 60 * (2 + (b - r) / delta);
        else               h = 60 * (4 + (r - g) / delta);
        if (h > 360) h -= 360;
    }
}

const std::vector<KeyDefinition>& KeyLayout::getStandardLayout() {
    static std::vector<KeyDefinition> layout(s_standardLayout, s_standardLayout + s_layoutCount);
    return layout;
}

const KeyDefinition* KeyLayout::getKeyByName(const std::string& name) {
    for (size_t i = 0; i < s_layoutCount; ++i) {
        if (s_standardLayout[i].name == name) return &s_standardLayout[i];
    }
    return nullptr;
}

const KeyDefinition* KeyLayout::getKeyByIndex(uint8_t index) {
    if (index < s_layoutCount) return &s_standardLayout[index];
    return nullptr;
}

uint8_t KeyLayout::getTotalKeys() {
    return static_cast<uint8_t>(s_layoutCount);
}

uint8_t KeyLayout::getKeyIndex(uint8_t row, uint8_t column) {
    for (size_t i = 0; i < s_layoutCount; ++i) {
        if (s_standardLayout[i].row == row && s_standardLayout[i].column == column)
            return s_standardLayout[i].index;
    }
    return 0xFF;
}

const std::vector<ZoneDefinition>& ZoneRegistry::getZones() {
    static std::vector<ZoneDefinition> zones(s_zones, s_zones + s_zoneCount);
    return zones;
}

const ZoneDefinition* ZoneRegistry::getZone(ZoneType type) {
    for (size_t i = 0; i < s_zoneCount; ++i) {
        if (s_zones[i].type == type) return &s_zones[i];
    }
    return nullptr;
}

} // namespace wraith
