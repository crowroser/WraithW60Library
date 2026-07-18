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

void test_power_bar_packet() {
    using namespace protocol;

    uint8_t packet[REPORT_SIZE];

    // Power bar uses underglow command with RGB order
    buildPowerBarPacket(packet, 255, 0, 0);

    assert(packet[OFFSET_REPORT_ID] == REPORT_ID);
    assert(packet[OFFSET_COMMAND] == CMD_UNDERGLOW);
    assert(packet[OFFSET_LENGTH] == BACKLIGHT_LENGTH);
    assert(packet[OFFSET_MODE] == 0x00);
    assert(packet[OFFSET_UNKNOWN] == 0x04);
    assert(packet[OFFSET_ZONE_TYPE] == ZONE_TYPE_UNDERGLOW);

    // RGB order
    assert(packet[OFFSET_RED]   == 255);
    assert(packet[OFFSET_GREEN] == 0);
    assert(packet[OFFSET_BLUE]  == 0);
    assert(packet[OFFSET_SPEED] == 0x80);

    // Green
    buildPowerBarPacket(packet, 0, 255, 0);
    assert(packet[OFFSET_RED]   == 0);
    assert(packet[OFFSET_GREEN] == 255);
    assert(packet[OFFSET_BLUE]  == 0);

    // Blue
    buildPowerBarPacket(packet, 0, 0, 255);
    assert(packet[OFFSET_RED]   == 0);
    assert(packet[OFFSET_GREEN] == 0);
    assert(packet[OFFSET_BLUE]  == 255);

    // Rest zeroed
    for (size_t i = 13; i < REPORT_SIZE; ++i) {
        assert(packet[i] == 0);
    }

    std::cout << "[PASS] test_power_bar_packet\n";
}

void test_socd_packet() {
    using namespace protocol;
    uint8_t packet[REPORT_SIZE];

    // Test enable
    buildSocdPacket(packet, true);
    assert(packet[OFFSET_REPORT_ID] == REPORT_ID);
    assert(packet[OFFSET_COMMAND] == CMD_SOCD);
    assert(packet[SOCD_OFFSET_SUBCMD] == 0x03);
    assert(packet[SOCD_OFFSET_LENGTH] == 0x01);
    assert(packet[SOCD_OFFSET_STATUS] == 0x01);

    // Test disable
    buildSocdPacket(packet, false);
    assert(packet[SOCD_OFFSET_STATUS] == 0x00);
    assert(packet[SOCD_OFFSET_SUBCMD] == 0x03);
    assert(packet[SOCD_OFFSET_LENGTH] == 0x01);

    // Rest zeroed
    for (size_t i = 7; i < REPORT_SIZE; ++i)
        assert(packet[i] == 0);

    // Verify against captured: 01 24 03 00 00 01 01 ...
    buildSocdPacket(packet, true);
    assert(packet[0] == 0x01);
    assert(packet[1] == 0x24);
    assert(packet[2] == 0x03);
    assert(packet[3] == 0x00);
    assert(packet[4] == 0x00);
    assert(packet[5] == 0x01);
    assert(packet[6] == 0x01);

    // Verify disable: 01 24 03 00 00 01 00 ...
    buildSocdPacket(packet, false);
    assert(packet[6] == 0x00);

    std::cout << "[PASS] test_socd_packet\n";
}

void test_rapid_trigger_packet() {
    using namespace protocol;
    uint8_t packet[REPORT_SIZE];

    // Test with captured values: up=0x55, down=0x19
    buildRapidTriggerPacket(packet, 0x55, 0x19);

    assert(packet[OFFSET_REPORT_ID] == REPORT_ID);
    assert(packet[OFFSET_COMMAND] == CMD_RAPID_TRIGGER);  // 0x21
    assert(packet[RT_OFFSET_SUBCMD] == 0x00);
    assert(packet[RT_OFFSET_ADDR_HIGH] == 0x18);
    assert(packet[RT_OFFSET_ADDR_LOW] == 0x0C);
    assert(packet[RT_OFFSET_PARAM] == 0x00);
    assert(packet[RT_OFFSET_PROFILE] == 0x04);

    // Thresholds at bytes 29-32
    assert(packet[RT_OFFSET_UP_THRESHOLD] == 0x55);
    assert(packet[RT_OFFSET_UP_THRESHOLD_2] == 0x55);
    assert(packet[RT_OFFSET_DOWN_THRESHOLD] == 0x19);
    assert(packet[RT_OFFSET_DOWN_THRESHOLD_2] == 0x19);

    // Verify exact captured packet: 01 21 00 00 00 18 0C 00 04 ... 55 55 19 19
    assert(packet[0] == 0x01);
    assert(packet[1] == 0x21);
    assert(packet[2] == 0x00);
    assert(packet[3] == 0x00);
    assert(packet[4] == 0x00);
    assert(packet[5] == 0x18);
    assert(packet[6] == 0x0C);
    assert(packet[7] == 0x00);
    assert(packet[8] == 0x04);

    // Test with different values
    buildRapidTriggerPacket(packet, 0x32, 0x0A);
    assert(packet[RT_OFFSET_UP_THRESHOLD] == 0x32);
    assert(packet[RT_OFFSET_UP_THRESHOLD_2] == 0x32);
    assert(packet[RT_OFFSET_DOWN_THRESHOLD] == 0x0A);
    assert(packet[RT_OFFSET_DOWN_THRESHOLD_2] == 0x0A);

    std::cout << "[PASS] test_rapid_trigger_packet\n";
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
    test_power_bar_packet();
    test_socd_packet();
    test_rapid_trigger_packet();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
