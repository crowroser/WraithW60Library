#include "RGBController_WraithW60.h"
#include "wraith/WraithW60Modes.h"
#include "wraith/WraithW60Leds.h"

using namespace wraith;

static const unsigned int matrix_map_data[] = {
    // Row 0: Esc F1-F12 Del (14 keys)
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
    // Row 1: ` 1-0 - = Bksp (14 keys)
    14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
    // Row 2: Tab Q-P [\] (14 keys)
    28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
    // Row 3: Caps A-L ;' Enter (13 keys)
    42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 0xFFFFFFFF,
    // Row 4: LShift Z-M ,. / RShift Up (13 keys)
    55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 0xFFFFFFFF,
    // Row 5: LCtrl LWin LAlt Space RAlt Fn Menu RCtrl Left Down Right (11 keys)
    68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
};

RGBController_WraithW60::RGBController_WraithW60(WraithW60Controller* controller)
    : m_controller(controller)
{
    name        = "Wraith W60";
    vendor      = "Wraith";
    description = "Wraith W60 RGB Keyboard";
    type        = DEVICE_TYPE_KEYBOARD;
    location    = controller->GetSerial();

    SetupModes();
    SetupZones();
    SetupLEDs();

    active_mode = 0;
    colors.resize(leds.size(), 0);
}

RGBController_WraithW60::~RGBController_WraithW60() {
    delete m_controller;
}

void RGBController_WraithW60::SetupModes() {
    auto addMode = [this](const char* name, uint8_t value, unsigned int flags, unsigned int colorMode) {
        mode m;
        m.name      = name;
        m.value     = value;
        m.flags     = flags;
        m.color_mode = colorMode;
        modes.push_back(m);
    };

    addMode("Static",   0x00, MODE_FLAG_HAS_PER_LED_COLOR,                                    MODE_COLORS_PER_LED);
    addMode("Breathe",  0x01, MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR,        MODE_COLORS_MODE_SPECIFIC);
    addMode("Wave",     0x02, MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR,               MODE_COLORS_NONE);
    addMode("Neon",     0x03, MODE_FLAG_HAS_SPEED,                                            MODE_COLORS_NONE);
    addMode("Sparkle",  0x04, MODE_FLAG_HAS_SPEED,                                            MODE_COLORS_NONE);
    addMode("Reactive", 0x07, MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR,        MODE_COLORS_MODE_SPECIFIC);
    addMode("Ripple",   0x0A, MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR,        MODE_COLORS_MODE_SPECIFIC);
}

void RGBController_WraithW60::SetupZones() {
    zone backlight_zone;
    backlight_zone.name       = "Backlight";
    backlight_zone.type       = ZONE_TYPE_SINGLE;
    backlight_zone.leds_min   = 1;
    backlight_zone.leds_max   = 1;
    backlight_zone.leds_count = 1;
    backlight_zone.start_idx  = 0;
    zones.push_back(backlight_zone);

    zone underglow_zone;
    underglow_zone.name       = "Underglow";
    underglow_zone.type       = ZONE_TYPE_SINGLE;
    underglow_zone.leds_min   = 1;
    underglow_zone.leds_max   = 1;
    underglow_zone.leds_count = 1;
    underglow_zone.start_idx  = 1;
    zones.push_back(underglow_zone);

    zone perkey_zone;
    perkey_zone.name       = "Per-Key";
    perkey_zone.type       = ZONE_TYPE_MATRIX;
    perkey_zone.leds_min   = 79;
    perkey_zone.leds_max   = 79;
    perkey_zone.leds_count = 79;
    perkey_zone.start_idx  = 2;
    perkey_zone.matrix_map.Set(6, 14, (unsigned int*)matrix_map_data);
    zones.push_back(perkey_zone);
}

void RGBController_WraithW60::SetupLEDs() {
    led new_led;

    new_led.name  = "Backlight";
    new_led.value = 0;
    leds.push_back(new_led);

    new_led.name  = "Underglow";
    new_led.value = 1;
    leds.push_back(new_led);

    const auto& layout = KeyLayout::getStandardLayout();
    for (const auto& key : layout) {
        new_led.name  = std::string("Key: ") + key.label;
        new_led.value = key.index + 2;
        leds.push_back(new_led);
    }
}

void RGBController_WraithW60::DeviceUpdateLEDs() {
    m_controller->SetAllLEDs(this);
}

void RGBController_WraithW60::UpdateZoneLEDs(int zone) {
    m_controller->SetZone(this, zone);
}

void RGBController_WraithW60::UpdateSingleLED(int led) {
    m_controller->SetLED(this, led);
}

void RGBController_WraithW60::SetCustomMode() {
    active_mode = 0;
}

void RGBController_WraithW60::DeviceUpdateMode() {
    m_controller->SetMode(this, modes[active_mode].value);
}

RGBController* RGBController_WraithW60::AutoDetect(WraithW60Controller* controller) {
    if (!controller) return nullptr;
    return new RGBController_WraithW60(controller);
}
