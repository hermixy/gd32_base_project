#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_exit.h"


//--------------------xyh
typedef struct
{
    uint32_t       			port;				//GPIOA
    rcu_periph_enum     	RCC_port;			//RCU_GPIOA
    uint32_t            	pin;				//GPIO_PIN_0
	
	
	exti_trig_type_enum		exti_type;			//EXTI_TRIG_FALLING
	exti_line_enum 			exti_line;			//EXTI_0
	uint8_t 				exti_irqn;			//EXTI0_IRQn
	uint8_t 				GPIO_PortSource;	//EXTI_SOURCE_GPIOA
	uint8_t 				GPIO_PinSource;		//EXTI_SOURCE_PIN0
	
} exit_item_t;

#define ADD_KEY_ITEM(port,pin,exti_type,exti_irqn)  \
		{											\
			port,									\
			RCU_##port,								\
			GPIO_PIN_##pin,							\
			exti_type,								\
			EXTI_##pin,								\
			exti_irqn,								\
			GPIO_PORT_SOURCE_##port,				\
			GPIO_PIN_SOURCE_##pin,					\
		}

const exit_item_t exit_tab[] =
{
	ADD_KEY_ITEM(GPIOA, 0, EXTI_TRIG_BOTH, EXTI0_IRQn),  	//PA0
};


void bsp_exit_init(void)
{
	uint8_t i = 0;
	
	rcu_periph_clock_enable(RCU_AF);
	
	for(i = 0; i < ARRY_ITEMS_NUM(exit_tab); i++)
	{
		//----------gpio config
		/* enable the TAMPER key gpio clock */
		rcu_periph_clock_enable(exit_tab[i].RCC_port);
		/* configure button pin as input */
		gpio_init(exit_tab[i].port, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, exit_tab[i].pin);
		
		//----------exti config
		/* enable and set key EXTI interrupt to the specified priority */
		nvic_irq_enable(exit_tab[i].exti_irqn, 2U, 0U);

		/* connect key EXTI line to key GPIO pin */
		gpio_exti_source_select(exit_tab[i].GPIO_PortSource, exit_tab[i].GPIO_PinSource);

		/* configure key EXTI line */
		exti_init(exit_tab[i].exti_line, EXTI_INTERRUPT, exit_tab[i].exti_type);
		exti_interrupt_flag_clear(exit_tab[i].exti_line);
		
		DBG_I("bsp_exti_init success");
	}
}

uint32_t bsp_exit_read(uint8_t ch)
{
    if( ch < ARRY_ITEMS_NUM(exit_tab) )
    {
        return gpio_input_bit_get(exit_tab[ch].port, exit_tab[ch].pin);
    }
    return 0;
}

#define EXTI_LINE_RX	EXTI_0
#define EXTI_PORT_RX	GPIOA
#define EXTI_PIN_RX		GPIO_PIN_0
#define EXTI_PIN_RX_NUM	0
void EXTI0_1_IRQHandler(void)
{
	//uint8_t level = 1;
    if (RESET != exti_interrupt_flag_get(EXTI_LINE_RX)) 
	{
		//level = gpio_input_bit_get(EXTI_PORT_RX, EXTI_PIN_RX);
		//DBG_I("EXTI0_1_IRQHandler---EXTI_LINE  level:%d",level);
		
		hal_dev_notify(DRV_EXIT, EXTI_PIN_RX_NUM, NULL, 0);
		
        exti_interrupt_flag_clear(EXTI_LINE_RX);
    }
}

