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

### Key Indices (Physical LED Index)

The keyboard uses physical LED indices (0-125) for per-key control. Use setkeys.exe for multiple keys.

| Key | Physical Index | Key | Physical Index |
|-----|---------------|-----|---------------|
| Esc | 22 | A | 68 |
| 1 | 23 | S | 69 |
| 2 | 24 | D | 70 |
| 3 | 25 | F | 71 |
| 4 | 26 | G | 72 |
| 5 | 27 | H | 73 |
| 6 | 28 | J | 74 |
| 7 | 29 | K | 75 |
| 8 | 30 | L | 76 |
| 9 | 31 | Þ | 77 |
| 0 | 32 | Ý | 78 |
| = | 33 | Enter | 80 |
| - | 34 | LShift | 88 |
| Bksp | 36 | Z | 90 |
| Tab | 44 | X | 91 |
| Q | 45 | C | 92 |
| W | 46 | V | 93 |
| E | 47 | B | 94 |
| R | 48 | N | 95 |
| T | 49 | M | 96 |
| Y | 50 | Ö | 97 |
| U | 51 | Ç | 98 |
| ý | 52 | . | 99 |
| O | 53 | RShift | 100 |
| P | 54 | LCtrl | 110 |
| ð | 55 | Win | 111 |
| ü | 56 | Alt | 112 |
| , | 58 | Space | 116 |
| Caps | 66 | Alt | 119 |
| | | Menu | 120 |
| | | RCtrl | 121 |
| | | Fn | 122 |

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

**Wraith W60** mekanik RGB klavyesinin aydýnlatmasýný kontrol etmek için baðýmsýz C++ kütüphanesi, CLI aracý ve OpenRGB sürücüsü.

### Özellikler

- **Backlight** (tuþ üstü LED): 16 aydýnlatma modu
- **Underglow** (kasa LED): Statik, nefes, dalga, neon
- **Per-key RGB**: 126 LED için tek tek renk kontrolü (8 chunk)
- **OpenRGB entegrasyonu**: Hazýr sürücü dosyalarý
- **Cross-platform**: Windows, Linux

### Hýzlý Baþlangýç

#### Ýndirme

[GitHub Releases](../../releases) adresinden en son sürümü indirin:

- **Windows**: `wraith-w60-windows-x64.zip` — portatif, kurulum gerektirmez
- **Linux**: `wraith-w60-linux-x64.tar.gz` — çýkarýn ve çalýþtýrýn

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

| Komut | Açýklama | Örnek |
|-------|----------|-------|
| `backlight <mode> <R> <G> <B> [speed]` | Backlight ayarla | `backlight static 255 0 0` |
| `underglow <mode> <R> <G> <B> [speed]` | Underglow ayarla | `underglow breathe 0 0 255` |
| `key <index> <R> <G> <B>` | Tek tuþ renk | `key 110 0 0 255` |
| `all <R> <G> <B>` | Tüm tuþlar | `all 255 255 255` |
| `range <start> <end> <R> <G> <B>` | Tuþ aralýðý | `range 0 12 255 0 0` |
| `enumerate` | Cihazlarý listele | `enumerate` |
| `modes` | Modlarý listele | `modes` |

### Aydýnlatma Modlarý

| Mod | Deðer | Özellik |
|-----|-------|---------|
| `static` | 0x00 | Renk |
| `breathe` | 0x01 | Renk + Hýz |
| `wave` | 0x02 | Hýz + Yön |
| `neon` | 0x03 | Hýz |
| `sparkle` | 0x04 | Hýz |
| `reactive` | 0x07 | Renk + Hýz |
| `ripple` | 0x0A | Renk + Hýz |

### Tuþ Ýndeksleri (Fiziksel LED Ýndeksi)

Klavye, per-key kontrol için fiziksel LED indeksleri (0-125) kullanýr. Birden fazla tuþ için setkeys.exe kullanýn.

| Tuþ | Fiziksel Ýndeks | Tuþ | Fiziksel Ýndeks |
|-----|----------------|-----|----------------|
| Esc | 22 | A | 68 |
| 1 | 23 | S | 69 |
| 2 | 24 | D | 70 |
| 3 | 25 | F | 71 |
| 4 | 26 | G | 72 |
| 5 | 27 | H | 73 |
| 6 | 28 | J | 74 |
| 7 | 29 | K | 75 |
| 8 | 30 | L | 76 |
| 9 | 31 | Þ | 77 |
| 0 | 32 | Ý | 78 |
| = | 33 | Enter | 80 |
| - | 34 | LShift | 88 |
| Bksp | 36 | Z | 90 |
| Tab | 44 | X | 91 |
| Q | 45 | C | 92 |
| W | 46 | V | 93 |
| E | 47 | B | 94 |
| R | 48 | N | 95 |
| T | 49 | M | 96 |
| Y | 50 | Ö | 97 |
| U | 51 | Ç | 98 |
| ý | 52 | . | 99 |
| O | 53 | RShift | 100 |
| P | 54 | LCtrl | 110 |
| ð | 55 | Win | 111 |
| ü | 56 | Alt | 112 |
| , | 58 | Space | 116 |
| Caps | 66 | Alt | 119 |
| | | Menu | 120 |
| | | RCtrl | 121 |
| | | Fn | 122 |

### Cihaz Bilgileri

| Özellik | Deðer |
|---------|-------|
| VID | 0x2E3C |
| PID | 0xC365 |
| Interface | 2 |
| Usage Page | 0xFF1B |
| Usage | 0x91 |
| Report ID | 0x01 |
| Paket Boyutu | 64 byte |

### C++ Kullanýmý

```cpp
#include "wraith/WraithW60.h"

auto device = wraith::WraithW60::openDevice();

// Kýrmýzý backlight
device->setBacklight(wraith::LightingMode::Static, 255, 0, 0);

// Mavi underglow
device->setUnderglow(wraith::LightingMode::Breathe, 0, 0, 255);

// LCtrl mavi
device->setKeyColor(110, 0, 0, 255);

// Tüm tuþlarý beyaz yap
std::vector<wraith::Color> colors(126, wraith::Color(255, 255, 255));
device->setAllKeys(colors);

device->apply();
```

### Kaynaktan Derleme

```bash
# Baðýmlýlýklarý kurun (Linux)
sudo apt-get install cmake g++ libhidapi-dev

# Derleyin
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Windows (MSYS2 UCRT64)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### OpenRGB Entegrasyonu

Ayrýntýlý talimatlar için [docs/OPENRGB.md](docs/OPENRGB.md) dosyasýna bakýn.

### Lisans

GPL-2.0-or-later
