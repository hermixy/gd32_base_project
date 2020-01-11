#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "x_frame.h"

#include "bsp_gpio.h"


typedef struct
{
    uint32_t            port;
	uint32_t            rcu;
    uint32_t            pin;
    uint8_t             mode;
	uint8_t				speed;
    uint8_t             default_io_level;
} gpio_cfg_t;


#define ADD_GPIO_ITEM(port,pin,mode,speed,default_level)  \
					{   	port,                   \
							RCU_##port,      \
							GPIO_PIN_##pin,         \
							mode,						\
							speed,						\
							default_level,				\
					}

/*!
    \brief      GPIO parameter initialization table
    \param[in]  port: GPIOx(x = A,B,C,D,E) 
    
    \param[in]  pin: GPIO pin
                one or more parameters can be selected which are shown as below:
      \arg        GPIO_PIN_x(x=0..15), GPIO_PIN_ALL    
    \param[in]  mode: gpio pin mode
                only one parameter can be selected which is shown as below:
      \arg        GPIO_MODE_AIN: analog input mode
      \arg        GPIO_MODE_IN_FLOATING: floating input mode
      \arg        GPIO_MODE_IPD: pull-down input mode
      \arg        GPIO_MODE_IPU: pull-up input mode
      \arg        GPIO_MODE_OUT_OD: GPIO output with open-drain
      \arg        GPIO_MODE_OUT_PP: GPIO output with push-pull
    \param[in]  speed: gpio output max speed value
                only one parameter can be selected which is shown as below:
      \arg        GPIO_OSPEED_10MHZ: output max speed 10MHz
      \arg        GPIO_OSPEED_2MHZ: output max speed 2MHz
      \arg        GPIO_OSPEED_50MHZ: output max speed 50MHz
      \arg        GPIO_OSPEED_MAX: output max speed more than 50MHz
    \param[in]  default bit_value: SET or RESET
      \arg        RESET: clear the port pin
      \arg        SET: set the port pin
*/
const gpio_cfg_t gpio_tab[] =
{
	//-----------auto_test
	ADD_GPIO_ITEM(GPIOC, 13,  GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,		SET),	//red_led
	ADD_GPIO_ITEM(GPIOC, 14,  GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,		RESET),	//blue_led
	ADD_GPIO_ITEM(GPIOC, 15,  GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,		SET),	//green_led
	ADD_GPIO_ITEM(GPIOB, 15,  GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,		RESET),	//beep  0:close
	
	//ADD_GPIO_ITEM(GPIOA, 10,  GPIO_MODE_IPU, 	GPIO_OSPEED_50MHZ,		SET),	//key
};


void bsp_gpio_init(void)
{
	uint32_t i = 0;

	/*configure dubug GPIO pin remap*/
	//gpio_pin_remap_config(GPIO_SWJ_DISABLE_REMAP, DISABLE);
	rcu_periph_clock_enable(RCU_AF);
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
    for(i = 0; i < ARRY_ITEMS_NUM(gpio_tab); i++)
    {
    	/* enable the led clock */
    	rcu_periph_clock_enable((rcu_periph_enum) gpio_tab[i].rcu);
		/* configure led GPIO port */ 
		gpio_init(gpio_tab[i].port, gpio_tab[i].mode, gpio_tab[i].speed ,gpio_tab[i].pin);
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

uint32_t bsp_gpio_change_mode(uint8_t ch,uint8_t mode)
{
    if( ch < ARRY_ITEMS_NUM(gpio_tab) )
    {
        //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
       // DBG_I("-------------------GPIO_Mode_IN_FLOATING ------ch:%d",ch);
		gpio_init(gpio_tab[ch].port, mode, gpio_tab[ch].speed, gpio_tab[ch].pin);
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




