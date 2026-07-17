#include "wraith/WraithW60Protocol.h"
#include "wraith/WraithW60Modes.h"
#include <cstring>

namespace wraith {
namespace protocol {

void buildBacklightPacket(uint8_t* packet, uint8_t mode, uint8_t r, uint8_t g, uint8_t b, uint8_t speed) {
    std::memset(packet, 0, REPORT_SIZE);
    packet[OFFSET_REPORT_ID] = REPORT_ID;
    packet[OFFSET_COMMAND] = CMD_BACKLIGHT;
    packet[OFFSET_LENGTH] = BACKLIGHT_LENGTH;
    packet[OFFSET_MODE] = mode;
    packet[OFFSET_UNKNOWN] = 0x04;
    packet[OFFSET_ZONE_TYPE] = ZONE_TYPE_BACKLIGHT;
    packet[OFFSET_RED] = r;
    packet[OFFSET_GREEN] = g;
    packet[OFFSET_BLUE] = b;
    packet[OFFSET_SPEED] = speed;
}

void buildUnderglowPacket(uint8_t* packet, uint8_t mode, uint8_t r, uint8_t g, uint8_t b, uint8_t speed) {
    std::memset(packet, 0, REPORT_SIZE);
    packet[OFFSET_REPORT_ID] = REPORT_ID;
    packet[OFFSET_COMMAND] = CMD_UNDERGLOW;
    packet[OFFSET_LENGTH] = BACKLIGHT_LENGTH;
    packet[OFFSET_MODE] = mode;
    packet[OFFSET_UNKNOWN] = 0x04;
    packet[OFFSET_ZONE_TYPE] = ZONE_TYPE_UNDERGLOW;
    packet[OFFSET_RED] = r;
    packet[OFFSET_GREEN] = g;
    packet[OFFSET_BLUE] = b;
    packet[OFFSET_SPEED] = speed;
}

void buildPerKeyPacket(uint8_t* packet, uint8_t chunkIndex, const uint8_t* rgbData, size_t keyCount) {
    std::memset(packet, 0, REPORT_SIZE);
    packet[OFFSET_REPORT_ID] = REPORT_ID;
    packet[OFFSET_COMMAND] = CMD_PER_KEY_RGB;
    packet[PER_KEY_OFFSET_SUBCMD] = 0x01;
    packet[PER_KEY_OFFSET_CHUNK] = chunkIndex;
    bool isLast = (chunkIndex == PER_KEY_TOTAL_CHUNKS - 1);
    packet[PER_KEY_OFFSET_DATA_LEN] = isLast ? PER_KEY_PARTIAL_LENGTH : PER_KEY_FULL_LENGTH;

    size_t bytesToCopy = keyCount * 3;
    if (bytesToCopy > REPORT_SIZE - PER_KEY_OFFSET_DATA) {
        bytesToCopy = REPORT_SIZE - PER_KEY_OFFSET_DATA;
    }
    std::memcpy(packet + PER_KEY_OFFSET_DATA, rgbData, bytesToCopy);
}

void buildApplyPacket(uint8_t* packet) {
    std::memset(packet, 0, REPORT_SIZE);
    packet[OFFSET_REPORT_ID] = REPORT_ID;
    packet[OFFSET_COMMAND] = CMD_APPLY;
}

} // namespace protocol
} // namespace wraith
