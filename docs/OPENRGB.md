# Wraith W60 — OpenRGB Entegrasyonu

Wraith W60 klavyenizi OpenRGB ile kontrol etmek için bu talimatları izleyin.

## Yöntem 1: Kaynak Koduna Ekleme (Önerilen)

### 1. OpenRGB'i Clone Edin

```bash
git clone https://github.com/CalcProgrammer1/OpenRGB.git
cd OpenRGB
```

### 2. Wraith W60 Driver Dosyalarını Kopyalayın

```bash
# Linux
cp -r /path/to/Klavye/src/openrgb/* Controllers/WraithW60KeyboardController/

# Windows (MSYS2 UCRT64)
xcopy /E /I C:\path\to\Klavye\src\openrgb Controllers\WraithW60KeyboardController
```

Kopyalanan dosyalar:
```
Controllers/WraithW60KeyboardController/
├── WraithW60KeyboardController.h
├── WraithW60KeyboardController.cpp
├── RGBController_WraithW60.h
├── RGBController_WraithW60Keyboard.cpp
└── WraithW60KeyboardControllerDetect.cpp
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

### 4. OpenRGB'i Çalıştırın

```bash
# Linux
./openrgb

# Windows
openrgb.exe
```

OpenRGB başlatıldığında "Wraith W60 Keyboard" cihazını otomatik olarak algılamalıdır.

## Yöntem 2: Mevcut OpenRGB'e Plugin Olarak Ekleme

Eğer OpenRGB zaten kuruluysa ve kaynak kodunu derlemek istemiyorsanız, driver dosyalarını OpenRGB source tree'e ekleyip yeniden derlemeniz gerekir. Bu, OpenRGB'in kendi derleme sürecini gerektirir.

## OpenRGB'de Kullanım

### Keşif

OpenRGB başlatıldığında:
1. **Devices** sekmesine gidin
2. "Wraith W60 Keyboard" cihazını bulun
3. Cihaza tıklayın

### Zone'lar

OpenRGB'de 3 zone görüntülenir:

| Zone | Tür | LED Sayısı | Açıklama |
|------|-----|-----------|----------|
| Backlight | Single | 1 | Tuş üstü LED (tüm tuşlar aynı renk) |
| Underglow | Single | 1 | Kasa LED (tüm kasa aynı renk) |
| Per-Key | Matrix | 79 | Her tuş için ayrı renk (60% layout) |

### Modlar

| Mod | Hız | Renk | Açıklama |
|-----|-----|------|----------|
| Static | - | Evet | Sabit renk |
| Breathe | Evet | Evet | Nefes animasyonu |
| Wave | Evet | - | Dalga animasyonu |
| Neon | Evet | - | Neon efekti |
| Sparkle | Evet | - | Pırıltı efekti |
| Reactive | Evet | Evet | Tuş basma tepkisi |
| Ripple | Evet | Evet | Dalga tepkisi |

### Per-Key RGB

Per-Key modunda:
1. Mode'ı "Static" olarak ayarlayın
2. "Direct" seçeneğini işaretleyin
3. Her tuşa ayrı renk verebilirsiniz
4. "Apply" butonuna tıklayın

### Profil Kaydetme

OpenRGB profillerini kullanarak farklı renk düzenlerini kaydedebilirsiniz:
1. İstediğiniz düzeni ayarlayın
2. **Profiles** sekmesine gidin
3. **Save Profile** butonuna tıklayın
4. Dosya adı verin

## Cihaz Bilgileri

| Özellik | Değer |
|---------|-------|
| VID | 0x2E3C |
| PID | 0xC365 |
| Interface | 2 |
| Usage Page | 0xFF1B |
| Usage | 0x91 |
| Report ID | 0x01 |
| Paket Boyutu | 64 byte |

## Sorun Giderme

### Cihaz Algılanmıyor

1. USB bağlantısını kontrol edin
2. `wraith enumerate` ile cihazın görünüp görünmediğini kontrol edin
3. OpenRGB'i yönetici haklarıyla çalıştırın (Linux: `sudo ./openrgb`)

### Renkler Yanlış

1. Backlight ve Underglow karıştırılmış olabilir
2. Per-Key modunda farklı chunk'lar farklı renklere sahip olabilir
3. Doğru zone'ı seçtiğinizden emin olun

### Performans Sorunu

Per-Key RGB 8 chunk halinde gönderilir (her biri 18 tuş). 126 tuşun tamamını güncellemek ~80ms sürer.

## Katkıda Bulunma

Yeni özellikler veya hata düzeltmeleri için:
1. Fork edin
2. Değişikliklerinizi yapın
3. Test edin
4. Pull request açın

## Lisans

GPL-2.0-or-later
