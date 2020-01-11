#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_pwm.h"

#if 0
typedef struct
{
	uint8_t 	g;
	uint8_t 	r;
	uint8_t 	b;
} ws2812_grb_dat_t;	//高位在前


#define TOTLE_LED_NUM		10
ws2812_grb_dat_t ws2812_grb_dat[TOTLE_LED_NUM] = {0};

#define WS_2812_ARRAY	ws2812_grb_dat

/*
	+------------------------------+-------+---------+
	| H/L | H_ms | L_ms |  period  |  fre  | pulse   |
	+--------------------------------------+---------+
	|  0  |0.35us|0.8us |  1.25us  | 800k  |  28%    |
	+--------------------------------------+---------+
	|  1  |0.7us |0.6us |  1.25us  | 800k  |  56%    |
	+--------------------------------------+---------+
*/


#define WS_LOW_PULSE		2800u
#define WS_HIGH_PULSE		5600u
#define WS_RESET			0

#define WS_DMA_NUM			DMA0
#define WS_DMA_CH			DMA_CH5		//TIMER2_CH0

uint16_t LED_BYTE_Buffer[24*2*TOTLE_LED_NUM] = {0};

static void bsp_ws2812_write_low(void);

#if 0
static void ws2812_dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;

    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);  
	nvic_irq_enable(DMA0_Channel5_IRQn, 1, 0);

	
    /* ADC DMA_channel configuration */
    dma_deinit(WS_DMA_NUM, WS_DMA_CH);
    dma_struct_para_init(&dma_data_parameter);
	
    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&TIMER_CH0CV(TIMER2));	//TIMER2_CH0
    dma_data_parameter.periph_inc 	= DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr 	= (uint32_t)(&LED_BYTE_Buffer);	
    dma_data_parameter.memory_inc 	= DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
    dma_data_parameter.direction 	= DMA_MEMORY_TO_PERIPHERAL;
    dma_data_parameter.number 		= ARRY_ITEMS_NUM(LED_BYTE_Buffer);
    dma_data_parameter.priority 	= DMA_PRIORITY_MEDIUM;
    dma_init(WS_DMA_NUM, WS_DMA_CH, &dma_data_parameter);


//	dma_interrupt_enable(WS_DMA_NUM, WS_DMA_CH, DMA_INT_FTF);
//	
//    dma_circulation_enable(WS_DMA_NUM, WS_DMA_CH);
//    /* enable DMA channel */
//    dma_channel_enable(WS_DMA_NUM, WS_DMA_CH);
	
	
	//bsp_pwm_write(0, 2000);
	bsp_ws2812_write_low();
	//timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, 20);	//75*2=150
	//TIMER_CH0CV(TIMER2) = 65;
	
	
	//LED_BYTE_Buffer[0] = 22;
}
REG_INIT_MOD(ws2812_dma_config);
#endif

void DMA0_Channel5_IRQHandler(void)
{
	if(dma_interrupt_flag_get(WS_DMA_NUM, WS_DMA_CH, DMA_INT_FLAG_FTF) != RESET)
	{
		/*清除DMA中断标志位*/
		dma_interrupt_flag_clear(WS_DMA_NUM, WS_DMA_CH, DMA_INT_FLAG_FTF);
		dma_interrupt_flag_clear(WS_DMA_NUM, WS_DMA_CH, DMA_INT_FLAG_G);
		
		DBG_I_LIMIT(1000, "DMA0_Channel5_IRQHandler");
	}
}


#if 1
static void bsp_ws2812_write_low(void)
{
	bsp_pwm_write(IDX_PWM_LED, WS_LOW_PULSE);
}

static void bsp_ws2812_write_high(void)
{
	bsp_pwm_write(IDX_PWM_LED, WS_HIGH_PULSE);
}	

static void bsp_ws2812_reset(void)
{
	bsp_pwm_write(IDX_PWM_LED, WS_RESET);
}


static void bsp_ws2812_write_byte(uint8_t byte)
{
	uint8_t val = byte;
	uint8_t i = 0;
	for(i = 0; i < 8;i++)
	{
		if(val&0x80)	//最高位
		{
			bsp_ws2812_write_high();
		}
		else
		{
			bsp_ws2812_write_low();
		}
		val <<= 1;
	}
}

static void set_ws2812_grb_val(uint8_t g_val, uint8_t r_val, uint8_t b_val)
{
	bsp_ws2812_write_byte(g_val);
	bsp_ws2812_write_byte(r_val);
	bsp_ws2812_write_byte(b_val);
}

static void set_ws2812_grb_by_index(uint8_t index)
{
	set_ws2812_grb_val(WS_2812_ARRAY[index].g, WS_2812_ARRAY[index].r, WS_2812_ARRAY[index].b);
}

static void updata_ws2812_data(uint8_t max_num)
{
	uint8_t i = 0;
	for(i = 0;i < max_num;i++)
	{
		set_ws2812_grb_by_index(i);
	}
}

static void ws2812_test_func(void)
{
	static uint8_t i = 0;
	
	i++;
	i = i%4;
	
	if(i==0)
	{
		bsp_ws2812_write_low();
	}
	else if(i == 1)
	{
		bsp_ws2812_reset();
	}
	else if(i == 2)
	{
		bsp_ws2812_write_high();
	}
	else
	{
		bsp_ws2812_reset();
	}

	#if 0
	if(i==0)
	{
		ws2812_grb_dat[0].g = 0x0;
		ws2812_grb_dat[0].r = 0x0;
		ws2812_grb_dat[0].b = 0x0;
		

		updata_ws2812_data(2);
	}
	else if(i == 1)
	{
		ws2812_grb_dat[0].g = 0x0;
		ws2812_grb_dat[0].r = 0x0;
		ws2812_grb_dat[0].b = 0x0;
		

		updata_ws2812_data(2);
	}
	else
	{
		bsp_ws2812_reset();
	}
	#endif
	
}
REG_TASK(3, 500, ws2812_test_func);



//static void ws2812_test_func(void)
//{
//	uint8_t i = 0;
//	
//	i++;
//	i = i%2;
//	
//	if(i==0)
//	{
//		ws2812_grb_dat[0].g = 0x0;
//		ws2812_grb_dat[0].r = 0xff;
//		ws2812_grb_dat[0].b = 0x0;
//	}
//	else
//	{
//		ws2812_grb_dat[0].g = 0x0;
//		ws2812_grb_dat[0].r = 0x0;
//		ws2812_grb_dat[0].b = 0xff;
//	}
//	
//	
//	
//	updata_ws2812_data(3);
//	
//	bsp_ws2812_reset();
//}
////REG_TASK(3, 500, ws2812_test_func);


void TIMER2_IRQHandler(void)
{
	static uint8_t cnt = 0;
	if(timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP) == SET)
	{
		timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
		
		//bsp_ws2812_write_byte(0xff);
		
		cnt++;
		
		//if(cnt<)
		//bsp_ws2812_write_high();
		
		//DBG_I_LIMIT(500, "TIMER2_IRQHandler");
	}
}
#endif

#if 0
#include "bsp_gpio.h"
#define DI(val) 	bsp_gpio_write(IDX_GPIO_BEEP, val)
#define SNUM	1
//-------------------------------------------------------------------------------
//子程序名称:ResetDateFlow(void)
//功能：复位，为下一次发送做准备，
//说明：将DI置位为0后，延时约65us
//-------------------------------------------------------------------------------
void ResetDataFlow(void)
{
	unsigned char i,j;
	DI(0);					//DI置为0后，延时50us以上，实现帧复位
	for(i=0;i<15;i++)		//此处33Mhz时延时65us
	{
		for(j=0;j<20;j++)
		{
			;
		}
	}
}
//-------------------------------------------------------------------------------
//子程序名称:SendOnePix(unsigned char *ptr)
//功能：发送一个像素点的24bit数据
//参数：接收的参数是一个指针，此函数会将此指针指向的地址的连续的三个Byte的数据发送
//说明：在主函数中直接调用此函数时，在整个帧发送开始前需要先执行 ResetDataFlow()
//		数据是按归零码的方式发送，速率为800KBPS
//-------------------------------------------------------------------------------
void SendOnePix(unsigned char *ptr)
{
	unsigned char i,j;
	unsigned char temp;

	for(j=0;j<3;j++)
	{
		temp=ptr[j];
		for(i=0;i<8;i++)
		{
			if(temp&0x80)		 //从高位开始发送
			{
				DI(1);			 //发送“1”码
				__nop();		 //不可省略的nop(),延时指定时间作用，晶振频率33MHz
				__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();


				DI(0);
				__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();
			}
			else				//发送“0”码
			{
				DI(1);
				__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();

				DI(0);
				__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();
				__nop();__nop();__nop();__nop();__nop();
			}
			temp=(temp<<1);		 //左移位
		}
	}
}
//-------------------------------------------------------------------------------
//子程序名称:SendOneFrame(unsigned char *ptr)
//功能：发送一帧数据（即发送整个数组的数据）
//参数：接收的参数是一个指针，此函数会将此指针指向的地址的整个数组的数据发送
//-------------------------------------------------------------------------------
void SendOneFrame(unsigned char *ptr)
{
	unsigned char k;

	ResetDataFlow();				 //发送帧复位信号

	for(k=0;k<SNUM;k++)				 //发送一帧数据，SNUM是板子LED的个数
	{
		SendOnePix(&ptr[(3*k)]);
	}

	ResetDataFlow();				 //发送帧复位信号
}

//-------------------------------------------------------------------------------
//子程序名称:SendSameColor(unsigned char *ptr,unsigned char cnt)
//功能：相同颜色的点发送cnt次
//参数：接收的参数是一个指针，指向像素点颜色数组，cnt传递发送个数
//-------------------------------------------------------------------------------
void SendSameColor(unsigned char *ptr,unsigned char cnt)
{
	unsigned char k;

	ResetDataFlow();				 //发送帧复位信号

	for(k=0;k<cnt;k++)				 //发送一帧数据，SNUM是板子LED的个数
	{
		SendOnePix(&ptr[0]);
	}

	ResetDataFlow();				 //发送帧复位信号
}
//-------------------------------------------------------------------------------
//子程序名称:SendOneFrameFrom(unsigned char i,unsigned char *ptr)
//功能：从指定的像素点开始发送一帧数据（即发送整个数组的数据）
//参数：接收的参数是一个指针，此函数会将此指针指向的地址的整帧数据发送
//		i:把数组的第0个像素数据发送到第i个像素点（第0个像素是板上标号为01的像素）
//说明：即原本对应第一个像素的数据会发送到第i个像素点（LED）上
//-------------------------------------------------------------------------------
void SendOneFrameFrom(unsigned char i,unsigned char *ptr)
{
	unsigned char k;

	ResetDataFlow();				 //发送帧复位信号

   	for(k=(SNUM-i);k<SNUM;k++)		 //发送一帧数据
	{
		SendOnePix(&ptr[(3*k)]);
	}
	for(k=0;k<(SNUM-i);k++)
	{
		SendOnePix(&ptr[(3*k)]);
	}

	ResetDataFlow();				 //发送帧复位信号
}

//-------------------------------------------------------------------------------
//子程序名称:SendOneFrameSince(unsigned char i,unsigned char *ptr)
//功能：从第i个像素点的数据开始发送一帧数据（即发送整个数组的数据）
//参数：接收的参数是一个指针，此函数会将此指针指向的地址的整帧数据发送
//		i:把数组的第i个像素数据发送到第1个像素点
//说明：即原本对应第i像素的数据会发送到第1个像素点（LED）上，第i+1个像素点的数据
//		发送到第2个像素上
//-------------------------------------------------------------------------------
void SendOneFrameSince(unsigned char i,unsigned char *ptr)
{
	unsigned char k;

	ResetDataFlow();				 //发送帧复位信号

	for(k=i;k<SNUM;k++)				 //发送一帧数据
	{
		SendOnePix(&ptr[(3*k)]);
	}
	for(k=0;k<i;k++)
	{
		SendOnePix(&ptr[(3*k)]);
	}
	ResetDataFlow();				 //发送帧复位信号
}
/**********************************THE END**********************************/ 


void test_ws2812_func(void)
{
	uint8_t str[3] = {0xff, 0, 0};
	SendOneFrameFrom(0, str);
}
//REG_TASK(3, 1000, test_ws2812_func);
#endif

#endif
