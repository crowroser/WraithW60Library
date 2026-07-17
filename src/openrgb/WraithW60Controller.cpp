#include "WraithW60Controller.h"
#include "RGBController.h"

WraithW60Controller::WraithW60Controller(hid_device* dev, const char* path)
    : m_device(dev)
    , m_path(path ? path : "")
    , m_wraith(nullptr)
{
}

WraithW60Controller::~WraithW60Controller() {
    Shutdown();
}

void WraithW60Controller::SetDescription(const std::string& desc) { /* stored externally */ }
void WraithW60Controller::SetDeviceName(const std::string& name) { m_name = name; }
void WraithW60Controller::SetSerial(const std::string& serial) { m_serial = serial; }
void WraithW60Controller::SetFirmwareVersion(const std::string& ver) { m_firmware = ver; }

void WraithW60Controller::Initialize() {
    m_wraith = WraithW60::openDevice(m_serial);
}

void WraithW60Controller::Shutdown() {
    m_wraith.reset();
}

std::string WraithW60Controller::GetDeviceName() const { return m_name; }
std::string WraithW60Controller::GetSerial() const { return m_serial; }
std::string WraithW60Controller::GetFirmwareVersion() const { return m_firmware; }

void WraithW60Controller::SendBacklight(LightingMode mode, Color color, uint8_t speed) {
    if (m_wraith) m_wraith->setBacklight(mode, color.r, color.g, color.b, speed);
}

void WraithW60Controller::SendUnderglow(LightingMode mode, Color color, uint8_t speed) {
    if (m_wraith) m_wraith->setUnderglow(mode, color.r, color.g, color.b, speed);
}

void WraithW60Controller::SendPerKey(const std::vector<Color>& colors) {
    if (m_wraith) m_wraith->setAllKeys(colors);
}

void WraithW60Controller::SendApply() {
    if (m_wraith) m_wraith->apply();
}

void WraithW60Controller::SetMode(RGBController* control, int mode) {
    if (!m_wraith || !control) return;

    LightingMode wMode = ModeRegistry::fromByte(static_cast<uint8_t>(mode));
    auto& modes = control->modes;
    int activeMode = control->active_mode;

    if (control->zones.size() > ZONE_BACKLIGHT) {
        SendBacklight(wMode, Color(255, 255, 255), 0x80);
    }
    if (control->zones.size() > ZONE_UNDERGLOW) {
        SendUnderglow(wMode, Color(255, 255, 255), 0x80);
    }
}

void WraithW60Controller::SetAllLEDs(RGBController* control) {
    if (!m_wraith || !control) return;

    int activeMode = control->active_mode;
    auto& colors = control->colors;

    if (control->modes[activeMode].flags & MODE_FLAG_HAS_PER_LED_COLOR) {
        std::vector<Color> wraithColors;
        wraithColors.reserve(colors.size());
        for (auto& c : colors) {
            wraithColors.push_back(Color(
                RGBGetRValue(c),
                RGBGetGValue(c),
                RGBGetBValue(c)
            ));
        }
        SendPerKey(wraithColors);
    } else {
        if (!colors.empty()) {
            Color singleColor(RGBGetRValue(colors[0]), RGBGetGValue(colors[0]), RGBGetBValue(colors[0]));
            LightingMode wMode = ModeRegistry::fromByte(static_cast<uint8_t>(control->modes[activeMode].value));
            SendBacklight(wMode, singleColor, 0x80);
            SendUnderglow(wMode, singleColor, 0x80);
        }
    }
}

void WraithW60Controller::SetLED(RGBController* control, int led) {
    SetAllLEDs(control);
}

void WraithW60Controller::SetZone(RGBController* control, int zone) {
    SetAllLEDs(control);
}

bool WraithW60Controller::IsWraithW60Device(hid_device* dev) {
    return dev != nullptr;
}

WraithW60Controller* WraithW60Controller::AutoDetect(hid_device_info* info) {
    if (!info) return nullptr;
    if (info->vendor_id != protocol::DEFAULT_VID || info->product_id != protocol::DEFAULT_PID)
        return nullptr;
    if (info->interface_number != protocol::INTERFACE_NUMBER)
        return nullptr;

    hid_device* dev = hid_open_path(info->path);
    if (!dev) return nullptr;

    return new WraithW60Controller(dev, info->path);
}
