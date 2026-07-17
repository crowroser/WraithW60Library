#pragma once

#include "WraithController.h"
#include "wraith/WraithW60Device.h"
#include "wraith/WraithW60Protocol.h"
#include "wraith/WraithW60Modes.h"
#include "wraith/WraithW60Leds.h"
#include <string>
#include <vector>

using namespace wraith;

class WraithW60Controller : public WraithController {
public:
    WraithW60Controller(hid_device* dev, const char* path);
    ~WraithW60Controller() override;

    void SetDescription(const std::string& description) override;
    void SetDeviceName(const std::string& name) override;
    void SetSerial(const std::string& serial) override;
    void SetFirmwareVersion(const std::string& version) override;

    void Initialize() override;
    void Shutdown() override;

    void SetMode(RGBController* control, int mode) override;
    void SetAllLEDs(RGBController* control) override;
    void SetLED(RGBController* control, int led) override;
    void SetZone(RGBController* control, int zone) override;

    std::string GetDeviceName() const override;
    std::string GetSerial() const override;
    std::string GetFirmwareVersion() const override;

    static bool IsWraithW60Device(hid_device* dev);
    static WraithW60Controller* AutoDetect(hid_device_info* info);

private:
    hid_device* m_device;
    std::string m_path;
    std::string m_name;
    std::string m_serial;
    std::string m_firmware;
    std::unique_ptr<WraithW60> m_wraith;

    void SendBacklight(LightingMode mode, Color color, uint8_t speed);
    void SendUnderglow(LightingMode mode, Color color, uint8_t speed);
    void SendPerKey(const std::vector<Color>& colors);
    void SendApply();

    static constexpr int ZONE_BACKLIGHT = 0;
    static constexpr int ZONE_UNDERGLOW = 1;
    static constexpr int ZONE_PER_KEY = 2;
};
