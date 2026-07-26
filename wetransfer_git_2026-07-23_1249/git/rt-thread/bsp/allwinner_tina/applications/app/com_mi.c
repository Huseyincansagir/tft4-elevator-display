/*
 * Program list: This is a uart device usage routine
 * The routine exports the uart_sample command to the control terminal
 * Format of command: uart_sample uart2
 * Command explanation: the second parameter of the command is the name of the uart device. If it is null, the default uart device wil be used
 * Program function: output the string "hello RT-Thread!" through the serial port, and then malposition the input character
 * 
*/

#include <rtthread.h>

#define SAMPLE_UART_NAME       "uart0"

/* Semaphore used to receive messages */
static struct rt_semaphore rx_sem;
static rt_device_t serial;

static uint8_t* rxdata[64];

volatile uint8_t myse = 0;

uint8_t mi_get(uint8_t* data, uint32_t len)
{
    if(myse)
    {
        rt_memcpy(data,rxdata,len);
        return len;
    }
    
    return 0;
}

/* Receive data callback function */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    /* After the uart device receives the data, it generates an interrupt, calls this callback function, and then sends the received semaphore. */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

static void serial_thread_entry(void *parameter)
{
    char ch;
    uint32_t index = 0, len = 0;
    uint8_t data[64];


    while (1)
    {
        /* Read a byte of data from the serial port and wait for the receiving semaphore if it is not read */
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            /* Being Suspended and waiting for the semaphore */
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }
        /* Read the data from the serial port and output through dislocation */

        data[index] = ch;

        switch(index)
        {
            case 0:if(ch == 'M') index++; break;
            case 1:if(ch == 'C') index++; break;
            case 2:if(ch == 'I') index++; break;
            case 3:if(ch == '1') index++; break;      

            default:
                index++;
                break;
        }

        if(index == 49)
        {
            rt_memcpy(rxdata, &data[4], 45);
            rt_memset(data,0,64);
            index = 0;
            myse = 1;
        }
    }
}

int uart_sample(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];
    char str[] = "hello RT-Thread!\r\n";

    if (argc == 2)
    {
        rt_strncpy(uart_name, argv[1], RT_NAME_MAX);
    }
    else
    {
        rt_strncpy(uart_name, SAMPLE_UART_NAME, RT_NAME_MAX);
    }

    /* Find uart devices in the system */
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", uart_name);
        return RT_ERROR;
    }

    /* Initialize the semaphore */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* Open the uart device in interrupt receive and polling send mode */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* Set the receive callback function */
    rt_device_set_rx_indicate(serial, uart_input);
    /* Send string */
    rt_device_write(serial, 0, str, (sizeof(str) - 1));

    /* Create a serial thread */
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 1024, 25, 10);
    /* Start the thread successfully */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }

    return ret;
}
/* Export to the msh command list */
//m MSH_CMD_EXPORT(uart_sample, uart device sample);