#include "wraith/WraithW60Device.h"
#include "wraith/WraithW60Protocol.h"
#include <hidapi/hidapi.h>
#include <cstring>

namespace wraith {

class HidDeviceImpl : public IHidDevice {
public:
    HidDeviceImpl() : m_handle(nullptr), m_vid(0), m_pid(0) {}
    ~HidDeviceImpl() override { close(); }

    bool open(uint16_t vid, uint16_t pid, const std::string& serial) override {
        if (m_handle) return true;

        hid_device_info* devs = hid_enumerate(vid, pid);
        if (!devs) return false;

        hid_device_info* found = nullptr;
        for (hid_device_info* d = devs; d; d = d->next) {
            if (d->interface_number == protocol::INTERFACE_NUMBER &&
                d->usage_page == protocol::USAGE_PAGE &&
                d->usage == protocol::USAGE) {
                found = d;
                break;
            }
        }

        // Fallback: try any interface matching
        if (!found) {
            for (hid_device_info* d = devs; d; d = d->next) {
                if (d->interface_number == protocol::INTERFACE_NUMBER) {
                    found = d;
                    break;
                }
            }
        }

        if (!found) {
            hid_free_enumeration(devs);
            return false;
        }

        m_handle = hid_open_path(found->path);
        m_path = found->path ? found->path : "";
        m_vid = found->vendor_id;
        m_pid = found->product_id;
        hid_free_enumeration(devs);
        return m_handle != nullptr;
    }

    void close() override {
        if (m_handle) {
            hid_close(m_handle);
            m_handle = nullptr;
        }
    }

    bool isOpen() const override { return m_handle != nullptr; }

    bool sendReport(uint8_t reportId, const uint8_t* data, size_t length) override {
        if (!m_handle) return false;

        uint8_t buffer[protocol::REPORT_SIZE + 1];
        std::memset(buffer, 0, sizeof(buffer));
        buffer[0] = reportId;
        std::memcpy(buffer + 1, data, length);

        int result = hid_write(m_handle, buffer, protocol::REPORT_SIZE + 1);
        return result > 0;
    }

    bool receiveReport(uint8_t* buffer, size_t bufferSize, int timeoutMs) override {
        if (!m_handle) return false;

        int result = hid_read_timeout(m_handle, buffer, bufferSize, timeoutMs);
        return result > 0;
    }

    std::string getSerial() const override {
        if (!m_handle) return "";
        wchar_t wbuf[256] = {0};
        int res = hid_get_indexed_string(m_handle, 0, wbuf, 256);
        if (res < 0) return "";
        char buf[256] = {0};
        wcstombs(buf, wbuf, sizeof(buf));
        return std::string(buf);
    }

    std::string getPath() const override { return m_path; }
    uint16_t getVID() const override { return m_vid; }
    uint16_t getPID() const override { return m_pid; }

private:
    hid_device* m_handle;
    std::string m_path;
    uint16_t m_vid;
    uint16_t m_pid;
};

std::unique_ptr<IHidDevice> IHidDevice::create() {
    return std::make_unique<HidDeviceImpl>();
}

std::vector<WraithW60DeviceInfo> IHidDevice::enumerate(uint16_t vid, uint16_t pid) {
    std::vector<WraithW60DeviceInfo> result;

    hid_device_info* devs = hid_enumerate(vid, pid);
    if (!devs) return result;

    for (hid_device_info* d = devs; d; d = d->next) {
        if (d->interface_number == protocol::INTERFACE_NUMBER &&
            d->usage_page == protocol::USAGE_PAGE &&
            d->usage == protocol::USAGE) {
            WraithW60DeviceInfo info;
            info.vid = d->vendor_id;
            info.pid = d->product_id;
            info.path = d->path ? d->path : "";
            if (d->serial_number) {
                wchar_t serial[256] = {0};
                wcscpy(serial, d->serial_number);
                char serialStr[256] = {0};
                wcstombs(serialStr, serial, sizeof(serialStr));
                info.serial = serialStr;
            }
            result.push_back(info);
        }
    }

    hid_free_enumeration(devs);
    return result;
}

} // namespace wraith
