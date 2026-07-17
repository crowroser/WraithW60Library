#include <iostream>
#include <cstring>
#include <windows.h>
#include <hidsdi.h>
#include <hidpi.h>
#include <hidapi/hidapi.h>

int main() {
    hid_init();

    struct hid_device_info* devs = hid_enumerate(0x2E3C, 0xC365);
    if (!devs) {
        std::cout << "No devices found\n";
        return 1;
    }

    int idx = 0;
    for (struct hid_device_info* d = devs; d; d = d->next) {
        if (d->interface_number != 1) continue;

        std::cout << "[" << idx << "] UsagePage=0x" << std::hex << d->usage_page
                  << " Usage=0x" << d->usage << std::dec << "\n";

        // Open with CreateFile for HidD_SetOutputReport
        HANDLE hDev = CreateFileA(
            d->path,
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr);

        if (hDev == INVALID_HANDLE_VALUE) {
            std::cout << "    CANNOT OPEN, error=" << GetLastError() << "\n\n";
            idx++;
            continue;
        }

        // Get preparsed data for HidP_GetCaps
        PHIDP_PREPARSED_DATA preparsed = nullptr;
        if (!HidD_GetPreparsedData(hDev, &preparsed)) {
            std::cout << "    HidD_GetPreparsedData failed\n\n";
            CloseHandle(hDev);
            idx++;
            continue;
        }

        HIDP_CAPS caps;
        NTSTATUS status = HidP_GetCaps(preparsed, &caps);
        if (status == HIDP_STATUS_SUCCESS) {
            std::cout << "    OutputReportByteLength: " << caps.OutputReportByteLength << "\n";
        }

        // Try HidD_SetOutputReport (this is what WebHID sendReport uses)
        unsigned char packet[65] = {0};
        packet[0] = 0x01;  // Report ID
        packet[1] = 0x07;  // Backlight
        packet[5] = 0x0E;
        packet[6] = 0x00;  // Static
        packet[7] = 0x04;
        packet[8] = 0x04;
        packet[9] = 0x00;
        packet[10] = 0x00;
        packet[11] = 0xFF;

        BOOL res = HidD_SetOutputReport(hDev, packet, 65);
        std::cout << "    HidD_SetOutputReport: " << (res ? "OK" : "FAIL");
        if (!res) std::cout << " err=" << GetLastError();
        std::cout << "\n\n";

        HidD_FreePreparsedData(preparsed);
        CloseHandle(hDev);
        idx++;
    }

    hid_free_enumeration(devs);
    hid_exit();
    return 0;
}
