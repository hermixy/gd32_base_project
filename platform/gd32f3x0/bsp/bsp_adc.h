#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#include "x_frame.h"
#include MCU_HEADER        // Device header

void bsp_adc_init(void);
uint32_t bsp_adc_read(uint8_t *pSrc, uint32_t len, uint32_t offset);

#endif //__BSP_ADC_H__
