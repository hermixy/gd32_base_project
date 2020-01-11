#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

#include "x_frame.h"
#include MCU_HEADER        // Device header


void bsp_gpio_init(void);
int32_t bsp_gpio_write(uint8_t ch, uint32_t val);
uint32_t bsp_gpio_read(uint8_t ch);
uint32_t bsp_gpio_pin_toggle(uint8_t ch);


#endif //__BSP_GPIO_H__
