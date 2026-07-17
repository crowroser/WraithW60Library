# Wraith W60 — OpenRGB Entegrasyonu

Wraith W60 klavyenizi OpenRGB ile kontrol etmek için bu talimatlarý izleyin.

## Yöntem 1: Kaynak Koduna Ekleme (Önerilen)

### 1. OpenRGB'i Clone Edin

```bash
git clone https://github.com/CalcProgrammer1/OpenRGB.git
cd OpenRGB
```

### 2. Wraith W60 Driver Dosyalarýný Kopyalayýn

```bash
# Linux
cp -r /path/to/Klavye/src/openrgb/* Controllers/WraithW60KeyboardController/

# Windows (MSYS2 UCRT64)
xcopy /E /I C:\path\to\Klavye\src\openrgb Controllers\WraithW60KeyboardController
```

Kopyalanan dosyalar:
```
Controllers/WraithW60KeyboardController/
+¦¦ WraithW60KeyboardController.h
+¦¦ WraithW60KeyboardController.cpp
+¦¦ RGBController_WraithW60.h
+¦¦ RGBController_WraithW60Keyboard.cpp
L¦¦ WraithW60KeyboardControllerDetect.cpp
```

### 3. OpenRGB'i Derleyin

#### Linux

```bash
sudo apt-get install cmake g++ libhidapi-dev libusb-1.0-0-dev pkg-config \
    qtbase5-dev qttools5-dev

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Windows (MSYS2 UCRT64)

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake \
    mingw-w64-ucrt-x86_64-qt5-base mingw-w64-ucrt-x86_64-qt5-tools \
    mingw-w64-ucrt-x86_64-hidapi mingw-w64-ucrt-x86_64-libusb

qmake-qt5 OpenRGB.pro -spec win32-g++
make -j4
```

### 4. OpenRGB'i Çalýþtýrýn

```bash
# Linux
./openrgb

# Windows
openrgb.exe
```

OpenRGB baþlatýldýðýnda "Wraith W60 Keyboard" cihazýný otomatik olarak algýlamalýdýr.

## Yöntem 2: Mevcut OpenRGB'e Plugin Olarak Ekleme

Eðer OpenRGB zaten kuruluysa ve kaynak kodunu derlemek istemiyorsanýz, driver dosyalarýný OpenRGB source tree'e ekleyip yeniden derlemeniz gerekir. Bu, OpenRGB'in kendi derleme sürecini gerektirir.

## OpenRGB'de Kullaným

### Keþif

OpenRGB baþlatýldýðýnda:
1. **Devices** sekmesine gidin
2. "Wraith W60 Keyboard" cihazýný bulun
3. Cihaza týklayýn

### Zone'lar

OpenRGB'de 3 zone görüntülenir:

| Zone | Tür | LED Sayýsý | Açýklama |
|------|-----|-----------|----------|
| Backlight | Single | 1 | Tuþ üstü LED (tüm tuþlar ayný renk) |
| Underglow | Single | 1 | Kasa LED (tüm kasa ayný renk) |
| Per-Key | Matrix | 79 | Her tuþ için ayrý renk (60% layout) |

### Modlar

| Mod | Hýz | Renk | Açýklama |
|-----|-----|------|----------|
| Static | - | Evet | Sabit renk |
| Breathe | Evet | Evet | Nefes animasyonu |
| Wave | Evet | - | Dalga animasyonu |
| Neon | Evet | - | Neon efekti |
| Sparkle | Evet | - | Pýrýltý efekti |
| Reactive | Evet | Evet | Tuþ basma tepkisi |
| Ripple | Evet | Evet | Dalga tepkisi |

### Per-Key RGB

Per-Key modunda:
1. Mode'ý "Static" olarak ayarlayýn
2. "Direct" seçeneðini iþaretleyin
3. Her tuþa ayrý renk verebilirsiniz
4. "Apply" butonuna týklayýn

### Profil Kaydetme

OpenRGB profillerini kullanarak farklý renk düzenlerini kaydedebilirsiniz:
1. Ýstediðiniz düzeni ayarlayýn
2. **Profiles** sekmesine gidin
3. **Save Profile** butonuna týklayýn
4. Dosya adý verin

## Cihaz Bilgileri

| Özellik | Deðer |
|---------|-------|
| VID | 0x2E3C |
| PID | 0xC365 |
| Interface | 2 |
| Usage Page | 0xFF1B |
| Usage | 0x91 |
| Report ID | 0x01 |
| Paket Boyutu | 64 byte |

## Key Mapping (Physical LED Indices)

The Wraith W60 uses physical LED indices (0-125) for per-key control. Verified via hardware testing.

### Number Row (Row 0)
| Key | Index | Key | Index |
|-----|-------|-----|-------|
| Esc | 22 | 8 | 30 |
| 1 | 23 | 9 | 31 |
| 2 | 24 | 0 | 32 |
| 3 | 25 | - (_) | 33 |
| 4 | 26 | = (+) | 34 |
| 5 | 27 | Bksp | 36 |
| 6 | 28 | | |
| 7 | 29 | | |

### QWERTY Row (Row 1)
| Key | Index | Key | Index |
|-----|-------|-----|-------|
| Tab | 44 | U | 51 |
| Q | 45 | I (ý) | 52 |
| W | 46 | O | 53 |
| E | 47 | P | 54 |
| R | 48 | Ð ([{) | 55 |
| T | 49 | Ü (]}) | 56 |
| Y | 50 | | |

### Home Row (Row 2)
| Key | Index | Key | Index |
|-----|-------|-----|-------|
| Caps | 66 | K | 75 |
| A | 68 | L | 76 |
| S | 69 | Þ (;:) | 77 |
| D | 70 | Ý (') | 78 |
| F | 71 | Enter | 80 |
| G | 72 | | |
| H | 73 | | |
| J | 74 | | |

### Shift Row (Row 3)
| Key | Index | Key | Index |
|-----|-------|-----|-------|
| LShift | 88 | Ö (,<) | 97 |
| Z | 90 | Ç (.>) | 98 |
| X | 91 | / (?) | 99 |
| C | 92 | RShift | 100 |
| V | 93 | | |
| B | 94 | | |
| N | 95 | | |
| M | 96 | | |

### Bottom Row (Row 4)
| Key | Index | Key | Index |
|-----|-------|-----|-------|
| LCtrl | 110 | RAlt | 119 |
| Win | 111 | Menu | 120 |
| LAlt | 112 | RCtrl | 121 |
| Space | 116 | Fn | 122 |

## Sorun Giderme


### Cihaz Algýlanmýyor

1. USB baðlantýsýný kontrol edin
2. `wraith enumerate` ile cihazýn görünüp görünmediðini kontrol edin
3. OpenRGB'i yönetici haklarýyla çalýþtýrýn (Linux: `sudo ./openrgb`)

### Renkler Yanlýþ

1. Backlight ve Underglow karýþtýrýlmýþ olabilir
2. Per-Key modunda farklý chunk'lar farklý renklere sahip olabilir
3. Doðru zone'ý seçtiðinizden emin olun

### Performans Sorunu

Per-Key RGB 8 chunk halinde gönderilir (her biri 18 tuþ). 126 tuþun tamamýný güncellemek ~80ms sürer.

## Katkýda Bulunma

Yeni özellikler veya hata düzeltmeleri için:
1. Fork edin
2. Deðiþikliklerinizi yapýn
3. Test edin
4. Pull request açýn

## Lisans

GPL-2.0-or-later
