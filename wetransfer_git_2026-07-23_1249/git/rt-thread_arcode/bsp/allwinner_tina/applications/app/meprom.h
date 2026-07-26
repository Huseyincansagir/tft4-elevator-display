#ifndef MEPROM_H_
#define MEPROM_H_

#include <stdint.h>

/*
 * Kart uzerindeki I2C EEPROM'da tutulan kalici ayarlar.
 * I2C0 = F1C100S PE11 (SCL) / PE12 (SDA), "i2c0" olarak kayitli, 400 kHz.
 */

void    meprom_init(void);

uint8_t meprom_load_orient(uint8_t fallback);
void    meprom_save_orient(uint8_t orient);

#endif /* MEPROM_H_ */
