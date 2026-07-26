/*
 * meprom.c — kart uzerindeki I2C EEPROM'da kalici ayar saklama.
 *
 * Neden EEPROM (SPI flash degil):
 *   - byte yazilabilir, sektor silme yok  -> CPU durmuyor
 *   - ~1.000.000 yazma cevrimi (NOR flash'ta ~10.000)
 *   - boot imajiyla ayni cipte degil      -> yanlis adres karti tuglalastirmaz
 *
 * ############################## DIKKAT ##############################
 * Kartlarda IKI EEPROM var ve ikisinin de I2C adresi 0x50:
 *
 *   U3  24LC128T-I/SN  16 KB  (base board)  -> 2 BYTE kelime adresi
 *   U4  24AA02HT-I/OT  256 B  (module)      -> 1 BYTE kelime adresi
 *
 * Ayni hatta ikisi birden olamaz; biri dizilmiyor olmali. Hangisinin
 * takili oldugu semanin metin katmanindan cikarilamadi. Adres genisligi
 * yanlis secilirse yazma islemi hedefi tutmaz, bu yuzden asagidaki iki
 * tanim GERCEK KARTTA DOGRULANMALIDIR.
 *
 * Dogrulandiktan sonra tek yapilacak: MEPROM_ADDR_BYTES'i 1 veya 2 yapmak.
 * ####################################################################
 *
 * Tum I2C hatalari sessizce yutulur: EEPROM yoksa/erisilemezse yonelim
 * yine calisir, sadece guc kesildiginde varsayilana doner.
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "meprom.h"

#define DBG_TAG  "meprom"
#define DBG_LVL  DBG_WARNING
#include <rtdbg.h>

#define MEPROM_I2C_BUS      "i2c0"
#define MEPROM_I2C_ADDR     0x50    /* 7 bit */

#define MEPROM_ADDR_BYTES   2       /* 24LC128 -> 2,  24AA02 -> 1   (DOGRULA!) */

/* Ayar bloguna, ilk sayfanin disinda, sade bir yer ayirdik */
#define MEPROM_OFF_MAGIC    0x0010
#define MEPROM_OFF_ORIENT   0x0011
#define MEPROM_MAGIC        0x5A    /* "burasi bizim tarafimizdan yazildi" */

static struct rt_i2c_bus_device *meprom_bus = RT_NULL;

void meprom_init(void)
{
    meprom_bus = rt_i2c_bus_device_find(MEPROM_I2C_BUS);

    if (meprom_bus == RT_NULL)
        LOG_W("%s bulunamadi, ayarlar kalici olmayacak", MEPROM_I2C_BUS);
}

static void meprom_fill_addr(rt_uint8_t *buf, rt_uint16_t off)
{
#if MEPROM_ADDR_BYTES == 2
    buf[0] = (rt_uint8_t)(off >> 8);
    buf[1] = (rt_uint8_t)(off & 0xFF);
#else
    buf[0] = (rt_uint8_t)(off & 0xFF);
#endif
}

static rt_err_t meprom_read(rt_uint16_t off, rt_uint8_t *val)
{
    struct rt_i2c_msg msg[2];
    rt_uint8_t addr[MEPROM_ADDR_BYTES];

    if (meprom_bus == RT_NULL)
        return -RT_ERROR;

    meprom_fill_addr(addr, off);

    msg[0].addr  = MEPROM_I2C_ADDR;
    msg[0].flags = RT_I2C_WR;
    msg[0].len   = MEPROM_ADDR_BYTES;
    msg[0].buf   = addr;

    msg[1].addr  = MEPROM_I2C_ADDR;
    msg[1].flags = RT_I2C_RD;
    msg[1].len   = 1;
    msg[1].buf   = val;

    return (rt_i2c_transfer(meprom_bus, msg, 2) == 2) ? RT_EOK : -RT_ERROR;
}

static rt_err_t meprom_write(rt_uint16_t off, rt_uint8_t val)
{
    struct rt_i2c_msg msg;
    rt_uint8_t buf[MEPROM_ADDR_BYTES + 1];

    if (meprom_bus == RT_NULL)
        return -RT_ERROR;

    meprom_fill_addr(buf, off);
    buf[MEPROM_ADDR_BYTES] = val;

    msg.addr  = MEPROM_I2C_ADDR;
    msg.flags = RT_I2C_WR;
    msg.len   = MEPROM_ADDR_BYTES + 1;
    msg.buf   = buf;

    if (rt_i2c_transfer(meprom_bus, &msg, 1) != 1)
        return -RT_ERROR;

    /* EEPROM'un ic yazma cevrimi: veri sayfasinda 5 ms, guvenli tarafta kaliyoruz */
    rt_thread_mdelay(10);

    return RT_EOK;
}

uint8_t meprom_load_orient(uint8_t fallback)
{
    rt_uint8_t magic = 0, orient = 0;

    if (meprom_read(MEPROM_OFF_MAGIC, &magic) != RT_EOK || magic != MEPROM_MAGIC)
        return fallback;                    /* hic yazilmamis ya da EEPROM yok */

    if (meprom_read(MEPROM_OFF_ORIENT, &orient) != RT_EOK || orient > 1)
        return fallback;                    /* bozuk deger */

    return (uint8_t)orient;
}

void meprom_save_orient(uint8_t orient)
{
    if (meprom_write(MEPROM_OFF_ORIENT, (rt_uint8_t)(orient & 0x01)) != RT_EOK)
    {
        LOG_W("yonelim yazilamadi");
        return;
    }

    /* Sihirli byte'i en sona yaziyoruz: yazma yarida kesilirse deger
       gecersiz kalir ve acilista fallback kullanilir. */
    if (meprom_write(MEPROM_OFF_MAGIC, MEPROM_MAGIC) != RT_EOK)
        LOG_W("magic yazilamadi");
}
