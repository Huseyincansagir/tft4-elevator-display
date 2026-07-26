
#include <NuMicro.h>
#include <string.h>


volatile uint8_t com_RxReady;

void com_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init USCI                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset USCI0 */
    SYS_ResetModule(USCI0_RST);

    /* Configure USCI0 as UART mode */
    UUART_Open(UUART0, 9600);
	
	/* Set UART line configuration and control signal output inverse */
    UUART_SetLine_Config(UUART0, 0, UUART_WORD_LEN_9, UUART_PARITY_NONE, UUART_STOP_BIT_1);
    //UUART0->LINECTL |= UUART_LINECTL_CTLOINV_Msk;

    /* Enable RTS auto direction function */
    //UUART0->PROTCTL |= UUART_PROTCTL_RTSAUDIREN_Msk;
	
	UUART_ENABLE_TRANS_INT(UUART0, UUART_INTEN_RXENDIEN_Msk);
	
    NVIC_EnableIRQ(USCI_IRQn);
}

void com_Put(uint16_t ch)
{
	while(UUART_GET_TX_FULL(UUART0));	/* Wait if Tx is full */
	UUART_WRITE(UUART0, (uint32_t)ch);    		/* Send UART Data from buffer */
}


void com_SendPack(uint8_t* data, uint32_t len)
{
	uint16_t bit9 = 0x100;
	while(len--)
	{
		com_Put((uint16_t)(*data) | bit9);
		data++;
		bit9 = 0;
	}
}

uint8_t RxData[32];
uint32_t RxIndex; 
uint32_t RxLen;

uint32_t com_GetRxData(uint8_t* data)
{
	uint32_t len = 0;
	
	if(com_RxReady)
	{
		if(RxLen > 0)
		{
			memcpy(data, RxData, RxLen);
			len = RxLen;
			RxLen = 0;
		}
		com_RxReady = 0;
	}
	
	return len;
}

void USCI01_IRQHandler(void){
	
	uint16_t recv = 0;
    uint32_t u32IntSts = UUART_GET_PROT_STATUS(UUART0);

    if (u32IntSts & UUART_PROTSTS_RXENDIF_Msk)
    {

        /* Clear RX end interrupt flag */
        UUART_CLR_PROT_INT_FLAG(UUART0, UUART_PROTSTS_RXENDIF_Msk);
		
		recv = UUART_READ(UUART0);
        //test 8. bit of data
        if(recv&0x100)
        {
            RxLen = (uint8_t)recv;
            RxIndex = 0;
        }
    
        if(RxLen > 0 && com_RxReady == 0)
        {
            RxData[RxIndex] = (uint8_t)recv;
            
            if(RxIndex<sizeof(RxData)) 
				RxIndex++;
            
            if(RxIndex == RxLen)
				com_RxReady  = 1;
            
        }
    }
}