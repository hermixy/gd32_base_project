#ifndef __BSP_EXIT_H__
#define __BSP_EXIT_H__

#include "gd32e10x.h"
#include "x_frame.h"

void bsp_exit_init(void);
uint32_t bsp_exit_read(uint8_t ch);

#ifndef NULL
#define NULL	(void*) 0
#endif

//typedef void (*EXIT_CallbackType)(void);	//外部中断函数指针
//void set_exit_callback(uint8_t ch, EXIT_CallbackType pfnCallback);

#endif //__BSP_EXIT_H__
