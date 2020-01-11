#ifndef __APP_DRIVER_CFG_H__
#define __APP_DRIVER_CFG_H__


enum
{
    DRV_SYSTICK  = 0,   /* sys tick 必须为0*/  //系统时钟是必须的一个驱动设备，必须注册
	
	DRV_GPIO,
	DRV_RTT,
	DRV_UART0,
	DRV_UART1,
	DRV_UART2,
	DRV_UART3,
	DRV_UART4,
	DRV_FALSH,
	DRV_PWM,
};

#define MAX_LED_NUM		1
enum pwm_ch
{
	IDX_PWM_LED = 0,
};

#define MAX_AD_CH		1


enum 
{
	IDX_GPIO_RED_LED = 0,
	IDX_GPIO_BLUE_LED,
	IDX_GPIO_GREEN_LED,
	IDX_GPIO_BEEP,
};

//是否开启对应驱动
#define ENABLE_SYS_TICK_DRV         1	
#define ENABLE_RTT_DRV				1
#define ENABLE_GPIO_DRV				1
#define ENABLE_UART_0_DRV			1
#define ENABLE_UART_1_DRV			1
#define ENABLE_UART_2_DRV			1
#define ENABLE_UART_3_DRV			1
#define ENABLE_UART_4_DRV			1
#define ENABLE_PWM_DRV				1
//#define KEY_SHORT   0x01
//#define KEY_DOUBLE  0x02
//#define KEY_LONG    0x03
//#define KEY_LONG_UP 0x04



#endif //__APP_DRIVER_CFG_H__
