# Wraith W60 Library

Wraith W60 mekanik klavyesi için C++ kontrol kütüphanesi, CLI aracı ve OpenRGB entegrasyonu.

## Özellikler

- USB HID üzerinden klavye iletişimi
- Backlight (arka ışık) kontrolü: mod, renk, hız
- Underglow (alt aydınlatma) kontrolü: mod, renk, hız
- Per-key RGB renk kontrolü (126 tuş)
- 15 farklı aydınlatma modu
- OpenRGB eklentisi ile uyumlu
- C++17 kütüphane + komut satırı aracı

## Kurulum

### Bağımlılıklar

- C++17 uyumlu derleyici (GCC, Clang, MSVC)
- CMake 3.16+
- [hidapi](https://github.com/libusb/hidapi) kütüphanesi

#### Linux (Debian/Ubuntu)

```bash
sudo apt install cmake g++ libhidapi-dev
```

#### Windows (vcpkg)

```bash
vcpkg install hidapi
```

#### macOS

```bash
brew install cmake hidapi
```

### Derleme

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Opsiyonel CMake Seçenekleri

| Seçenek | Varsayılan | Açıklama |
|---------|------------|----------|
| `WRAITH_BUILD_CLI` | ON | CLI aracını derle |
| `WRAITH_BUILD_OPENRGB` | OFF | OpenRGB eklentisini derle |
| `WRAITH_BUILD_TESTS` | OFF | Testleri derle |

```bash
# OpenRGB entegrasyonu ile derleme
cmake .. -DWRAITH_BUILD_OPENRGB=ON
```

## Kullanım

### CLI Aracı

```bash
# Cihazları listele
wraith_cli enumerate

# Cihaz bilgisi göster
wraith_cli info

# Mevcut modları listele
wraith_cli modes

# Arka ışık ayarla (sabit kırmızı)
wraith_cli backlight static 255 0 0

# Arka ışık ayarla (nefes alma efekti, mavi, özel hız)
wraith_cli breathe 0 0 255 128

# Underglow ayarla (dalga efekti)
wraith_cli underglow wave 0 255 0

# Tek tuş rengi ayarla (tuş 0 = kırmızı)
wraith_cli key 0 255 0 0

# Tüm tuşları tek renge ayarla
wraith_cli all 255 255 255

# Tuş aralığı ayarla (tuş 0-12 arası = kırmızı)
wraith_cli range 0 12 255 0 0

# Değişiklikleri uygula
wraith_cli apply
```

### Kütüphane Kullanımı (C++)

```cpp
#include "wraith/WraithW60.h"

using namespace wraith;

// Cihazları listele
auto devices = WraithW60::enumerateDevices();

// Cihaza bağlan
auto keyboard = WraithW60::openDevice();
if (!keyboard) {
    // Hata yönetimi
    return;
}

// Arka ışık: sabit kırmızı
keyboard->setBacklight(LightingMode::Static, 255, 0, 0);

// Underglow: dalga efekti, yeşil
keyboard->setUnderglow(LightingMode::Wave, 0, 255, 0, 0x80);

// Tek tuş rengi
keyboard->setKeyColor(0, 0, 0, 255);  // Tuş 0: mavi

// Tüm tuşları beyaz yap
std::vector<Color> white(126, Color(255, 255, 255));
keyboard->setAllKeys(white);

// Değişiklikleri uygula
keyboard->apply();
```

### Aydınlatma Modları

| Mod | Değer | Açıklama |
|-----|-------|----------|
| `static` | 0x00 | Sabit renk |
| `breathe` | 0x01 | Nefes alma |
| `wave` | 0x02 | Dalga |
| `neon` | 0x03 | Neon |
| `sparkle` | 0x04 | Kıvılcım |
| `reactive` | 0x07 | Tepkili |
| `ripple` | 0x0A | Dalga efekti |
| `mod6` | 0x06 | Mod 6 |
| `mod8` | 0x08 | Mod 8 |
| `mod9` | 0x09 | Mod 9 |
| `mod11` | 0x0B | Mod 11 |
| `mod12` | 0x0C | Mod 12 |
| `mod14` | 0x0E | Mod 14 |
| `mod15` | 0x0F | Mod 15 |
| `mod16` | 0x10 | Mod 16 |

## OpenRGB Entegrasyonu

OpenRGB eklentisi, klavyenizi OpenRGB yazılımıyla kontrol etmenizi sağlar.

```bash
# OpenRGB ile derleme
cmake .. -DWRAITH_BUILD_OPENRGB=ON
cmake --build .
```

Derlenen eklenti `.dll`/`.so` dosyasını OpenRGB eklenti klasörüne kopyalayın.

## Testler

```bash
cmake .. -DWRAITH_BUILD_TESTS=ON
cmake --build .
ctest
```

## Proje Yapısı

```
WraithW60Library/
├── CMakeLists.txt
├── include/wraith/           # Başlık dosyaları
│   ├── WraithW60.h          # Ana kütüphane
│   ├── WraithW60Device.h    # Cihaz yönetimi
│   ├── WraithW60Leds.h      # LED ve tuş tanımları
│   ├── WraithW60Modes.h     # Aydınlatma modları
│   ├── WraithW60Protocol.h  # USB protokolü
│   ├── WraithW60Types.h     # Tip tanımları
│   └── WraithW60Error.h     # Hata yönetimi
├── src/
│   ├── core/                 # Kütüphane çekirdeği
│   ├── cli/                  # Komut satırı aracı
│   ├── openrgb/             # OpenRGB eklentisi
│   └── platform/            # Platform bağımlı kod
└── tests/                   # Test dosyaları
```

## Lisans

MIT License
