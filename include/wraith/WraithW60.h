#pragma once

#include "WraithW60Device.h"
#include "WraithW60Leds.h"
#include "WraithW60Modes.h"
#include "WraithW60Error.h"
#include "WraithW60Types.h"
#include <string>
#include <vector>
#include <memory>

namespace wraith {

class WraithW60 {
public:
    static std::vector<WraithW60DeviceInfo> enumerateDevices();
    static std::unique_ptr<WraithW60> openDevice(const std::string& serial = "");

    ~WraithW60();
    bool isConnected() const;
    void disconnect();

    std::string getSerial() const;
    uint16_t getVID() const;
    uint16_t getPID() const;

    WraithW60Error setBacklightMode(LightingMode mode);
    WraithW60Error setBacklightColor(uint8_t r, uint8_t g, uint8_t b);
    WraithW60Error setBacklightSpeed(uint8_t speed);
    WraithW60Error setBacklight(LightingMode mode, uint8_t r, uint8_t g, uint8_t b, uint8_t speed = 0x80);
    LightingMode getBacklightMode() const;
    Color getBacklightColor() const;

    WraithW60Error setUnderglowMode(LightingMode mode);
    WraithW60Error setUnderglowColor(uint8_t r, uint8_t g, uint8_t b);
    WraithW60Error setUnderglowSpeed(uint8_t speed);
    WraithW60Error setUnderglow(LightingMode mode, uint8_t r, uint8_t g, uint8_t b, uint8_t speed = 0x80);
    LightingMode getUnderglowMode() const;
    Color getUnderglowColor() const;

    WraithW60Error setPowerBarColor(uint8_t r, uint8_t g, uint8_t b);

    WraithW60Error setSocd(bool enable);

    WraithW60Error setRapidTrigger(uint8_t upThreshold, uint8_t downThreshold);

    WraithW60Error setKeyColor(uint8_t keyIndex, uint8_t r, uint8_t g, uint8_t b);
    WraithW60Error setAllKeys(const std::vector<Color>& colors);
    WraithW60Error setKeyRange(uint8_t startKey, uint8_t endKey, uint8_t r, uint8_t g, uint8_t b);
    Color getKeyColor(uint8_t keyIndex) const;
    std::vector<Color> getAllKeyColors() const;

    WraithW60Error apply();
    WraithW60Error sendRawReport(const uint8_t* data, size_t length);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;

    WraithW60(std::unique_ptr<Impl> impl);
    WraithW60(const WraithW60&) = delete;
    WraithW60& operator=(const WraithW60&) = delete;
};

} // namespace wraith
