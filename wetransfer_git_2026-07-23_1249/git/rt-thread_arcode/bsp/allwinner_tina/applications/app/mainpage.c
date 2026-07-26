#include "mainpage.h"
#include <rtthread.h> 
#include <rtdevice.h> 
#include "lv_rtt_port.h"
#include "serialio.h"
#include "convert.h"
#include "arcode.h"
#include "meprom.h"
#include <ctype.h>

// *****  Display Settings     **********

#define ELEV_DEMO //demo modda calısir
//#define BUTSAN //butsana ait ok logosunu kullanir

//#define DISPLAY_MSG	//ingilizce tam sayfa mesajlari gosterir

/*
 * Ekran yonelimi artik CALISMA ZAMANINDA degisir (NXT butonu).
 *
 * Eskiden burada DISPLAY_HOR / DISPLAY_VER #define'i vardi. Asagidaki yerlesim
 * makrolari ayni isimlerle duruyor ama degerlerini g_orient uzerinden seciyor;
 * boylece lv_obj_align()/lv_img_set_angle() cagri noktalari aynen korundu.
 *
 * ORIENT_VER = dik montaj, ORIENT_HOR = yan montaj (icerik 90 derece doner).
 */
#define ORIENT_VER		0
#define ORIENT_HOR		1

uint8_t g_orient = ORIENT_VER;		/* acilista meprom_load_orient() ile ezilir */

#define MLV_WHITE           lv_color_hex(0xffffff)
#define MLV_BLACK           lv_color_hex(0x000000)
#define MLV_LIGHT           lv_color_hex(0xf3f8fe)
#define MLV_GRAY            lv_color_hex(0x8a8a8a)
#define MLV_LIGHT_GRAY      lv_color_hex(0xc4c4c4)
#define MLV_BLUE            lv_color_hex(0x1E88E5)
#define MLV_NAVYBLUE        lv_color_hex(0x1621c8)
#define MLV_GREEN           lv_color_hex(0x4cb242)
#define MLV_RED             lv_color_hex(0xF01008)
#define MLV_ORANGE          lv_color_hex(0xFC4500)
#define MLV_AMBER 			lv_color_hex(0xFA7e00)

#define THEME_def
#define SERVICE_ICON_COLOR	MLV_AMBER
#define BUSY_ICON_COLOR		MLV_RED	

//theme names first number color and bg color
//#define THEME_ORANGE_BLACK
//#define THEME_WHITE_BLACK
//#define THEME_BLUE_BLACK
//#define THEME_BLACK_WHITE


//calisma basladi

#ifdef THEME_ORANGE_BLACK
 #define FLOOR_NUM_OPA		255
 #define FLOOR_NUM_COLOR 	MLV_ORANGE
 #define SCR_BG_COLOR		MLV_BLACK
#elif defined THEME_WHITE_BLACK
 #define FLOOR_NUM_OPA		255
 #define FLOOR_NUM_COLOR 	MLV_WHITE
 #define SCR_BG_COLOR		MLV_BLACK
#elif defined THEME_BLACK_WHITE
 #define FLOOR_NUM_OPA		255
 #define FLOOR_NUM_COLOR 	MLV_BLACK
 #define SCR_BG_COLOR		MLV_WHITE
#elif defined THEME_BLUE_BLACK
 #define FLOOR_NUM_OPA		255
 #define FLOOR_NUM_COLOR 	MLV_BLUE
 #define SCR_BG_COLOR		MLV_BLACK
#else
 #define FLOOR_NUM_OPA		255
 #define FLOOR_NUM_COLOR 	MLV_NAVYBLUE
 #define SCR_BG_COLOR		MLV_WHITE
#endif

/*
 * Yerlesim sabitleri — degerler eskisiyle BIREBIR ayni, sadece secim
 * derleme zamanindan calisma zamanina tasindi.
 *
 *                          DIK (VER)              YAN (HOR)
 *   THIS_ROTATION            0                      900  (= 90.0 derece)
 *   SIGNAL_Y_OFFSET         14                       10
 *   MID_CHAR_Y_OFFSET      -34                        0
 *   LEFT_CHAR  X/Y       -110 / -34                 0 / -120
 *   RIGHT_CHAR X/Y        110 / -34                 0 /  100
 */
#define THIS_ROTATION			((g_orient == ORIENT_HOR) ? 900 : 0)

#define SIGNAL_ALIGN			LV_ALIGN_IN_TOP_MID
#define SIGNAL_X_OFFSET			0
#define SIGNAL_Y_OFFSET			((g_orient == ORIENT_HOR) ? 10 : 14)

#define MID_CHAR_ALIGN			LV_ALIGN_IN_BOTTOM_MID
#define MID_CHAR_X_OFFSET		0
#define MID_CHAR_Y_OFFSET		((g_orient == ORIENT_HOR) ? 0 : -34)

#define LEFT_CHAR_ALIGN			LV_ALIGN_IN_BOTTOM_MID
#define LEFT_CHAR_X_OFFSET		((g_orient == ORIENT_HOR) ? 0 : -110)
#define LEFT_CHAR_Y_OFFSET		((g_orient == ORIENT_HOR) ? -120 : -34)

#define RIGHT_CHAR_ALIGN		LV_ALIGN_IN_BOTTOM_MID
#define RIGHT_CHAR_X_OFFSET		((g_orient == ORIENT_HOR) ? 0 : 110)
#define RIGHT_CHAR_Y_OFFSET		((g_orient == ORIENT_HOR) ? 100 : -34)



/* 桌面的ICON信息定义 */

//static lv_obj_t * _main_page_app_null(lv_obj_t *parent, void *user_data);

/**
* @brief APP空函数
* @param user_data-自定义数据
* @retval	None
*/
/*
static lv_obj_t * _main_page_app_null(lv_obj_t *parent, void *user_data)
{

	(void)user_data;
	(void)parent;
	return NULL;
}
*/
static void _main_page_msg_hander(MAIN_PAGE_T *p_main_page, uint8_t *buffer)
{
	switch (buffer[0]) {
	case GUI_MQ_KEY:
	//printf("key:%d",buffer[1]);
	if (p_main_page->p_app != NULL) {
		lv_event_send(p_main_page->p_app, LV_EVENT_KEY, (const void *)&buffer[1]);/* 向当前打开的app 发送按键值 */
	}

	break;
	
	default:
	break;
	}
}

static void _main_page_update_task_cb(lv_task_t *t)
{
	static uint8_t mq_rec_bufffer[MQ_MAX_LEN] = { 0 };
	if (common_message_recv(mq_rec_bufffer, MQ_MAX_LEN) == 0) { /* 接收消息 */
		_main_page_msg_hander((MAIN_PAGE_T*)t->user_data,mq_rec_bufffer);					/* 处理消息 */
	}
}

lv_obj_t* p_main_bg = RT_NULL;
lv_obj_t* p_number_mid = RT_NULL;
lv_obj_t* p_number_left = RT_NULL;
lv_obj_t* p_number_right = RT_NULL;
lv_obj_t* p_signal = RT_NULL;


LV_IMG_DECLARE(bg_png)

/* Her iki yonelim setinin tamami: secim calisma zamaninda yapiliyor. */

	LV_IMG_DECLARE(fa_0)
	LV_IMG_DECLARE(fa_1)
	LV_IMG_DECLARE(fa_2)
	LV_IMG_DECLARE(fa_3)
	LV_IMG_DECLARE(fa_4)
	LV_IMG_DECLARE(fa_5)
	LV_IMG_DECLARE(fa_6)
	LV_IMG_DECLARE(fa_7)
	LV_IMG_DECLARE(fa_8)
	LV_IMG_DECLARE(fa_9)

	LV_IMG_DECLARE(fa_a)
	LV_IMG_DECLARE(fa_b)
	LV_IMG_DECLARE(fa_c)
	LV_IMG_DECLARE(fa_d)
	LV_IMG_DECLARE(fa_e)
	LV_IMG_DECLARE(fa_f)
	LV_IMG_DECLARE(fa_g)
	LV_IMG_DECLARE(fa_h)
	LV_IMG_DECLARE(fa_i)
	LV_IMG_DECLARE(fa_j)
	LV_IMG_DECLARE(fa_k)
	LV_IMG_DECLARE(fa_l)
	LV_IMG_DECLARE(fa_m)
	LV_IMG_DECLARE(fa_n)
	LV_IMG_DECLARE(fa_o)
	LV_IMG_DECLARE(fa_p)
	LV_IMG_DECLARE(fa_q)
	LV_IMG_DECLARE(fa_r)
	LV_IMG_DECLARE(fa_s)
	LV_IMG_DECLARE(fa_t)
	LV_IMG_DECLARE(fa_u)
	LV_IMG_DECLARE(fa_v)
	LV_IMG_DECLARE(fa_w)
	LV_IMG_DECLARE(fa_y)
	LV_IMG_DECLARE(fa_z)

	LV_IMG_DECLARE(sy_m)
	
#ifdef BUTSAN
	LV_IMG_DECLARE(iconUpArrowVerButsan)
	LV_IMG_DECLARE(iconDownArrowVerButsan)
#else
	LV_IMG_DECLARE(iconUpArrowVer)
	LV_IMG_DECLARE(iconDownArrowVer)
#endif
	LV_IMG_DECLARE(messageOutOfServiceVerEn)			//0
	LV_IMG_DECLARE(messageOverloadedVerEn)				//1
	LV_IMG_DECLARE(messageLiftOnMaintenanceVerEn)		//2
	LV_IMG_DECLARE(messageFireEvacuationVerEn)			//3
	LV_IMG_DECLARE(messageBackupPowerEvacuationVerEn)	//4
	LV_IMG_DECLARE(messageStartupVerEn)					//5
	LV_IMG_DECLARE(messageDoorOpenErrorVerEn)			//6
	LV_IMG_DECLARE(messageIdleVerEn)					//7
	LV_IMG_DECLARE(messageServiceControlVerEn)			//8,9	

	LV_IMG_DECLARE(fa0_png)
	LV_IMG_DECLARE(fa1_png)
	LV_IMG_DECLARE(fa2_png)
	LV_IMG_DECLARE(fa3_png)
	LV_IMG_DECLARE(fa4_png)
	LV_IMG_DECLARE(fa5_png)
	LV_IMG_DECLARE(fa6_png)
	LV_IMG_DECLARE(fa7_png)
	LV_IMG_DECLARE(fa8_png)
	LV_IMG_DECLARE(fa9_png)

	LV_IMG_DECLARE(faa_png)
	LV_IMG_DECLARE(fab_png)
	LV_IMG_DECLARE(fac_png)
	LV_IMG_DECLARE(fad_png)
	LV_IMG_DECLARE(fae_png)
	LV_IMG_DECLARE(faf_png)
	LV_IMG_DECLARE(fag_png)
	LV_IMG_DECLARE(fah_png)
	LV_IMG_DECLARE(fai_png)
	LV_IMG_DECLARE(faj_png)
	LV_IMG_DECLARE(fak_png)
	LV_IMG_DECLARE(fal_png)
	LV_IMG_DECLARE(fam_png)
	LV_IMG_DECLARE(fan_png)
	LV_IMG_DECLARE(fao_png)
	LV_IMG_DECLARE(fap_png)
	LV_IMG_DECLARE(faq_png)
	LV_IMG_DECLARE(far_png)
	LV_IMG_DECLARE(fas_png)
	LV_IMG_DECLARE(fat_png)
	LV_IMG_DECLARE(fau_png)
	LV_IMG_DECLARE(fav_png)
	LV_IMG_DECLARE(faw_png)
	LV_IMG_DECLARE(fay_png)
	LV_IMG_DECLARE(faz_png)

	LV_IMG_DECLARE(faminus_png)

#ifdef BUTSAN
	LV_IMG_DECLARE(iconUpArrowHorButsan)
	LV_IMG_DECLARE(iconDownArrowHorButsan)
#else
	LV_IMG_DECLARE(iconUpArrowHor)
	LV_IMG_DECLARE(iconDownArrowHor)
#endif

	LV_IMG_DECLARE(messageOutOfServiceHorEn)			//0
	LV_IMG_DECLARE(messageOverloadedHorEn)				//1
	LV_IMG_DECLARE(messageLiftOnMaintenanceHorEn)		//2
	LV_IMG_DECLARE(messageFireEvacuationHorEn)			//3
	LV_IMG_DECLARE(messageBackupPowerEvacuationHorEn)	//4
	LV_IMG_DECLARE(messageStartupHorEn)					//5
	LV_IMG_DECLARE(messageDoorOpenErrorHorEn)			//6
	LV_IMG_DECLARE(messageIdleHorEn)					//7
	LV_IMG_DECLARE(messageServiceControlHorEn)			//8,9	


LV_IMG_DECLARE(ay_png)

LV_IMG_DECLARE(yo2_png)
LV_IMG_DECLARE(servisd_png)
LV_IMG_DECLARE(mesgul_png)

LV_IMG_DECLARE(iconFiremanControl)

LV_IMG_DECLARE(empty_png)
LV_IMG_DECLARE(bg_white_png)


//	LV_IMG_DECLARE(butsan)


void display_state(uint32_t state)
{
	if(state == LS_FIREMAN_CONTROL)
	{
		lv_obj_set_hidden(p_signal, RT_FALSE);

		lv_obj_reset_style_list(p_signal, LV_OBJ_PART_MAIN);

		lv_img_set_src(p_signal, &iconFiremanControl);
		lv_img_set_angle(p_signal, THIS_ROTATION);
		lv_obj_align(p_signal,NULL, SIGNAL_ALIGN, SIGNAL_X_OFFSET , SIGNAL_Y_OFFSET);

		lv_obj_set_style_local_image_recolor_opa(p_signal, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		lv_obj_set_style_local_image_recolor(p_signal, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, BUSY_ICON_COLOR);
	}
	else if (state == LS_OUT_OF_SERVICE)
	{
		lv_obj_set_hidden(p_signal, RT_FALSE);

		lv_img_set_src(p_signal, &servisd_png);
		lv_img_set_angle(p_signal, THIS_ROTATION);
		lv_obj_align(p_signal,NULL, SIGNAL_ALIGN, SIGNAL_X_OFFSET, SIGNAL_Y_OFFSET);

		lv_obj_set_style_local_image_recolor_opa(p_signal, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		lv_obj_set_style_local_image_recolor(p_signal, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, SERVICE_ICON_COLOR);
	}
	else if(state == LS_OVERLOAD)
	{
		lv_obj_set_hidden(p_signal, RT_FALSE);

		lv_img_set_src(p_signal, &ay_png);
		lv_img_set_angle(p_signal, THIS_ROTATION);
		lv_obj_align(p_signal, NULL, SIGNAL_ALIGN, SIGNAL_X_OFFSET, SIGNAL_Y_OFFSET);

		lv_obj_set_style_local_image_recolor_opa(p_signal, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		lv_obj_set_style_local_image_recolor(p_signal, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, BUSY_ICON_COLOR);

	}
	else if((state == LS_MOVING_UP) || (state == LS_COLLECT_UP))
	{
		lv_obj_set_hidden(p_signal, RT_FALSE);
		if(g_orient == ORIENT_HOR)
		{
			#ifdef BUTSAN
			lv_img_set_src(p_signal, &iconUpArrowHorButsan);
			#else
			lv_img_set_src(p_signal, &iconUpArrowHor);
			#endif
			lv_obj_set_size(p_signal, 480, 320);
		}
		else
		{
			#ifdef BUTSAN
			lv_img_set_src(p_signal, &iconUpArrowVerButsan);
			#else
			lv_img_set_src(p_signal, &iconUpArrowVer);
			#endif
			lv_obj_set_size(p_signal, 280, 380);
		}
		lv_img_set_angle(p_signal, 0);
		lv_obj_reset_style_list(p_signal, LV_OBJ_PART_MAIN);
		if(state == LS_MOVING_UP)
			lv_obj_align(p_signal, NULL, SIGNAL_ALIGN, SIGNAL_X_OFFSET, SIGNAL_Y_OFFSET);
		else
			lv_obj_align(p_signal, NULL, LV_ALIGN_CENTER, SIGNAL_X_OFFSET, SIGNAL_Y_OFFSET);
		
		//lv_obj_set_style_local_image_recolor_opa(p_signal, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		//lv_obj_set_style_local_image_recolor(p_signal, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, MLV_WHITE);
	}
	else if((state == LS_MOVING_DOWN)||(state == LS_COLLECT_DOWN))
	{
		lv_obj_set_hidden(p_signal, RT_FALSE);
		if(g_orient == ORIENT_HOR)
		{
			#ifdef BUTSAN
			lv_img_set_src(p_signal, &iconDownArrowHorButsan);
			#else
			lv_img_set_src(p_signal, &iconDownArrowHor);
			#endif
			lv_obj_set_size(p_signal, 480, 320);
		}
		else
		{
			#ifdef BUTSAN
			lv_img_set_src(p_signal, &iconDownArrowVerButsan);
			#else
			lv_img_set_src(p_signal, &iconDownArrowVer);
			#endif
			lv_obj_set_size(p_signal, 280, 380);
		}
		lv_img_set_angle(p_signal, 0);
		lv_obj_reset_style_list(p_signal, LV_OBJ_PART_MAIN);
		
		if(state == LS_MOVING_DOWN)
			lv_obj_align(p_signal, NULL, SIGNAL_ALIGN, SIGNAL_X_OFFSET, SIGNAL_Y_OFFSET);
		else
			lv_obj_align(p_signal, NULL, LV_ALIGN_CENTER, SIGNAL_X_OFFSET, SIGNAL_Y_OFFSET);
		
		//lv_obj_set_style_local_image_recolor_opa(p_signal, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		//lv_obj_set_style_local_image_recolor(p_signal, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, MLV_WHITE);
	}
	else
	{
		lv_obj_set_hidden(p_signal, RT_TRUE);
	}
	lv_img_cache_invalidate_src(p_signal);
}


/*
 * Karakter -> gorsel eslesmesi. Iki sutun da eski #ifdef bloklarindan
 * BIREBIR alindi; 'x' harfi eskiden de her iki sette yoktu, oyle birakildi.
 */
typedef struct {
	char c;
	const lv_img_dsc_t *ver;
	const lv_img_dsc_t *hor;
} glyph_map_t;

static const glyph_map_t glyph_map[] = {
	{ '0', &fa_0, &fa0_png },
	{ '1', &fa_1, &fa1_png },
	{ '2', &fa_2, &fa2_png },
	{ '3', &fa_3, &fa3_png },
	{ '4', &fa_4, &fa4_png },
	{ '5', &fa_5, &fa5_png },
	{ '6', &fa_6, &fa6_png },
	{ '7', &fa_7, &fa7_png },
	{ '8', &fa_8, &fa8_png },
	{ '9', &fa_9, &fa9_png },
	{ 'a', &fa_a, &faa_png },
	{ 'b', &fa_b, &fab_png },
	{ 'c', &fa_c, &fac_png },
	{ 'd', &fa_d, &fad_png },
	{ 'e', &fa_e, &fae_png },
	{ 'f', &fa_f, &faf_png },
	{ 'g', &fa_g, &fag_png },
	{ 'h', &fa_h, &fah_png },
	{ 'i', &fa_i, &fai_png },
	{ 'j', &fa_j, &faj_png },
	{ 'k', &fa_k, &fak_png },
	{ 'l', &fa_l, &fal_png },
	{ 'm', &fa_m, &fam_png },
	{ 'n', &fa_n, &fan_png },
	{ 'o', &fa_o, &fao_png },
	{ 'p', &fa_p, &fap_png },
	{ 'q', &fa_q, &faq_png },
	{ 'r', &fa_r, &far_png },
	{ 's', &fa_s, &fas_png },
	{ 't', &fa_t, &fat_png },
	{ 'u', &fa_u, &fau_png },
	{ 'v', &fa_v, &fav_png },
	{ 'w', &fa_w, &faw_png },
	{ 'y', &fa_y, &fay_png },
	{ 'z', &fa_z, &faz_png },
	{ '-', &sy_m, &faminus_png },
};

static const lv_img_dsc_t * glyph_img(char c)
{
	for(unsigned i = 0; i < sizeof(glyph_map)/sizeof(glyph_map[0]); i++)
	{
		if(glyph_map[i].c == c)
			return (g_orient == ORIENT_HOR) ? glyph_map[i].hor : glyph_map[i].ver;
	}
	return RT_NULL;
}

void display_floor(int8_t left, int8_t right)
{

//	rt_kprintf("left:%x  right:%x \n", left, right);

	if((!isspace(left))&&isprint(left) && isprint(right))
	{
		lv_obj_set_hidden(p_number_mid, RT_TRUE);
		lv_obj_set_hidden(p_number_left, RT_FALSE);
		lv_obj_set_hidden(p_number_right, RT_FALSE);
	}
	else
	{
		lv_obj_set_hidden(p_number_mid, RT_FALSE);
		lv_obj_set_hidden(p_number_left, RT_TRUE);
		lv_obj_set_hidden(p_number_right, RT_TRUE);
	}

	for(int i=0;i<3;i++)
	{
		lv_obj_t* pobj = RT_NULL;
		char c = 0;
		switch (i)
		{
		case 0:
			pobj = p_number_mid;
			if((!isspace(left))&&isprint(left))
			{
				c = left;
			}
			else
			{
				c = right;
			}
			break;
		
		case 1:
			pobj = p_number_left;
			c = left;
			break;

		case 2:
			pobj = p_number_right;
			c = right;
			break;
		}

		if(pobj)
		{
			c = tolower(c);

			const lv_img_dsc_t *img = glyph_img(c);
			if(img)
				lv_img_set_src(pobj, img);
		}
	}

	lv_obj_align(p_number_mid, NULL,   MID_CHAR_ALIGN, MID_CHAR_X_OFFSET, MID_CHAR_Y_OFFSET);
	lv_obj_align(p_number_left, NULL,  LEFT_CHAR_ALIGN, LEFT_CHAR_X_OFFSET, LEFT_CHAR_Y_OFFSET);
	lv_obj_align(p_number_right, NULL, RIGHT_CHAR_ALIGN, RIGHT_CHAR_X_OFFSET, RIGHT_CHAR_Y_OFFSET);

	lv_img_set_angle(p_number_mid, THIS_ROTATION);
	lv_img_set_angle(p_number_left, THIS_ROTATION);
	lv_img_set_angle(p_number_right, THIS_ROTATION);

	lv_img_cache_invalidate_src(p_number_mid);
	lv_img_cache_invalidate_src(p_number_left);
	lv_img_cache_invalidate_src(p_number_right);

}



/*
 * NXT butonu ile yonelim degistirme.
 *
 * M031 pakete byte 13'te buton MASKESI koyuyor (arcodeLop.reserved2) ve buton
 * basili kaldigi surece maske set kaliyor. Burada KENAR yakaliyoruz: yalnizca
 * "yoktu -> var oldu" gecisinde tetikleniyor. Boylece bir paket kaybolsa bile
 * bir sonraki ayni seviyeyi tasidigi icin cift tetikleme olmuyor.
 */
#define BTN_CODE_NXT		0x01
#define BTN_CODE_RTN		0x02

static void handle_buttons(uint8_t btn)
{
	static uint8_t btn_prev = 0;

	if((btn & BTN_CODE_NXT) && !(btn_prev & BTN_CODE_NXT))
	{
		g_orient = (g_orient == ORIENT_HOR) ? ORIENT_VER : ORIENT_HOR;

		meprom_save_orient(g_orient);

		/* Yerlesim makrolari artik yeni degeri okuyacak; dondurulmus
		   gorseller onbellekte durdugu icin onbellegi bosaltip tam
		   ekran yeniden cizim istiyoruz. Bir sonraki display_floor() /
		   display_state() cagrisi her seyi yeni yonelimle kuruyor. */
		lv_img_cache_invalidate_src(NULL);
		lv_obj_invalidate(lv_scr_act());
	}

	btn_prev = btn;
}

MAIN_PAGE_T * main_page_create(void)
{  
	MAIN_PAGE_T *p_main_page = lv_mem_alloc(sizeof(MAIN_PAGE_T));


	p_main_bg = lv_img_create(lv_scr_act(), NULL);
	lv_obj_align(p_main_bg, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
//	lv_obj_set_style_local_image_recolor_opa(p_main_bg, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, FLOOR_NUM_OPA);		//girilmez işareti renk sorunu
//  lv_obj_set_style_local_image_recolor(p_main_bg, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, FLOOR_NUM_COLOR);		//girilmez işareti renk sorunu
	lv_img_set_src(p_main_bg, &empty_png);
	lv_obj_set_hidden(p_main_bg, RT_TRUE);

	lv_obj_t* pscr = lv_scr_act();

	lv_obj_set_style_local_bg_color(pscr, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, SCR_BG_COLOR);

	p_number_mid = lv_img_create(lv_scr_act(), NULL);
	p_number_left = lv_img_create(lv_scr_act(), NULL);
	p_number_right = lv_img_create(lv_scr_act(), NULL);

	lv_img_set_src(p_number_mid, &empty_png);
	lv_img_set_src(p_number_left, &empty_png);
	lv_img_set_src(p_number_right, &empty_png);

    lv_obj_set_style_local_image_recolor_opa(p_number_mid, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, FLOOR_NUM_OPA);
	lv_obj_set_style_local_image_recolor_opa(p_number_left, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, FLOOR_NUM_OPA);
	lv_obj_set_style_local_image_recolor_opa(p_number_right, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, FLOOR_NUM_OPA);
	
    lv_obj_set_style_local_image_recolor(p_number_mid, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, FLOOR_NUM_COLOR);
	lv_obj_set_style_local_image_recolor(p_number_left, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, FLOOR_NUM_COLOR);
	lv_obj_set_style_local_image_recolor(p_number_right, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, FLOOR_NUM_COLOR);

	lv_obj_align(p_number_mid, NULL,   MID_CHAR_ALIGN, MID_CHAR_X_OFFSET, MID_CHAR_Y_OFFSET);
	lv_obj_align(p_number_left, NULL,  LEFT_CHAR_ALIGN, LEFT_CHAR_X_OFFSET, LEFT_CHAR_Y_OFFSET);
	lv_obj_align(p_number_right, NULL, RIGHT_CHAR_ALIGN, RIGHT_CHAR_X_OFFSET, RIGHT_CHAR_Y_OFFSET);

	p_main_page->p_task = lv_task_create(_main_page_update_task_cb, 10, LV_TASK_PRIO_LOW, (void *)p_main_page);
	
	p_signal = lv_img_create(lv_scr_act(), NULL);

	display_state(LS_BUSY);

	lv_task_ready(p_main_page->p_task);
	
	return p_main_page;
}

void main_page_delete(MAIN_PAGE_T *p_main_page)
{
	lv_obj_del((lv_obj_t *)p_main_page);
}

uint8_t arcode_data[34];
struct  ArcodeM2H arcodeLop;


static void main_page_run(void *p)
{
	meprom_init();
	g_orient = meprom_load_orient(g_orient);

    main_page_create();

	arcode_init();

	rt_uint32_t sec_tick = 130;
	rt_uint32_t tick_loop = 20;

    #ifdef  RT_USING_WDT
		rt_uint16_t wdt_tick=0;
    	rt_device_t wdg_dev=rt_device_find("wdt");
	#endif
    while (1){
        
		rt_thread_mdelay(10);
        lv_task_handler();
		
        #ifdef  RT_USING_WDT
        if(wdt_tick++>300){
            rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
            wdt_tick=0;        }
        #endif

		if(sec_tick-- == 0)
		{
			sec_tick = 70;
		}

		if(tick_loop-- == 0)
		{
		
#ifdef ELEV_DEMO
			tick_loop = 90;

			static uint32_t cstop = 0;
			static uint8_t  elev_dir= 0;

			static const char* LeftNumbers = "          111";
			static const char* RightNumbers= "0123456789012";

			if(cstop >11)  cstop = 11;
			display_floor( LeftNumbers[cstop], RightNumbers[cstop]);

			switch(elev_dir)
			{
				case 0:	
					display_state(LS_STOP);
					elev_dir++ ;
					break;

				case 1:	
					display_state(LS_MOVING_UP);
					cstop++;
					if(cstop >10) 
						elev_dir++;
					break;

				case 2:	
					display_state(LS_STOP);
					elev_dir++;
					break;

				case 3:	
					display_state(LS_OUT_OF_SERVICE);
				case 4:	
					elev_dir++;
					break;

				case 5:	
					display_state(LS_STOP);
					elev_dir++ ;
					break;

				case 6:	
					display_state(LS_OVERLOAD);
				case 7:	
					elev_dir++;
					break;

				case 8:	
					display_state(LS_STOP);
					elev_dir++ ;
					break;

				case 9:	
					display_state(LS_MOVING_DOWN);
					cstop--;
					if(cstop == 0)
						elev_dir++;

					break;
				default: 
					display_state(LS_STOP);
					elev_dir = 0;
					break;
			}
#else
			tick_loop = 35;

			arcode_xfer(arcode_data);
			uint8_t len = arcode_data[0];

			uint8_t crc = arcode_crc(arcode_data);
			
			if(len>0)
			{
	//			display_floor('1','2');
	//			display_state(LS_MOVING_UP);
	//		}
	//		else
	//		{
			if(crc == arcode_data[len-1])
			{
			
				memcpy(&arcodeLop, &arcode_data[2],sizeof(arcodeLop));

				handle_buttons(arcodeLop.reserved2);

#ifdef DISPLAY_MSG

				if(arcodeLop.infoMessageNo != 0xFF && arcodeLop.infoMessageNo != 7)
				{
					lv_obj_set_hidden(p_main_bg, RT_FALSE);

					lv_obj_set_hidden(p_signal, RT_TRUE);
					lv_obj_set_hidden(p_number_mid, RT_TRUE);
					lv_obj_set_hidden(p_number_left, RT_TRUE);
					lv_obj_set_hidden(p_number_right, RT_TRUE);


					if(arcodeLop.infoMessageNo == 0)
					{
						//Out of service
						lv_img_set_src(p_main_bg, (g_orient == ORIENT_HOR) ? &messageOutOfServiceHorEn : &messageOutOfServiceVerEn);
					}
					else if(arcodeLop.infoMessageNo == 1)
					{
						//Overloaded
						lv_img_set_src(p_main_bg, (g_orient == ORIENT_HOR) ? &messageOverloadedHorEn : &messageOverloadedVerEn);
					}
					else if(arcodeLop.infoMessageNo == 2)
					{
						//On Maintenance
						lv_img_set_src(p_main_bg, (g_orient == ORIENT_HOR) ? &messageLiftOnMaintenanceHorEn : &messageLiftOnMaintenanceVerEn);
					}
					else if(arcodeLop.infoMessageNo == 3)
					{
						//Fire evacuation
						lv_img_set_src(p_main_bg, (g_orient == ORIENT_HOR) ? &messageFireEvacuationHorEn : &messageFireEvacuationVerEn);
					}
					else if(arcodeLop.infoMessageNo == 4)
					{
						//backup power evacuation
						lv_img_set_src(p_main_bg, (g_orient == ORIENT_HOR) ? &messageBackupPowerEvacuationHorEn : &messageBackupPowerEvacuationVerEn);
					}
					else if(arcodeLop.infoMessageNo == 5)
					{
						//startup
						lv_img_set_src(p_main_bg, (g_orient == ORIENT_HOR) ? &messageStartupHorEn : &messageStartupVerEn);
					}
					else if(arcodeLop.infoMessageNo == 6)
					{
						//startup
						lv_img_set_src(p_main_bg, (g_orient == ORIENT_HOR) ? &messageDoorOpenErrorHorEn : &messageDoorOpenErrorVerEn);
					}
					else if(arcodeLop.infoMessageNo == 8 || arcodeLop.infoMessageNo == 9)
					{
						//Service control
						lv_img_set_src(p_main_bg, (g_orient == ORIENT_HOR) ? &messageServiceControlHorEn : &messageServiceControlVerEn);
					}

				}
				else 
#endif
				{
					lv_obj_set_hidden(p_main_bg, RT_TRUE);

					if(arcodeLop.fireMode == 2)
					{
						display_floor(arcodeLop.leftCharacter, arcodeLop.rightCharacter);
				
						display_state(LS_FIREMAN_CONTROL);
					} 
					else if(arcodeLop.outOfService)
					{
						display_floor(arcodeLop.leftCharacter, arcodeLop.rightCharacter);
				
						display_state(LS_OUT_OF_SERVICE);
					}
					else if(arcodeLop.overLoad)
					{
						display_floor(arcodeLop.leftCharacter, arcodeLop.rightCharacter);

						display_state(LS_OVERLOAD);
					}
					else if(arcodeLop.travelUpDirection)
					{
						display_floor(arcodeLop.leftCharacter, arcodeLop.rightCharacter);

						display_state(LS_MOVING_UP);
					}
					else if(arcodeLop.travelDownDirection)
					{
						display_floor(arcodeLop.leftCharacter, arcodeLop.rightCharacter);

						display_state(LS_MOVING_DOWN);
					}
					else if(arcodeLop.collectionUpDirection || arcodeLop.collectionDownDirection)
					{
						if((arcodeLop.collectionUpDirection == arcodeLop.collectionDownDirection) ||
							(arcodeLop.configFloorNo != arcodeLop.currentFloorNo))
						{
							display_floor(arcodeLop.leftCharacter, arcodeLop.rightCharacter);

							display_state(LS_STOP);
						}
						else
						{
							lv_obj_set_hidden(p_number_mid, RT_TRUE);
							lv_obj_set_hidden(p_number_left, RT_TRUE);
							lv_obj_set_hidden(p_number_right, RT_TRUE);
							
							if(arcodeLop.collectionUpDirection) 
								display_state(LS_COLLECT_UP);
								
							if(arcodeLop.collectionDownDirection)
								display_state(LS_COLLECT_DOWN);
						}
					}
					else
					{
						display_floor(arcodeLop.leftCharacter, arcodeLop.rightCharacter);

						display_state(LS_STOP);
					}
				}

				//rt_kprintf("C:%2X F:%2X M:%2X S:%2X \n", arcodeLop.currentFloorNo, arcodeLop.fireMode, arcodeLop.infoMessageNo, arcodeLop.outOfService);

			}
			else
			{
				/*
				for (size_t i = 0; i < 15; i++)
				{
					rt_kprintf("%2X ", arcode_data[i]);
				}
				rt_kprintf("crc! \n");
				*/
			}
		}
#endif
		}

    }
}

#include "lv_lib_png/lv_png.h"

int main_page_init(void)
{
    rt_err_t ret       = RT_EOK;
    rt_thread_t thread = RT_NULL;

    /* init littlevGL */
    ret = lv_rtt_port_init("lcd");
    if (ret != RT_EOK){
        return ret;
    }

	//
  	lv_png_init();
  	//

    /* littleGL demo gui thread */
    thread = rt_thread_create("lv_demo", main_page_run, RT_NULL, 4096, 5, 10);
    if (thread == RT_NULL){
        return RT_ERROR;
    }
    rt_thread_startup(thread);

	/* add spi lcd initialize */
	

    return RT_EOK;
}



INIT_APP_EXPORT(main_page_init);
