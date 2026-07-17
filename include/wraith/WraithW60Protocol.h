#pragma once

#include <cstdint>
#include <cstddef>

namespace wraith {
namespace protocol {

constexpr uint16_t DEFAULT_VID = 0x2E3C;
constexpr uint16_t DEFAULT_PID = 0xC365;
constexpr uint8_t INTERFACE_NUMBER = 2;
constexpr uint8_t ENDPOINT_OUT = 0x04;
constexpr uint16_t USAGE_PAGE = 0xFF1B;
constexpr uint16_t USAGE = 0x91;

constexpr uint8_t REPORT_ID = 0x01;
constexpr size_t REPORT_SIZE = 64;
constexpr size_t PAYLOAD_SIZE = 63;

constexpr uint8_t CMD_HEARTBEAT = 0x01;
constexpr uint8_t CMD_BACKLIGHT = 0x07;
constexpr uint8_t CMD_UNDERGLOW = 0x08;
constexpr uint8_t CMD_PER_KEY_RGB = 0x09;
constexpr uint8_t CMD_APPLY = 0x17;

constexpr size_t OFFSET_REPORT_ID = 0;
constexpr size_t OFFSET_COMMAND = 1;
constexpr size_t OFFSET_PADDING_A = 2;
constexpr size_t OFFSET_PADDING_B = 4;
constexpr size_t OFFSET_LENGTH = 5;
constexpr size_t OFFSET_MODE = 6;
constexpr size_t OFFSET_UNKNOWN = 7;
constexpr size_t OFFSET_ZONE_TYPE = 8;
constexpr size_t OFFSET_RED = 9;
constexpr size_t OFFSET_GREEN = 10;
constexpr size_t OFFSET_BLUE = 11;
constexpr size_t OFFSET_SPEED = 12;

constexpr uint8_t ZONE_TYPE_BACKLIGHT = 0x04;
constexpr uint8_t ZONE_TYPE_UNDERGLOW = 0x03;
constexpr uint8_t BACKLIGHT_LENGTH = 0x0E;

constexpr size_t PER_KEY_OFFSET_SUBCMD = 2;
constexpr size_t PER_KEY_OFFSET_CHUNK = 4;
constexpr size_t PER_KEY_OFFSET_DATA_LEN = 5;
constexpr size_t PER_KEY_OFFSET_DATA = 6;
constexpr size_t PER_KEY_FULL_CHUNK_KEYS = 18;
constexpr size_t PER_KEY_TOTAL_KEYS = 126;
constexpr size_t PER_KEY_TOTAL_CHUNKS = 8;
constexpr uint8_t PER_KEY_FULL_LENGTH = 0x36;
constexpr uint8_t PER_KEY_PARTIAL_LENGTH = 0x12;

void buildBacklightPacket(uint8_t* packet, uint8_t mode, uint8_t r, uint8_t g, uint8_t b, uint8_t speed);
void buildUnderglowPacket(uint8_t* packet, uint8_t mode, uint8_t r, uint8_t g, uint8_t b, uint8_t speed);
void buildPerKeyPacket(uint8_t* packet, uint8_t chunkIndex, const uint8_t* rgbData, size_t keyCount);
void buildApplyPacket(uint8_t* packet);

} // namespace protocol
} // namespace wraith
