#include <iostream>
#include <cstring>
#include <hidapi/hidapi.h>

int main() {
    hid_init();

    // Enumerate ALL devices from this vendor
    struct hid_device_info* devs = hid_enumerate(0x2E3C, 0x0000);
    if (!devs) {
        std::cout << "No devices found from VID 0x2E3C\n";
        hid_exit();
        return 1;
    }

    int idx = 0;
    for (struct hid_device_info* d = devs; d; d = d->next) {
        std::cout << "[" << idx << "] PID=0x" << std::hex << d->product_id << std::dec
                  << " IF=" << d->interface_number
                  << " UsagePage=0x" << std::hex << d->usage_page
                  << " Usage=0x" << d->usage << std::dec << "\n";

        wchar_t serial[256] = {0};
        if (d->serial_number) {
            wcscpy(serial, d->serial_number);
        }
        std::wcout << L"    Serial: " << serial << L"\n";
        std::wcout << L"    Product: " << (d->product_string ? d->product_string : L"N/A") << L"\n";

        // Try to open
        hid_device* dev = hid_open_path(d->path);
        if (!dev) {
            std::cout << "    Status: CANNOT OPEN\n";
        } else {
            std::cout << "    Status: OPENED\n";

            // Try sending the backlight command
            unsigned char packet[65] = {0};
            packet[0] = 0x01;
            packet[1] = 0x07;
            packet[5] = 0x0E;
            packet[6] = 0x00;
            packet[7] = 0x04;
            packet[8] = 0x04;
            packet[9] = 0x00;
            packet[10] = 0x00;
            packet[11] = 0xFF;

            int res = hid_write(dev, packet, 65);
            std::cout << "    hid_write(0x07/blue): " << res;
            if (res < 0) std::cout << " FAIL";
            std::cout << "\n";

            hid_close(dev);
        }
        std::cout << "\n";
        idx++;
    }

    hid_free_enumeration(devs);
    hid_exit();
    return 0;
}
