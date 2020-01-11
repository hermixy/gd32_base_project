#ifndef __BSP_I2C_HW_H__
#define __BSP_I2C_HW_H__

#include "x_frame.h"
#include MCU_HEADER      // Device header

#define  I2C_ADDRESS7    		0x01

#define TRANS_RECV   0
#define TRANS_SEND   1


void bsp_i2c_hw_init(uint8_t ch);
uint8_t bsp_i2c_hw_read(uint8_t ch,uint8_t addr,uint8_t reg,uint8_t* pbuf, uint8_t len);						
uint8_t bsp_i2c_hw_write(uint8_t ch,uint8_t addr,uint8_t reg,uint8_t* pbuf, uint8_t len);						


void I2C_hw_init(uint8_t ch);
uint8_t I2C_read(uint8_t ch,uint8_t addr,uint8_t reg,uint8_t* pbuf, uint8_t len);						
uint8_t I2C_write(uint8_t ch,uint8_t addr,uint8_t reg,uint8_t* pbuf, uint8_t len);		
						
#endif //__BSP_I2C_DMA_H__
