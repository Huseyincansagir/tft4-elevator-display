/******************************************************************************
 * @file     main.c
 * @version  V0.1
 * @brief    
 *
 * 
 * @copyright (C)
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "global.h"
#include "arcode_lop.h"
#include "com.h"
#include "sound.h"
#include "spi_transfer.h"

uint8_t  tx_index;
uint8_t  tx_buff[16];
uint16_t tx_data;

volatile uint16_t tmr_counter;

uint8_t upx, downx;
uint8_t bip_now, chime;

uint16_t nextb_cnt, retnb_cnt;

volatile uint8_t button;

uint16_t CallDownCounter;
uint16_t CallUpCounter;
uint16_t PI1Counter;
uint16_t PI2Counter;

uint8_t LedBlink;

struct ModuleSendToLopHost  loprxstruct;
struct ModuleRcvFromLopHost loptxstruct;

uint8_t uuart_rx_data[32];
uint8_t spi_tx_data[2][32];
uint32_t spi_buff;

/* Function prototype declaration */
static void sys_Init(void);
static void tmr_Init(void);
static void pio_Init(void);

uint8_t IsCallDown(void);
uint8_t IsCallUp(void);
uint8_t IsPI1(void);
uint8_t IsPI2(void);

static void scan_Buttons(void);

uint8_t CalculateChecksum(uint8_t *ptr);

/*
 * NXT / RTN panel butonlari.
 *
 * Sema: 3V3 --[10k]-- pin, buton pini GND'ye kisa devre eder  ->  AKTIF LOW.
 * Donanim debounce yok, yazilimda filtreleniyor.
 *
 * DIKKAT: PC.2 ve PC.3'un NXT/RTN ile eslesmesi semanin metin katmanindan
 * kesin cikarilamadi. Ters cikarsa asagidaki iki tanimi yer degistirmek yeterli.
 */
#define BTN_NXT_PRESSED()   (PC2 == 0)
#define BTN_RTN_PRESSED()   (PC3 == 0)

/* Pakette byte 13 ile ekrana tasinan buton maskesi (arcodeLop.reserved2) */
#define BTN_CODE_NXT        0x01
#define BTN_CODE_RTN        0x02

#define BTN_FIL_DELAY       20      /* IsCallUp/Down ile ayni filtre suresi */

uint16_t NxtCounter;
uint16_t RtnCounter;

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
	uint32_t bloop,loop;
	
	sys_Init();
	com_Init();
	tmr_Init();
	pio_Init();
	spi_Init();
	
	while(1)
	{
		if(com_RxReady)
        {
			if(com_GetRxData(uuart_rx_data))
			{
				uint8_t crc = CalculateChecksum(uuart_rx_data);
    
				uint8_t len = uuart_rx_data[0];

				if(crc == uuart_rx_data[len-1])
				{    
				if(uuart_rx_data[1] == MODULE_TO_LOP_HOST_PACKET_ID)
				{
					memcpy((void *)&loprxstruct, &uuart_rx_data[2], len-2);
            
					uuart_rx_data[13]=button;
					
					uuart_rx_data[len-1] = CalculateChecksum(uuart_rx_data);
            
					memcpy((void*)spi_tx_data[spi_buff], uuart_rx_data, len);
					spi_SetTxBuff(spi_tx_data[spi_buff]);
            
					spi_buff++;
					spi_buff &= 0x01;       
					}
				}
			}
        }
			
		if(tmr_counter > 0)
		{
			tmr_counter = 0;
			
			bloop++;
			if(bloop&1)
			{
				LedBlink++;
				if(LedBlink < 25)
					SetLED();
				else
					ClearLED();
		
				if(IsCallUp())
				{
					loptxstruct.upCallButton = 1;
					if(loprxstruct.enabledButtonSound)
					{
						if(upx) bip_now = 1;
					}
					upx = 0;
				}
				else
				{
					upx = 1;
					loptxstruct.upCallButton = 0;
				}
			
				if(IsCallDown())
				{
					loptxstruct.downCallButton = 1;
					if(loprxstruct.enabledButtonSound)
					{
						if(downx) bip_now = 1;
					}
					downx = 0;
				}
				else
				{
					downx = 1;
					loptxstruct.downCallButton = 0;
				}
			
				if(IsPI1())
					loptxstruct.pi1 = 1;
				else
					loptxstruct.pi1 = 0;
			
				if(IsPI2())
					loptxstruct.pi2 = 1;
				else
					loptxstruct.pi2 = 0;
				
				loptxstruct.reserved1 = 0;
				loptxstruct.reserved2 = 0;
			
				{	//transmit message
					if(tx_index == 0)
					{
						tx_buff[0]= LOP_HOST_TO_MODULE_PACKET_SIZE;
						tx_buff[1]= LOP_HOST_TO_MODULE_PACKET_ID;
				
						memcpy(&tx_buff[2],&loptxstruct,2);
						tx_buff[4] = CalculateChecksum(tx_buff);
						
					}
					
					tx_data = (uint16_t)tx_buff[tx_index];
					
					if(tx_index == 0)
					{
						tx_data |= 0x100;	
					}
					
					com_Put(tx_data);
					
					tx_index++;
					if(tx_index >= tx_buff[0])
					{
						tx_index = 0;
					}
					
				}
			
				if(loprxstruct.po1)
				{
					SetPO1();
				}
				else
				{
					ClearPO1();
				}
			
				if(loprxstruct.po2)
				{
					SetPO2();
				}
				else
				{
					ClearPO2();
				}
			
				if(loprxstruct.upCallLed || loptxstruct.upCallButton)
				{
					SetLedUp();
				}
				else
				{
					ClearLedUp();
				}
			
				if(loprxstruct.downCallLed || loptxstruct.downCallButton)
				{
					SetLedDown();
				}
				else
				{
					ClearLedDown();
				}
			
				if(loprxstruct.playLopFloorChime)
				{
					if(loprxstruct.enabledLopFloorChime && !chime)
					{
						if(loprxstruct.currentFloorNo == loprxstruct.configFloorNo)
						{
							if(loprxstruct.collectionDownDirection || loprxstruct.travelDownDirection)
							{
								chime = Chimes[loprxstruct.floorChimeType][0];
							}
												
							if(loprxstruct.collectionUpDirection || loprxstruct.travelUpDirection)
							{
								chime = Chimes[loprxstruct.floorChimeType][1];
							}
							
							sound_play(chime);
						}
					}
				}
				else
				{
					chime = 0;
				}
			
				if(bip_now)
				{
					sound_play( SOUND_BUT );
					bip_now = 0;
				}
				
				scan_Buttons();
			}
		}
	}
}

#define INPUT_FIL_DELAY 20
uint8_t IsCallDown(void)
{
    if(PB0)
	{
		CallDownCounter++;
	}
	else
	{
		CallDownCounter=0;
	}
	return (CallDownCounter > INPUT_FIL_DELAY) ? -1:0;
}

uint8_t IsCallUp(void)
{
    if(PB2)
	{
		CallUpCounter++;
	}
	else
	{
		CallUpCounter=0;
	}
	return (CallUpCounter > INPUT_FIL_DELAY) ? -1:0;
}

uint8_t IsPI1(void)
{
    if(PB4)
	{
		PI1Counter++;
	}
	else
	{
		PI1Counter=0;
	}
	return (PI1Counter > INPUT_FIL_DELAY) ? -1:0;
}

uint8_t IsPI2(void)
{
    if(PB6)
	{
		PI2Counter++;
	}
	else
	{
		PI2Counter=0;
	}
	return (PI2Counter > INPUT_FIL_DELAY) ? -1:0;
}

/*
 * Panel butonlarini filtreleyip pakete tasinan maskeyi uretir.
 *
 * Ekran tarafi bu byte'in KENARINI yakalar (seviye degil), bu yuzden burada
 * basili tutuldugu surece maske set kalir; birakinca sifirlanir. Boylece tek
 * bir paket kaybolsa bile bir sonraki paket ayni seviyeyi tasir.
 */
static void scan_Buttons(void)
{
	uint8_t mask = 0;

	if(BTN_NXT_PRESSED())
	{
		if(NxtCounter <= BTN_FIL_DELAY)
			NxtCounter++;
	}
	else
	{
		NxtCounter = 0;
	}

	if(BTN_RTN_PRESSED())
	{
		if(RtnCounter <= BTN_FIL_DELAY)
			RtnCounter++;
	}
	else
	{
		RtnCounter = 0;
	}

	if(NxtCounter > BTN_FIL_DELAY)
		mask |= BTN_CODE_NXT;

	if(RtnCounter > BTN_FIL_DELAY)
		mask |= BTN_CODE_RTN;

	button = mask;
}

static void pio_Init(void)
{
	GPIO_SET_OUT_DATA(PB, 0);
	GPIO_SET_OUT_DATA(PA, 0);

	GPIO_SetMode(PB, BIT1|BIT3|BIT5|BIT7|BIT15, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, BIT4, GPIO_MODE_OUTPUT);

	/* NXT / RTN butonlari: giris + dahili pull-up (harici 10k'ya ek guvence) */
	GPIO_SetMode(PC, BIT2|BIT3, GPIO_MODE_INPUT);
	GPIO_SetPullCtl(PC, BIT2|BIT3, GPIO_PUSEL_PULL_UP);
}

static void tmr_Init(void)
{
		/* Open Timer1 in periodic mode, enable interrupt and 2 interrupt ticks per second */
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER1);
	
	/* Enable Timer0 NVIC */
	NVIC_EnableIRQ(TMR1_IRQn);
	
	/* Start Timer0 counting */
	TIMER_Start(TIMER1);
}

void TMR1_IRQHandler(void)
{
   if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER1);
		tmr_counter++;
	}
}

void sys_Init(void){
	
	    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable base clock */
    /* If the macros do not exist in your project, please refer to the related clk.h in Header folder of the tool package */
    /* Enable clock source */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Waiting for clock source ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Set HCLK clock */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Set PCLK-related clock */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV1 | CLK_PCLKDIV_APB1DIV_DIV1);

    /* Enable module clock and set clock source */
    CLK_EnableModuleClock(ISP_MODULE);
	
	/*SPI Clock Enable*/
	CLK_EnableModuleClock(SPI0_MODULE);
	
	/*USCI Clock Enable*/
	CLK_EnableModuleClock(USCI0_MODULE);
	
	/* Enable TIMER module clock */
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);
	
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, 0);
	
	/* PWM clock frequency can be set equal or double to HCLK by choosing case 1 or case 2 */
    /* case 1.PWM clock frequency is set equal to HCLK: select PWM module clock source as PCLK */
    CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL2_PWM0SEL_PCLK0, 0);
	
	/* Enable PWM0 module clock */
    CLK_EnableModuleClock(PWM0_MODULE);
	
	/* Reset PWM0 module */
    SYS_ResetModule(PWM0_RST);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();
	

	//PA
    SYS->GPA_MFPH &= ~(SYS_GPA_MFPH_PA11MFP_Msk);
    SYS->GPA_MFPH |= (SYS_GPA_MFPH_PA11MFP_GPIO);
    SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA4MFP_Msk);
    SYS->GPA_MFPL |= (SYS_GPA_MFPL_PA4MFP_GPIO);
	//	PWM0 pins
	SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA5MFP_Msk);
    SYS->GPA_MFPL |= (SYS_GPA_MFPL_PA5MFP_PWM0_CH0);
	//	SPI0 pins
	SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA3MFP_Msk | SYS_GPA_MFPL_PA2MFP_Msk | SYS_GPA_MFPL_PA1MFP_Msk | SYS_GPA_MFPL_PA0MFP_Msk);
    SYS->GPA_MFPL |= (SYS_GPA_MFPL_PA3MFP_SPI0_SS | SYS_GPA_MFPL_PA2MFP_SPI0_CLK | SYS_GPA_MFPL_PA1MFP_SPI0_MISO | SYS_GPA_MFPL_PA0MFP_SPI0_MOSI);
	//	USCI0 pins
	SYS->GPA_MFPH &= ~(SYS_GPA_MFPH_PA10MFP_Msk | SYS_GPA_MFPH_PA9MFP_Msk);
    SYS->GPA_MFPH |= (SYS_GPA_MFPH_PA10MFP_USCI0_DAT0 | SYS_GPA_MFPH_PA9MFP_USCI0_DAT1);
	
	//PB
	SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB7MFP_Msk | SYS_GPB_MFPL_PB6MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk | SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk | SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk | SYS_GPB_MFPL_PB0MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB7MFP_GPIO | SYS_GPB_MFPL_PB6MFP_GPIO | SYS_GPB_MFPL_PB5MFP_GPIO | SYS_GPB_MFPL_PB4MFP_GPIO | SYS_GPB_MFPL_PB3MFP_GPIO | SYS_GPB_MFPL_PB2MFP_GPIO | SYS_GPB_MFPL_PB1MFP_GPIO | SYS_GPB_MFPL_PB0MFP_GPIO);
    
	SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB15MFP_Msk|SYS_GPB_MFPH_PB13MFP_Msk | SYS_GPB_MFPH_PB12MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB15MFP_GPIO|SYS_GPB_MFPH_PB13MFP_UART0_TXD | SYS_GPB_MFPH_PB12MFP_UART0_RXD);
	
	//PC
    SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC3MFP_Msk | SYS_GPC_MFPL_PC2MFP_Msk);
    SYS->GPC_MFPL |= (SYS_GPC_MFPL_PC3MFP_GPIO | SYS_GPC_MFPL_PC2MFP_GPIO);
	
	//PF
	SYS->GPF_MFPH &= ~(SYS_GPF_MFPH_PF15MFP_Msk);
    SYS->GPF_MFPH |= (SYS_GPF_MFPH_PF15MFP_GPIO);
	
    SYS->GPF_MFPL &= ~(SYS_GPF_MFPL_PF5MFP_Msk | SYS_GPF_MFPL_PF4MFP_Msk | SYS_GPF_MFPL_PF3MFP_Msk | SYS_GPF_MFPL_PF2MFP_Msk);
    SYS->GPF_MFPL |= (SYS_GPF_MFPL_PF5MFP_GPIO | SYS_GPF_MFPL_PF4MFP_GPIO | SYS_GPF_MFPL_PF3MFP_GPIO | SYS_GPF_MFPL_PF2MFP_GPIO);
	
	SYS->GPF_MFPL &= ~(SYS_GPF_MFPL_PF1MFP_Msk | SYS_GPF_MFPL_PF0MFP_Msk);
    SYS->GPF_MFPL |= (SYS_GPF_MFPL_PF1MFP_ICE_CLK | SYS_GPF_MFPL_PF0MFP_ICE_DAT);

    /* Lock protected registers */
    SYS_LockReg();
}

uint8_t CalculateChecksum(uint8_t *ptr){
	uint8_t size, i, checksum;
	size = *ptr;
	checksum = 0x33;
	for(i = 0; i < size - 1; i++){
		checksum += *ptr;
		ptr++;	
	}
	return checksum;	
}


/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
