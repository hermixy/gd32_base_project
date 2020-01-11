#ifndef __BSP_LCD_DRIVER_H__
#define __BSP_LCD_DRIVER_H__

#include "gd32e10x.h"
#include "x_frame.h"


#define FLASH_FLG_TIME 6   //control eg.eff_flag  flash speed


#define WRITE_MODE_BYTE 0
#define WRITE_MODE_HALF 1

#define LOW_BIT  0
#define HIGH_BIT 1

//V1.1
#define LCD_CS_PIN      GPIO_PIN_5
#define LCD_WR_PIN      GPIO_PIN_3
#define LCD_RD_PIN      GPIO_PIN_4
#define LCD_DATA_PIN    GPIO_PIN_15
//#define LCD_EN			GPIO_Pin_1

#define LCD_PIN_PORT    GPIOB
#define LCD_DATA_PORT   GPIOA

#define LCD_CS_H    gpio_bit_set(LCD_PIN_PORT, LCD_CS_PIN);
#define LCD_CS_L  gpio_bit_reset(LCD_PIN_PORT, LCD_CS_PIN);

#define LCD_RD_H    gpio_bit_set(LCD_PIN_PORT, LCD_RD_PIN);
#define LCD_RD_L  gpio_bit_reset(LCD_PIN_PORT, LCD_RD_PIN);

#define LCD_WR_H    gpio_bit_set(LCD_PIN_PORT, LCD_WR_PIN);
#define LCD_WR_L  gpio_bit_reset(LCD_PIN_PORT, LCD_WR_PIN);

#define LCD_DA_H    gpio_bit_set(LCD_DATA_PORT, LCD_DATA_PIN);
#define LCD_DA_L  gpio_bit_reset(LCD_DATA_PORT, LCD_DATA_PIN);

//#define LCD_EN_H 	GPIO_SetBits(LCD_PIN_PORT, LCD_EN);
//#define LCD_EN_L  GPIO_ResetBits(LCD_PIN_PORT, LCD_EN);

#define LCD_SEG_BYTE_NUM 14

#define CMD_READ  0xc0	//1100 0000
#define CMD_WRITE 0xa0
#define CMD_COMMAND 0x80
#define CMD_READ_MODIFY_WRITE 0xa0

#define CMD_SYS_DISEN   0x00
#define CMD_SYS_EN      0x03
#define CMD_LCD_OFF     0x04
#define CMD_LCD_ON      0x06
//1/4鍗犵┖姣?1/3鍋忓帇
#define CMD_BIAS3_COM4  0x52

#define WRITE_BYTES      1
#define WRITE_HALF_BYTES 2
#define WRITE_HIGH_BITS  3
#define WRITE_LOW_BITS   4

//show switch
#define SHOW_OFF			0
#define SHOW_ON				1


#define LCD_CIRCLE_FLICKER_TIME		20		

//lcd
enum
{
	INPUT_WATTS_L	=	0	,
	INPUT_WATTS_M			,
	INPUT_WATTS_H			,
	
	OUTPUT_WATTS_L	=	3	,
	OUTPUT_WATTS_M			,
	OUTPUT_WATTS_H			,
	
	SOC_UNIT		=	9	,
	SOC_TEN					,
	
	TIME_H			=	9	,
	TIME_L					,
};


//新版LCD
enum
{
    u_AFED_D = (0x1 << 0),
    u_AFED_C = (0x1 << 1),
    u_AFED_B = (0x1 << 2),
    u_AFED_A = (0x1 << 3),
    u_AFED_E = (0x1 << 5),
    u_AFED_G = (0x1 << 6),
    u_AFED_F = (0x1 << 7),
};

#define LETTER_O   0xaf
#define LETTER_F   0xe8

#define SEG_VAL(var,num)   do{\
        var##A = ((num & u_AFED_A)>0)?1:0;\
        var##B = ((num & u_AFED_B)>0)?1:0;\
        var##C = ((num & u_AFED_C)>0)?1:0;\
        var##D = ((num & u_AFED_D)>0)?1:0;\
        var##E = ((num & u_AFED_E)>0)?1:0;\
        var##F = ((num & u_AFED_F)>0)?1:0;\
        var##G = ((num & u_AFED_G)>0)?1:0;\
        }while(0)

#define SHOW_CIRCLE_SEG(num)	lcd_buf.bits.bat_circle_S##num = SHOW_ON;
typedef struct
{
	uint8_t		fan_state;
	uint8_t 	fan_speed;
	
	uint8_t 	circle_dir;
	uint8_t		circle_speed;
	uint8_t		circle_mode;
	uint8_t		cirlce_seg;
	uint8_t		cirlce_soc;
	
	uint8_t		car_mode;
	uint8_t		car_state;
	
	uint8_t		overload_mode;
	uint8_t		overload_state;
	
	uint8_t		ac_mode;
	uint8_t		ac_state;
	uint8_t 	ac_fre_state;
	
	uint8_t		typec_mode;
	uint8_t		typec_state;
	
	uint8_t		temp_mode;
	uint8_t		temp_state;
	
	uint8_t 	input_watts_state;
	uint8_t 	output_watts_state;
} lcd_ctrl_t;



#if 1
#pragma pack(1)
typedef union lcd_buf_		//ef03_LCD	new
{
    uint8_t lcd_raw[17];

    struct
    {
		//byte0
		uint8_t 					: 1;
		uint8_t						: 1;
		uint8_t						: 1;
		uint8_t						: 1;
		
		uint8_t 					: 1;
		uint8_t						: 1;
		uint8_t 					: 1;
		uint8_t						: 1;
		
		//byte1
		uint8_t bat_output_watts_11D	: 1;
		uint8_t bat_output_watts_11C	: 1;
		uint8_t bat_output_watts_11B	: 1;
		uint8_t bat_output_watts_11A	: 1;
		
		uint8_t bat_out_watts_flag_P14		: 1;//watts
		uint8_t bat_output_watts_11E	: 1;
		uint8_t bat_output_watts_11G	: 1;
		uint8_t bat_output_watts_11F	: 1;
		
		//byte2
		uint8_t bat_output_watts_10D	: 1;
		uint8_t bat_output_watts_10C	: 1;
		uint8_t bat_output_watts_10B	: 1;
		uint8_t bat_output_watts_10A	: 1;
		
		uint8_t bat_ups_flag_P13		: 1;	//ups
		uint8_t bat_output_watts_10E	: 1;
		uint8_t bat_output_watts_10G	: 1;
		uint8_t bat_output_watts_10F	: 1;
		
		//byte3
		uint8_t bat_output_watts_9D		: 1;
		uint8_t bat_output_watts_9C		: 1;
		uint8_t bat_output_watts_9B		: 1;
		uint8_t bat_output_watts_9A		: 1;
		
		uint8_t bat_wifi_flag_P12		: 1;	//wifi
		uint8_t bat_output_watts_9E		: 1;
		uint8_t bat_output_watts_9G		: 1;
		uint8_t bat_output_watts_9F		: 1;
		
		//byte4
		uint8_t bat_in_watts_flag_P16	: 1;	//watts
		uint8_t bat_input_flag_P17		: 1;	//input
		uint8_t fan_0_degrees_P16_3		: 1;
		uint8_t fan_45_degrees_P16_2	: 1;
		
		uint8_t bat_hi_temp_P19			: 1;	//hi_temp
		uint8_t bat_low_temp_P18		: 1;	//low_temp
		uint8_t fan_central_P16_4		: 1;
		uint8_t fan_90_degrees_P16_1	: 1;
		
		//byte5
		uint8_t bat_input_watts_8D		: 1;
		uint8_t bat_input_watts_8C		: 1;
		uint8_t bat_input_watts_8B		: 1;
		uint8_t bat_input_watts_8A		: 1;
		
		uint8_t bat_overload_flag_P20	: 1;	//overload
		uint8_t bat_input_watts_8E		: 1;
		uint8_t bat_input_watts_8G		: 1;
		uint8_t bat_input_watts_8F		: 1;
		
		//byte6
		uint8_t bat_input_watts_7D		: 1;
		uint8_t bat_input_watts_7C		: 1;
		uint8_t bat_input_watts_7B		: 1;
		uint8_t bat_input_watts_7A		: 1;
		
		uint8_t 						: 1;
		uint8_t bat_input_watts_7E		: 1;
		uint8_t bat_input_watts_7G		: 1;
		uint8_t bat_input_watts_7F		: 1;
		
		//byte7
		uint8_t bat_input_watts_6D		: 1;
		uint8_t bat_input_watts_6C		: 1;
		uint8_t bat_input_watts_6B		: 1;
		uint8_t bat_input_watts_6A		: 1;
		
		uint8_t bat_recharging_time_flag	: 1;	
		uint8_t bat_input_watts_6E		: 1;
		uint8_t bat_input_watts_6G		: 1;
		uint8_t bat_input_watts_6F		: 1;
		
		//byte8
		uint8_t bat_input_watts_5D		: 1;
		uint8_t bat_input_watts_5C		: 1;
		uint8_t bat_input_watts_5B		: 1;
		uint8_t bat_input_watts_5A		: 1;
		
		uint8_t bat_circle_S4			: 1;
		uint8_t bat_input_watts_5E		: 1;
		uint8_t bat_input_watts_5G		: 1;
		uint8_t bat_input_watts_5F		: 1;
		
		//byte9
		uint8_t bat_circle_S5			: 1;
		uint8_t bat_soc_per_flag_p23	: 1;	//%
		uint8_t bat_err_P22				: 1;
		uint8_t bat_circle_S3			: 1;
		
		uint8_t bat_soc_4D				: 1;
		uint8_t bat_soc_4C				: 1;
		uint8_t bat_soc_4B				: 1;
		uint8_t bat_soc_4A				: 1;
		
		//byte10
		uint8_t bat_circle_S2			: 1;
		uint8_t bat_soc_4E				: 1;
		uint8_t bat_soc_4G				: 1;
		uint8_t bat_soc_4F				: 1;
		
		uint8_t bat_soc_3D				: 1;
		uint8_t bat_soc_3C				: 1;
		uint8_t bat_soc_3B				: 1;
		uint8_t bat_soc_3A				: 1;
		
		//byte11
		uint8_t bat_circle_S1			: 1;
		uint8_t bat_soc_3E				: 1;
		uint8_t bat_soc_3G				: 1;
		uint8_t bat_soc_3F				: 1;
		
		uint8_t bat_circle_S6			: 1;
		uint8_t bat_circle_S7			: 1;
		uint8_t bat_soc_P21				: 1;
		uint8_t bat_circle_S8			: 1;
		
		//byte12
		uint8_t bat_time_2D				: 1;
		uint8_t bat_time_2C				: 1;
		uint8_t bat_time_2B				: 1;
		uint8_t bat_time_2A				: 1;
		
		uint8_t	bat_hours_flag_P3		: 1;
		uint8_t bat_time_2E				: 1;
		uint8_t bat_time_2G				: 1;
		uint8_t bat_time_2F				: 1;
		
		//byte13
		uint8_t bat_time_1D				: 1;
		uint8_t bat_time_1C				: 1;
		uint8_t bat_time_1B				: 1;
		uint8_t bat_time_1A				: 1;
		
		uint8_t	bat_50hz_flag_P8		: 1;
		uint8_t bat_ac_flag_P6			: 1;
		uint8_t	bat_out_bms_flag_P5		: 1;
		uint8_t bat_paralleling_flag_P4	: 1;//paralleling,联机
		
		
		
		//byte14
		uint8_t	bat_mins_flag_P2		: 1;
		uint8_t bat_time_1E				: 1;
		uint8_t bat_time_1G				: 1;
		uint8_t bat_time_1F				: 1;
		
		uint8_t	bat_60hz_flag_P7		: 1;
		uint8_t	bat_usb_flag_P9			: 1;
		uint8_t	bat_typec_flag_P10		: 1;
		uint8_t	bat_car_flag_P11		: 1;
		
		//byte15
		uint8_t 					: 1;
		uint8_t						: 1;
		uint8_t						: 1;
		uint8_t						: 1;
		
		uint8_t 					: 1;
		uint8_t						: 1;
		uint8_t 					: 1;
		uint8_t						: 1;
		
		//byte16
		uint8_t bat_output_watts_12D	: 1;
		uint8_t bat_output_watts_12C	: 1;
		uint8_t bat_output_watts_12B	: 1;
		uint8_t bat_output_watts_12A	: 1;
		
		uint8_t bat_output_flag_P15		: 1;	//output
		uint8_t bat_output_watts_12E	: 1;
		uint8_t bat_output_watts_12G	: 1;
		uint8_t bat_output_watts_12F	: 1;
    } bits;

} lcd_buf_u;
#pragma pack()

#else

#pragma pack(1)
typedef union lcd_buf_		//ef03_LCD	old
{
    uint8_t lcd_raw[17];
	
    struct
    {
		//byte0
		uint8_t 					: 1;
		uint8_t						: 1;
		uint8_t						: 1;
		uint8_t						: 1;
		
		uint8_t 					: 1;
		uint8_t						: 1;
		uint8_t 					: 1;
		uint8_t						: 1;
		
		//byte1
		uint8_t bat_output_watts_11D	: 1;
		uint8_t bat_output_watts_11C	: 1;
		uint8_t bat_output_watts_11B	: 1;
		uint8_t bat_output_watts_11A	: 1;
		
		uint8_t bat_out_watts_flag_P12		: 1;//watts
		uint8_t bat_output_watts_11E	: 1;
		uint8_t bat_output_watts_11G	: 1;
		uint8_t bat_output_watts_11F	: 1;
		
		//byte2
		uint8_t bat_output_watts_10D	: 1;
		uint8_t bat_output_watts_10C	: 1;
		uint8_t bat_output_watts_10B	: 1;
		uint8_t bat_output_watts_10A	: 1;
		
		uint8_t bat_wifi_flag_P11		: 1;	//wifi
		uint8_t bat_output_watts_10E	: 1;
		uint8_t bat_output_watts_10G	: 1;
		uint8_t bat_output_watts_10F	: 1;
		
		//byte3
		uint8_t bat_output_watts_9D		: 1;
		uint8_t bat_output_watts_9C		: 1;
		uint8_t bat_output_watts_9B		: 1;
		uint8_t bat_output_watts_9A		: 1;
		
		uint8_t bat_paralleling_flag_P10	: 1;//paralleling,联机
		uint8_t bat_output_watts_9E		: 1;
		uint8_t bat_output_watts_9G		: 1;
		uint8_t bat_output_watts_9F		: 1;
		
		//byte4
		uint8_t bat_low_temp_P18		: 1;
		uint8_t bat_hi_temp_P17			: 1;
		uint8_t fan_0_degrees_P16_3		: 1;
		uint8_t fan_45_degrees_P16_2	: 1;
		
		uint8_t bat_err_P19				: 1;//Output
		uint8_t bat_overload_P20		: 1;
		uint8_t fan_central_P16_4		: 1;
		uint8_t fan_90_degrees_P16_1	: 1;
		
		//byte5
		uint8_t bat_input_watts_8D		: 1;
		uint8_t bat_input_watts_8C		: 1;
		uint8_t bat_input_watts_8B		: 1;
		uint8_t bat_input_watts_8A		: 1;
		
		uint8_t bat_input_flag_P15		: 1;//input
		uint8_t bat_input_watts_8E		: 1;
		uint8_t bat_input_watts_8G		: 1;
		uint8_t bat_input_watts_8F		: 1;
		
		//byte6
		uint8_t bat_input_watts_7D		: 1;
		uint8_t bat_input_watts_7C		: 1;
		uint8_t bat_input_watts_7B		: 1;
		uint8_t bat_input_watts_7A		: 1;
		
		uint8_t bat_in_watts_flag_P14	: 1;//watts
		uint8_t bat_input_watts_7E		: 1;
		uint8_t bat_input_watts_7G		: 1;
		uint8_t bat_input_watts_7F		: 1;
		
		//byte7
		uint8_t bat_input_watts_6D		: 1;
		uint8_t bat_input_watts_6C		: 1;
		uint8_t bat_input_watts_6B		: 1;
		uint8_t bat_input_watts_6A		: 1;
		
		uint8_t 						: 1;
		uint8_t bat_input_watts_6E		: 1;
		uint8_t bat_input_watts_6G		: 1;
		uint8_t bat_input_watts_6F		: 1;
		
		//byte8
		uint8_t bat_input_watts_5D		: 1;
		uint8_t bat_input_watts_5C		: 1;
		uint8_t bat_input_watts_5B		: 1;
		uint8_t bat_input_watts_5A		: 1;
		
		uint8_t bat_charge_flag_P6		: 1;
		uint8_t bat_input_watts_5E		: 1;
		uint8_t bat_input_watts_5G		: 1;
		uint8_t bat_input_watts_5F		: 1;
		
		//byte9
		uint8_t bat_circle_S5			: 1;
		uint8_t bat_circle_S4			: 1;
		uint8_t bat_soc_per_flag_P22	: 1;
		uint8_t bat_circle_S3			: 1;
		
		uint8_t bat_soc_4D				: 1;
		uint8_t bat_soc_4C				: 1;
		uint8_t bat_soc_4B				: 1;
		uint8_t bat_soc_4A				: 1;
		
		//byte10
		uint8_t bat_circle_S2			: 1;
		uint8_t bat_soc_4E				: 1;
		uint8_t bat_soc_4G				: 1;
		uint8_t bat_soc_4F				: 1;
		
		uint8_t bat_soc_3D				: 1;
		uint8_t bat_soc_3C				: 1;
		uint8_t bat_soc_3B				: 1;
		uint8_t bat_soc_3A				: 1;
		
		//byte11
		uint8_t bat_circle_S1			: 1;
		uint8_t bat_soc_3E				: 1;
		uint8_t bat_soc_3G				: 1;
		uint8_t bat_soc_3F				: 1;
		
		//---------------------------------------------
		uint8_t bat_circle_S6			: 1;
		uint8_t bat_circle_S7			: 1;
		uint8_t bat_soc_P21				: 1;
		uint8_t bat_circle_S8			: 1;
		
		//byte12
		uint8_t bat_time_2D				: 1;
		uint8_t bat_time_2C				: 1;
		uint8_t bat_time_2B				: 1;
		uint8_t bat_time_2A				: 1;
		
		uint8_t	bat_hours_flag_P3		: 1;
		uint8_t bat_time_2E				: 1;
		uint8_t bat_time_2G				: 1;
		uint8_t bat_time_2F				: 1;
		
		//byte13
		uint8_t bat_time_1D				: 1;
		uint8_t bat_time_1C				: 1;
		uint8_t bat_time_1B				: 1;
		uint8_t bat_time_1A				: 1;
		
		uint8_t	bat_out_bms_flag_P5		: 1;
		uint8_t	bat_ac_flag_P4			: 1;
		uint8_t	bat_50hz_flag_P24		: 1;
		uint8_t	bat_recharge_time_flag_P1		: 1;
		
		//byte14
		uint8_t	bat_mins_flag_P2		: 1;
		uint8_t bat_time_1E				: 1;
		uint8_t bat_time_1G				: 1;
		uint8_t bat_time_1F				: 1;
		
		uint8_t	bat_usb_flag_P7			: 1;
		uint8_t	bat_typec_flag_P8		: 1;
		uint8_t	bat_60hz_flag_P23		: 1;
		uint8_t	bat_car_flag_P9			: 1;
		
		//byte15
		uint8_t 					: 1;
		uint8_t						: 1;
		uint8_t						: 1;
		uint8_t						: 1;
		
		uint8_t 					: 1;
		uint8_t						: 1;
		uint8_t 					: 1;
		uint8_t						: 1;
		
		//byte16
		uint8_t bat_output_watts_12D	: 1;
		uint8_t bat_output_watts_12C	: 1;
		uint8_t bat_output_watts_12B	: 1;
		uint8_t bat_output_watts_12A	: 1;
		
		uint8_t bat_output_flag_P13		: 1;//output
		uint8_t bat_output_watts_12E	: 1;
		uint8_t bat_output_watts_12G	: 1;
		uint8_t bat_output_watts_12F	: 1;
    } bits;

} lcd_buf_u;
#pragma pack()

#endif

extern lcd_buf_u lcd_buf;

void bsp_lcd_init(void);
void bsp_lcd_write(uint8_t *pdat, uint32_t len);


#endif  //_BSP_LCD_DRIVER_H

