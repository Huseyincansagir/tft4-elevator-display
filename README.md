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
| ~~`DISPLAY_VER` / `DISPLAY_HOR`~~ | **Kaldırıldı** — yönelim artık çalışma zamanında, NXT butonuyla |
| `ELEV_DEMO` | Arkel'i hiç okumaz, gömülü demo senaryosunu döndürür |
| `BUTSAN` | Butsan'a özel ok logoları |
| `DISPLAY_MSG` | İkon yerine tam sayfa İngilizce mesajlar |
| `THEME_*` | Kat numarası ve arka plan rengi |

## Donanım notları (şemalardan)

| Sinyal | Pin | Not |
|---|---|---|
| **NXT butonu** | M031 **PC.2** ⚠️ | Aktif LOW, harici 10k pull-up (R14), donanım debounce yok |
| **RTN butonu** | M031 **PC.3** ⚠️ | Aktif LOW, harici 10k pull-up (R13), donanım debounce yok |
| Aşırı akım (SC) | M031 PA.11 | Koruma hattı — buton **değil** |
| Çağrı girişleri | M031 PB.0 / PB.2 | 24 V saha girişi, 5.6k/820 bölücü → **aktif HIGH** |
| Buzzer | M031 PA.4 / PA.5 | PA.5 = PWM0_CH0 |
| SPI flash | F1C100S SPI0 (`spi00`) | **W25Q128JVSIQ** 16 MB (U5, Module) — SPL + firmware + ayar sektoru |
| EEPROM | — | Semada `24LC128`/`24AA02` gorunuyor ama **dizilmiyor** |
| Arkel↔ekran SPI | F1C100S PE7–PE10 | CS / MOSI / CLK / MISO → M031 PA.3–PA.0 |

> ⚠️ **NXT/RTN eşleşmesi doğrulanmadı.** İki butonun PC.2 ve PC.3'e gittiği kesin
> (M031 sayfasında dışarı çıkan tek boş GPIO çifti bunlar ve `sys_Init()` ikisini de
> GPIO olarak yapılandırıp hiç okumuyor), ama *hangisinin hangisi* olduğu şema
> PDF'inin metin katmanından çıkarılamadı. Ters çıkarsa `acm_dr.c` içindeki
> `BTN_NXT_PRESSED` / `BTN_RTN_PRESSED` tanımlarını yer değiştirmek yeterli.

> **Dikkat:** Şemada işlemci `M031LC2AE`, Keil projesinin hedefi ise `M032SE3AE`
> (128 KB flash / 16 KB SRAM). Karttaki gerçek parça doğrulanmalıdır.

Şemada tasarımcının kendi notu (`Input.SchDoc`): *"6.8K/10K gerilim bölme oranı
sahada yaşanan uzun kablolama gürültüsü nedeniyle 5.6K/820 şeklinde değiştirildi.
Toplam direnç de aynı sebeple 16.8K yerine 6.4K olarak değiştirildi."*

## Çalışma zamanı yönelim (NXT butonu)

Yönelim artık derleme zamanı `#define` değil; NXT butonuyla değişiyor ve SPI flash'te
saklanıyor. Zincir:

```
NXT butonu (M031 PC.2, aktif LOW)
   │  scan_Buttons() — 20 tur filtre (~40 ms)
   ▼
button = 0x01                       acm_dr.c
   │  uuart_rx_data[13] = button; checksum yeniden hesaplanir
   ▼  (mevcut Arkel paketi, yeni alan yok)
arcodeLop.reserved2                 mainpage.c
   │  handle_buttons() — KENAR yakalar (seviye degil)
   ▼
g_orient ^= 1  →  meprom_save_orient()  →  SPI flash 0x200000
   │
   ▼  lv_img_cache_invalidate_src(NULL) + lv_obj_invalidate(lv_scr_act())
sonraki display_floor()/display_state() cagrisi yeni yonelimle cizer
```

Tasarım notları:

- **Protokolde değişiklik yok.** Byte 13 zaten M031 tarafından yazılıyordu, ekran
  tarafında okunmuyordu. Yeni alan eklenmedi, paket boyu aynı.
- **Seviye gönderilir, kenar yakalanır.** M031 buton basılı olduğu sürece maskeyi set
  tutar; ekran yalnızca `0 → 1` geçişinde tetiklenir. Tek paket kaybolsa bir sonraki
  aynı seviyeyi taşıdığı için ne komut kaçar ne çift tetikleme olur.
- **Kalıcılık SPI flash'te.** Kartta EEPROM entegresi yok, o yüzden ayar **U5 W25Q128**'in
  `0x200000` adresindeki 4 KB sektöründe tutuluyor. Firmware `0x10000`'den başlayıp
  ~`0xA3000`'de bittiği için arada ~1.4 MB güvenlik payı var. `meprom.c` açılışta JEDEC
  ID okuyup Winbond görmezse **hiç yazma yapmaz**, yazdıktan sonra geri okuyup doğrular.
  Tüm hatalar yutulur: flash erişilemezse yönelim yine çalışır, sadece güç kesildiğinde
  varsayılana döner.
- **Yerleşim sabitleri birebir korundu.** `THIS_ROTATION`, `*_ALIGN`, `*_OFFSET`
  makroları aynı isimlerle duruyor, sadece değerlerini `g_orient` üzerinden seçiyor —
  böylece `lv_obj_align()` / `lv_img_set_angle()` çağrı noktalarının hiçbiri değişmedi.

> **ELEV_DEMO açıkken buton çalışmaz.** Demo modunda Arkel paketi hiç okunmadığı için
> byte 13 ekrana ulaşmaz. Test için `mainpage.c` başındaki `#define ELEV_DEMO`
> satırı yorum satırı yapılmalıdır.

> **Binary büyüdü.** İki görsel seti de artık referanslandığı için `--gc-sections`
> birini atmıyor. Yükleme öncesi `rtthread.bin` boyutunun SPI flash yerleşimine
> (firmware 0x10000'den başlıyor) sığdığı doğrulanmalı.
