#include <cassert>
#include <cstring>
#include <iostream>
#include "wraith/WraithW60Protocol.h"
#include "wraith/WraithW60Modes.h"
#include "wraith/WraithW60Leds.h"
#include "wraith/WraithW60Error.h"

using namespace wraith;

void test_protocol_constants() {
    using namespace protocol;
    assert(DEFAULT_VID == 0x2E3C);
    assert(DEFAULT_PID == 0xC365);
    assert(REPORT_ID == 0x01);
    assert(REPORT_SIZE == 64);
    assert(PAYLOAD_SIZE == 63);
    assert(CMD_BACKLIGHT == 0x07);
    assert(CMD_UNDERGLOW == 0x08);
    assert(CMD_PER_KEY_RGB == 0x09);
    assert(CMD_APPLY == 0x17);
    assert(PER_KEY_TOTAL_KEYS == 126);
    assert(PER_KEY_TOTAL_CHUNKS == 8);
    assert(PER_KEY_FULL_CHUNK_KEYS == 18);
    std::cout << "[PASS] test_protocol_constants\n";
}

void test_mode_registry() {
    assert(ModeRegistry::isValidMode(0x00));
    assert(ModeRegistry::isValidMode(0x01));
    assert(ModeRegistry::isValidMode(0x0A));
    assert(!ModeRegistry::isValidMode(0x05));
    assert(!ModeRegistry::isValidMode(0x0D));

    auto info = ModeRegistry::getModeInfo(LightingMode::Static);
    assert(info.mode == LightingMode::Static);
    assert(std::string(info.name) == "static");

    auto found = ModeRegistry::findByName("breathe");
    assert(found.mode == LightingMode::Breathe);

    assert(ModeRegistry::toByte(LightingMode::Static) == 0x00);
    assert(ModeRegistry::toByte(LightingMode::Ripple) == 0x0A);

    std::cout << "[PASS] test_mode_registry\n";
}

void test_color_hsv() {
    Color red(255, 0, 0);
    uint16_t h; uint8_t s, v;
    red.toHSV(h, s, v);
    assert(h == 0);
    assert(s == 255);
    assert(v == 255);

    Color white(255, 255, 255);
    white.toHSV(h, s, v);
    assert(s == 0);
    assert(v == 255);

    Color back = Color::fromHSV(0, 255, 255);
    assert(back.r == 255);
    assert(back.g == 0);
    assert(back.b == 0);

    std::cout << "[PASS] test_color_hsv\n";
}

void test_error_handling() {
    WraithW60Error ok;
    assert(ok.isSuccess());
    assert(!ok.isError());

    WraithW60Error err(ErrorCode::SendFailed, "test");
    assert(err.isError());
    assert(err.code() == ErrorCode::SendFailed);
    assert(err.message() == "test");

    assert(isConnectionError(ErrorCode::DeviceNotFound));
    assert(isConnectionError(ErrorCode::ConnectionFailed));
    assert(!isConnectionError(ErrorCode::SendFailed));
    assert(isProtocolError(ErrorCode::SendFailed));

    std::cout << "[PASS] test_error_handling\n";
}

void test_key_layout() {
    assert(KeyLayout::getTotalKeys() > 0);
    assert(KeyLayout::getTotalKeys() == 79);

    const auto* esc = KeyLayout::getKeyByName("esc");
    assert(esc != nullptr);
    assert(esc->index == 0);
    assert(esc->row == 0);
    assert(esc->column == 0);

    const auto* key0 = KeyLayout::getKeyByIndex(0);
    assert(key0 != nullptr);
    assert(std::string(key0->name) == "esc");

    const auto* bad = KeyLayout::getKeyByIndex(200);
    assert(bad == nullptr);

    uint8_t idx = KeyLayout::getKeyIndex(0, 0);
    assert(idx == 0);

    std::cout << "[PASS] test_key_layout\n";
}

void test_zone_registry() {
    const auto& zones = ZoneRegistry::getZones();
    assert(zones.size() == 3);

    const auto* bl = ZoneRegistry::getZone(ZoneType::Backlight);
    assert(bl != nullptr);
    assert(std::string(bl->name) == "backlight");
    assert(bl->ledCount == 1);

    const auto* pk = ZoneRegistry::getZone(ZoneType::PerKey);
    assert(pk != nullptr);
    assert(pk->ledCount == 126);

    std::cout << "[PASS] test_zone_registry\n";
}

int main() {
    std::cout << "Wraith W60 Unit Tests\n"
              << "=====================\n\n";

    test_protocol_constants();
    test_mode_registry();
    test_color_hsv();
    test_error_handling();
    test_key_layout();
    test_zone_registry();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
