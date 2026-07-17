#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "WraithW60Types.h"

namespace wraith {

class IHidDevice {
public:
    virtual ~IHidDevice() = default;

    virtual bool open(uint16_t vid, uint16_t pid, const std::string& serial = "") = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;

    virtual bool sendReport(uint8_t reportId, const uint8_t* data, size_t length) = 0;
    virtual bool receiveReport(uint8_t* buffer, size_t bufferSize, int timeoutMs = 1000) = 0;

    virtual std::string getSerial() const = 0;
    virtual std::string getPath() const = 0;
    virtual uint16_t getVID() const = 0;
    virtual uint16_t getPID() const = 0;

    static std::unique_ptr<IHidDevice> create();
    static std::vector<WraithW60DeviceInfo> enumerate(uint16_t vid = 0x2E3C, uint16_t pid = 0xC365);
};

} // namespace wraith
