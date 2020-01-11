#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "x_frame.h"
#include "bsp_lcd_driver.h"

//EXT_REC(LCD_DAT);



static void ht1621b_Write_Command(uint8_t command);
void ht1621b_Write_Data(uint8_t addr, uint8_t *p_data, uint8_t cnt, uint8_t write_mode);
static void ht1621b_Write_Bit(uint8_t w_data, uint8_t b_cnt, uint8_t high_low_bit);
//static lcd_buf_u lcd_buf = {0};
lcd_buf_u lcd_buf = {0};
void lcd_gpio_init(void)
{
	/* enable the led clock */
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	/* configure led GPIO port */ 
	gpio_init(LCD_PIN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ ,LCD_CS_PIN | LCD_WR_PIN | LCD_RD_PIN);
	
	gpio_init(LCD_DATA_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ ,LCD_DATA_PIN);
	
	
	//LCD_EN_L;//关闭屏幕背光
}


//系统掉电以后是否需要重新初始化
void bsp_lcd_init(void)
{
    //uint32_t pin_state = 1;
    lcd_gpio_init();
    ht1621b_Write_Command(CMD_BIAS3_COM4); //set lcd 1/3 bias, 1/4 duty
    //ht1621b_Write_Command(0x24);
    ht1621b_Write_Command(CMD_SYS_EN);
    ht1621b_Write_Command(CMD_LCD_ON);
    
    memset(&lcd_buf, 0x0, sizeof(lcd_buf_u));
    ht1621b_Write_Data(0, lcd_buf.lcd_raw, sizeof(lcd_buf), 0);
    //hal_app_write(DRV_GPIO,(uint8_t*)&pin_state,sizeof(uint32_t),LCD_ON_PIN,NULL);//开LCD背光
}

/*
  * @brief  ht1621b write continue mode
  * @param
  * @retval
 */
void ht1621b_Write_Data(uint8_t addr, uint8_t *p_data, uint8_t cnt, uint8_t write_mode)
{
    uint8_t i = 0;
    LCD_CS_L;
    ht1621b_Write_Bit(CMD_WRITE , 3 , LOW_BIT);//write "101" bit
    ht1621b_Write_Bit( (addr << 2) , 6 , LOW_BIT); //write 6 bits of addr
    switch(write_mode)
    {

    case 0:	for (i = 0; i < cnt; i++){ht1621b_Write_Bit(*p_data++ , 8 , HIGH_BIT);} break;
    case 1:	ht1621b_Write_Bit(*p_data++, 4 , HIGH_BIT);								break;
    default:																	    break;
    }
    LCD_CS_H;
}
////-----------------以上为驱动部分

static lcd_ctrl_t lcd_ctrl = {0};

////新版显示屏 FGE' ',ABCD(high bit-->low bit)
						 //    0    1     2     3     4     5     6     7     8     9
const uint8_t num_tab[10] = {0xaf, 0x06, 0x6d, 0x4f, 0xc6, 0xcb, 0xeb, 0x0e, 0xef, 0xcf};

void soc_num_to_seg(uint8_t select,uint8_t num)	//ok
{
	if(num > 9)	{return;}
	if(select == SOC_TEN)
	{
		lcd_buf.lcd_raw[select]		 = lcd_buf.lcd_raw[select] 		| ( (num_tab[num] & 0x0f)<<4 );
		lcd_buf.lcd_raw[select + 1]	 = lcd_buf.lcd_raw[select + 1] 	| ( (num_tab[num] & 0xe0)>>4 );
	}
	else if(select == SOC_UNIT)
	{
		lcd_buf.lcd_raw[select]		 = lcd_buf.lcd_raw[select] 		| ( (num_tab[num] & 0x0f)<<4 );
		lcd_buf.lcd_raw[select + 1]	 = lcd_buf.lcd_raw[select + 1] 	| ( (num_tab[num] & 0xe0)>>4 );
	}
}
void show_time(uint32_t time)//单位：min		//ok
{
	uint32_t hour	= 0;
	uint32_t min	= 0;
	if(time > TIME_MAX_MINS)	{time = TIME_MAX_MINS;}
	hour 	= time / 60;
	min		= time % 60;
	if(time >= 60)	//显示小时
	{
		SEG_VAL(lcd_buf.bits.bat_time_1,num_tab[hour/10]);
		SEG_VAL(lcd_buf.bits.bat_time_2,num_tab[hour%10]);
		lcd_buf.bits.bat_hours_flag_P3 	= SHOW_ON;	//Hours
	}
	else if(time < 60)
	{
		SEG_VAL(lcd_buf.bits.bat_time_1,num_tab[min/10]);
		SEG_VAL(lcd_buf.bits.bat_time_2,num_tab[min%10]);
		lcd_buf.bits.bat_mins_flag_P2  	= SHOW_ON;	//Mins
	}
}

void show_off(uint32_t lcd_state)	//ok
{
	if(lcd_state==EF_LCD_STATE_OFF){ memset(&lcd_buf, 0, sizeof(lcd_buf_u));}
	
    if(lcd_state==EF_LCD_STATE_SHOW_OFF)
    {
        //memset(&lcd_buf, 0, sizeof(lcd_buf_u));
        SEG_VAL(lcd_buf.bits.bat_output_watts_10, LETTER_O);
        SEG_VAL(lcd_buf.bits.bat_output_watts_11, LETTER_F);
        SEG_VAL(lcd_buf.bits.bat_output_watts_12, LETTER_F);
		
		SEG_VAL(lcd_buf.bits.bat_output_watts_9, 0);
    }
}



void show_soc( uint32_t soc )	//ok
{
	uint8_t soc_ten 	= 0;
	uint8_t soc_uint	= 0;
	if(soc > 100)	{soc = 100;}
	if(soc == 100)
	{
		lcd_buf.bits.bat_soc_P21	= 1;	//1	--->百位
		soc_num_to_seg(SOC_TEN,0);		//3号-->十位
		soc_num_to_seg(SOC_UNIT,0);		//4号-->个位
	}
	else if(soc < 100)
	{
		soc_ten  = soc / 10;
		soc_uint = soc % 10;
		if(soc_ten  > 0)	{soc_num_to_seg(SOC_TEN, soc_ten);}	//3号
		
		soc_num_to_seg(SOC_UNIT,soc_uint);	//4号
	}
	lcd_buf.bits.bat_soc_per_flag_p23		= SHOW_ON;	//%
}


void show_input_watts(uint8_t enable,uint32_t input_watts)//ok
{	
	uint8_t watts_qw = 0;	//千位
	uint8_t watts_bw = 0;	//百位
	uint8_t watts_sw = 0;	//十位
	uint8_t watts_gw = 0;	//个位
	
	if(enable == 0)	{return;}
	
	lcd_buf.bits.bat_input_flag_P17 			= SHOW_ON;			//input
	lcd_buf.bits.bat_in_watts_flag_P16 			= SHOW_ON;			//input watts

	if(input_watts > 9999)	{input_watts = 9999;}
	//if(input_watts < WATTS_MIN_ACTION)	{return;}
	
	//input_watts = input_watts % 1000;	//保证只有三位数
	watts_qw	= input_watts / 1000;		//千位
	watts_bw	= input_watts % 1000 / 100;	//百位
	watts_sw	= input_watts % 100/10;		//十位
	watts_gw	= input_watts % 10;			//个位
	
	//if(watts_h > 0)	
	{
		SEG_VAL(lcd_buf.bits.bat_input_watts_5, num_tab[watts_qw]);			
		SEG_VAL(lcd_buf.bits.bat_input_watts_6, num_tab[watts_bw]);
	}										//5,6号
		SEG_VAL(lcd_buf.bits.bat_input_watts_7, num_tab[watts_sw]);
	//if(watts_m > 0)
		{SEG_VAL(lcd_buf.bits.bat_input_watts_8, num_tab[watts_gw]);}	//6号
																		//7号
}
void show_output_watts(uint8_t enable, uint32_t output_watts)//ok
{
	uint8_t watts_qw = 0;	//千位
	uint8_t watts_bw = 0;	//百位
	uint8_t watts_sw = 0;	//十位
	uint8_t watts_gw = 0;	//个位
	
	//-------
	lcd_buf.bits.bat_output_flag_P15 		= SHOW_ON;		   		//output sign 
	lcd_buf.bits.bat_out_watts_flag_P14 	= SHOW_ON;				//watts  sign 
	
	if(enable == 0)	{return;}
	
	//if(output_watts > OUTPUT_MAX_WATTS)	{if(lcd_ctrl.overload_mode==0){lcd_buf.bits.bat_overload_P7 = SHOW_ON;}} //overload

	//if(output_watts	< WATTS_MIN_ACTION)		{output_watts = 0;}

	if(output_watts > 9999)	{output_watts = 9999;}
	//output_watts 	= output_watts % 1000;		//保证只有三位数
	watts_qw	= output_watts / 1000;		//千位
	watts_bw	= output_watts % 1000 / 100;	//百位
	watts_sw	= output_watts % 100/10;		//十位
	watts_gw	= output_watts % 10;			//个位

	//if(watts_h > 0)		
		{SEG_VAL(lcd_buf.bits.bat_output_watts_9,  num_tab[watts_qw]);
		 SEG_VAL(lcd_buf.bits.bat_output_watts_10,  num_tab[watts_bw]);}	//8,9号
																							
	//if(watts_m > 0)		{SEG_VAL(lcd_buf.bits.bat_output_watts_9,  num_tab[watts_m]);}	//9号

	SEG_VAL(lcd_buf.bits.bat_output_watts_11, num_tab[watts_sw]);						//10号
	SEG_VAL(lcd_buf.bits.bat_output_watts_12, num_tab[watts_gw]);						//10号
	
	
}

//--------temp
void temp_toggle_ctrl(void)
{
	if(lcd_ctrl.temp_mode == 1)
	{
		static uint8_t  hi_temp_state = 0;
		hi_temp_state^=0x01;
		lcd_ctrl.temp_state = hi_temp_state;
	}
	else if(lcd_ctrl.temp_mode == 2)
	{
		static uint8_t  low_temp_state = 0;
		low_temp_state^=0x02;
		lcd_ctrl.temp_state = low_temp_state;
	}
}
void show_temp_sign(uint8_t enable, uint8_t temp_state)		//ok
{
	if(enable == 0)	{return;}
		 if(temp_state == 1)	{lcd_buf.bits.bat_hi_temp_P19		= SHOW_ON;}
	else if(temp_state == 2) 	{lcd_buf.bits.bat_low_temp_P18 		= SHOW_ON;}
}

void temp_all_ctrl(uint8_t mode)
{
	switch(mode)
	{
		case 0:
		{
			lcd_ctrl.temp_mode = 0;
		}break;
		case 1:
		{
			lcd_ctrl.temp_mode = 1;
			RUN_BY_LIMIT_FUNC(500,temp_toggle_ctrl);
		}break;
		case 2:
		{
			lcd_ctrl.temp_mode = 2;
			RUN_BY_LIMIT_FUNC(500,temp_toggle_ctrl);
		}break;
		
		default:
			break;
	}
}
//--------usb
void show_usb_sign(uint8_t enable,  uint8_t state)
{
	if(enable == 0)	{return;}
	if(state == 1)		{lcd_buf.bits.bat_usb_flag_P9	= SHOW_ON;}
}
//--------typec
void show_typec_sign(uint8_t enable,  uint8_t state)
{
	if(enable == 0)	{return;}
	if(state == 1)		{lcd_buf.bits.bat_typec_flag_P10 = SHOW_ON;}
}
//----------ac
void show_ac_fre_sign(uint8_t enable, uint8_t ac_fre)
{
	if(enable == 0)	{return;}
	if(ac_fre == 50)		
	{
		lcd_buf.bits.bat_50hz_flag_P8	= SHOW_ON;
		lcd_buf.bits.bat_ac_flag_P6		= SHOW_ON;
	}
	else if(ac_fre == 60)
	{
		lcd_buf.bits.bat_50hz_flag_P8	= SHOW_ON;
		lcd_buf.bits.bat_60hz_flag_P7	= SHOW_ON;
		lcd_buf.bits.bat_ac_flag_P6		= SHOW_ON;
	}
}
//---------car
void show_car_sign(uint8_t enable, uint8_t state)
{
	if(enable == 0)	{return;}
	if(state == 1)		{lcd_buf.bits.bat_car_flag_P11 = SHOW_ON;}
}

void show_overload_sign(uint8_t enable, uint8_t state)
{
	if(enable == 0)	{return;}
	if(state == 1)		{lcd_buf.bits.bat_overload_flag_P20 = SHOW_ON;}
}

void show_wifi_sign(uint8_t enable, uint8_t state)
{
	if(enable == 0)	{return;}
	if(state == 1)		{lcd_buf.bits.bat_wifi_flag_P12 = SHOW_ON;}
}
void show_ups_sign(uint8_t enable, uint8_t state)
{
	if(enable == 0)	{return;}
	if(state == 1)		{lcd_buf.bits.bat_ups_flag_P13 = SHOW_ON;}
}
void show_out_bms_sign(uint8_t enable, uint8_t state)
{
	if(enable == 0)	{return;}
	if(state == 1)		{lcd_buf.bits.bat_out_bms_flag_P5 = SHOW_ON;}
}
void show_parallel_machine_sign(uint8_t enable, uint8_t state)
{
	if(enable == 0)	{return;}
	if(state == 1)		{lcd_buf.bits.bat_paralleling_flag_P4 = SHOW_ON;}
}

//-------------fan
void fan_running_show(uint8_t state)
{
	if(state == 0 )
	{
		lcd_buf.bits.fan_45_degrees_P16_2 	= SHOW_ON;
		lcd_buf.bits.fan_90_degrees_P16_1  	= SHOW_ON;
		lcd_buf.bits.fan_central_P16_4 	  	= SHOW_ON;
	}
	else if(state == 1 )
	{
		lcd_buf.bits.fan_0_degrees_P16_3   	= SHOW_ON;
		lcd_buf.bits.fan_45_degrees_P16_2  	= SHOW_ON;
		lcd_buf.bits.fan_central_P16_4 	  	= SHOW_ON;
	}
	else if(state == 2 )
	{
		lcd_buf.bits.fan_90_degrees_P16_1  	= SHOW_ON;
		lcd_buf.bits.fan_0_degrees_P16_3   	= SHOW_ON;
		lcd_buf.bits.fan_central_P16_4 	  	= SHOW_ON;
	}
}

void fan_running_ctrl(uint8_t speed)	//1-100
{
	if(speed == 0)	{lcd_ctrl.fan_state = 0xff;return;}
	
	if(speed > 100)	{speed = 100;}
	lcd_ctrl.fan_speed = 101 - speed;
	
	lcd_ctrl.fan_state++;
	lcd_ctrl.fan_state = lcd_ctrl.fan_state%3;	//0-2
}
void fan_running_ctrl_task(void)
{
	fan_running_ctrl(ef_state.lcd_app_dat.state_fan_gear);
}


//------------circle
//------mode 1
void circle_toggle_ctrl(void)
{
	static uint8_t  cir_state = 0;
	
	cir_state^=0x01;
	
	lcd_ctrl.cirlce_seg = cir_state;
}
void circle_toggle_show(uint8_t state)
{
	if(state == 1)
	{
		SHOW_CIRCLE_SEG(1);
		SHOW_CIRCLE_SEG(2);
		SHOW_CIRCLE_SEG(3);
		SHOW_CIRCLE_SEG(4);
		SHOW_CIRCLE_SEG(5);
		SHOW_CIRCLE_SEG(6);
		SHOW_CIRCLE_SEG(7);
		SHOW_CIRCLE_SEG(8);
	}
}

//-------mode 2
void circle_running_ctrl(void)
{
	static uint8_t seg_cnt		= 0;

	if(lcd_ctrl.circle_dir == 0)		//顺时针
	{
		seg_cnt++;		//0-7
		if(seg_cnt > 7) {seg_cnt = 0;}
	}
	else if(lcd_ctrl.circle_dir == 1)	//逆时针
	{
		seg_cnt--;		//0-7
		if(seg_cnt > 7) {seg_cnt = 7;}	//seg_cnt == 255
	}
	lcd_ctrl.cirlce_seg = seg_cnt;
}


void circle_running_show(uint8_t seg)
{
	if(seg == 0)			//close s1
	{
		SHOW_CIRCLE_SEG(2);SHOW_CIRCLE_SEG(3);SHOW_CIRCLE_SEG(4);SHOW_CIRCLE_SEG(5);
		SHOW_CIRCLE_SEG(6);SHOW_CIRCLE_SEG(7);SHOW_CIRCLE_SEG(8);
	}
	else if(seg == 1)		//close s2
	{
		SHOW_CIRCLE_SEG(1);SHOW_CIRCLE_SEG(3);SHOW_CIRCLE_SEG(4);SHOW_CIRCLE_SEG(5);
		SHOW_CIRCLE_SEG(6);SHOW_CIRCLE_SEG(7);SHOW_CIRCLE_SEG(8);
	}
	else if(seg == 2)		//close s3
	{
		SHOW_CIRCLE_SEG(1);SHOW_CIRCLE_SEG(2);SHOW_CIRCLE_SEG(4);SHOW_CIRCLE_SEG(5);
		SHOW_CIRCLE_SEG(6);SHOW_CIRCLE_SEG(7);SHOW_CIRCLE_SEG(8);
	}
	else if(seg == 3)		//close s4
	{
		SHOW_CIRCLE_SEG(1);SHOW_CIRCLE_SEG(2);SHOW_CIRCLE_SEG(3);SHOW_CIRCLE_SEG(5);
		SHOW_CIRCLE_SEG(6);SHOW_CIRCLE_SEG(7);SHOW_CIRCLE_SEG(8);
	}
	else if(seg == 4)		//close s5
	{
		SHOW_CIRCLE_SEG(1);SHOW_CIRCLE_SEG(2);SHOW_CIRCLE_SEG(3);SHOW_CIRCLE_SEG(4);
		SHOW_CIRCLE_SEG(6);SHOW_CIRCLE_SEG(7);SHOW_CIRCLE_SEG(8);
	}
	else if(seg == 5)		//close s6
	{
		SHOW_CIRCLE_SEG(1);SHOW_CIRCLE_SEG(2);SHOW_CIRCLE_SEG(3);SHOW_CIRCLE_SEG(4);
		SHOW_CIRCLE_SEG(5);SHOW_CIRCLE_SEG(7);SHOW_CIRCLE_SEG(8);
	}
	else if(seg == 6)		//close s7
	{
		SHOW_CIRCLE_SEG(1);SHOW_CIRCLE_SEG(2);SHOW_CIRCLE_SEG(3);SHOW_CIRCLE_SEG(4);
		SHOW_CIRCLE_SEG(5);SHOW_CIRCLE_SEG(6);SHOW_CIRCLE_SEG(8);
	}
	else if(seg == 7)		//close s8
	{
		SHOW_CIRCLE_SEG(1);SHOW_CIRCLE_SEG(2);SHOW_CIRCLE_SEG(3);SHOW_CIRCLE_SEG(4);
		SHOW_CIRCLE_SEG(5);SHOW_CIRCLE_SEG(6);SHOW_CIRCLE_SEG(7);
	}
}

//mode 3
void circle_show_soc(uint8_t soc)//ok
{
	uint8_t cur_bat_cnt = 0;
	
	if(soc == 0)		{cur_bat_cnt = 0;}
	else if(soc > 0)	{cur_bat_cnt = soc / 12.5 + 1;}	//1-12.5 定义为1格电
	
	if(soc <= MIN_SOC)			{cur_bat_cnt = 0;} 		//if soc<5,not show circle
	
	if(cur_bat_cnt > 0 ){SHOW_CIRCLE_SEG(2);}
	if(cur_bat_cnt > 1) {SHOW_CIRCLE_SEG(3);}
	if(cur_bat_cnt > 2) {SHOW_CIRCLE_SEG(4);}
	if(cur_bat_cnt > 3) {SHOW_CIRCLE_SEG(5);}
	if(cur_bat_cnt > 4) {SHOW_CIRCLE_SEG(6);}
	if(cur_bat_cnt > 5) {SHOW_CIRCLE_SEG(7);}
	if(cur_bat_cnt > 6) {SHOW_CIRCLE_SEG(8);}
	if(cur_bat_cnt > 7) {SHOW_CIRCLE_SEG(1);}
}

void circle_all_ctrl(uint8_t dir, uint8_t speed, uint8_t mode, uint8_t soc)
{
	switch(mode)
	{
		case 1: 
		{
			lcd_ctrl.circle_mode = 1;
			RUN_BY_LIMIT_FUNC(500,circle_toggle_ctrl);
		}break;
		
		case 2: 
		{
			lcd_ctrl.circle_mode	= 2;
			
			lcd_ctrl.circle_dir		= dir;
			
			if(speed == 0)	{return;}			//if speed = 0,stop
			if(speed > 200)	{speed = 200;}
			speed = 201 - speed;
			
			RUN_BY_LIMIT_FUNC( (10+speed), circle_running_ctrl);
			
		}break;
		case 3: 
		{
			lcd_ctrl.circle_mode = 3;
			lcd_ctrl.cirlce_soc = soc;
		}break;
		default:
			break;
	}
}
void circle_all_show(uint8_t dir, uint8_t seg, uint8_t mode, uint8_t soc)
{
	if(mode == 1)					//闪烁
	{
		circle_toggle_show(seg);
	}
	else if(mode == 2)				//转动
	{
		circle_running_show(seg);
	}
	else if(mode == 3)				//soc
	{
		circle_show_soc(soc);
	}
}

void input_watts_ctrl(uint8_t mode)
{
	if(mode == 1)
	{
		static uint8_t  input_state = 0;
		input_state^=0x01;
		lcd_ctrl.input_watts_state = input_state;
	}
}
void output_watts_ctrl(uint8_t mode)
{
	if(mode == 1)
	{
		static uint8_t  output_state = 0;
		output_state^=0x01;
		lcd_ctrl.output_watts_state = output_state;
	}
}

void all_in_output_watts_ctrl(void)
{
	input_watts_ctrl(ef_state.lcd_app_dat.input_watts_mode);
	
	output_watts_ctrl(ef_state.lcd_app_dat.output_watts_mode);
}

void lcd_all_ctrl(void)
{
	//fan ctrl
	RUN_BY_LIMIT_FUNC((10+lcd_ctrl.fan_speed),fan_running_ctrl_task);
	
	//circle ctrl
	circle_all_ctrl(ef_state.lcd_app_dat.state_circle_dir, 		\
					ef_state.lcd_app_dat.state_circle_speed, 	\
					ef_state.lcd_app_dat.state_circle_mode,		\
					ef_state.lcd_app_dat.soc);
	
	//----not handle
//	car_all_ctrl(ef_state.lcd_app_dat.car_mode);
//	overload_all_ctrl(ef_state.lcd_app_dat.overload_mode);
//	ac_all_ctrl(ef_state.lcd_app_dat.ac_mode);
//	typec_all_ctrl(ef_state.lcd_app_dat.typec_mode);
//	hi_temp_all_ctrl(ef_state.lcd_app_dat.hi_temp_mode);
//	RUN_BY_LIMIT_FUNC(500,all_in_output_watts_ctrl);
}
void lcd_all_show(void)
{
	//data
	show_soc(ef_state.lcd_app_dat.soc);
	show_time(ef_state.lcd_app_dat.remain_time);
	
	//state
	fan_running_show(lcd_ctrl.fan_state);
	circle_all_show(lcd_ctrl.circle_dir, lcd_ctrl.cirlce_seg, lcd_ctrl.circle_mode, lcd_ctrl.cirlce_soc);
	
	//flag
	show_usb_sign(1,ef_state.lcd_app_dat.flag_usb);
	
	show_temp_sign(1,ef_state.lcd_app_dat.state_temp);
	
	show_typec_sign(1, ef_state.lcd_app_dat.flag_typec);
	
	show_ac_fre_sign(1,ef_state.lcd_app_dat.flag_ac_fre);

	show_car_sign(1, ef_state.lcd_app_dat.flag_car);

	show_overload_sign(1, ef_state.lcd_app_dat.flag_overload);

	show_input_watts(1,ef_state.lcd_app_dat.input_watts);

	show_output_watts(1,ef_state.lcd_app_dat.output_watts);
	
	show_off(ef_state.lcd_app_dat.state_lcd_show_off);
}

//void lcd_test_show(void)
//{
//	//show_off(EF_LCD_STATE_SHOW_OFF);
//	show_soc(100);
//	circle_show_soc(100);	//err
//	
//	show_time(5999);
//	
//	show_input_watts(1,	1234);
//	show_output_watts(1,5678);
//	
//	//---------
//	lcd_ctrl.temp_mode  = 2;		//1:hi_temp		2:low_temp
//	RUN_BY_LIMIT_FUNC(500,temp_toggle_ctrl);
//	show_temp_sign(1,lcd_ctrl.temp_state);

//	show_usb_sign(1,1);
//	show_typec_sign(1,1);
//	show_ac_fre_sign(1,50);
//	show_car_sign(1,1);
//	show_overload_sign(1,1);

//	//---------
//	//fan_running_ctrl(50);
//	fan_running_show(lcd_ctrl.fan_state);
//	
//	show_wifi_sign(1,1);
//	show_ups_sign(1,1);
//	show_out_bms_sign(1,1);
//	show_parallel_machine_sign(1,1);
//}


uint8_t lcd_val_byte[17] = {0};
static void lcd_driver_task(void)
{
	static uint8_t flag_re_init = 0;
	if(flag_re_init==0)
	{
		flag_re_init  =1;
		bsp_lcd_init();
	}
	
	memset(lcd_buf.lcd_raw,0,sizeof(lcd_buf));
	
	
	lcd_all_ctrl();
	
	lcd_all_show();
	
	//lcd_test_show();

	
	//memset(lcd_buf.lcd_raw,0xff,sizeof(lcd_buf));//全显
	
	ht1621b_Write_Data(0, lcd_buf.lcd_raw, sizeof(lcd_buf), 0);
}

REG_TASK(2, 10, lcd_driver_task);

REG_CFG_ITEM(lcd_buf_0,	lcd_val_byte[0] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_1,	lcd_val_byte[1] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_2,	lcd_val_byte[2] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_3,	lcd_val_byte[3] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_4,	lcd_val_byte[4] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_5,	lcd_val_byte[5] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_6,	lcd_val_byte[6] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_7,	lcd_val_byte[7] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_8,	lcd_val_byte[8] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_9,	lcd_val_byte[9] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_10,lcd_val_byte[10] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_11,lcd_val_byte[11] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_12,lcd_val_byte[12] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_13,lcd_val_byte[13] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_14,lcd_val_byte[14] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_15,lcd_val_byte[15] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );
REG_CFG_ITEM(lcd_buf_16,lcd_val_byte[16] ,eu_uint8      ,PARM_RW        ,0         ,0.0f       ,255.0f  );

//-------------以下为LCD驱动代码
static void ht1621b_Write_Command(uint8_t command)	//如果命令ID:100定义为 high 3 bit，则需要LOW_BIT配合以便左移
{
    LCD_CS_L;
    ht1621b_Write_Bit(CMD_COMMAND , 4 , LOW_BIT); //write "100" bit		
    ht1621b_Write_Bit(command , 8 , LOW_BIT); //write command and last bit "X"
    LCD_CS_H;
}
static void ht1621b_Write_Bit(uint8_t w_data, uint8_t b_cnt, uint8_t low_high_bit)//low_high_bit = LOW_BIT左移
{
    uint8_t i = 0;
    uint8_t compare_bit = 0;

    switch(low_high_bit)
    {
    case LOW_BIT:
        compare_bit = 0x80;
        break;
    case HIGH_BIT:
        compare_bit = 0x01;
        break;
    }
    for(i = 0 ; i < b_cnt ; i++)
    {
        if((w_data & compare_bit) == 0)	{LCD_DA_L;}
        else							{LCD_DA_H;}
        LCD_WR_L;
        //delay_us(20);
        //__NOP();
        LCD_WR_H;
        w_data = (low_high_bit > LOW_BIT) ? (w_data >> 1) : (w_data << 1);
        //__NOP();
        //delay_us(20);
    }
}


int32_t lcd_w(uint8_t argc, uint8_t *argv[])
{
    if(argc >= 2)
    {
        uint32_t reg = 0;
        //uint32_t read_len = 0;
        uint32_t addr = 0;
        //uint8_t i = 0;

        uint8_t dat[4] = {0};
        sscanf((const char *)argv[1], "%d", &addr);
        sscanf((const char *)argv[2], "%x", &reg);
        dat[0] = (uint8_t)(reg);
        ht1621b_Write_Data(addr, dat, 1, 0);

        DBG_I("lcd_w  : flg:0x%x  ", addr);
    }
    else
    {
        DBG_W("warn the lcd_w param is error: eg:lcd_w addr reg len");
    }
    return RET_OK;
}

REG_SHELL_CMD(lcd_w, 0, lcd_w, "lcd_w", "eg:lcd_w addr reg len\r\n ");

int32_t lcd_clr(uint8_t argc, uint8_t *argv[])
{
    uint32_t on = 0;
    sscanf((const char *)argv[1], "%d", &on);
    if(on)
    {
        memset(&lcd_buf, 0, sizeof(lcd_buf_u));
    }
    else
    {
        memset(&lcd_buf, 0xff, sizeof(lcd_buf_u));
    }
    ht1621b_Write_Data(0, lcd_buf.lcd_raw, 16, 0);
	DBG_I("lcd_clr : %d",on);
    return RET_OK;
}
REG_SHELL_CMD(lcd_clr, 0, lcd_clr, "lcd_clr", "eg:lcd_clr  on/off\r\n ");


//int32_t set_speed(uint8_t argc, uint8_t *argv[])
//{
//    uint32_t speed = 0;
//    sscanf((const char *)argv[1], "%d", &speed);
//   	speed_charge = (uint8_t)speed;
//    return RET_OK;
//}
//REG_SHELL_CMD(set_speed, 0, set_speed, "set_speed", "eg:set_speed  1\r\n ");


