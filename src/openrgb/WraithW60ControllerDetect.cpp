#include "RGBController_WraithW60.h"
#include "WraithW60Controller.h"
#include "wraith/WraithW60Protocol.h"
#include "DetectionManager.h"
#include <vector>

using namespace wraith;

DetectedControllers DetectWraithW60(hid_device_info* info, const std::string& name) {
    DetectedControllers detected;

    hid_device_info* current = info;
    while (current) {
        if (current->vendor_id  == protocol::DEFAULT_VID &&
            current->product_id == protocol::DEFAULT_PID &&
            current->interface_number == protocol::INTERFACE_NUMBER)
        {
            hid_device* dev = hid_open_path(current->path);
            if (dev) {
                if (WraithW60Controller::IsWraithW60Device(dev)) {
                    WraithW60Controller* controller =
                        new WraithW60Controller(dev, current->path);

                    std::string serial;
                    if (current->serial_number) {
                        wchar_t buf[256] = {0};
                        wcscpy(buf, current->serial_number);
                        char s[256] = {0};
                        wcstombs(s, buf, sizeof(s));
                        serial = s;
                    }
                    controller->SetSerial(serial);
                    controller->SetDeviceName("Wraith W60");

                    RGBController_WraithW60* rgb =
                        RGBController_WraithW60::AutoDetect(controller);

                    if (rgb) {
                        detected.push_back(rgb);
                    } else {
                        delete controller;
                        hid_close(dev);
                    }
                } else {
                    hid_close(dev);
                }
            }
        }
        current = current->next;
    }

    return detected;
}

REGISTER_HID_DETECTOR_IP(
    "Wraith W60",
    DetectWraithW60,
    protocol::DEFAULT_VID,
    protocol::DEFAULT_PID,
    protocol::INTERFACE_NUMBER,
    0xFF43
);
