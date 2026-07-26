
#include <NuMicro.h>
#include "spi_transfer.h"


volatile uint8_t* dptr;
volatile uint8_t* tptr;
volatile uint8_t  tindex;

void spi_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/

    SPI_Open(SPI0, SPI_SLAVE, SPI_MODE_3, 8,0);
	
	SPI_SetFIFO(SPI0,3,3);
	
	SPI_EnableInt(SPI0, SPI_UNIT_INT_MASK);
	
	SPI_WRITE_TX(SPI0, 0);
	
	NVIC_EnableIRQ(SPI0_IRQn);
}

void spi_SetTxBuff(uint8_t* ptr)
{
	dptr = ptr;
}
	
void SPI0_IRQHandler(void)
{
	uint32_t trx; 
	
	while(!SPI_GET_RX_FIFO_EMPTY_FLAG(SPI0))
	{
		trx = SPI_READ_RX(SPI0);
		if(trx & 0x80)
		{
			tptr = dptr;
			tindex = trx & 0xF;
			
			SPI_ClearTxFIFO(SPI0);
		}
	}
		

	if(tptr)
	{
		while(!SPI_GET_TX_FIFO_FULL_FLAG(SPI0) && tindex<22)
		{
			SPI_WRITE_TX(SPI0, (uint32_t)tptr[tindex++]);
		}
	}
		
	
	
	SPI0->STATUS = 0xFFFF;
}