
#ifndef	_COM_H
#define	_COM_H

#include <stdint.h>

extern volatile uint8_t com_RxReady;

void com_Init(void);
void com_Put(uint16_t ch);
void com_SendPack(uint8_t* data, uint32_t len);

uint32_t com_GetRxData(uint8_t* data);


#endif
