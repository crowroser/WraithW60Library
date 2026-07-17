#include "wraith/WraithW60.h"
#include "wraith/WraithW60Protocol.h"
#include "wraith/WraithW60Device.h"
#include "wraith/WraithW60Modes.h"
#include <cstring>
#include <thread>
#include <chrono>

namespace wraith {

class WraithW60::Impl {
public:
    std::unique_ptr<IHidDevice> m_device;
    DeviceState m_state;
    bool m_connected = false;

    Impl() : m_device(IHidDevice::create()) {
        m_state.perKeyColors.resize(protocol::PER_KEY_TOTAL_KEYS, Color(0, 0, 0));
    }

    ~Impl() { disconnect(); }

    WraithW60Error connect(const std::string& serial) {
        if (m_device->isOpen()) { m_connected = true; return WraithW60Error::success(); }

        for (int attempt = 0; attempt < 3; ++attempt) {
            if (m_device->open(protocol::DEFAULT_VID, protocol::DEFAULT_PID, serial)) {
                m_connected = true;
                return WraithW60Error::success();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return WraithW60Error::error(ErrorCode::ConnectionFailed);
    }

    void disconnect() {
        if (m_device) m_device->close();
        m_connected = false;
    }

    WraithW60Error sendBacklight(LightingMode mode, uint8_t r, uint8_t g, uint8_t b, uint8_t speed) {
        uint8_t packet[protocol::REPORT_SIZE];
        protocol::buildBacklightPacket(packet, static_cast<uint8_t>(mode), r, g, b, speed);
        if (!m_device->sendReport(protocol::REPORT_ID, packet + 1, protocol::PAYLOAD_SIZE))
            return WraithW60Error::error(ErrorCode::SendFailed, "Backlight send failed");
        m_state.backlightMode = mode;
        m_state.backlightColor = Color(r, g, b);
        m_state.backlightSpeed = speed;
        return WraithW60Error::success();
    }

    WraithW60Error sendUnderglow(LightingMode mode, uint8_t r, uint8_t g, uint8_t b, uint8_t speed) {
        uint8_t packet[protocol::REPORT_SIZE];
        protocol::buildUnderglowPacket(packet, static_cast<uint8_t>(mode), r, g, b, speed);
        if (!m_device->sendReport(protocol::REPORT_ID, packet + 1, protocol::PAYLOAD_SIZE))
            return WraithW60Error::error(ErrorCode::SendFailed, "Underglow send failed");
        m_state.underglowMode = mode;
        m_state.underglowColor = Color(r, g, b);
        m_state.underglowSpeed = speed;
        return WraithW60Error::success();
    }

    WraithW60Error sendPerKeyChunk(uint8_t chunkIndex, const std::vector<Color>& colors) {
        if (chunkIndex >= protocol::PER_KEY_TOTAL_CHUNKS)
            return WraithW60Error::error(ErrorCode::InvalidChunkIndex);

        bool isLast = (chunkIndex == protocol::PER_KEY_TOTAL_CHUNKS - 1);
        uint8_t keyCount = isLast ? 6 : static_cast<uint8_t>(protocol::PER_KEY_FULL_CHUNK_KEYS);

        uint8_t rgbData[protocol::PER_KEY_FULL_CHUNK_KEYS * 3] = {0};
        for (size_t i = 0; i < keyCount && i < colors.size(); ++i) {
            rgbData[i * 3]     = colors[i].r;
            rgbData[i * 3 + 1] = colors[i].g;
            rgbData[i * 3 + 2] = colors[i].b;
        }

        uint8_t packet[protocol::REPORT_SIZE];
        protocol::buildPerKeyPacket(packet, chunkIndex, rgbData, keyCount);
        if (!m_device->sendReport(protocol::REPORT_ID, packet + 1, protocol::PAYLOAD_SIZE))
            return WraithW60Error::error(ErrorCode::SendFailed,
                "Per-key chunk " + std::to_string(chunkIndex) + " send failed");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return WraithW60Error::success();
    }

    WraithW60Error sendApply() {
        uint8_t packet[protocol::REPORT_SIZE];
        protocol::buildApplyPacket(packet);
        if (!m_device->sendReport(protocol::REPORT_ID, packet + 1, protocol::PAYLOAD_SIZE))
            return WraithW60Error::error(ErrorCode::SendFailed, "Apply send failed");
        return WraithW60Error::success();
    }
};

WraithW60::WraithW60(std::unique_ptr<Impl> impl) : pImpl(std::move(impl)) {}
WraithW60::~WraithW60() = default;

std::vector<WraithW60DeviceInfo> WraithW60::enumerateDevices() {
    return IHidDevice::enumerate();
}

std::unique_ptr<WraithW60> WraithW60::openDevice(const std::string& serial) {
    auto impl = std::make_unique<Impl>();
    if (impl->connect(serial).isError()) return nullptr;
    return std::unique_ptr<WraithW60>(new WraithW60(std::move(impl)));
}

bool WraithW60::isConnected() const { return pImpl->m_connected; }
void WraithW60::disconnect() { pImpl->disconnect(); }
std::string WraithW60::getSerial() const { return pImpl->m_device->getSerial(); }
uint16_t WraithW60::getVID() const { return pImpl->m_device->getVID(); }
uint16_t WraithW60::getPID() const { return pImpl->m_device->getPID(); }

WraithW60Error WraithW60::setBacklightMode(LightingMode mode) {
    auto& s = pImpl->m_state;
    return pImpl->sendBacklight(mode, s.backlightColor.r, s.backlightColor.g, s.backlightColor.b, s.backlightSpeed);
}

WraithW60Error WraithW60::setBacklightColor(uint8_t r, uint8_t g, uint8_t b) {
    auto& s = pImpl->m_state;
    return pImpl->sendBacklight(s.backlightMode, r, g, b, s.backlightSpeed);
}

WraithW60Error WraithW60::setBacklightSpeed(uint8_t speed) {
    auto& s = pImpl->m_state;
    return pImpl->sendBacklight(s.backlightMode, s.backlightColor.r, s.backlightColor.g, s.backlightColor.b, speed);
}

WraithW60Error WraithW60::setBacklight(LightingMode mode, uint8_t r, uint8_t g, uint8_t b, uint8_t speed) {
    return pImpl->sendBacklight(mode, r, g, b, speed);
}

LightingMode WraithW60::getBacklightMode() const { return pImpl->m_state.backlightMode; }
Color WraithW60::getBacklightColor() const { return pImpl->m_state.backlightColor; }

WraithW60Error WraithW60::setUnderglowMode(LightingMode mode) {
    auto& s = pImpl->m_state;
    return pImpl->sendUnderglow(mode, s.underglowColor.r, s.underglowColor.g, s.underglowColor.b, s.underglowSpeed);
}

WraithW60Error WraithW60::setUnderglowColor(uint8_t r, uint8_t g, uint8_t b) {
    auto& s = pImpl->m_state;
    return pImpl->sendUnderglow(s.underglowMode, r, g, b, s.underglowSpeed);
}

WraithW60Error WraithW60::setUnderglowSpeed(uint8_t speed) {
    auto& s = pImpl->m_state;
    return pImpl->sendUnderglow(s.underglowMode, s.underglowColor.r, s.underglowColor.g, s.underglowColor.b, speed);
}

WraithW60Error WraithW60::setUnderglow(LightingMode mode, uint8_t r, uint8_t g, uint8_t b, uint8_t speed) {
    return pImpl->sendUnderglow(mode, r, g, b, speed);
}

LightingMode WraithW60::getUnderglowMode() const { return pImpl->m_state.underglowMode; }
Color WraithW60::getUnderglowColor() const { return pImpl->m_state.underglowColor; }

WraithW60Error WraithW60::setKeyColor(uint8_t keyIndex, uint8_t r, uint8_t g, uint8_t b) {
    if (keyIndex >= protocol::PER_KEY_TOTAL_KEYS)
        return WraithW60Error::error(ErrorCode::InvalidKeyIndex);

    pImpl->m_state.perKeyColors[keyIndex] = Color(r, g, b);

    uint8_t chunkIndex = keyIndex / static_cast<uint8_t>(protocol::PER_KEY_FULL_CHUNK_KEYS);
    uint8_t startKey = chunkIndex * static_cast<uint8_t>(protocol::PER_KEY_FULL_CHUNK_KEYS);
    uint8_t endKey = startKey + static_cast<uint8_t>(protocol::PER_KEY_FULL_CHUNK_KEYS);
    if (endKey > protocol::PER_KEY_TOTAL_KEYS) endKey = static_cast<uint8_t>(protocol::PER_KEY_TOTAL_KEYS);

    std::vector<Color> chunkColors(
        pImpl->m_state.perKeyColors.begin() + startKey,
        pImpl->m_state.perKeyColors.begin() + endKey
    );
    return pImpl->sendPerKeyChunk(chunkIndex, chunkColors);
}

WraithW60Error WraithW60::setAllKeys(const std::vector<Color>& colors) {
    if (colors.size() != protocol::PER_KEY_TOTAL_KEYS)
        return WraithW60Error::error(ErrorCode::InvalidParameter, "Expected 126 colors");

    pImpl->m_state.perKeyColors = colors;

    for (uint8_t chunk = 0; chunk < protocol::PER_KEY_TOTAL_CHUNKS; ++chunk) {
        bool isLast = (chunk == protocol::PER_KEY_TOTAL_CHUNKS - 1);
        uint8_t keyCount = isLast ? 6 : static_cast<uint8_t>(protocol::PER_KEY_FULL_CHUNK_KEYS);
        uint8_t startKey = chunk * static_cast<uint8_t>(protocol::PER_KEY_FULL_CHUNK_KEYS);

        std::vector<Color> chunkColors(
            colors.begin() + startKey,
            colors.begin() + startKey + keyCount
        );

        auto err = pImpl->sendPerKeyChunk(chunk, chunkColors);
        if (err.isError()) return err;
    }
    return WraithW60Error::success();
}

WraithW60Error WraithW60::setKeyRange(uint8_t startKey, uint8_t endKey, uint8_t r, uint8_t g, uint8_t b) {
    if (startKey >= protocol::PER_KEY_TOTAL_KEYS || endKey >= protocol::PER_KEY_TOTAL_KEYS || startKey > endKey)
        return WraithW60Error::error(ErrorCode::InvalidParameter);

    for (uint8_t i = startKey; i <= endKey; ++i) {
        pImpl->m_state.perKeyColors[i] = Color(r, g, b);
    }

    uint8_t startChunk = startKey / static_cast<uint8_t>(protocol::PER_KEY_FULL_CHUNK_KEYS);
    uint8_t endChunk = endKey / static_cast<uint8_t>(protocol::PER_KEY_FULL_CHUNK_KEYS);

    for (uint8_t chunk = startChunk; chunk <= endChunk; ++chunk) {
        bool isLast = (chunk == protocol::PER_KEY_TOTAL_CHUNKS - 1);
        uint8_t keyCount = isLast ? 6 : static_cast<uint8_t>(protocol::PER_KEY_FULL_CHUNK_KEYS);
        uint8_t chunkStart = chunk * static_cast<uint8_t>(protocol::PER_KEY_FULL_CHUNK_KEYS);

        std::vector<Color> chunkColors(
            pImpl->m_state.perKeyColors.begin() + chunkStart,
            pImpl->m_state.perKeyColors.begin() + chunkStart + keyCount
        );

        auto err = pImpl->sendPerKeyChunk(chunk, chunkColors);
        if (err.isError()) return err;
    }
    return WraithW60Error::success();
}

Color WraithW60::getKeyColor(uint8_t keyIndex) const {
    if (keyIndex >= protocol::PER_KEY_TOTAL_KEYS) return Color();
    return pImpl->m_state.perKeyColors[keyIndex];
}

std::vector<Color> WraithW60::getAllKeyColors() const {
    return pImpl->m_state.perKeyColors;
}

WraithW60Error WraithW60::apply() {
    return pImpl->sendApply();
}

WraithW60Error WraithW60::sendRawReport(const uint8_t* data, size_t length) {
    if (!pImpl->m_device->sendReport(protocol::REPORT_ID, data, length))
        return WraithW60Error::error(ErrorCode::SendFailed, "Raw report send failed");
    return WraithW60Error::success();
}

} // namespace wraith
