#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "gd32e10x.h"                  // Device header
#include "x_frame.h"
#include "kfifo.h"

#define DEFAULT_BUF_SIZE    128
#define DEF_STOP            USART_STB_1BIT             
#define DEF_CHK             USART_PM_NONE  
		
#define UART0_IRQ  					    5							
#define UART0_TX_BUF_SIZE  	            2048	//use PROTOCOL_EF02_PLUS >  1024
#define UART0_RX_BUF_SIZE  	            1024

#define UART1_IRQ  					    6
#define UART1_TX_BUF_SIZE  	            256					
#define UART1_RX_BUF_SIZE  	            DEFAULT_BUF_SIZE

#define UART2_IRQ  					    9
#define UART2_TX_BUF_SIZE  	            256					
#define UART2_RX_BUF_SIZE  	            DEFAULT_BUF_SIZE	

#define UART3_IRQ  					    4
#define UART3_TX_BUF_SIZE  	            DEFAULT_BUF_SIZE
#define UART3_RX_BUF_SIZE  	            DEFAULT_BUF_SIZE

#define UART4_IRQ  					    8
#define UART4_TX_BUF_SIZE  	            DEFAULT_BUF_SIZE
#define UART4_RX_BUF_SIZE  	            DEFAULT_BUF_SIZE

void bsp_uart_init          (uint8_t uart_port);
int32_t bsp_uart_send       (uint8_t uart_port,uint8_t* pbuf,int32_t len);
int32_t bsp_uart_read		(uint8_t uart_port,uint8_t* p_dest,int32_t len);
void  bsp_uart_set_mode		(uint8_t uart_port,uint32_t mode);
void bsp_uart_set(uint8_t uart_port,uint32_t baud, uint32_t data_bits,uint8_t is_wait_mod);

#endif //__BSP_UART_H__

