
#include "lv_rtt_port.h"

#include "lvgl.h"
#include "lv_conf.h"


#include "drv_touch.h"
#include <rtdevice.h>

#define DBG_TAG  "lcd"
#define DBG_LVL  DBG_WARNING
#include <rtdbg.h>



static lv_disp_buf_t disp_buf;

static rt_device_t lcd_device;
static struct rt_device_graphic_info info;

static void lvgl_rtt_tick_run(void *p);
static void lvgl_rtt_lcd_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

rt_err_t lv_rtt_port_init(const char *name)
{
    /* littlevgl Init */
    lv_init();
    rt_err_t result;

    static lv_color_t lv_disp_buf1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
    static lv_color_t lv_disp_buf2[LV_HOR_RES_MAX * LV_VER_RES_MAX];

    /* LCD Device Init */
    lcd_device = rt_device_find("lcd");

    if(lcd_device == 0)
    {
        LOG_E("lcd can not be found!");
        return RT_ERROR;
    }

    result = rt_device_open(lcd_device, RT_DEVICE_OFLAG_RDWR);

    if( result != RT_EOK)
    {
        LOG_E("lcd_device can not be opened");  
        return RT_ERROR;
    }

    result = rt_device_control(lcd_device, RTGRAPHIC_CTRL_GET_INFO, &info);
    
    if( result != RT_EOK)
    {
        LOG_E("lcd_device info error"); 
        return RT_ERROR;
    }

    RT_ASSERT(info.bits_per_pixel == 8 || info.bits_per_pixel == 16 || 
              info.bits_per_pixel == 24 || info.bits_per_pixel == 32);

    if ((info.bits_per_pixel != LV_COLOR_DEPTH) && (info.bits_per_pixel != 32 && LV_COLOR_DEPTH != 24))
    {
        rt_kprintf("Error: framebuffer color depth mismatch! (Should be %d to match with LV_COLOR_DEPTH)",
                info.bits_per_pixel);
        return RT_ERROR;
    }



#if USE_LV_LOG
    /* littlevgl Log Init */
    lv_log_register_print(lvgl_rtt_log_register);
#endif

    //  rt_memset(buffer, 0x00, LV_HOR_RES_MAX * LV_VER_RES_MAX*sizeof(lv_color_t));
    /* Example for 1) */
    
    lv_disp_buf_init(&disp_buf, lv_disp_buf1, lv_disp_buf2, LV_HOR_RES_MAX * LV_VER_RES_MAX);
    /* littlevGL Display device interface */
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    /*Set the resolution of the display*/
    disp_drv.hor_res = info.width;
    disp_drv.ver_res = info.height;

    /*Set a display buffer*/
    disp_drv.buffer = &disp_buf;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = lvgl_rtt_lcd_flush_cb;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);

#ifdef    RT_USING_TOUCH
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lvgl_rtt_input_read;
    start_touch_listen(lvgl_rtt_send_input_event);
    lv_indev_drv_register(&indev_drv);
#endif
    /* littlevGL Tick thread */
    rt_thread_t thread = RT_NULL;

    thread = rt_thread_create("lv_tick", lvgl_rtt_tick_run, RT_NULL, 512, 6, 10);
    if (thread == RT_NULL)
    {
        return RT_ERROR;
    }
    rt_thread_startup(thread);

    return RT_EOK;
}


static void lvgl_rtt_lcd_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*Return if the area is out the screen*/
    lv_coord_t hres = disp_drv->rotated == 0 ? disp_drv->hor_res : disp_drv->ver_res;
    lv_coord_t vres = disp_drv->rotated == 0 ? disp_drv->ver_res : disp_drv->hor_res;

    /*Return if the area is out the screen*/
    if (area->x2 < 0 || area->y2 < 0 || area->x1 > hres - 1 || area->y1 > vres - 1)
    {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    int32_t y;
#if LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32    /*32 is valid but support 24 for backward compatibility too*/

    uint32_t w = lv_area_get_width(area);
    uint32_t *fbp32 = (uint32_t*) info.framebuffer;
    for (y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++)
    {
        memcpy(&fbp32[y * info.width + area->x1], color_p, w * sizeof(lv_color_t));
        color_p += w;
    }
#elif
    int32_t x;
    for(y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++)
    {
        for(x = area->x1; x <= area->x2; x++)
        {
            fbp32[y * disp_drv->hor_res + x] = lv_color_to32(*color_p);
            color_p++;
        }

    }
#endif
//    if(area->x2 < 0) return;
//    if(area->y2 < 0) return;
//    if(area->x1 > info.width  - 1) return;
//    if(area->y1 > info.height - 1) return;
//
//    /*Truncate the area to the screen*/
//    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
//    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
//    int32_t act_x2 = area->x2 > info.width  - 1 ? info.width  - 1 : area->x2;
//    int32_t act_y2 = area->y2 > info.height - 1 ? info.height - 1 : area->y2;
//
//    uint32_t x;
//    uint32_t y;
//    long int location = 0;
//   // rt_kprintf("i.w=%d i.h=%d x1=%d y1=%d x2=%d y2=%d\n", info.width,info.height,act_x1,act_y1,act_x2,act_y2);
//    /* 24 or 32 bit per pixel */
//    if(info.bits_per_pixel == 24 || info.bits_per_pixel == 32)
//       {
//        uint32_t *fbp32 = (uint32_t*)info.framebuffer;
//
//           for(y = act_y1; y <= act_y2; y++)
//           {
//               for(x = act_x1; x <= act_x2; x++)
//               {
//                   location = (x) + (y) * info.width;
//                   fbp32[location] = (color_p->full);
//                   color_p++;
//               }
//
//             color_p += area->x2 - act_x2;
//           }
//       }
//    /* 8 bit per pixel */
//    else if(info.bits_per_pixel == 8)
//    {
//        uint8_t *fbp8 = (uint8_t*)info.framebuffer;
//
//        for(y = act_y1; y <= act_y2; y++)
//        {
//            for(x = act_x1; x <= act_x2; x++)
//            {
//                location = (x) + (y) * info.width;
//                fbp8[location] = color_p->full;
//                color_p++;
//            }
//
//            color_p += area->x2 - act_x2;
//        }
//    }
//
//    /* 16 bit per pixel */
//    else if(info.bits_per_pixel == 16)
//    {
//        uint16_t *fbp16 = (uint16_t*)info.framebuffer;
//
//        for(y = act_y1; y <= act_y2; y++)
//        {
//            for(x = act_x1; x <= act_x2; x++)
//            {
//                location = (x) + (y) * info.width;
//                fbp16[location] = color_p->full;
//                color_p++;
//            }
//
//            color_p += area->x2 - act_x2;
//        }
//    }
//

    struct rt_device_rect_info rect_info;

    rect_info.x = area->x1;
    rect_info.y = area->y1;
    rect_info.width = area->x2 - area->x1;
    rect_info.height = area->y2 - area->y1;
    rt_device_control(lcd_device, RTGRAPHIC_CTRL_RECT_UPDATE, &rect_info);

    lv_disp_flush_ready(disp_drv);
}

static rt_bool_t touch_down = RT_FALSE;
static rt_int16_t last_x = 0;
static rt_int16_t last_y = 0;

static bool lvgl_rtt_input_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = (touch_down == RT_TRUE) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    return false;
}

static void lvgl_rtt_tick_run(void *p)
{
    while (1)
    {
        lv_tick_inc(5);
        rt_thread_mdelay(5);
    }
}

void lvgl_rtt_send_input_event(touch_message_t msg)
{
    last_x = msg->x;
    last_y = msg->y;
    switch (msg->event)
    {
    case TOUCH_EVENT_UP:
    case TOUCH_EVENT_NONE:
        touch_down = RT_FALSE;
        break;
    case TOUCH_EVENT_DOWN:
    case TOUCH_EVENT_MOVE:
        touch_down = RT_TRUE;
        break;
    }
}

#if USE_LV_LOG
void lvgl_rtt_log_register(lv_log_level_t level, const char * file, uint32_t line, const char * dsc)
{
    if(level >= LV_LOG_LEVEL)
    {
        //Show the log level if you want
        if(level == LV_LOG_LEVEL_TRACE)
        {
            rt_kprintf("Trace:");
        }

        rt_kprintf("%s\n", dsc);
        //You can write 'file' and 'line' too similary if required.
    }
}
#endif
