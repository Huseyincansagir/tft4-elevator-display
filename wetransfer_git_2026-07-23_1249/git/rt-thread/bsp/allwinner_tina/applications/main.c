/*
 * File      : main.c

 */

#include <rtthread.h>
#include "drv_clock.h"
#include "board.h"
#include "spi/drv_spi.h"
#include "drv_gpio.h"


#define LCD_vSPI_RES	GPIO_PORT_A, GPIO_PIN_0
#define LCD_vSPI_CS		GPIO_PORT_A, GPIO_PIN_1
#define LCD_vSPI_CLK	GPIO_PORT_A, GPIO_PIN_2
#define LCD_vSPI_DI		GPIO_PORT_A, GPIO_PIN_3

void vSPI_WriteComm(unsigned char b)	  //spec page 46
{
	uint32_t mask, data = (uint32_t)b;

	gpio_set_value(LCD_vSPI_CS,0);
   
   	for(mask = 0x100; mask !=0; mask >>=1)			
   	{  
		if( data & mask) 
	  		gpio_set_value(LCD_vSPI_DI,1);
    	else 
			gpio_set_value(LCD_vSPI_DI,0);

	  	gpio_set_value(LCD_vSPI_CLK,0);
    	gpio_set_value(LCD_vSPI_CLK,1);			
   	}

   	gpio_set_value(LCD_vSPI_CS,1);
}

void vSPI_WriteData(unsigned char b)
{
	uint32_t mask, data = (uint32_t)b | 0x100;

	gpio_set_value(LCD_vSPI_CS, 0);

	for( mask = 0x100; mask !=0; mask >>=1)
   	{  
		if(data & mask) 
	  		gpio_set_value(LCD_vSPI_DI,1);
    	else 
			gpio_set_value(LCD_vSPI_DI,0);

	  	gpio_set_value(LCD_vSPI_CLK,0);
    	gpio_set_value(LCD_vSPI_CLK,1);			
   	}

   	gpio_set_value(LCD_vSPI_CS,1);
}



void LCD_Init(void)
{

	gpio_set_value(LCD_vSPI_RES,1);
	rt_thread_mdelay(10);
	gpio_set_value(LCD_vSPI_RES,0);
    rt_thread_mdelay(500);
    gpio_set_value(LCD_vSPI_RES,1);
	rt_thread_mdelay(500);

	// ****************************** denendi **********************************************

	// R61408
	vSPI_WriteComm(0xB0);	//manufacturer commands enable
	vSPI_WriteData(0x04);

	vSPI_WriteComm(0x6F);	//	Display Refresh Control
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xB3);	// Frame Memory Access and Interface Setting
	vSPI_WriteData(0x12);	// DPI + WE
	vSPI_WriteData(0x00);
	/*
	vSPI_WriteComm(0xB6);	// DSI Control
	vSPI_WriteData(0x51);	// DSI_CLK/8
	vSPI_WriteData(0x83);	// DSI_CLK 50-90MHz
	*//*
	vSPI_WriteComm(0xB7);	// MDDI Control
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x80);
	vSPI_WriteData(0x15);
	vSPI_WriteData(0x25);
	*/
	vSPI_WriteComm(0xBD);	// Resizing Control
	vSPI_WriteData(0x00);	// x1

	vSPI_WriteComm(0xC0);	// Panel Driving Settings 1
	vSPI_WriteData(0x12);	// DE_PL 
	vSPI_WriteData(0x87);
	/*
	vSPI_WriteComm(0xC1);	// Panel Driving Setting 2
	vSPI_WriteData(0x43);	// 1-SS
	vSPI_WriteData(0x31);	// 2-NL   31  800  3f  854
	vSPI_WriteData(0x99);	// 3-GIPMODE
	vSPI_WriteData(0x21);	// 4-GSPF
	vSPI_WriteData(0x20);	// 5-GSPS
	vSPI_WriteData(0x00);	// 6-
	vSPI_WriteData(0x10);	// 7-DIVI
	vSPI_WriteData(0x28);	// 8-RTN
	vSPI_WriteData(0x0C);	// 9-BP
	vSPI_WriteData(0x0A);	// A-FP
	vSPI_WriteData(0x00);	// B
	vSPI_WriteData(0x00);	// C
	vSPI_WriteData(0x00);	// D
	vSPI_WriteData(0x21);	// E
	vSPI_WriteData(0x01);	// F

	vSPI_WriteComm(0xC2);	// Display V-Timing
	vSPI_WriteData(0x00);	// 1-LINEINV
	vSPI_WriteData(0x06);	// 2-SEQGND
	vSPI_WriteData(0x06);	// 3-SEQVCI
	vSPI_WriteData(0x01);	// 4-GEQ1W
	vSPI_WriteData(0x03);	//
	vSPI_WriteData(0x00);	//

	vSPI_WriteComm(0xC4);	// Panel Driving Settings 3
	vSPI_WriteData(0x00);	// 
	vSPI_WriteData(0x01);	// 

	vSPI_WriteComm(0xC6);	// Outline Sharpening Control
	vSPI_WriteData(0x00);	// 1-AVST ADST
	vSPI_WriteData(0x00);	// 2-DTHU DTHL

	vSPI_WriteComm(0xC7);	// Panel Driving Settings
	vSPI_WriteData(0x11);	// 1-VP_SEL DIR_SEL NECLK
	vSPI_WriteData(0x8d);	// 2-STV_L_SEL STV_R_SEL RST_L_SEL RST_R_SEL
	vSPI_WriteData(0xa0);	// 3-CLK1_L_SEL CLK2_L_SEL CLK1_R_SEL CLK2_R_SEL
	vSPI_WriteData(0xf5);	// 4-
	vSPI_WriteData(0x27);
	*/
	// *********************** Gamma Control ****************************

	vSPI_WriteComm(0xC8);	//Gamma Setting A
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);

	vSPI_WriteComm(0xC9);	// Gamma Setting B
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);

	vSPI_WriteComm(0xCA);	// Gamma Setting C
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);

	// *********************** Power Control ****************************
	vSPI_WriteComm(0xD0);	// Power Setting
	vSPI_WriteData(0x19);	// 1-DC
	vSPI_WriteData(0x03);	// 2-
	vSPI_WriteData(0xCE);	// 3-VLMT
	vSPI_WriteData(0xA6);	// 4-DC, VLMT
	vSPI_WriteData(0x00);	// 5-SWRON SWRONM (0 is charge pump)
	vSPI_WriteData(0x43);	// 6-VC3, VC2
	vSPI_WriteData(0x20);	//
	vSPI_WriteData(0x10);	//
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);

	/*
	vSPI_WriteComm(0xD1);	// Switching Regulator
	vSPI_WriteData(0x18);	
	vSPI_WriteData(0x0C);
	vSPI_WriteData(0x23);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x75);
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x50);
	*/
	vSPI_WriteComm(0xD3);	// Power Setting for Internal Mode
	vSPI_WriteData(0x33);	// APN AP

	vSPI_WriteComm(0xD5);	// VPLVL VNLVL Setting
	vSPI_WriteData(0x2A);	// PVH
	vSPI_WriteData(0x2A);	// NVH
	/*
	vSPI_WriteComm(0xD6);	// Test Mode 2
	vSPI_WriteData(0xA8);

	vSPI_WriteComm(0xD7);	// Test Mode 3
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0xAA);
	vSPI_WriteData(0xC0);
	vSPI_WriteData(0x2A);
	vSPI_WriteData(0x2C);
	vSPI_WriteData(0x22);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x71);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x03);

	vSPI_WriteComm(0xD8);	// Test Mode 4
	vSPI_WriteData(0x44);
	vSPI_WriteData(0x44);
	vSPI_WriteData(0x22);
	vSPI_WriteData(0x44);
	vSPI_WriteData(0x21);
	vSPI_WriteData(0x46);
	vSPI_WriteData(0x42);
	vSPI_WriteData(0x40);

	vSPI_WriteComm(0xD9);	// Test Mode 5
	vSPI_WriteData(0xCF);
	vSPI_WriteData(0x2D);
	vSPI_WriteData(0x51);

	vSPI_WriteComm(0xDA);	// Test Mode 6
	vSPI_WriteData(0x01);
	*/
	vSPI_WriteComm(0xDE);	// VcomDC Setting
	vSPI_WriteData(0x01);	// WCVDC
	vSPI_WriteData(0x4f);	// 59

	vSPI_WriteComm(0xE6);	// VcomDC Setting 2
	vSPI_WriteData(0x4f);	// VDC

	vSPI_WriteComm(0xFA);	// VDC_SEL Setting
	vSPI_WriteData(0x03);	// VDC_SEL

	vSPI_WriteComm(0xB0);	//commands controll
	vSPI_WriteData(0x03);	//manufacturer commands disable

	vSPI_WriteComm(0x2A);	// column addres
	vSPI_WriteData(0x00);	// start H
	vSPI_WriteData(0x00);	// start L
	vSPI_WriteData(0x03);	// stop H
	vSPI_WriteData(0x1F);	// stop L

	vSPI_WriteComm(0x2B);	// page addres
	vSPI_WriteData(0x00);	// start H
	vSPI_WriteData(0x00);	// start L
	vSPI_WriteData(0x01);	// stop H
	vSPI_WriteData(0xDF);	// stop L

	vSPI_WriteComm(0x36);
	vSPI_WriteData(0x08);

	vSPI_WriteComm(0x3A);
	vSPI_WriteData(0x77);

	vSPI_WriteComm(0x11);
	rt_thread_mdelay(200);

	vSPI_WriteComm(0x29);
	rt_thread_mdelay(200);

	vSPI_WriteComm(0x13);
	vSPI_WriteComm(0x2C);



	//vSPI_WriteComm(0x00);	// NOP
	//vSPI_WriteComm(0x01);	// Soft Reset
	//vSPI_WriteComm(0x10);	// Enter Sleep Mode
	//vSPI_WriteComm(0x11);	// Exit Sleep Mode
	//vSPI_WriteComm(0x12);	// Enter Partial Mode
	//vSPI_WriteComm(0x13);	// Enter Normal Mode
	//vSPI_WriteComm(0x20);	// Exit Invert Mode
	//vSPI_WriteComm(0x21);	// Enter Invert Mode

	// ****************************************************************************
/*
 *
 *	NOP (00h)
 *	SWRESET (01h)
	SLPIN (10H)
	SLPOUT (11h)
	PTLON (12h)
	NORON (13h)
	INVOFF (20h)
	INVON (21h)
	ALLPOFF (22h)
	ALLPON (23h)
	DISPOFF (28h)
	DISPON (29h)
	IDMOFF (38h)
	IDMON (39h)
 */
					
}

void __LCD_Init(void);

int main(int argc, char **argv)
{

	gpio_set_func(LCD_vSPI_CS, IO_OUTPUT);
	gpio_set_func(LCD_vSPI_DI, IO_OUTPUT);
	gpio_set_func(LCD_vSPI_CLK, IO_OUTPUT);
	gpio_set_func(LCD_vSPI_RES, IO_OUTPUT);

    gpio_direction_output(LCD_vSPI_CS, 1);
	gpio_direction_output(LCD_vSPI_DI, 1);
	gpio_direction_output(LCD_vSPI_CLK, 1);
	gpio_direction_output(LCD_vSPI_RES, 1);


	rt_thread_mdelay(100);
	//LCD_Init();		//V1
	__LCD_Init();   	//V2
		
    return 0;
}

//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------

void __LCD_Init(void)
{
	// #3 BAFANG C010 HD397-40RGB-GL450

	gpio_set_value(LCD_vSPI_RES,1);
	rt_thread_mdelay(10);
	gpio_set_value(LCD_vSPI_RES,0);
	rt_thread_mdelay(200);
	gpio_set_value(LCD_vSPI_RES,1);
	rt_thread_mdelay(200);

	// ****************************************************************************		

	vSPI_WriteComm(0x11);
	//------------------------------------------Bank0 Setting----------------------------------------------------//
	//------------------------------------Display Control setting----------------------------------------------//
	rt_thread_mdelay(100);

	vSPI_WriteComm(0xFF);
	vSPI_WriteData(0x77);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x10);


	vSPI_WriteComm(0xC0);
	vSPI_WriteData(0x63);
	vSPI_WriteData(0x00);


	vSPI_WriteComm(0xC1);
	vSPI_WriteData(0x11);
	vSPI_WriteData(0x02);


	vSPI_WriteComm(0xC2);
	vSPI_WriteData(0x31);
	vSPI_WriteData(0x08);

	vSPI_WriteComm(0xC7);	//SDIR (Source Direction)
	vSPI_WriteData(0x04);
	
	//-------------------------------------Gamma Cluster Setting-------------------------------------------//

	vSPI_WriteComm(0xB0);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x0D);
	vSPI_WriteData(0x14);
	vSPI_WriteData(0x0D);
	vSPI_WriteData(0x10);
	vSPI_WriteData(0x05);
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x08);
	vSPI_WriteData(0x08);
	vSPI_WriteData(0x1E);
	vSPI_WriteData(0x05);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x11);
	vSPI_WriteData(0xA3);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x18); 


	vSPI_WriteComm(0xB1);
	vSPI_WriteData(0x40);
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x86);
	vSPI_WriteData(0x0D);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x09);
	vSPI_WriteData(0x05);
	vSPI_WriteData(0x09);
	vSPI_WriteData(0x09);
	vSPI_WriteData(0x1F);
	vSPI_WriteData(0x07);
	vSPI_WriteData(0x15);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x15);
	vSPI_WriteData(0x19);
	vSPI_WriteData(0x08);

	//---------------------------------------End Gamma Setting----------------------------------------------//
	//------------------------------------End Display Control setting----------------------------------------//
	//-----------------------------------------Bank0 Setting End---------------------------------------------//
	//-------------------------------------------Bank1 Setting---------------------------------------------------//
	//-------------------------------- Power Control Registers Initial --------------------------------------//

	vSPI_WriteComm(0xFF);
	vSPI_WriteData(0x77);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x11);


	vSPI_WriteComm(0xB0);
	vSPI_WriteData(0x50);
	//-------------------------------------------Vcom Setting---------------------------------------------------//

	vSPI_WriteComm(0xB1);
	vSPI_WriteData(0x68);
	//-----------------------------------------End Vcom Setting-----------------------------------------------//

	vSPI_WriteComm(0xB2);
	vSPI_WriteData(0x07);


	vSPI_WriteComm(0xB3);
	vSPI_WriteData(0x80);


	vSPI_WriteComm(0xB5);
	vSPI_WriteData(0x47);


	vSPI_WriteComm(0xB7);
	vSPI_WriteData(0x8A);


	vSPI_WriteComm(0xB8);
	vSPI_WriteData(0x21);

	vSPI_WriteComm(0xC1);
	vSPI_WriteData(0x78);


	vSPI_WriteComm(0xC2);
	vSPI_WriteData(0x78);

	vSPI_WriteComm(0xD0);
	vSPI_WriteData(0x88);
	
	//---------------------------------End Power Control Registers Initial -------------------------------//
	rt_thread_mdelay(100);
	//---------------------------------------------GIP Setting----------------------------------------------------//
	
	vSPI_WriteComm(0xE0);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x02);


	vSPI_WriteComm(0xE1);
	vSPI_WriteData(0x08);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x07);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x09);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x33);
	vSPI_WriteData(0x33);


	vSPI_WriteComm(0xE2);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);


	vSPI_WriteComm(0xE3);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x33);
	vSPI_WriteData(0x33);


	vSPI_WriteComm(0xE4);
	vSPI_WriteData(0x44);
	vSPI_WriteData(0x44);


	vSPI_WriteComm(0xE5);
	vSPI_WriteData(0x0E);
	vSPI_WriteData(0x2D);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0x10);
	vSPI_WriteData(0x2D);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x2D);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0x0C);
	vSPI_WriteData(0x2D);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0xA0);


	vSPI_WriteComm(0xE6);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x33);
	vSPI_WriteData(0x33);


	vSPI_WriteComm(0xE7);
	vSPI_WriteData(0x44);
	vSPI_WriteData(0x44);


	vSPI_WriteComm(0xE8);
	vSPI_WriteData(0x0D);
	vSPI_WriteData(0x2D);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x2D);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0x09);
	vSPI_WriteData(0x2D);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0x0B);
	vSPI_WriteData(0x2D);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0xA0);


	vSPI_WriteComm(0xEB);
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0xE4);
	vSPI_WriteData(0xE4);
	vSPI_WriteData(0x44);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x40);


	vSPI_WriteComm(0xEC);
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x01);


	vSPI_WriteComm(0xED);
	vSPI_WriteData(0xAB);
	vSPI_WriteData(0x89);
	vSPI_WriteData(0x76);
	vSPI_WriteData(0x54);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0xFF);
	vSPI_WriteData(0xFF);
	vSPI_WriteData(0xFF);
	vSPI_WriteData(0xFF);
	vSPI_WriteData(0xFF);
	vSPI_WriteData(0xFF);
	vSPI_WriteData(0x10);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x67);
	vSPI_WriteData(0x98);
	vSPI_WriteData(0xBA);
	
	//---------------------------------------------End GIP Setting-----------------------------------------------//
	//------------------------------ Power Control Registers Initial End-----------------------------------//
	//------------------------------------------Bank1 Setting----------------------------------------------------//

	vSPI_WriteComm(0xFF); // #3 Bank and Command sel
	vSPI_WriteData(0x77);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0x36);	// #3 OK
	vSPI_WriteData(0x18);


	rt_thread_mdelay(10);

	vSPI_WriteComm(0x29);





/*
	
	vSPI_WriteComm(0xB0);
	vSPI_WriteData(0x04);

	vSPI_WriteComm(0x6F);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xB3);//Frame Memory Access and Interface Setting
	vSPI_WriteData(0x12);//02(MCU)
	vSPI_WriteData(0x00);


	vSPI_WriteComm(0xB6);//DSI Control
	vSPI_WriteData(0x51);
	vSPI_WriteData(0x83);

	vSPI_WriteComm(0xB7);//MDDI Control
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x80);
	vSPI_WriteData(0x15);
	vSPI_WriteData(0x25);

	vSPI_WriteComm(0xBD);//Resizing Control
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xC0);//Panel Driving Setting 1
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x87);//PCDIVH,L

	vSPI_WriteComm(0xC1);//Panel Driving Setting 2
	vSPI_WriteData(0x43);//SS 23
	vSPI_WriteData(0x31);//NL
	vSPI_WriteData(0x99);//GIPMODE
	vSPI_WriteData(0x21);//GSPF
	vSPI_WriteData(0x20);//GSPS
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x10);//DIVI
	vSPI_WriteData(0x28);//RTN
	vSPI_WriteData(0x0C);//BP
	vSPI_WriteData(0x0A);//FP
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x21);
	vSPI_WriteData(0x01);

	vSPI_WriteComm(0xC2);//Display V-Timing Setting
	vSPI_WriteData(0x00);//LINEINV
	vSPI_WriteData(0x06);//SEQGND
	vSPI_WriteData(0x06);//SEQVCI
	vSPI_WriteData(0x01);//GEQ1W
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xC4);//Panel Driving Setting 3
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x01);

	vSPI_WriteComm(0xC6);//Outline Sharpening Control
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xC7);//Panel Driving Setting 4
	vSPI_WriteData(0x11);
	vSPI_WriteData(0x8D);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0xF5);
	vSPI_WriteData(0x27);


	vSPI_WriteComm(0xC8);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);

	vSPI_WriteComm(0xC9);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);

	vSPI_WriteComm(0xCA);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);

	vSPI_WriteComm(0xD0);//Power Setting (Charge Pump Setting)
	vSPI_WriteData(0x99);//DC
	vSPI_WriteData(0x03);
	vSPI_WriteData(0xCE);//VLMT
	vSPI_WriteData(0xA6);//DC, VLMT
	vSPI_WriteData(0x0C);//00 ²»ÒªMOS
	vSPI_WriteData(0x43);//VC3, VC2
	vSPI_WriteData(0x20);
	vSPI_WriteData(0x10);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xD1);//Power Setting (Switching Regulator Setting)
	vSPI_WriteData(0x18);
	vSPI_WriteData(0x0C);
	vSPI_WriteData(0x23);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x75);
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x50);

	vSPI_WriteComm(0xD3);//Power Setting for Internal Mode
	vSPI_WriteData(0x33);

	vSPI_WriteComm(0xD5);//VPLVL/VNLVL Setting
	vSPI_WriteData(0x2A);//VPLVL
	vSPI_WriteData(0x2A);//VNLVL

	vSPI_WriteComm(0xD6);
	vSPI_WriteData(0x28);

	vSPI_WriteComm(0xDA);
	vSPI_WriteData(0x01);

	vSPI_WriteComm(0xDE);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x4F);//VDC

	vSPI_WriteComm(0xE6);
	vSPI_WriteData(0x4F);//VDC2

	vSPI_WriteComm(0xFA);//VDC_SEL Setting
	vSPI_WriteData(0x03);
	
	vSPI_WriteComm(0xB0);	//commands controll
	vSPI_WriteData(0x03);	//manufacturer commands disable

	vSPI_WriteComm(0x2A);	// column addres
	vSPI_WriteData(0x00);	// start H
	vSPI_WriteData(0x00);	// start L
	vSPI_WriteData(0x03);	// stop H
	vSPI_WriteData(0x1F);	// stop L

	vSPI_WriteComm(0x2B);	// page addres
	vSPI_WriteData(0x00);	// start H
	vSPI_WriteData(0x00);	// start L
	vSPI_WriteData(0x01);	// stop H
	vSPI_WriteData(0xDF);	// stop L

	vSPI_WriteComm(0x36);
	vSPI_WriteData(0x08);

	vSPI_WriteComm(0x3A);
	vSPI_WriteData(0x77);

	vSPI_WriteComm(0x11);
	rt_thread_mdelay(200);

	vSPI_WriteComm(0x29);
	rt_thread_mdelay(200);

	vSPI_WriteComm(0x13);
	vSPI_WriteComm(0x2C);

	/*

	//SSD2805_Generic_packetsize_set(2);
	vSPI_WriteComm(0xB0);//B0
	vSPI_WriteData(0x04);

	//SSD2805_Generic_packetsize_set(3);
	vSPI_WriteComm(0x6f);//B0
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x00);

	//SSD2805_Generic_packetsize_set(3);
	vSPI_WriteComm(0xB3);//B3
	vSPI_WriteData(0x12);//12
	vSPI_WriteData(0x00);

	//SSD2805_Generic_packetsize_set(3);
	vSPI_WriteComm(0xB6);//B6
	vSPI_WriteData(0x51);
	vSPI_WriteData(0x83);

	//SSD2805_Generic_packetsize_set(2);
	vSPI_WriteComm(0xBd);//B6
	vSPI_WriteData(0x00);

	//SSD2805_Generic_packetsize_set(3);
	vSPI_WriteComm(0xc0);//B6
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x87);
	//*********************************************
	//SSD2805_Generic_packetsize_set(16);
	vSPI_WriteComm(0xC1);//C1
	vSPI_WriteData(0x43);//SS
	vSPI_WriteData(0x3F);//NL   31  800  3f  854
	vSPI_WriteData(0x99);//GIPMODE
	vSPI_WriteData(0x21);//GSPF
	vSPI_WriteData(0x20);//GSPS
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x10);//DIVI
	vSPI_WriteData(0x28);//RTN
	vSPI_WriteData(0x0C);//BP
	vSPI_WriteData(0x0A);//FP
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x21);
	vSPI_WriteData(0x01);

	//SSD2805_Generic_packetsize_set(7);
	vSPI_WriteComm(0xC2);//C2
	vSPI_WriteData(0x00);//LINEINV
	vSPI_WriteData(0x06);//SEQGND
	vSPI_WriteData(0x06);//SEQVCI
	vSPI_WriteData(0x01);//GEQ1W
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x00);

	//SSD2805_Generic_packetsize_set(3);
	vSPI_WriteComm(0xC4);//C2
	vSPI_WriteData(0x00);//LINEINV
	vSPI_WriteData(0x01);//SEQGND

	//SSD2805_Generic_packetsize_set(3);
	vSPI_WriteComm(0xC6);//C2
	vSPI_WriteData(0x00);//LINEINV
	vSPI_WriteData(0x00);//SEQGND

	//SSD2805_Generic_packetsize_set(6);
	vSPI_WriteComm(0xC7);//C2
	vSPI_WriteData(0x11);//LINEINV
	vSPI_WriteData(0x8d);//SEQGND
	vSPI_WriteData(0xa0);//SEQVCI
	vSPI_WriteData(0xf5);//GEQ1W
	vSPI_WriteData(0x27);


	//SSD2805_Generic_packetsize_set(25);
	vSPI_WriteComm(0xC8);//C8
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);

	//SSD2805_Generic_packetsize_set(25);
	vSPI_WriteComm(0xC9);//C9
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);

	//SSD2805_Generic_packetsize_set(25);
	vSPI_WriteComm(0xCA);//CA
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);

	//SSD2805_Generic_packetsize_set(17);
	vSPI_WriteComm(0xD0);//D0
	vSPI_WriteData(0x99);//DC
	vSPI_WriteData(0x03);
	vSPI_WriteData(0xCE);//VLMT
	vSPI_WriteData(0xA6);//DC, VLMT
	vSPI_WriteData(0x00);//...........
	vSPI_WriteData(0x43);//VC3, VC2
	vSPI_WriteData(0x20);//
	vSPI_WriteData(0x10);//
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);

	//SSD2805_Generic_packetsize_set(8);
	vSPI_WriteComm(0xD1);//D1
	vSPI_WriteData(0x18);
	vSPI_WriteData(0x0C);
	vSPI_WriteData(0x23);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x75);
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x50);

	//SSD2805_Generic_packetsize_set(2);
	vSPI_WriteComm(0xD3);//D3
	vSPI_WriteData(0x33);

	//SSD2805_Generic_packetsize_set(3);
	vSPI_WriteComm(0xD5);//D5
	vSPI_WriteData(0x2A);
	vSPI_WriteData(0x2A);

	//SSD2805_Generic_packetsize_set(2);
	vSPI_WriteComm(0xD6);//D3
	vSPI_WriteData(0x28);

	//SSD2805_Generic_packetsize_set(2);
	vSPI_WriteComm(0xDa);//D3
	vSPI_WriteData(0x01);

	//SSD2805_Generic_packetsize_set(3);
	vSPI_WriteComm(0xDE);//DE
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x4f);//59

	//SSD2805_Generic_packetsize_set(2);
	vSPI_WriteComm(0xe6);//D3
	vSPI_WriteData(0x4f);

	//SSD2805_Generic_packetsize_set(2);
	vSPI_WriteComm(0xFA);//FA
	vSPI_WriteData(0x03);


	vSPI_WriteComm(0xB0);
	vSPI_WriteData(0x03);

	vSPI_WriteComm(0x2A);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0xDF);

	vSPI_WriteComm(0x2B);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x1F);

	vSPI_WriteComm(0x36);
	vSPI_WriteData(0x08);

	vSPI_WriteComm(0x3A);
	vSPI_WriteData(0x55);

	vSPI_WriteComm(0x11);
	rt_thread_mdelay(200);

	vSPI_WriteComm(0x29);
	rt_thread_mdelay(20);

	vSPI_WriteComm(0x2C);

    */
	//ÄãºÃ£¡
	//    ÒÔÏÂÊÇÈýÖÖ²£Á§+R61408E×ª180¶ÈµÄÉèÖÃ

	//·­×ª180¶È

	

	//CMI 4.0, HATICH4.0

	//0¶È£ºC1H->43,31,00; 36H->00

	//180¶È£ºC1H->42,31,04; 36H->00

	

	//LG 4.0

	//0¶È£ºC1H->23,31,99; 36H->00

	//180¶È£ºC1H->22,31,99; 36H->01



	/*
	//GP_COMMAD_PA(2);
	vSPI_WriteComm(0xB0);//B0
	vSPI_WriteData(0x04);
	
	//GP_COMMAD_PA(3);
	vSPI_WriteComm(0x6f);//B0
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x00);

	//GP_COMMAD_PA(3);
	vSPI_WriteComm(0xB3);//B3
	vSPI_WriteData(0x12);//12
	vSPI_WriteData(0x00);

	//GP_COMMAD_PA(3);
	vSPI_WriteComm(0xB6);//B6
	vSPI_WriteData(0x51);
	vSPI_WriteData(0x83);

	//GP_COMMAD_PA(2);
	vSPI_WriteComm(0xBd);//B6
	vSPI_WriteData(0x00);

	//GP_COMMAD_PA(3);
	vSPI_WriteComm(0xc0);//B6
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x87);
	//*********************************************
	//GP_COMMAD_PA(16);
	vSPI_WriteComm(0xC1);//C1
	vSPI_WriteData(0x43);//SS
	vSPI_WriteData(0x31);//NL
	vSPI_WriteData(0x99);//GIPMODE
	vSPI_WriteData(0x21);//GSPF
	vSPI_WriteData(0x20);//GSPS
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x10);//DIVI
	vSPI_WriteData(0x28);//RTN
	vSPI_WriteData(0x0C);//BP
	vSPI_WriteData(0x0A);//FP
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x21);
	vSPI_WriteData(0x01);

	//GP_COMMAD_PA(7);
	vSPI_WriteComm(0xC2);//C2
	vSPI_WriteData(0x00);//LINEINV
	vSPI_WriteData(0x06);//SEQGND
	vSPI_WriteData(0x06);//SEQVCI
	vSPI_WriteData(0x01);//GEQ1W
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x00);

	//GP_COMMAD_PA(3);
	vSPI_WriteComm(0xC4);//C2
	vSPI_WriteData(0x00);//LINEINV
	vSPI_WriteData(0x01);//SEQGND

	//GP_COMMAD_PA(3);
	vSPI_WriteComm(0xC6);//C2
	vSPI_WriteData(0x00);//LINEINV
	vSPI_WriteData(0x00);//SEQGND

	//GP_COMMAD_PA(6);
	vSPI_WriteComm(0xC7);//C2
	vSPI_WriteData(0x11);//LINEINV
	vSPI_WriteData(0x8d);//SEQGND
	vSPI_WriteData(0xa0);//SEQVCI
	vSPI_WriteData(0xf5);//GEQ1W
	vSPI_WriteData(0x27);


	//GP_COMMAD_PA(25);
	vSPI_WriteComm(0xC8);//C8
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);

	//GP_COMMAD_PA(25);
	vSPI_WriteComm(0xC9);//C9
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);

	//GP_COMMAD_PA(25);
	vSPI_WriteComm(0xCA);//CA
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);

	//GP_COMMAD_PA(17);
	vSPI_WriteComm(0xD0);//D0
	vSPI_WriteData(0x99);//DC
	vSPI_WriteData(0x03);
	vSPI_WriteData(0xCE);//VLMT
	vSPI_WriteData(0xA6);//DC, VLMT
	vSPI_WriteData(0x00);//...........
	vSPI_WriteData(0x43);//VC3, VC2
	vSPI_WriteData(0x20);//
	vSPI_WriteData(0x10);//
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);

	//GP_COMMAD_PA(8);
	vSPI_WriteComm(0xD1);//D1
	vSPI_WriteData(0x18);
	vSPI_WriteData(0x0C);
	vSPI_WriteData(0x23);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x75);
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x50);


	vSPI_WriteComm(0xD3);//D3
	vSPI_WriteData(0x33);


	vSPI_WriteComm(0xD5);//D5
	vSPI_WriteData(0x2A);
	vSPI_WriteData(0x2A);


	vSPI_WriteComm(0xD6);//D3
	vSPI_WriteData(0x28);


	vSPI_WriteComm(0xDa);//D3
	vSPI_WriteData(0x01);


	vSPI_WriteComm(0xDE);//DE
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x4f);//59


	vSPI_WriteComm(0xe6);//D3
	vSPI_WriteData(0x4f);


	vSPI_WriteComm(0xFA);//FA
	vSPI_WriteData(0x03);


	vSPI_WriteComm(0xB0);//B0
	vSPI_WriteData(0x03);
	

	vSPI_WriteComm(0x11);//
	rt_thread_mdelay(200); 
	

	vSPI_WriteComm(0x36);//36
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0x3A);//3A
	vSPI_WriteData(0x70);

	vSPI_WriteComm(0x29);//29
	
	rt_thread_mdelay(200); 
	*/

	/*
	vSPI_WriteComm(0xB0);
	vSPI_WriteData(0x04);


	vSPI_WriteComm(0x6F);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x00);


	vSPI_WriteComm(0xB3);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x00);



	vSPI_WriteComm(0xB6);
	vSPI_WriteData(0x51);
	vSPI_WriteData(0x83);

	vSPI_WriteComm(0xB7);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x80);
	vSPI_WriteData(0x15);
	vSPI_WriteData(0x25);


	vSPI_WriteComm(0xBD);
	vSPI_WriteData(0x00);


	vSPI_WriteComm(0xC0);
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x87);


	vSPI_WriteComm(0xC1);
	vSPI_WriteData(0x43);
	vSPI_WriteData(0x31);
	vSPI_WriteData(0x99);
	vSPI_WriteData(0x21);
	vSPI_WriteData(0x20);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x10);
	vSPI_WriteData(0x28);
	vSPI_WriteData(0x0C);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x21);
	vSPI_WriteData(0x01);

	vSPI_WriteComm(0xC2);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x06);
	vSPI_WriteData(0x06);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xC3);
	vSPI_WriteData(0x40);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x03);


	vSPI_WriteComm(0xC4);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x01);

	vSPI_WriteComm(0xC6);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xC7);
	vSPI_WriteData(0x11);
	vSPI_WriteData(0x8D);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0xF5);
	vSPI_WriteData(0x27);

	vSPI_WriteComm(0xC8);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);


	vSPI_WriteComm(0xC9);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);


	vSPI_WriteComm(0xCA);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x1C);
	vSPI_WriteData(0x2B);
	vSPI_WriteData(0x45);
	vSPI_WriteData(0x3F);
	vSPI_WriteData(0x29);
	vSPI_WriteData(0x17);
	vSPI_WriteData(0x13);
	vSPI_WriteData(0x0F);
	vSPI_WriteData(0x04);


	vSPI_WriteComm(0xD0);
	vSPI_WriteData(0x99);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0xCE);
	vSPI_WriteData(0xA6);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x43);
	vSPI_WriteData(0x20);
	vSPI_WriteData(0x10);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xD1);
	vSPI_WriteData(0x18);
	vSPI_WriteData(0x0C);
	vSPI_WriteData(0x23);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x75);
	vSPI_WriteData(0x02);
	vSPI_WriteData(0x50);

	vSPI_WriteComm(0xD3);
	vSPI_WriteData(0x33);


	vSPI_WriteComm(0xD5);
	vSPI_WriteData(0x2A);
	vSPI_WriteData(0x2A);

	vSPI_WriteComm(0xD6);
	vSPI_WriteData(0x28);

	vSPI_WriteComm(0xD7);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0xAA);
	vSPI_WriteData(0xC0);
	vSPI_WriteData(0x2A);
	vSPI_WriteData(0x2C);
	vSPI_WriteData(0x22);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x71);
	vSPI_WriteData(0x0A);
	vSPI_WriteData(0x12);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0xA0);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x03);

	vSPI_WriteComm(0xD8);
	vSPI_WriteData(0x44);
	vSPI_WriteData(0x44);
	vSPI_WriteData(0x22);
	vSPI_WriteData(0x44);
	vSPI_WriteData(0x21);
	vSPI_WriteData(0x46);
	vSPI_WriteData(0x42);
	vSPI_WriteData(0x40);

	vSPI_WriteComm(0xD9);
	vSPI_WriteData(0xCF);
	vSPI_WriteData(0x2D);
	vSPI_WriteData(0x51);

	vSPI_WriteComm(0xDA);
	vSPI_WriteData(0x01);

	vSPI_WriteComm(0xDE);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0x4F);

	vSPI_WriteComm(0xE2);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xE3);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xE4);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x22);
	vSPI_WriteData(0xAA);
	vSPI_WriteData(0x00);


	vSPI_WriteComm(0xE5);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xE6);
	vSPI_WriteData(0x4F);

	vSPI_WriteComm(0xF3);
	vSPI_WriteData(0x06);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x24);
	vSPI_WriteData(0x00);


	vSPI_WriteComm(0xF8);
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0xFA);
	vSPI_WriteData(0x03);

	vSPI_WriteComm(0xFB);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);


	vSPI_WriteComm(0xFC);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);



	vSPI_WriteComm(0xFD);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x70);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x72);
	vSPI_WriteData(0x31);
	vSPI_WriteData(0x37);
	vSPI_WriteData(0x70);
	vSPI_WriteData(0x32);
	vSPI_WriteData(0x31);
	vSPI_WriteData(0x07);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);


	vSPI_WriteComm(0xFE);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x20);

	vSPI_WriteComm(0xB7);
	vSPI_WriteData(0x43);
	vSPI_WriteData(0x02);


	vSPI_WriteComm(0x2A);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x01);
	vSPI_WriteData(0xDF);

	vSPI_WriteComm(0x2B);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x00);
	vSPI_WriteData(0x03);
	vSPI_WriteData(0x1F);


	vSPI_WriteComm(0x20);

	vSPI_WriteComm(0x36); 
	vSPI_WriteData(0x00);

	vSPI_WriteComm(0x3A); 
	vSPI_WriteData(0x77);

	vSPI_WriteComm(0x11); 
	rt_thread_mdelay(500); 
	vSPI_WriteComm(0x29); 
	rt_thread_mdelay(500); 
	vSPI_WriteComm(0x13); 

	vSPI_WriteComm(0x2C); 
	*/    	
}

