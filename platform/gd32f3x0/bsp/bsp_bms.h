#ifndef __BSP_BMS_H__
#define __BSP_BMS_H__

#include "gd32e10x.h"
#include "x_frame.h"

void bsp_bms_init(void);
int32_t bsp_bms_read		(uint8_t *p_dest, uint8_t len);
#endif //__BSP_UART_H__
