#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "wraith/WraithW60.h"
#include "wraith/WraithW60Modes.h"

using namespace wraith;

void printUsage() {
    std::cout << "Wraith W60 Keyboard CLI v1.0\n"
              << "=============================\n\n"
              << "Usage: wraith_cli <command> [options]\n\n"
              << "Commands:\n"
              << "  info                          Show device information\n"
              << "  backlight <mode> <R> <G> <B> [speed]   Set backlight\n"
              << "  underglow <mode> <R> <G> <B> [speed]   Set underglow\n"
              << "  key <index> <R> <G> <B>       Set single key color\n"
              << "  all <R> <G> <B>               Set all keys to color\n"
              << "  range <start> <end> <R> <G> <B>  Set key range\n"
              << "  apply                         Apply changes\n"
              << "  modes                         List available modes\n"
              << "  enumerate                     List connected devices\n"
              << "\nModes:\n"
              << "  static, breathe, wave, neon, sparkle,\n"
              << "  reactive, ripple, mod6, mod8, mod9,\n"
              << "  mod11, mod12, mod14, mod15, mod16\n"
              << "\nExamples:\n"
              << "  wraith_cli backlight static 255 0 0\n"
              << "  wraith_cli key 0 0 255 0\n"
              << "  wraith_cli all 255 255 255\n"
              << "  wraith_cli range 0 12 255 0 0\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string command = argv[1];

    if (command == "enumerate") {
        auto devices = WraithW60::enumerateDevices();
        if (devices.empty()) {
            std::cout << "No Wraith W60 devices found.\n";
            return 1;
        }
        std::cout << "Found " << devices.size() << " device(s):\n";
        for (const auto& d : devices) {
            std::cout << "  VID: 0x" << std::hex << d.vid
                      << " PID: 0x" << d.pid
                      << " Serial: " << d.serial
                      << " Path: " << d.path << "\n";
        }
        return 0;
    }

    if (command == "modes") {
        std::cout << "Available lighting modes:\n";
        for (const auto& m : ModeRegistry::getAllModes()) {
            std::cout << "  " << m.name << " (0x"
                      << std::hex << static_cast<int>(m.mode) << std::dec << ")"
                      << " - " << m.displayName;
            if (m.supportsSpeed) std::cout << " [speed]";
            if (m.isReactive) std::cout << " [reactive]";
            std::cout << "\n";
        }
        return 0;
    }

    auto device = WraithW60::openDevice();
    if (!device) {
        std::cerr << "Error: No Wraith W60 device found.\n";
        return 1;
    }

    std::cout << "Connected to Wraith W60 (Serial: " << device->getSerial() << ")\n";

    if (command == "info") {
        std::cout << "  VID:     0x" << std::hex << device->getVID() << "\n"
                  << "  PID:     0x" << device->getPID() << "\n"
                  << "  Serial:  " << std::dec << device->getSerial() << "\n";
    }
    else if (command == "backlight" && argc >= 5) {
        auto mi = ModeRegistry::findByName(argv[2]);
        uint8_t r = static_cast<uint8_t>(std::stoi(argv[3]));
        uint8_t g = static_cast<uint8_t>(std::stoi(argv[4]));
        uint8_t b = static_cast<uint8_t>(std::stoi(argv[5]));
        uint8_t speed = (argc > 6) ? static_cast<uint8_t>(std::stoi(argv[6])) : 0x80;

        auto err = device->setBacklight(mi.mode, r, g, b, speed);
        std::cout << (err.isSuccess() ? "Backlight set successfully.\n" : ("Error: " + err.message() + "\n"));
    }
    else if (command == "underglow" && argc >= 5) {
        auto mi = ModeRegistry::findByName(argv[2]);
        uint8_t r = static_cast<uint8_t>(std::stoi(argv[3]));
        uint8_t g = static_cast<uint8_t>(std::stoi(argv[4]));
        uint8_t b = static_cast<uint8_t>(std::stoi(argv[5]));
        uint8_t speed = (argc > 6) ? static_cast<uint8_t>(std::stoi(argv[6])) : 0x80;

        auto err = device->setUnderglow(mi.mode, r, g, b, speed);
        std::cout << (err.isSuccess() ? "Underglow set successfully.\n" : ("Error: " + err.message() + "\n"));
    }
    else if (command == "key" && argc >= 5) {
        uint8_t idx = static_cast<uint8_t>(std::stoi(argv[2]));
        uint8_t r = static_cast<uint8_t>(std::stoi(argv[3]));
        uint8_t g = static_cast<uint8_t>(std::stoi(argv[4]));
        uint8_t b = static_cast<uint8_t>(std::stoi(argv[5]));

        auto err = device->setKeyColor(idx, r, g, b);
        std::cout << (err.isSuccess() ? "Key color set.\n" : ("Error: " + err.message() + "\n"));
    }
    else if (command == "all" && argc >= 4) {
        uint8_t r = static_cast<uint8_t>(std::stoi(argv[2]));
        uint8_t g = static_cast<uint8_t>(std::stoi(argv[3]));
        uint8_t b = static_cast<uint8_t>(std::stoi(argv[4]));

        std::vector<Color> colors(126, Color(r, g, b));
        auto err = device->setAllKeys(colors);
        std::cout << (err.isSuccess() ? "All keys set.\n" : ("Error: " + err.message() + "\n"));
    }
    else if (command == "range" && argc >= 7) {
        uint8_t start = static_cast<uint8_t>(std::stoi(argv[2]));
        uint8_t end = static_cast<uint8_t>(std::stoi(argv[3]));
        uint8_t r = static_cast<uint8_t>(std::stoi(argv[4]));
        uint8_t g = static_cast<uint8_t>(std::stoi(argv[5]));
        uint8_t b = static_cast<uint8_t>(std::stoi(argv[6]));

        auto err = device->setKeyRange(start, end, r, g, b);
        std::cout << (err.isSuccess() ? "Key range set.\n" : ("Error: " + err.message() + "\n"));
    }
    else if (command == "apply") {
        auto err = device->apply();
        std::cout << (err.isSuccess() ? "Applied.\n" : ("Error: " + err.message() + "\n"));
    }
    else {
        printUsage();
        return 1;
    }

    return 0;
}
