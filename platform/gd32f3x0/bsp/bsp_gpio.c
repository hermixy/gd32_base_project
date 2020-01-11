#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_gpio.h"


typedef struct
{
    uint32_t            port;
	uint32_t            rcu;
    uint32_t            pin;
    uint8_t             mode;
    uint8_t             default_io_level;
} gpio_cfg_t;


#define ADD_GPIO_ITEM(port,pin,mode,default_level)  \
					{   	port,                   \
							RCU_##port,      \
							GPIO_PIN_##pin,         \
							mode,						\
							default_level,				\
					}


const gpio_cfg_t gpio_tab[] =
{
	//-----------auto_test
	ADD_GPIO_ITEM(GPIOC, 13,  GPIO_MODE_OUTPUT, 	SET),	//red_led
};


void bsp_gpio_init(void)
{
	uint32_t i = 0;

	/*configure dubug GPIO pin remap*/
	//gpio_pin_remap_config(GPIO_SWJ_DISABLE_REMAP, DISABLE);
	//rcu_periph_clock_enable(RCU_AF);
	//gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
    for(i = 0; i < ARRY_ITEMS_NUM(gpio_tab); i++)
    {
    	/* enable the led clock */
    	rcu_periph_clock_enable((rcu_periph_enum) gpio_tab[i].rcu);
		/* configure led GPIO port */ 
		gpio_mode_set(gpio_tab[i].port, gpio_tab[i].mode,GPIO_PUPD_NONE,gpio_tab[i].pin);
		gpio_output_options_set(gpio_tab[i].port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, gpio_tab[i].pin);
		
		gpio_bit_write(gpio_tab[i].port,gpio_tab[i].pin,(bit_status)gpio_tab[i].default_io_level);
    }
}


int32_t bsp_gpio_write(uint8_t ch, uint32_t val)
{
	//DBG_I("gpio_index :%d  val:%d",ch,val);
    if( ch < ARRY_ITEMS_NUM(gpio_tab) )
    {
    	gpio_bit_write(gpio_tab[ch].port, gpio_tab[ch].pin, (bit_status)val);
        //DBG_I("bsp_gpio_write success!! ch:%d  level:%d",ch,val);
    }
    return 0;
}

uint32_t bsp_gpio_read(uint8_t ch)
{
    if( ch < ARRY_ITEMS_NUM(gpio_tab) )
    {
        return gpio_input_bit_get(gpio_tab[ch].port, gpio_tab[ch].pin);
    }
    return 0;
}

uint32_t bsp_gpio_pin_toggle(uint8_t ch)
{
    if( ch < ARRY_ITEMS_NUM(gpio_tab) )
    {
        gpio_bit_toggle(gpio_tab[ch].port, gpio_tab[ch].pin);
    }
    return 0;
}


#if 1
int32_t set_gpio(uint8_t argc, uint8_t *argv[])
{
    if(argc >= 3)
    {
        uint32_t ch = 0;
        uint32_t val = 0;

        sscanf((const char *)argv[1], "%d", &ch);
        sscanf((const char *)argv[2], "%d", &val);

        bsp_gpio_write((uint8_t)ch, val);
        DBG_I("bsp_gpio_write index:%d val:%d success!!", ch, val);
    }
    else
    {
        DBG_W("warn the bsp_gpio_write param is error: eg:bsp_gpio_write ch val");
    }
    return RET_OK;
}

REG_SHELL_CMD(set_gpio, 0, set_gpio, "set_gpio", "eg:set_gpio ch val\r\n ");
#endif




