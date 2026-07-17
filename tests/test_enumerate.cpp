#include <iostream>
#include <cstring>
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

        std::wcout << L"[" << idx << L"] Path: " << d->path << std::endl;
        std::wcout << L"    Usage Page: 0x" << std::hex << d->usage_page << std::endl;
        std::wcout << L"    Usage: 0x" << std::hex << d->usage << std::endl;

        hid_device* dev = hid_open_path(d->path);
        if (!dev) {
            std::cout << "    Status: CANNOT OPEN\n\n";
            idx++;
            continue;
        }

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
        std::cout << "    hid_write result: " << res;
        if (res < 0) {
            std::cout << " ERROR: " << hid_error(dev);
        }
        std::cout << "\n\n";

        hid_close(dev);
        idx++;
    }

    hid_free_enumeration(devs);
    hid_exit();
    return 0;
}
