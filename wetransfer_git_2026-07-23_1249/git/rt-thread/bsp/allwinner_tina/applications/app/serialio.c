
#include <rtthread.h>
#include <rtdevice.h>
#include "serialio.h"
#include "drv_gpio.h"


struct rt_spi_device* arcode_dev;

void arcode_init(void)
{
	
	
	arcode_dev = (struct rt_spi_device *)rt_device_find("spi10");

    if (arcode_dev)
    {
		struct  rt_spi_configuration cfg;

		cfg.data_width = 8;
		cfg.max_hz = 1*500*1000;
		cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;

		rt_spi_configure(arcode_dev, &cfg);
    }
	else
	{
		rt_kprintf("spi sample run failed!\n");
	}
}


uint8_t arcode_crc(uint8_t *ptr)
{
	uint8_t size, i, checksum;
	size = *ptr;
	checksum = 0x33;
	for(i = 0; i < size - 1; i++){
		checksum += *ptr;
		ptr++;	
	}
	return checksum;	
}


void arcode_xfer(uint8_t* rx_data)
{
	uint8_t tx_data[2] = {0,0};

    if (arcode_dev)
    {
		struct rt_spi_message msg;
		tx_data[0] = 0x80;
		msg.send_buf = tx_data;
		msg.recv_buf = RT_NULL;
		msg.length = 1;
		msg.cs_take= 1;
		msg.cs_release = 1;
		msg.next = RT_NULL;

		rt_spi_transfer_message(arcode_dev, &msg);

		tx_data[0] = 0;
		msg.send_buf = tx_data;
		msg.length = 1;
		msg.cs_take= 1;
		msg.cs_release = 1;
		msg.next = RT_NULL;
		
		int i;
		for( i = 0; i<28 ;i++)
		{
			msg.recv_buf = &rx_data[i];
        	rt_spi_transfer_message(arcode_dev, &msg);
		}
    }
}

/*
#define IO_SER_CLK		GPIO_PORT_E, GPIO_PIN_8
#define IO_SER_SDI		GPIO_PORT_E, GPIO_PIN_7
#define IO_SER_SDO		GPIO_PORT_E, GPIO_PIN_6
#define IO_SER_LO		GPIO_PORT_E, GPIO_PIN_5
#define IO_SER_LI		GPIO_PORT_E, GPIO_PIN_4

void serialio_init(void)
{
	gpio_set_func(IO_SER_SDO, IO_INPUT);

	gpio_set_func(IO_SER_CLK, IO_OUTPUT);
	gpio_set_func(IO_SER_SDI, IO_OUTPUT);
	gpio_set_func(IO_SER_LO,  IO_OUTPUT);
	gpio_set_func(IO_SER_LI,  IO_OUTPUT);

	gpio_direction_input(IO_SER_SDO);

    gpio_direction_output(IO_SER_CLK, 1);
	gpio_direction_output(IO_SER_SDI, 1);
	gpio_direction_output(IO_SER_LO,  1);
	gpio_direction_output(IO_SER_LI,  1);

}

void serialio_send( unsigned int data )
{
	gpio_set_value( IO_SER_CLK, 0); //HC595_SCLK_LO;
	gpio_set_value( IO_SER_LO,  0); //HC595_RCLK_LO;

	for(unsigned int i=0;i<8;i++)
	{
		if(data & (1<<7))			//MSB first IO_SER_SDI
			gpio_set_value( IO_SER_SDI, 1);//HC595_DATA_HI;
		else
			gpio_set_value( IO_SER_SDI, 0);//HC595_DATA_LO;
		data<<=1;
			
		gpio_set_value( IO_SER_CLK, 1);//HC595_SCLK_HI;
		gpio_set_value( IO_SER_CLK, 0);//HC595_SCLK_LO;
	}	
	
	gpio_set_value( IO_SER_LO,  1); //HC595_RCLK_HI;
	gpio_set_value( IO_SER_LO,  0); //HC595_RCLK_LO;
}

unsigned int serialio_get(void)
{
	unsigned int hc=0;
	
	gpio_set_value( IO_SER_CLK, 1);//HC165_SCLK_HI;
	
	gpio_set_value( IO_SER_LI,  1); //HC165_LOAD_HI;
	gpio_set_value( IO_SER_LI,  0); //HC165_LOAD_LO;
	gpio_set_value( IO_SER_LI,  1); //HC165_LOAD_HI;
	
	gpio_set_value( IO_SER_CLK, 0);//HC165_SCLK_LO;
	
	for(int i=0;i<16;i++)
	{
		hc <<=1;
		
		if( gpio_get_value( IO_SER_SDO))//HC165_DATA_GET )
		{
			hc |= 1;
		}
		gpio_set_value( IO_SER_CLK,  1); //HC165_SCLK_HI;
		gpio_set_value( IO_SER_CLK,  0); //HC165_SCLK_LO;

	}
	
	return hc;
}
*/