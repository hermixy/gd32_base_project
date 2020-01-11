#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

#include "gd32e10x.h"
#include "x_frame.h"

void bsp_gpio_init(void);
int32_t bsp_gpio_write(uint8_t ch, uint32_t val);
uint32_t bsp_gpio_read(uint8_t ch);
uint32_t bsp_gpio_change_mode(uint8_t ch,uint8_t mode);


#endif //__BSP_GPIO_H__
