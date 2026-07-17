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

---

## CLI Aracı Kullanım Kılavuzu

`wraith_cli` komut satırı aracı, klavyenizi terminallerden kontrol etmenizi sağlar. Tüm komutlar `wraith_cli <komut> [parametreler]` formatındadır.

### Temel Komutlar

#### 1. `enumerate` - Bağlı Cihazları Listele

Bilgisayara bağlı tüm Wraith W60 cihazlarını bulur ve gösterir.

```bash
wraith_cli enumerate
```

**Çıktı örneği:**
```
Found 1 device(s):
  VID: 0x2e3c PID: 0xc365 Serial: ABC123 Path: /dev/hidraw0
```

Bu komut cihaza bağlanmadan çalışır. Bağlantı sorunlarında önce bu komutu çalıştırın.

#### 2. `info` - Cihaz Bilgisi

Bağlı cihazın detaylı bilgilerini gösterir (VID, PID, seri numarası).

```bash
wraith_cli info
```

**Çıktı örneği:**
```
Connected to Wraith W60 (Serial: ABC123)
  VID:     0x2e3c
  PID:     0xc365
  Serial:  ABC123
```

#### 3. `modes` - Aydınlatma Modlarını Listele

Kullanılabilir tüm aydınlatma modlarını ve özelliklerini gösterir.

```bash
wraith_cli modes
```

**Çıktı örneği:**
```
Available lighting modes:
  static (0x00) - Static
  breathe (0x01) - Breathing [speed]
  wave (0x02) - Wave [speed]
  neon (0x03) - Neon
  ...
```

### Aydınlatma Kontrolleri

#### 4. `backlight` - Arka Işık Ayarlama

Klavyenin arka ışık (backlight) aydınlatmasını ayarlar.

**Sözdizimi:**
```bash
wraith_cli backlight <mod> <R> <G> <B> [hız]
```

- **mod**: Aydınlatma modu adı (static, breathe, wave, vb.)
- **R, G, B**: Renk değerleri (0-255 arası)
- **hız** (isteğe bağlı): Animasyon hızı (0-255, varsayılan: 128)

**Örnekler:**
```bash
# Sabit kırmızı arka ışık
wraith_cli backlight static 255 0 0

# Nefes alma efekti, mavi renk
wraith_cli backlight breathe 0 0 255

# Dalga efekti, yeşil renk, hızlı
wraith_cli backlight wave 0 255 0 200

# Neon efekti, mor renk, yavaş
wraith_cli backlight neon 128 0 255 50
```

#### 5. `underglow` - Alt Aydınlatma Ayarlama

Klavyenin alt aydınlatma (underglow) bölgesini ayarlar.

**Sözdizimi:**
```bash
wraith_cli underglow <mod> <R> <G> <B> [hız]
```

**Örnekler:**
```bash
# Sabit beyaz alt aydınlatma
wraith_cli underglow static 255 255 255

# Tepkili mod, kırmızı
wraith_cli underglow reactive 255 0 0

# Dalga efekti, renk geçişli
wraith_cli underglow wave 0 128 255 160
```

### Tuş Kontrolleri

#### 6. `key` - Tek Tuş Rengi Değiştirme

Belirli bir tuşun rengini ayarlar.

**Sözdizimi:**
```bash
wraith_cli key <tuş_indeksi> <R> <G> <B>
```

- **tuş_indeksi**: 0-125 arası tuş numarası (0 = Esc, 1 = F1, vb.)

**Örnekler:**
```bash
# Tuş 0 (Esc) kırmızı yap
wraith_cli key 0 255 0 0

# Tuş 10 (Enter civarı) mavi yap
wraith_cli key 10 0 0 255

# Tuş 50 (boşluk tuşu civarı) yeşil yap
wraith_cli key 50 0 255 0
```

#### 7. `all` - Tüm Tuşları Tek Renge Boyama

126 tuşun hepsini aynı renge ayarlar.

**Sözdizimi:**
```bash
wraith_cli all <R> <G> <B>
```

**Örnekler:**
```bash
# Tüm tuşları beyaz yap
wraith_cli all 255 255 255

# Tüm tuşları kırmızı yap
wraith_cli all 255 0 0

# Tüm tuşları kapat (siyah)
wraith_cli all 0 0 0
```

#### 8. `range` - Tuş Aralığı Renklendirme

Belirli bir aralıktaki tuşları aynı renge boyar.

**Sözdizimi:**
```bash
wraith_cli range <başlangıç> <bitiş> <R> <G> <B>
```

- **başlangıç**: İlk tuş indeksi (dahil)
- **bitiş**: Son tuş indeksi (dahil)

**Örnekler:**
```bash
# İlk 13 tuşu (Esc - F12 arası) kırmızı yap
wraith_cli range 0 12 255 0 0

# QWERTY satırını mavi yap (tuş 13-24 arası)
wraith_cli range 13 24 0 0 255

# Son 10 tuşu yeşil yap
wraith_cli range 116 125 0 255 0
```

### Uygulama

#### 9. `apply` - Değişiklikleri Uygula

Yapılan tüm renk ve mod değişikliklerini klavyeye gönderir.

```bash
wraith_cli apply
```

**Önemli:** `key`, `all`, `range` komutları sadece dahili tamponu değiştirir. Değişikliklerin klavyeye yansıması için `apply` komutunu çalıştırmanız gerekir.

### Hızlı Başlangıç Örnekleri

#### Tüm Klavyeyi Kırmızı Yap
```bash
wraith_cli backlight static 255 0 0
wraith_cli underglow static 255 0 0
wraith_cli all 255 0 0
wraith_cli apply
```

#### Dalga Efektli Renkli Klavye
```bash
wraith_cli backlight wave 0 128 255
wraith_cli underglow wave 255 0 128
wraith_cli apply
```

#### Oyun Modu (WASD Kırmızı, Geri Kalan Mavi)
```bash
wraith_cli all 0 0 255
wraith_cli range 17 19 255 0 0
wraith_cli key 24 255 0 0
wraith_cli apply
```

#### Sessiz Mod (Düşük Işık)
```bash
wraith_cli backlight static 10 10 10
wraith_cli underglow static 5 5 5
wraith_cli all 10 10 10
wraith_cli apply
```

### Komut Özeti Tablosu

| Komut | Sözdizimi | Açıklama |
|-------|-----------|----------|
| `enumerate` | `wraith_cli enumerate` | Bağlı cihazları listele |
| `info` | `wraith_cli info` | Cihaz bilgisi göster |
| `modes` | `wraith_cli modes` | Aydınlatma modlarını listele |
| `backlight` | `wraith_cli backlight <mod> <R> <G> <B> [hız]` | Arka ışık ayarla |
| `underglow` | `wraith_cli underglow <mod> <R> <G> <B> [hız]` | Alt aydınlatma ayarla |
| `key` | `wraith_cli key <index> <R> <G> <B>` | Tek tuş rengi |
| `all` | `wraith_cli all <R> <G> <B>` | Tüm tuşları boyama |
| `range` | `wraith_cli range <baş> <bit> <R> <G> <B>` | Tuş aralığı |
| `apply` | `wraith_cli apply` | Değişiklikleri uygula |

### Hata Durumları

| Hata | Sebep | Çözüm |
|------|-------|-------|
| `No Wraith W60 device found` | Cihaz bağlı değil | USB kablosunu kontrol edin |
| `Device not found` | Cihaz tanınamadı | `wraith_cli enumerate` ile kontrol edin |
| `Send failed` | Veri gönderilemedi | USB bağlantısını kontrol edin |
| `Permission denied` | Yetki hatası | Linux'ta `sudo` kullanın veya udev kuralı ekleyin |

#### Linux udev Kuralı (Opsiyonel)

Linux'ta root olmadan erişim için:

```bash
sudo nano /etc/udev/rules.d/99-wraith-w60.rules
```

İçeriğe şunu ekleyin:
```
SUBSYSTEM=="hidraw", ATTRS{idVendor}=="2e3c", ATTRS{idProduct}=="c365", MODE="0666"
```

Sonra:
```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

---

## Kütüphane Kullanımı (C++)

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

## Aydınlatma Modları

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
├── README.md
├── .gitignore
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
