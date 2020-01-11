#ifndef __BSP_USB_H__
#define __BSP_USB_H__

#include "gd32e10x.h"
#include "x_frame.h"

void bsp_usb_init           (void);
int32_t bsp_usb_write       (uint8_t* pbuf,int32_t len);
int32_t bsp_usb_read		(uint8_t* p_dest,int32_t len);
void  bsp_usb_set_mode		(uint32_t mode);

#endif //__BSP_USB_H__
