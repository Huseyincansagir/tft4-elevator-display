#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include <NuMicro.h>

static inline uint8_t IsSC(void)
{
    return PA11;
}

static inline void SetLedDown(void){
	PB1 = 1;
}

static inline void ClearLedDown(void){
	PB1 = 0;
}

static inline void SetLedUp(void){
	PB3 = 1;
}

static inline void ClearLedUp(void){
    PB3 = 0 ;
}

static inline void SetPO1(void){
    PB5 = 1; 
}

static inline void ClearPO1(void){
	PB5 = 0;
}

static inline void SetPO2(void){
	PB7 = 1;
}

static inline void ClearPO2(void){
	PB7 = 0;
}

static inline void SetLED(void){
	PB15 = 1;
}

static inline void ClearLED(void){
	PB15 = 0;
}

static inline void ToggleLED(void){
	PB15 = !PB15;
}






#endif //_HARDWARE_H_
