# TFT 4.3" Asansör Göstergesi

İki karttan oluşan bir asansör kat göstergesinin firmware arşivi:

| Kart | İşlemci | Görev |
|---|---|---|
| **Module** | Allwinner **F1C100S** (ARM926EJ-S) | Ekran sürme — RT-Thread + LittlevGL 7.2.0 |
| **Base board** | Nuvoton **M031LC2AE** (LQFP48) | Arkel kumandası ↔ ekran veri köprüsü, buton/LED/buzzer |

Panel: **480 × 800 portre**, RGB666, ~50 Hz, ST7701S sürücü (BAFANG HD397-40RGB-GL450).

> Kartta LPC1769 **yoktur**. Şemalar (`Module.pdf` → `CPU.SchDoc`, `tft-base-board.pdf` →
> `Nuvoton-M031.SchDoc`) bunu doğrular.

## Veri yolu

```
Arkel kumanda kartı
      │  UART 9600 8N1, 9-bit çerçeveleme (ilk byte'ta bit8=1 → paket başı)
      │  Module→Host: ID 0x0B, 15 byte   |   Host→Module: ID 0x0A, 5 byte
      ▼
Nuvoton M031           ← NXT / RTN butonları, çağrı butonları, LED, PO1/PO2, buzzer
      │  SPI mode 3, 8 bit, 500 kHz  (M031 = slave, F1C100S = master "spi10")
      ▼
Allwinner F1C100S
      │  24-bit paralel RGB (DEFE/DEBE/TCON) + bit-bang SPI ile panel register kurulumu
      ▼
ST7701S 480×800 panel
```

## Depo yapısı

```
savasel-data-converter-52c0573257ab/     Nuvoton M031 — Keil MDK projesi (acm_drive.uvprojx)
  acm_dr.c                               ana döngü, buton/LED/PO, Arkel↔SPI köprüleme
  com.c                                  9-bit UART (USCI0)
  spi_transfer.c                         SPI0 slave, çift tamponlu
  sound.c                                PWM buzzer, kat gongu
  arcode_lop.h                           Arkel LOP paket tanımları

wetransfer_git_2026-07-23_1249/git/
  rt-thread/                             F1C100S — MİKEL kumandası varyantı
  rt-thread_arcode/                      F1C100S — ARKEL kumandası varyantı  ◄── aktif hat
    bsp/allwinner_tina/
      applications/main.c                ST7701S panel init (bit-bang SPI, PA0–PA3)
      applications/app/mainpage.c        tüm ekran mantığı + DİK/YAN yerleşimi
      applications/app/arcode.h          Arkel paketinin ekran tarafındaki karşılığı
      applications/app/serialio.c        SPI master, Arkel paketini M031'den çeker
      applications/app/media/            rakam/harf/ok/mesaj görselleri
      drivers/                           fb, gpio, spi, sdio, i2c, wdt sürücüleri
      littlevGL7.1.0/                    LVGL 7.2.0 + RT-Thread portu
  tools/sunxi-tools/                     sunxi-fel yükleme aracı
```

İki `rt-thread*` ağacı tüm vendor kaynağının **tam kopyası**dır; aralarındaki gerçek fark
yalnızca `mainpage.c`, `com_mi.*`, `mikel.h` ve iki görseldir.

## Derleme

**F1C100S tarafı** (SCons, arm-none-eabi-gcc):

```bash
cd wetransfer_git_2026-07-23_1249/git/rt-thread_arcode/bsp/allwinner_tina
scons -c && scons          # → rtthread.bin
```

Toolchain depoda yok; `tools/gcc-arm-none-eabi/` altına kendin yerleştir
(`rtconfig.py` → `EXEC_PATH` oradan okur).

### Harici bağımlılıklar

Bu ikisi bilerek depoya alınmadı — biri çok büyük, diğeri kendi git deposuna sahip
(dokunulmadı, yerel diskte duruyor):

| Ne | Nereye | Nereden |
|---|---|---|
| `arm-none-eabi` GCC | `git/tools/gcc-arm-none-eabi/` | ARM Developer sitesi |
| sunxi-tools | `git/tools/sunxi-tools/` | `https://github.com/Icenowy/sunxi-tools.git` @ **`11a9d20`** |

```bash
git clone https://github.com/Icenowy/sunxi-tools.git
cd sunxi-tools && git checkout 11a9d20 && make
```

**Yükleme** (sunxi-fel, FEL modunda):

```bash
sudo sunxi-fel -p spiflash-write 0x00000000 f1c100s_spl/boot.bin
sudo sunxi-fel -p spiflash-write 0x00010000 rtthread.bin
```

**M031 tarafı**: `savasel-data-converter-*/acm_drive.uvprojx` — Keil MDK + Nuvoton NuMicro SDK.

## Yapılandırma anahtarları

Hepsi `applications/app/mainpage.c` başında, derleme zamanı `#define`:

| Anahtar | Etki |
|---|---|
| `DISPLAY_VER` / `DISPLAY_HOR` | Ekran yönelimi — dönme açısı, hizalama, görsel seti |
| `ELEV_DEMO` | Arkel'i hiç okumaz, gömülü demo senaryosunu döndürür |
| `BUTSAN` | Butsan'a özel ok logoları |
| `DISPLAY_MSG` | İkon yerine tam sayfa İngilizce mesajlar |
| `THEME_*` | Kat numarası ve arka plan rengi |

## Donanım notları (şemalardan)

| Sinyal | Pin | Not |
|---|---|---|
| **NXT butonu** | M031 **PC.2** | Aktif LOW, harici 10k pull-up (R14), donanım debounce yok |
| **RTN butonu** | M031 **PC.3** | Aktif LOW, harici 10k pull-up (R13), donanım debounce yok |
| Aşırı akım (SC) | M031 PA.11 | Koruma hattı — buton **değil** |
| Çağrı girişleri | M031 PB.0 / PB.2 | 24 V saha girişi, 5.6k/820 bölücü → **aktif HIGH** |
| Buzzer | M031 PA.4 / PA.5 | PA.5 = PWM0_CH0 |
| SPI flash | F1C100S SPI0 | **W25Q128** 16 MB (U5, Module) |
| Arkel↔ekran SPI | F1C100S PE7–PE10 | CS / MOSI / CLK / MISO → M031 PA.3–PA.0 |

> **Dikkat:** Şemada işlemci `M031LC2AE`, Keil projesinin hedefi ise `M032SE3AE`
> (128 KB flash / 16 KB SRAM). Flash'a ayar yazmadan önce karttaki gerçek parça
> doğrulanmalıdır.

## Devam eden iş

Yönelimi (DİK/YAN) derleme zamanından çıkarıp **NXT butonuyla çalışma zamanında**
değiştirilebilir hâle getirmek. Taşıma kanalı zaten mevcut: M031, Arkel paketini ekrana
iletmeden önce `uuart_rx_data[13]`'e kendi buton byte'ını yazıp checksum'u yeniden
hesaplıyor (`acm_dr.c`); ekran tarafında bu byte `arcodeLop.reserved2` olarak geliyor ama
şu an okunmuyor.
