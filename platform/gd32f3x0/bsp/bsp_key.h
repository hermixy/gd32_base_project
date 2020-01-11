#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "gd32e10x.h"
#include "x_frame.h"

void bsp_key_init(void);
uint32_t bsp_key_read(uint8_t ch);

#endif //__BSP_KEY_H__
