#pragma once

#include <cstdint>
#include <string>

namespace wraith {

enum class ErrorCode : uint8_t {
    Success = 0,
    DeviceNotFound,
    DeviceBusy,
    ConnectionFailed,
    SendFailed,
    ReceiveFailed,
    InvalidParameter,
    InvalidMode,
    InvalidKeyIndex,
    InvalidChunkIndex,
    InvalidColor,
    NotSupported,
    TimeoutError,
    PermissionError,
    UnknownError
};

class WraithW60Error {
public:
    WraithW60Error() : m_code(ErrorCode::Success) {}
    WraithW60Error(ErrorCode code) : m_code(code), m_message(defaultMessage(code)) {}
    WraithW60Error(ErrorCode code, const std::string& message)
        : m_code(code), m_message(message) {}

    bool isSuccess() const { return m_code == ErrorCode::Success; }
    bool isError() const { return m_code != ErrorCode::Success; }
    ErrorCode code() const { return m_code; }
    const std::string& message() const { return m_message; }
    const char* what() const { return m_message.c_str(); }

    operator bool() const { return isSuccess(); }

    static WraithW60Error success() { return WraithW60Error(); }
    static WraithW60Error error(ErrorCode code, const std::string& msg = "") {
        return msg.empty() ? WraithW60Error(code) : WraithW60Error(code, msg);
    }

private:
    ErrorCode m_code;
    std::string m_message;

    static const char* defaultMessage(ErrorCode code) {
        switch (code) {
            case ErrorCode::Success:           return "Success";
            case ErrorCode::DeviceNotFound:    return "Device not found";
            case ErrorCode::DeviceBusy:        return "Device busy";
            case ErrorCode::ConnectionFailed:  return "Connection failed";
            case ErrorCode::SendFailed:        return "Send failed";
            case ErrorCode::ReceiveFailed:     return "Receive failed";
            case ErrorCode::InvalidParameter:  return "Invalid parameter";
            case ErrorCode::InvalidMode:       return "Invalid mode";
            case ErrorCode::InvalidKeyIndex:   return "Invalid key index";
            case ErrorCode::InvalidChunkIndex: return "Invalid chunk index";
            case ErrorCode::InvalidColor:      return "Invalid color";
            case ErrorCode::NotSupported:      return "Not supported";
            case ErrorCode::TimeoutError:      return "Timeout";
            case ErrorCode::PermissionError:   return "Permission denied";
            case ErrorCode::UnknownError:      return "Unknown error";
            default:                           return "Unknown error";
        }
    }
};

inline bool isConnectionError(ErrorCode code) {
    return code == ErrorCode::DeviceNotFound ||
           code == ErrorCode::ConnectionFailed ||
           code == ErrorCode::TimeoutError;
}

inline bool isProtocolError(ErrorCode code) {
    return code == ErrorCode::SendFailed ||
           code == ErrorCode::ReceiveFailed ||
           code == ErrorCode::InvalidParameter;
}

} // namespace wraith
