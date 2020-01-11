#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H
#include "stm32f10x.h"
#include "usb_type.h" 

#define USART_RX_DATA_SIZE   2048


void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Cable_Config (FunctionalState NewState);
void IntToUnicode (uint32_t value,uint8_t *pbuf,uint8_t len);
void Get_SerialNum(void);

#endif  
























