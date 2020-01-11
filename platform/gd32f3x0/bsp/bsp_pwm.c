#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_pwm.h"

//-----------------------xyh
typedef struct
{
    uint32_t			port;					//GPIOB
    rcu_periph_enum     RCC_port;				//RCU_GPIOB
    uint16_t            pin;					//GPIO_PIN_0
    uint32_t			Timer;					//TIMER2
	uint16_t			Timer_ch;				//TIMER_CH_0
	uint32_t			af_func;				//GPIO_AF_1
	//uint32_t 			remap_type;				//GPIO_TIMER0_PARTIAL_REMAP
	
    rcu_periph_enum     RCC_TIM;				//RCU_TIMER2
	
	uint32_t			fre;					//1000HZ
	uint16_t			pulse;					//0-100
}pwm_cfg_t;




#define ADD_PWM_ITEM(port,pin,timer_num,ch,af_func,fre,pulse)  \
					{										\
						port,								\
						RCU_##port,							\
						GPIO_PIN_##pin,						\
						TIMER##timer_num,					\
						TIMER_CH_##ch,						\
						af_func,							\
						RCU_TIMER##timer_num,				\
						fre,								\
						pulse,								\
					}

//fre = 72000k / ( (psc+1)*(period+1) )
const pwm_cfg_t pwm_tab[]=
{
				//port		pin		timer	ch		af,			fre(Hz)		pulse
    ADD_PWM_ITEM(GPIOB,		4,  	2, 		0,	 GPIO_AF_2,		10000,		5600),	//led_blue
	//ADD_PWM_ITEM(GPIOB,		5,  	2, 		1,		1,		800000,		2500),	//led_green
	//ADD_PWM_ITEM(GPIOB, 	1,		2 , 	3, 		0, 		1000,		5000), 	//led_red
};


#define CKTIM			((uint32_t)SystemCoreClock)
#define PWM_ITEM		pwm_tab[i]
#define PWM_PSC			1U		//需要的频率过低，分频值可以大一点
#define PWM_PERIOD		CKTIM/(PWM_PSC*PWM_ITEM.fre*2)
#define PWM_PULSE		(PWM_PERIOD*PWM_ITEM.pulse/10000)	//100% -> 10000


#define EN_TIMER2_IRQ	0

void bsp_pwm_init(void)
{
    uint32_t i = 0;			//must is i
	
	//-------timer config
	timer_oc_parameter_struct timer_ocinitpara;
	timer_parameter_struct timer_initpara;
	
	/* deinit a TIMER */
	timer_deinit(pwm_tab[i].Timer);
	
	for(i = 0;i < ARRY_ITEMS_NUM(pwm_tab);i++)
	{
		//-------gpio config
		/* enable the GPIOB clock */
		rcu_periph_clock_enable(pwm_tab[i].RCC_port);
//		rcu_periph_clock_enable(RCU_AF);
		
		/*configure PB0(TIMER2 CH2) as alternate function*/
//		gpio_init(pwm_tab[i].port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ ,pwm_tab[i].pin);
		gpio_mode_set(pwm_tab[i].port, GPIO_MODE_AF, GPIO_PUPD_NONE, pwm_tab[i].pin);
		gpio_output_options_set(pwm_tab[i].port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,pwm_tab[i].pin);
		
		gpio_af_set(pwm_tab[i].port, pwm_tab[i].af_func, pwm_tab[i].pin);
//		if(pwm_tab[i].is_remap != 0)
//		{
//			if(pwm_tab[i].Timer == TIMER0)
//			{
//				gpio_pin_remap_config(GPIO_TIMER0_PARTIAL_REMAP,  ENABLE);
//			}
//			if(pwm_tab[i].Timer == TIMER2)
//			{
//				gpio_pin_remap_config(GPIO_TIMER2_PARTIAL_REMAP,  ENABLE);
//			}
//		}
		

		/* enable the TIMER clock */
		rcu_periph_clock_enable(pwm_tab[i].RCC_TIM);
		
		/* initialize TIMER init parameter struct */
		timer_struct_para_init(&timer_initpara);
		/* TIMER2 configuration */
		timer_initpara.prescaler         = PWM_PSC-1;
		timer_initpara.alignedmode       = TIMER_COUNTER_CENTER_BOTH;	//TIMER_COUNTER_CENTER_UP  TIMER_COUNTER_CENTER_BOTH
		timer_initpara.counterdirection  = TIMER_COUNTER_UP;
		timer_initpara.period            = PWM_PERIOD-1;
		timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
		timer_initpara.repetitioncounter = 0;
		timer_init(pwm_tab[i].Timer, &timer_initpara);
		
		/* initialize TIMER channel output parameter struct */
		timer_channel_output_struct_para_init(&timer_ocinitpara);
		/* configure TIMER channel output function */
		timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
		timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
		timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
		timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
		timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
		timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
		timer_channel_output_config(pwm_tab[i].Timer, pwm_tab[i].Timer_ch, &timer_ocinitpara);

		/* CH0 configuration in PWM mode0, duty cycle 25% */
		timer_channel_output_pulse_value_config(pwm_tab[i].Timer, pwm_tab[i].Timer_ch, PWM_PULSE);
		timer_channel_output_mode_config(pwm_tab[i].Timer, pwm_tab[i].Timer_ch, TIMER_OC_MODE_PWM0);
		timer_channel_output_shadow_config(pwm_tab[i].Timer, pwm_tab[i].Timer_ch, TIMER_OC_SHADOW_ENABLE);
		
		
		//----irq
		#if EN_TIMER2_IRQ
			nvic_irq_enable(TIMER2_IRQn, 2, 0);
			timer_interrupt_enable(PWM_ITEM.Timer, TIMER_INT_FLAG_UP);
		#endif
		
		/* auto-reload preload enable */
		timer_auto_reload_shadow_enable(pwm_tab[i].Timer);
		/* auto-reload preload enable */
		timer_enable(pwm_tab[i].Timer);
		
		/* TIMER0 primary output function enable */
		timer_primary_output_config(pwm_tab[i].Timer, ENABLE);
		
		/* TIMER0 update DMA request enable */
		timer_dma_transfer_config(pwm_tab[i].Timer, TIMER_DMACFG_DMATA_CH0CV, TIMER_DMACFG_DMATC_4TRANSFER);
		timer_dma_enable(pwm_tab[i].Timer, TIMER_DMA_UPD);
	}
	
	DBG_I("bsp_pwm_init success------");
}
//REG_INIT_MOD(bsp_pwm_init);

int32_t bsp_pwm_write(uint8_t i,uint32_t pulse)		//i-->pwm_ch	为了使用PWM_PERIOD，故改为i
{
    if( i<ARRY_ITEMS_NUM(pwm_tab) )
    {
		if(pulse > 10000)	{pulse = 10000;}
         //pwm_tab[pwm_ch].TIM_SetComparex(pwm_tab[pwm_ch].Timer,val);
		timer_channel_output_pulse_value_config(pwm_tab[i].Timer, pwm_tab[i].Timer_ch, PWM_PERIOD*pulse/10000);
    }
	return 0;
}

#if TINY_FUNC_CODE == 0
int32_t set_pwm(uint8_t argc, uint8_t *argv[])
{
    if(argc>=3)
    {
        uint32_t ch = 0;
        uint32_t pulse = 0;
        
        sscanf((const char*)argv[1],"%d",&ch);
        sscanf((const char*)argv[2],"%d",&pulse);    
        
        bsp_pwm_write((uint8_t)ch,pulse);
        DBG_I("set_pwm ch:%d pulse:%d success!!",ch,pulse);
    }
    else
    {
        DBG_W("warn the set_pwm param is error: eg:set_pwm ch pulse");
    }
	return RET_OK;
}

REG_SHELL_CMD(set_pwm,0,set_pwm,"set_pwm", "eg:set_pwm ch pulse\r\n ");
#endif

