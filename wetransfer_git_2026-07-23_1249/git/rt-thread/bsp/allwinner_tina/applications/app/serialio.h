

#ifndef	SERIALIO_H_
#define	SERIALIO_H_

#include "common.h"

unsigned int serialio_get(void);

void serialio_send( unsigned int data );

void serialio_init(void);

void arcode_init(void);
void arcode_xfer(uint8_t* rx_data);
uint8_t arcode_crc(uint8_t* ptr);

#endif