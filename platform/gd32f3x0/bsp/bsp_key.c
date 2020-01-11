#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_key.h"

typedef struct
{
    uint32_t            port;
	rcu_periph_enum     rcu;
    uint16_t            pin;
    //uint8_t             EXTI_portx;			//EXTI0_IRQn
    exti_line_enum      EXTI_Linex;			//EXTI_0
    exti_trig_type_enum trig_type;			//EXTI_TRIG_FALLING
	uint8_t 			NVIC_IRQCh;			//EXTI0_IRQn
	uint8_t 			GPIO_PortSource;	//GPIO_PORT_SOURCE_GPIOA
	uint8_t 			GPIO_PinSource;		//GPIO_PIN_SOURCE_0
	
} key_item_t;

#define ADD_KEY_ITEM(port,pin,type,ch)   	\
		{									\
			port,							\
			RCU_##port,						\
			GPIO_PIN_##pin,					\
			EXTI_##pin,						\
			type,							\
			ch,								\
			GPIO_PORT_SOURCE_##port,		\
			GPIO_PIN_SOURCE_##pin			\
		}

const key_item_t key_tab[] =
{
	ADD_KEY_ITEM(GPIOA, 10, EXTI_TRIG_BOTH,		EXTI10_15_IRQn),  	//PA1
};

enum
{
	POWER_KEY_PIN	= 0	,
	OUT_KEY_PIN			,
};

void bsp_key_init(void)
{
	uint32_t i = 0;
	rcu_periph_clock_enable(RCU_AF);
	
	for(i = 0; i < ARRY_ITEMS_NUM(key_tab); i++)
    {
        //------gpio init
		rcu_periph_clock_enable(key_tab[i].rcu);
		
		/* configure button pin as input */
		gpio_init(key_tab[i].port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, key_tab[i].pin);
		
		/* enable and set key wakeup EXTI interrupt to the lowest priority */
		nvic_irq_enable(key_tab[i].NVIC_IRQCh, 2U, 0U);
		
		/* connect key wakeup EXTI line to key GPIO pin */
		gpio_exti_source_select(key_tab[i].GPIO_PortSource, key_tab[i].GPIO_PinSource);
		
		/* configure key wakeup EXTI line */
		exti_init(key_tab[i].EXTI_Linex, EXTI_INTERRUPT, key_tab[i].trig_type);
		exti_interrupt_enable(key_tab[i].EXTI_Linex);
		
		//exti_software_interrupt_enable(key_tab[i].EXTI_Linex);	//软件使能进中断
		
		exti_interrupt_flag_clear(key_tab[i].EXTI_Linex);
    }
    DBG_I("bsp_key_init over!!");
}
//REG_INIT_MOD(bsp_key_init);

uint32_t bsp_key_read(uint8_t ch)
{
    if( ch < ARRY_ITEMS_NUM(key_tab) )
    {
        return gpio_input_bit_get(key_tab[ch].port, key_tab[ch].pin);
    }
    return 0;
}


enum
{
	IDX_POWER_KEY_PIN	= 0	,
	IDX_OUT_KEY_PIN			,
};

void key_irq_handle(uint8_t ch,uint32_t io_level);

//--------power_key
#define EXTI_LINE	EXTI_10
#define EXTI_PORT	GPIOA
#define EXTI_PIN	GPIO_PIN_10

void EXTI10_15_IRQHandler(void)
{
	uint8_t level = 1;
    if (RESET != exti_interrupt_flag_get(EXTI_LINE)) 
	{
		level = gpio_input_bit_get(EXTI_PORT, EXTI_PIN);
		//DBG_I("EXTI4_15_IRQHandler---%s  level:%d",EXTI_LINE,level);
		key_irq_handle(IDX_POWER_KEY_PIN,level);
		
        exti_interrupt_flag_clear(EXTI_LINE);
    }
}




static uint32_t down_time = 0;
static uint32_t up_time   = 0;

enum
{
    key_state_idle = 0,		//0
    key_state_down ,		//1
    key_state_hold ,		//2
    key_state_up,			//3
    key_state_confirm,		//4
    key_state_judge,		//5
    key_action,				//6
};

//------power_key
uint32_t power_key_test_state      = key_state_idle;
uint32_t power_last_key_test_state = key_state_idle;

//------ac_key
uint32_t ac_key_test_state      = key_state_idle;
uint32_t ac_last_key_test_state = key_state_idle;
static void change_key_state(uint32_t ch,uint32_t tag_state)
{
	if(ch == IDX_POWER_KEY_PIN)
	{
		power_last_key_test_state = power_key_test_state;
		power_key_test_state = tag_state;
		//DBG_I("change_key_state  %d",tag_state);
	}
	else if(ch == IDX_OUT_KEY_PIN)
	{
		ac_last_key_test_state = ac_key_test_state;
		ac_key_test_state = tag_state;
	}
}

void power_key_task(uint32_t ch)
{
	static uint32_t key_code =       0;
	static uint8_t  flag_is_double = 0;
	static uint8_t  flag_is_long   = 0;
    switch(power_key_test_state)
    {
		case key_state_idle:
			{
				flag_is_double = 0;
				flag_is_long =  0;
			}break;
		
		case key_state_down:
			{
				if(power_last_key_test_state == key_state_confirm)
				{
					flag_is_double = 1;
				}
				down_time = get_sys_ms();
				change_key_state(ch,key_state_hold);
			}break;
			
		case key_state_hold:
			{
				if( (get_sys_ms() - down_time) > 600)
				{
					key_code = KEY_LONG;
					flag_is_long = 1;
					hal_dev_notify(DRV_KEY, key_code, (uint8_t *)&ch, 4);
					//change_key_state(key_state_judge);
				}
				if( (get_sys_ms() - down_time) > 5600)
				{
					change_key_state(ch,key_state_judge);	//????5??????,????????
				}
			}break;
		
		case key_state_up:
			{
				if(power_last_key_test_state == key_state_idle)
				{
					change_key_state(ch,key_state_idle);
				}
				else
				{
					up_time =    get_sys_ms() ;
					if( (up_time - down_time) > 20)
					{
						change_key_state(ch,key_state_confirm);
					}
					else
					{
						change_key_state(ch,key_state_idle);
					}
				}
			}break;
			
		case key_state_confirm:
			{
				if( (get_sys_ms() - up_time)  > 200)
				{
					if(flag_is_long)
					{
						key_code = KEY_LONG_UP;
					}
					else
					{
						if(flag_is_double)
						{
							key_code = KEY_DOUBLE;
						}
						else
						{
							key_code = KEY_SHORT;
						}
					}
					
					change_key_state(ch,key_state_judge);
				}
			}break;
			
		case key_state_judge:
			{
				change_key_state(ch,key_action);
			}break;
			
		case key_action:
			{
				flag_is_double = 0;
				flag_is_long = 0;
				//DBG_I("key code :%d",key_code);
				change_key_state(ch,key_state_idle);
				hal_dev_notify(DRV_KEY, key_code, (uint8_t *)&ch, 4);
			}break;

		default:
			break;
	}
}

//void out_key_task(uint8_t ch)
//{
//	static uint32_t key_code =       0;
//	static uint8_t  flag_is_double = 0;
//	static uint8_t  flag_is_long   = 0;
//	
//    switch(ac_key_test_state)
//    {
//		case key_state_idle:
//			{
//				flag_is_double = 0;
//				flag_is_long =  0;
//			}break;
//			
//		case key_state_down:
//			{
//				if(ac_last_key_test_state == key_state_confirm)
//				{
//					flag_is_double = 1;
//				}
//				down_time = get_sys_ms();
//				change_key_state(ch,key_state_hold);
//			}break;
//		
//		case key_state_hold:
//			{
//				if( (get_sys_ms() - down_time) > 600)
//				{
//					key_code = KEY_LONG;
//					flag_is_long = 1;
//					hal_dev_notify(DRV_KEY, key_code, (uint8_t *)&ch, 1);
//					//change_key_state(key_state_judge);
//				}
//				if( (get_sys_ms() - down_time) > 5600)
//				{
//					change_key_state(ch,key_state_judge);//????5??????,????????
//				}
//			}break;
//		
//		case key_state_up:
//			{
//				if(ac_last_key_test_state == key_state_idle)
//				{
//					change_key_state(ch,key_state_idle);
//				}
//				else
//				{
//					up_time =    get_sys_ms() ;
//					if( (up_time - down_time) > 20)
//					{
//						change_key_state(ch,key_state_confirm);
//					}
//					else
//					{
//						change_key_state(ch,key_state_idle);
//					}
//				}
//			}break;
//			
//		case key_state_confirm:
//			{
//				if( (get_sys_ms() - up_time)  > 50)
//				{
//					if(flag_is_long)
//					{
//						key_code = KEY_LONG_UP;
//					}
//					else
//					{
//						if(flag_is_double)
//						{
//							key_code = KEY_DOUBLE;
//						}
//						else
//						{
//							key_code = KEY_SHORT;
//						}
//					}
//					
//					change_key_state(ch,key_state_judge);
//				}
//			}break;
//			
//		case key_state_judge:
//			{
//				change_key_state(ch,key_action);
//			}break;
//			
//		case key_action:
//			{
//				flag_is_double = 0;
//				flag_is_long = 0;
//				//DBG_I("key code :%d",key_code);
//				change_key_state(ch,key_state_idle);
//				hal_dev_notify(DRV_KEY, key_code, (uint8_t *)&ch, 4);
//			}break;

//		default:
//			break;
//    }
//}

void key_irq_handle(uint8_t ch,uint32_t io_level)
{
    //DBG_I_LIMIT(1000,"key_irq_handle level:%d  ms:%d  state:0x%x  is_double:%d",io_level,get_sys_ms(),power_key_test_state,flag_is_double);
	if(io_level == 0)
	{
		change_key_state(ch,key_state_down);
	}
	else
	{
		change_key_state(ch,key_state_up);
	}
}

void key_scan(void)
{
	power_key_task(IDX_POWER_KEY_PIN);	//?????,??,????????
	//out_key_task(IDX_OUT_KEY_PIN);
}
REG_TASK(ID_TASK, 20, key_scan);
