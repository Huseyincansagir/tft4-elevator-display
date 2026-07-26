#ifndef _SOUND_H_
#define _SOUND_H_

#include <stdint.h>

#define SOUND_BUT     1   //button için bilip 
#define SOUND_DIN     2   //din 
#define SOUND_DON     3   //don 
#define SOUND_DIDO    4   //din-don 

extern const uint8_t Chimes[7][2];

void sound_play(uint8_t sel);


#endif