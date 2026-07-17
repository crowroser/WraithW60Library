#include "wraith/WraithW60Leds.h"

namespace wraith {

static const KeyDefinition s_standardLayout[] = {
    // Row 0: Function row
    {  0, "esc",         "Esc",     0, 0 },
    {  1, "f1",          "F1",      0, 1 },
    {  2, "f2",          "F2",      0, 2 },
    {  3, "f3",          "F3",      0, 3 },
    {  4, "f4",          "F4",      0, 4 },
    {  5, "f5",          "F5",      0, 5 },
    {  6, "f6",          "F6",      0, 6 },
    {  7, "f7",          "F7",      0, 7 },
    {  8, "f8",          "F8",      0, 8 },
    {  9, "f9",          "F9",      0, 9 },
    { 10, "f10",         "F10",     0, 10 },
    { 11, "f11",         "F11",     0, 11 },
    { 12, "f12",         "F12",     0, 12 },
    { 13, "delete",      "Del",     0, 13 },
    // Row 1: Number row
    { 14, "grave",       "`",       1, 0 },
    { 15, "1",           "1",       1, 1 },
    { 16, "2",           "2",       1, 2 },
    { 17, "3",           "3",       1, 3 },
    { 18, "4",           "4",       1, 4 },
    { 19, "5",           "5",       1, 5 },
    { 20, "6",           "6",       1, 6 },
    { 21, "7",           "7",       1, 7 },
    { 22, "8",           "8",       1, 8 },
    { 23, "9",           "9",       1, 9 },
    { 24, "0",           "0",       1, 10 },
    { 25, "minus",       "-",       1, 11 },
    { 26, "equal",       "=",       1, 12 },
    { 27, "backspace",   "Bksp",    1, 13 },
    // Row 2: QWERTY
    { 28, "tab",         "Tab",     2, 0 },
    { 29, "q",           "Q",       2, 1 },
    { 30, "w",           "W",       2, 2 },
    { 31, "e",           "E",       2, 3 },
    { 32, "r",           "R",       2, 4 },
    { 33, "t",           "T",       2, 5 },
    { 34, "y",           "Y",       2, 6 },
    { 35, "u",           "U",       2, 7 },
    { 36, "i",           "I",       2, 8 },
    { 37, "o",           "O",       2, 9 },
    { 38, "p",           "P",       2, 10 },
    { 39, "lbracket",    "[",       2, 11 },
    { 40, "rbracket",    "]",       2, 12 },
    { 41, "backslash",   "\\",      2, 13 },
    // Row 3: Home row
    { 42, "capslock",    "Caps",    3, 0 },
    { 43, "a",           "A",       3, 1 },
    { 44, "s",           "S",       3, 2 },
    { 45, "d",           "D",       3, 3 },
    { 46, "f",           "F",       3, 4 },
    { 47, "g",           "G",       3, 5 },
    { 48, "h",           "H",       3, 6 },
    { 49, "j",           "J",       3, 7 },
    { 50, "k",           "K",       3, 8 },
    { 51, "l",           "L",       3, 9 },
    { 52, "semicolon",   ";",       3, 10 },
    { 53, "quote",       "'",       3, 11 },
    { 54, "enter",       "Enter",   3, 12 },
    // Row 4: Bottom row
    { 55, "lshift",      "LShift",  4, 0 },
    { 56, "z",           "Z",       4, 1 },
    { 57, "x",           "X",       4, 2 },
    { 58, "c",           "C",       4, 3 },
    { 59, "v",           "V",       4, 4 },
    { 60, "b",           "B",       4, 5 },
    { 61, "n",           "N",       4, 6 },
    { 62, "m",           "M",       4, 7 },
    { 63, "comma",       ",",       4, 8 },
    { 64, "period",      ".",       4, 9 },
    { 65, "slash",       "/",       4, 10 },
    { 66, "rshift",      "RShift",  4, 11 },
    { 67, "up",          "Up",      4, 12 },
    // Row 5: Space row
    { 68, "lctrl",       "LCtrl",   5, 0 },
    { 69, "lwin",        "LWin",    5, 1 },
    { 70, "lalt",        "LAlt",    5, 2 },
    { 71, "space",       "Space",   5, 3 },
    { 72, "ralt",        "RAlt",    5, 4 },
    { 73, "fn",          "Fn",      5, 5 },
    { 74, "menu",        "Menu",    5, 6 },
    { 75, "rctrl",       "RCtrl",   5, 7 },
    { 76, "left",        "Left",    5, 8 },
    { 77, "down",        "Down",    5, 9 },
    { 78, "right",       "Right",   5, 10 },
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
