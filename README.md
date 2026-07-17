# Wraith W60 Keyboard RGB Controller

[English](#english) | [Türkçe](#türkçe)

---

## English

A standalone C++ library, CLI tool, and OpenRGB driver for controlling the **Wraith W60** mechanical keyboard's RGB lighting.

### Features

- **Backlight** (under-key LEDs): 16 lighting modes
- **Underglow** (frame LEDs): Static, breathe, wave, neon
- **Per-key RGB**: Individual color control for 126 LEDs (8 chunks)
- **OpenRGB integration**: Ready-to-use driver files
- **Cross-platform**: Windows, Linux

### Quick Start

#### Download

Download the latest release from [GitHub Releases](../../releases):

- **Windows**: `wraith-w60-windows-x64.zip` — portable, no installation needed
- **Linux**: `wraith-w60-linux-x64.tar.gz` — extract and run

#### Windows

```cmd
cd wraith-w60-windows-x64
wraith.exe backlight static 255 0 0
```

#### Linux

```bash
chmod +x wraith
./wraith backlight static 255 0 0
```

### Commands

| Command | Description | Example |
|---------|-------------|---------|
| `backlight <mode> <R> <G> <B> [speed]` | Set backlight | `backlight static 255 0 0` |
| `underglow <mode> <R> <G> <B> [speed]` | Set underglow | `underglow breathe 0 0 255` |
| `key <index> <R> <G> <B>` | Set single key | `key 110 0 0 255` |
| `all <R> <G> <B>` | Set all keys | `all 255 255 255` |
| `range <start> <end> <R> <G> <B>` | Set key range | `range 0 12 255 0 0` |
| `enumerate` | List devices | `enumerate` |
| `modes` | List modes | `modes` |

### Lighting Modes

| Mode | Value | Features |
|------|-------|----------|
| `static` | 0x00 | Color |
| `breathe` | 0x01 | Color + Speed |
| `wave` | 0x02 | Speed + Direction |
| `neon` | 0x03 | Speed |
| `sparkle` | 0x04 | Speed |
| `reactive` | 0x07 | Color + Speed |
| `ripple` | 0x0A | Color + Speed |

### Key Indices (Verified)

| Key | Physical Index |
|-----|---------------|
| LCtrl | 110 |
| RCtrl | 121 |
| Menu | 120 |
| Fn | 122 |

### Device Info

| Property | Value |
|----------|-------|
| VID | 0x2E3C |
| PID | 0xC365 |
| Interface | 2 |
| Usage Page | 0xFF1B |
| Usage | 0x91 |
| Report ID | 0x01 |
| Packet Size | 64 bytes |

### C++ API

```cpp
#include "wraith/WraithW60.h"

auto device = wraith::WraithW60::openDevice();

// Set backlight to red
device->setBacklight(wraith::LightingMode::Static, 255, 0, 0);

// Set underglow to blue
device->setUnderglow(wraith::LightingMode::Breathe, 0, 0, 255);

// Set LCtrl to blue
device->setKeyColor(110, 0, 0, 255);

// Set all keys to white
std::vector<wraith::Color> colors(126, wraith::Color(255, 255, 255));
device->setAllKeys(colors);

device->apply();
```

### Building from Source

```bash
# Install dependencies (Linux)
sudo apt-get install cmake g++ libhidapi-dev

# Build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Windows (MSYS2 UCRT64)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### OpenRGB Integration

See [docs/OPENRGB.md](docs/OPENRGB.md) for detailed instructions.

### License

GPL-2.0-or-later

---

## Türkçe

**Wraith W60** mekanik RGB klavyesinin aydınlatmasını kontrol etmek için bağımsız C++ kütüphanesi, CLI aracı ve OpenRGB sürücüsü.

### Özellikler

- **Backlight** (tuş üstü LED): 16 aydınlatma modu
- **Underglow** (kasa LED): Statik, nefes, dalga, neon
- **Per-key RGB**: 126 LED için tek tek renk kontrolü (8 chunk)
- **OpenRGB entegrasyonu**: Hazır sürücü dosyaları
- **Cross-platform**: Windows, Linux

### Hızlı Başlangıç

#### İndirme

[GitHub Releases](../../releases) adresinden en son sürümü indirin:

- **Windows**: `wraith-w60-windows-x64.zip` — portatif, kurulum gerektirmez
- **Linux**: `wraith-w60-linux-x64.tar.gz` — çıkarın ve çalıştırın

#### Windows

```cmd
cd wraith-w60-windows-x64
wraith.exe backlight static 255 0 0
```

#### Linux

```bash
chmod +x wraith
./wraith backlight static 255 0 0
```

### Komutlar

| Komut | Açıklama | Örnek |
|-------|----------|-------|
| `backlight <mode> <R> <G> <B> [speed]` | Backlight ayarla | `backlight static 255 0 0` |
| `underglow <mode> <R> <G> <B> [speed]` | Underglow ayarla | `underglow breathe 0 0 255` |
| `key <index> <R> <G> <B>` | Tek tuş renk | `key 110 0 0 255` |
| `all <R> <G> <B>` | Tüm tuşlar | `all 255 255 255` |
| `range <start> <end> <R> <G> <B>` | Tuş aralığı | `range 0 12 255 0 0` |
| `enumerate` | Cihazları listele | `enumerate` |
| `modes` | Modları listele | `modes` |

### Aydınlatma Modları

| Mod | Değer | Özellik |
|-----|-------|---------|
| `static` | 0x00 | Renk |
| `breathe` | 0x01 | Renk + Hız |
| `wave` | 0x02 | Hız + Yön |
| `neon` | 0x03 | Hız |
| `sparkle` | 0x04 | Hız |
| `reactive` | 0x07 | Renk + Hız |
| `ripple` | 0x0A | Renk + Hız |

### Tuş İndeksleri (Doğrulanmış)

| Tuş | Fiziksel İndeks |
|-----|----------------|
| LCtrl | 110 |
| RCtrl | 121 |
| Menu | 120 |
| Fn | 122 |

### Cihaz Bilgileri

| Özellik | Değer |
|---------|-------|
| VID | 0x2E3C |
| PID | 0xC365 |
| Interface | 2 |
| Usage Page | 0xFF1B |
| Usage | 0x91 |
| Report ID | 0x01 |
| Paket Boyutu | 64 byte |

### C++ Kullanımı

```cpp
#include "wraith/WraithW60.h"

auto device = wraith::WraithW60::openDevice();

// Kırmızı backlight
device->setBacklight(wraith::LightingMode::Static, 255, 0, 0);

// Mavi underglow
device->setUnderglow(wraith::LightingMode::Breathe, 0, 0, 255);

// LCtrl mavi
device->setKeyColor(110, 0, 0, 255);

// Tüm tuşları beyaz yap
std::vector<wraith::Color> colors(126, wraith::Color(255, 255, 255));
device->setAllKeys(colors);

device->apply();
```

### Kaynaktan Derleme

```bash
# Bağımlılıkları kurun (Linux)
sudo apt-get install cmake g++ libhidapi-dev

# Derleyin
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Windows (MSYS2 UCRT64)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### OpenRGB Entegrasyonu

Ayrıntılı talimatlar için [docs/OPENRGB.md](docs/OPENRGB.md) dosyasına bakın.

### Lisans

GPL-2.0-or-later
