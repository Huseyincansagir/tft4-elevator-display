/*
 * meprom.c — kalici ayar saklama (SPI NOR flash).
 *
 * Hedef: U5  W25Q128JVSIQ  16 MB, F1C100S SPI0, "spi00" cihazi.
 * (Kartta EEPROM entegresi yok; semada gorunen 24LC128/24AA02 dizilmiyor.)
 *
 * ######################### FLASH YERLESIMI #########################
 *   0x000000   SPL (boot.bin)                ~10 KB
 *   0x010000   rtthread.bin                  ~600 KB  -> ~0x0A3000'de biter
 *   ...
 *   0x200000   AYAR SEKTORU (4 KB)           <-- burasi
 *   ...        16 MB'a kadar bos
 *
 * Ayar sektoru firmware'in bittigi yerden ~1.4 MB sonrada. Bu adres
 * DEGISTIRILMEMELIDIR: firmware imajinin uzerine denk gelirse kart acilmaz.
 * ###################################################################
 *
 * SFUD kapali (rtconfig.h'de elle devre disi birakilmis), bu yuzden flash'a
 * ham komutlarla erisiyoruz. Baska hicbir kod calisma aninda bu cihaza
 * dokunmuyor, dolayisiyla bus paylasimi sorunu yok.
 *
 * Guvenlik onlemleri:
 *   - Acilista JEDEC ID okunur; Winbond gorulmezse hicbir yazma yapilmaz.
 *   - Yazmadan sonra geri okunup dogrulanir.
 *   - Her hata sessizce yutulur: flash erisilemezse yonelim yine calisir,
 *     sadece guc kesildiginde varsayilana doner.
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "meprom.h"

#define DBG_TAG  "meprom"
#define DBG_LVL  DBG_WARNING
#include <rtdbg.h>

#define MEPROM_SPI_DEV      "spi00"
#define MEPROM_SPI_HZ       (10 * 1000 * 1000)

#define MEPROM_SECTOR       0x200000UL      /* 2 MB — firmware'den cok uzakta */
#define MEPROM_SECTOR_SIZE  4096

#define MEPROM_OFF_MAGIC    0               /* sektor icindeki ofsetler */
#define MEPROM_OFF_ORIENT   1
#define MEPROM_MAGIC        0x5A

/* W25Qxx komutlari */
#define CMD_WREN            0x06
#define CMD_RDSR1           0x05
#define CMD_READ            0x03
#define CMD_PP              0x02
#define CMD_SE_4K           0x20
#define CMD_JEDEC_ID        0x9F

#define SR1_BUSY            0x01

static struct rt_spi_device *meprom_dev = RT_NULL;
static rt_bool_t             meprom_ok  = RT_FALSE;

/* ------------------------------------------------------------------ */

static rt_err_t nor_cmd(rt_uint8_t cmd)
{
    return (rt_spi_send(meprom_dev, &cmd, 1) == 1) ? RT_EOK : -RT_ERROR;
}

static rt_err_t nor_wait_ready(rt_int32_t timeout_ms)
{
    rt_uint8_t cmd = CMD_RDSR1, sr = 0xFF;

    while (timeout_ms-- > 0)
    {
        if (rt_spi_send_then_recv(meprom_dev, &cmd, 1, &sr, 1) != RT_EOK)
            return -RT_ERROR;

        if ((sr & SR1_BUSY) == 0)
            return RT_EOK;

        rt_thread_mdelay(1);
    }
    return -RT_ETIMEOUT;
}

static rt_err_t nor_read(rt_uint32_t addr, rt_uint8_t *buf, rt_size_t len)
{
    rt_uint8_t hdr[4];

    hdr[0] = CMD_READ;
    hdr[1] = (rt_uint8_t)(addr >> 16);
    hdr[2] = (rt_uint8_t)(addr >> 8);
    hdr[3] = (rt_uint8_t)(addr);

    return rt_spi_send_then_recv(meprom_dev, hdr, 4, buf, len);
}

static rt_err_t nor_erase_sector(rt_uint32_t addr)
{
    rt_uint8_t hdr[4];

    if (nor_cmd(CMD_WREN) != RT_EOK)
        return -RT_ERROR;

    hdr[0] = CMD_SE_4K;
    hdr[1] = (rt_uint8_t)(addr >> 16);
    hdr[2] = (rt_uint8_t)(addr >> 8);
    hdr[3] = (rt_uint8_t)(addr);

    if (rt_spi_send(meprom_dev, hdr, 4) != 4)
        return -RT_ERROR;

    /* 4 KB sektor silme: tipik 45 ms, en kotu 400 ms */
    return nor_wait_ready(1000);
}

static rt_err_t nor_program(rt_uint32_t addr, const rt_uint8_t *buf, rt_size_t len)
{
    rt_uint8_t hdr[4];

    if (nor_cmd(CMD_WREN) != RT_EOK)
        return -RT_ERROR;

    hdr[0] = CMD_PP;
    hdr[1] = (rt_uint8_t)(addr >> 16);
    hdr[2] = (rt_uint8_t)(addr >> 8);
    hdr[3] = (rt_uint8_t)(addr);

    if (rt_spi_send_then_send(meprom_dev, hdr, 4, buf, len) != RT_EOK)
        return -RT_ERROR;

    return nor_wait_ready(100);
}

/* ------------------------------------------------------------------ */

void meprom_init(void)
{
    struct rt_spi_configuration cfg;
    rt_uint8_t cmd = CMD_JEDEC_ID, id[3] = { 0, 0, 0 };

    meprom_dev = (struct rt_spi_device *)rt_device_find(MEPROM_SPI_DEV);

    if (meprom_dev == RT_NULL)
    {
        LOG_W("%s yok, ayarlar kalici olmayacak", MEPROM_SPI_DEV);
        return;
    }

    cfg.data_width = 8;
    cfg.max_hz     = MEPROM_SPI_HZ;
    cfg.mode       = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;

    if (rt_spi_configure(meprom_dev, &cfg) != RT_EOK)
    {
        LOG_W("spi ayarlanamadi");
        return;
    }

    /* Dogru cihazla konustugumuzdan emin olmadan ASLA yazma yapma */
    if (rt_spi_send_then_recv(meprom_dev, &cmd, 1, id, 3) != RT_EOK)
    {
        LOG_W("JEDEC ID okunamadi");
        return;
    }

    if (id[0] != 0xEF)      /* 0xEF = Winbond */
    {
        LOG_W("beklenmeyen flash ureticisi %02X %02X %02X", id[0], id[1], id[2]);
        return;
    }

    meprom_ok = RT_TRUE;
}

uint8_t meprom_load_orient(uint8_t fallback)
{
    rt_uint8_t buf[2] = { 0, 0 };

    if (!meprom_ok)
        return fallback;

    if (nor_read(MEPROM_SECTOR, buf, sizeof(buf)) != RT_EOK)
        return fallback;

    if (buf[MEPROM_OFF_MAGIC] != MEPROM_MAGIC)
        return fallback;                    /* hic yazilmamis */

    if (buf[MEPROM_OFF_ORIENT] > 1)
        return fallback;                    /* bozuk deger */

    return (uint8_t)buf[MEPROM_OFF_ORIENT];
}

void meprom_save_orient(uint8_t orient)
{
    rt_uint8_t buf[2], rb[2] = { 0, 0 };

    if (!meprom_ok)
        return;

    buf[MEPROM_OFF_MAGIC]  = MEPROM_MAGIC;
    buf[MEPROM_OFF_ORIENT] = (rt_uint8_t)(orient & 0x01);

    /* NOR flash'ta bit yalnizca 1->0 gider; once sektoru silmek sart */
    if (nor_erase_sector(MEPROM_SECTOR) != RT_EOK)
    {
        LOG_W("sektor silinemedi");
        return;
    }

    if (nor_program(MEPROM_SECTOR, buf, sizeof(buf)) != RT_EOK)
    {
        LOG_W("yazilamadi");
        return;
    }

    if (nor_read(MEPROM_SECTOR, rb, sizeof(rb)) != RT_EOK ||
        rb[MEPROM_OFF_MAGIC]  != buf[MEPROM_OFF_MAGIC] ||
        rb[MEPROM_OFF_ORIENT] != buf[MEPROM_OFF_ORIENT])
    {
        LOG_W("dogrulama basarisiz");
    }
}
