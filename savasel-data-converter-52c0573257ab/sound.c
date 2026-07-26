
#include <stdint.h>
#include <NuMicro.h>
#include <pwm.h>
#include "sound.h"

const uint8_t Chimes[7][2] ={
    {SOUND_DIDO, SOUND_DIDO}, 
    {SOUND_DIDO, SOUND_DIN},
    {SOUND_DIN,  SOUND_DIDO},
    {SOUND_DON,  SOUND_DIN},
    {SOUND_DIN,  SOUND_DON},
    {0,0}, 
    {0,0}
};

const uint8_t expv[58] = {
255,255,255,255,255,255,255,255,255,255,255,255,255,254,254,254,252,252,250,247,238,230,222,214,206,
199,191,184,177,170,163,157,150,144,138,133,127,122,117,112,107,103,99,94,90,87,83,79,76,73,70,67,64,61,58,56,53,51
};

void sound_play(uint8_t sel);

#define F_625   26  //1.6    ms
#define F_500   32  //2      ms
#define F_450   32  //2.2222 ms
#define F_375   42  //2.6666 ms

struct Wave {
   uint8_t f;
   uint16_t emax; //
   uint8_t exp;
};

struct Wave waves[5] ={
    {0,0},
    {F_450, 51,  1},
    {F_375, 51,  1},
    {F_625, 6,  1}, 
    {F_500, 6,  1}    
};

uint8_t PlayList[5][3] = {
    {0,0},
    {3,4},
    {1,0},
    {2,0},
    {1,2}
};

struct Sound {
    struct Wave* wave;
    volatile uint16_t pCount;
    volatile uint8_t expIndex;
};


uint8_t pBusy = 0;

struct Sound playin;

uint8_t boom, max = 0, min = 0,next;
uint16_t loop;




void sound_stop(void)
{
    /* Start PWM0 counter */
	PWM_ForceStop(PWM0, 0x3F);
}

void sound_start(uint8_t n)
{
    if(n==0)
    {
		sound_stop();
        pBusy = 0;
    }
    else
    {
	
	//  Set PWM to up counter type(edge aligned)
	//PWM_SET_ALIGNED_TYPE(PWM0, PWM_CH_0_MASK, PWM_EDGE_ALIGNED);
	//  PWM0 channel 0 frequency and duty configuration
    PWM_SET_PRESCALER(PWM0, 0, 3); /* Divided by 1 */
		
	//  Set PWM Timer duty
    PWM_SET_CMR(PWM0, 0, 120);
    //  Set PWM Timer period
    PWM_SET_CNR(PWM0, 0, 256);
	
	//  Set waveform generation
    PWM_SET_OUTPUT_LEVEL(PWM0, PWM_CH_0_MASK, PWM_OUTPUT_HIGH, PWM_OUTPUT_LOW, PWM_OUTPUT_NOTHING, PWM_OUTPUT_NOTHING);

    // Enable output of PWM0 channel 0
    PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
	
	/* Enable counter synchronous start function for PWM0 channel 0~5 */
	PWM_ENABLE_TIMER_SYNC(PWM0, PWM_CH_0_MASK, PWM_SSCTL_SSRC_PWM0);

    PWM_EnablePeriodInt(PWM0, 0,  0);
	NVIC_EnableIRQ(PWM0_IRQn);
		
	// Start PWM0 counter 
    PWM_Start(PWM0, PWM_CH_0_MASK);


	
    playin.wave = &waves[n];
    playin.pCount = 0;
    playin.expIndex = 1;
    max = expv[playin.expIndex];
    loop = 0;
    }
}

void sound_play(uint8_t sel)
{
    if(pBusy == 0)
    {   
        pBusy = 1;
        next = PlayList[sel][1];
        sound_start(PlayList[sel][0]);
    }
}

void PWM0_IRQHandler(void)
{
   /* Clear channel 0 period interrupt flag */
    PWM_ClearPeriodIntFlag(PWM0, 0);
    
    playin.pCount++;
	
    if(playin.pCount > playin.wave->f)
    {
        if(boom)
        {
            PWM_SET_CMR(PWM0, 0, max);
            boom = 0;
        }
        else
        {
            PWM_SET_CMR(PWM0, 0, min);
            boom =0xff;
        }
        
         playin.pCount = 0;
    }
    
    //PWM
    loop++;
    if(loop >515)
    {
        loop=0;
        max = expv[playin.expIndex];
        
        if(playin.expIndex < playin.wave->emax)
            playin.expIndex++;
        
        if((playin.expIndex >= playin.wave->emax) && (boom == 0))
        {
            sound_start(next);
            next=0;
        }
    }
}