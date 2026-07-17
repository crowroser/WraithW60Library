#pragma once

#include "RGBController.h"
#include "WraithW60Controller.h"

class RGBController_WraithW60 : public RGBController {
public:
    RGBController_WraithW60(WraithW60Controller* controller);
    ~RGBController_WraithW60() override;

    void DeviceUpdateLEDs() override;
    void UpdateZoneLEDs(int zone) override;
    void UpdateSingleLED(int led) override;
    void SetCustomMode() override;
    void DeviceUpdateMode() override;

    static RGBController* AutoDetect(WraithW60Controller* controller);

private:
    WraithW60Controller* m_controller;

    void SetupZones();
    void SetupModes();
    void SetupLEDs();

    static constexpr int ZONE_BACKLIGHT = 0;
    static constexpr int ZONE_UNDERGLOW = 1;
    static constexpr int ZONE_PER_KEY = 2;
};
