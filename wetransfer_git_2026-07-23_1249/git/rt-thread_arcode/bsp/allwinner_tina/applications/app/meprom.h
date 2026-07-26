#ifndef MEPROM_H_
#define MEPROM_H_

#include <stdint.h>

/*
 * Kalici ayarlar — U5 W25Q128 SPI NOR flash, "spi00" cihazi.
 * Ayar sektoru 0x200000 (firmware imajinin cok otesinde).
 */

void    meprom_init(void);

uint8_t meprom_load_orient(uint8_t fallback);
void    meprom_save_orient(uint8_t orient);

#endif /* MEPROM_H_ */
