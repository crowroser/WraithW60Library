#include "wraith/WraithW60Leds.h"

namespace wraith {

// Physical LED indices are 0xFF (unverified) except where noted:
// LCtrl=110, RCtrl=121, Menu=120, Fn=122 (verified via hardware testing)
static const KeyDefinition s_standardLayout[] = {
    // Row 0: Function row
    {  0, 0xFF, "esc",         "Esc",     0, 0 },
    {  1, 0xFF, "f1",          "F1",      0, 1 },
    {  2, 0xFF, "f2",          "F2",      0, 2 },
    {  3, 0xFF, "f3",          "F3",      0, 3 },
    {  4, 0xFF, "f4",          "F4",      0, 4 },
    {  5, 0xFF, "f5",          "F5",      0, 5 },
    {  6, 0xFF, "f6",          "F6",      0, 6 },
    {  7, 0xFF, "f7",          "F7",      0, 7 },
    {  8, 0xFF, "f8",          "F8",      0, 8 },
    {  9, 0xFF, "f9",          "F9",      0, 9 },
    { 10, 0xFF, "f10",         "F10",     0, 10 },
    { 11, 0xFF, "f11",         "F11",     0, 11 },
    { 12, 0xFF, "f12",         "F12",     0, 12 },
    { 13, 0xFF, "delete",      "Del",     0, 13 },
    // Row 1: Number row
    { 14, 0xFF, "grave",       "",       1, 0 },
    { 15, 0xFF, "1",           "1",       1, 1 },
    { 16, 0xFF, "2",           "2",       1, 2 },
    { 17, 0xFF, "3",           "3",       1, 3 },
    { 18, 0xFF, "4",           "4",       1, 4 },
    { 19, 0xFF, "5",           "5",       1, 5 },
    { 20, 0xFF, "6",           "6",       1, 6 },
    { 21, 0xFF, "7",           "7",       1, 7 },
    { 22, 0xFF, "8",           "8",       1, 8 },
    { 23, 0xFF, "9",           "9",       1, 9 },
    { 24, 0xFF, "0",           "0",       1, 10 },
    { 25, 0xFF, "minus",       "-",       1, 11 },
    { 26, 0xFF, "equal",       "=",       1, 12 },
    { 27, 0xFF, "backspace",   "Bksp",    1, 13 },
    // Row 2: QWERTY
    { 28, 0xFF, "tab",         "Tab",     2, 0 },
    { 29, 0xFF, "q",           "Q",       2, 1 },
    { 30, 0xFF, "w",           "W",       2, 2 },
    { 31, 0xFF, "e",           "E",       2, 3 },
    { 32, 0xFF, "r",           "R",       2, 4 },
    { 33, 0xFF, "t",           "T",       2, 5 },
    { 34, 0xFF, "y",           "Y",       2, 6 },
    { 35, 0xFF, "u",           "U",       2, 7 },
    { 36, 0xFF, "i",           "I",       2, 8 },
    { 37, 0xFF, "o",           "O",       2, 9 },
    { 38, 0xFF, "p",           "P",       2, 10 },
    { 39, 0xFF, "lbracket",    "[",       2, 11 },
    { 40, 0xFF, "rbracket",    "]",       2, 12 },
    { 41, 0xFF, "backslash",   "\\",      2, 13 },
    // Row 3: Home row
    { 42, 0xFF, "capslock",    "Caps",    3, 0 },
    { 43, 0xFF, "a",           "A",       3, 1 },
    { 44, 0xFF, "s",           "S",       3, 2 },
    { 45, 0xFF, "d",           "D",       3, 3 },
    { 46, 0xFF, "f",           "F",       3, 4 },
    { 47, 0xFF, "g",           "G",       3, 5 },
    { 48, 0xFF, "h",           "H",       3, 6 },
    { 49, 0xFF, "j",           "J",       3, 7 },
    { 50, 0xFF, "k",           "K",       3, 8 },
    { 51, 0xFF, "l",           "L",       3, 9 },
    { 52, 0xFF, "semicolon",   ";",       3, 10 },
    { 53, 0xFF, "quote",       "'",       3, 11 },
    { 54, 0xFF, "enter",       "Enter",   3, 12 },
    // Row 4: Shift row
    { 55, 0xFF, "lshift",      "LShift",  4, 0 },
    { 56, 0xFF, "z",           "Z",       4, 1 },
    { 57, 0xFF, "x",           "X",       4, 2 },
    { 58, 0xFF, "c",           "C",       4, 3 },
    { 59, 0xFF, "v",           "V",       4, 4 },
    { 60, 0xFF, "b",           "B",       4, 5 },
    { 61, 0xFF, "n",           "N",       4, 6 },
    { 62, 0xFF, "m",           "M",       4, 7 },
    { 63, 0xFF, "comma",       ",",       4, 8 },
    { 64, 0xFF, "period",      ".",       4, 9 },
    { 65, 0xFF, "slash",       "/",       4, 10 },
    { 66, 0xFF, "rshift",      "RShift",  4, 11 },
    { 67, 0xFF, "up",          "Up",      4, 12 },
    // Row 5: Bottom row (physical indices verified)
    { 68, 110,  "lctrl",       "LCtrl",   5, 0 },
    { 69, 0xFF, "lwin",        "LWin",    5, 1 },
    { 70, 0xFF, "lalt",        "LAlt",    5, 2 },
    { 71, 0xFF, "space",       "Space",   5, 3 },
    { 72, 0xFF, "ralt",        "RAlt",    5, 4 },
    { 73, 122,  "fn",          "Fn",      5, 5 },
    { 74, 120,  "menu",        "Menu",    5, 6 },
    { 75, 121,  "rctrl",       "RCtrl",   5, 7 },
    { 76, 0xFF, "left",        "Left",    5, 8 },
    { 77, 0xFF, "down",        "Down",    5, 9 },
    { 78, 0xFF, "right",       "Right",   5, 10 },
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
