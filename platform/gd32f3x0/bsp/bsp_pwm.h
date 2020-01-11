#ifndef __BSP_PWM_H__
#define __BSP_PWM_H__

#include "x_frame.h"
#include MCU_HEADER        // Device header


void bsp_pwm_init(void);
int32_t bsp_pwm_write        (uint8_t pwm_ch,uint32_t val);

#endif //__BSP_PWM_H__
