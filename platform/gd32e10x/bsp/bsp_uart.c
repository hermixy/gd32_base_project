#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "bsp_uart.h"

//#define ENABLE_UARTS1_REMAP 0

typedef struct
{
	//uart-clk      tx-pin  rx-pin  def_baud irq_num  tx-buf   rx-buf  
	uint32_t				uart_port;
	uint32_t				uart_clk;
	uint32_t				uart_def_baud;
    uint16_t                uart_data_bits;
    uint16_t                uart_stop_bits;
    uint16_t                uart_check_bits;
	uint16_t				uart_irq_num;
    IRQn_Type               uart_irq_src;
	uint32_t		    	uart_tx_port;
    uint32_t                uart_tx_port_clk;
	uint16_t				uart_tx_pin;
	uint32_t		    	uart_rx_port;
    uint32_t                uart_rx_port_clk;
	uint16_t				uart_rx_pin;
	uint8_t*				uart_tx_buf;
	uint8_t*				uart_rx_buf;
	kfifo_t					uart_tx_fifo;
	kfifo_t					uart_rx_fifo;
	volatile uint8_t        flag_is_in_sending;
	uint8_t 				send_char;
	uint8_t 				recv_char;
    
}uart_init_tab_t;

#define ADD_UART_PORT(port,baud,stop,chk,irq,irq_src,tx_port,tx_pin,rx_port,rx_pin,tx_buf,tx_buf_size,rx_buf,rx_buf_size) \
				{port,					\
				RCU_##port,				\
				baud,					\
				USART_WL_8BIT,			\
				stop,					\
				chk,					\
				irq,					\
				irq_src,						\
				tx_port,						\
				RCU_##tx_port,		\
				tx_pin,rx_port,					\
				RCU_##rx_port,		\
				rx_pin,							\
				tx_buf,							\
				rx_buf,							\
				{NULL,tx_buf_size,tx_buf,0,0},	\
				{NULL,rx_buf_size,rx_buf,0,0}}

#if ENABLE_UART_0_DRV
uint8_t uart0_tx_buf[UART0_TX_BUF_SIZE];
uint8_t uart0_rx_buf[UART0_RX_BUF_SIZE];
#else
uint8_t uart0_tx_buf[1];
uint8_t uart0_rx_buf[1];
#endif
#if ENABLE_UART_1_DRV
uint8_t uart1_tx_buf[UART1_TX_BUF_SIZE];	
uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
#else
uint8_t uart1_tx_buf[1];//±ÜÃâÕ¼ÓÃ¹ý¶àÄÚ´æ
uint8_t uart1_rx_buf[1];
#endif
#if ENABLE_UART_2_DRV				
uint8_t uart2_tx_buf[UART2_TX_BUF_SIZE];	
uint8_t uart2_rx_buf[UART2_RX_BUF_SIZE];
#else
uint8_t uart2_tx_buf[1];//±ÜÃâÕ¼ÓÃ¹ý¶àÄÚ´æ
uint8_t uart2_rx_buf[1];
#endif
#if ENABLE_UART_3_DRV
uint8_t uart3_tx_buf[UART3_TX_BUF_SIZE];	
uint8_t uart3_rx_buf[UART3_RX_BUF_SIZE];
#else
uint8_t uart3_tx_buf[1];//±ÜÃâÕ¼ÓÃ¹ý¶àÄÚ´æ
uint8_t uart3_rx_buf[1];
#endif
#if ENABLE_UART_4_DRV				
uint8_t uart4_tx_buf[UART4_TX_BUF_SIZE];	
uint8_t uart4_rx_buf[UART4_RX_BUF_SIZE];
#else
uint8_t uart4_tx_buf[1];//±ÜÃâÕ¼ÓÃ¹ý¶àÄÚ´æ
uint8_t uart4_rx_buf[1];
#endif

#define EN_UART0_REMAP	1

//Ôö¼Ó´®¿ÚÇë°´Ë³ÐòÔö¼Ó£¬·ñÔòÐèÒªÐÞ¸ÄUARTx_IRQHandlerº¯Êý£¬ÒÔ¼°¶ÔÓ¦drv²ã
uart_init_tab_t g_uart_init_tab[]=
{
	#if EN_UART0_REMAP
		ADD_UART_PORT(USART0,115200 ,DEF_STOP  ,DEF_CHK    ,UART0_IRQ,USART0_IRQn,GPIOB,GPIO_PIN_6 ,GPIOB,GPIO_PIN_7  ,uart0_tx_buf,UART0_TX_BUF_SIZE,uart0_rx_buf,UART0_RX_BUF_SIZE),	//bms <-> front
	#else
		ADD_UART_PORT(USART0,115200 ,DEF_STOP  ,DEF_CHK    ,UART0_IRQ,USART0_IRQn,GPIOA,GPIO_PIN_9 ,GPIOA,GPIO_PIN_10  ,uart0_tx_buf,UART0_TX_BUF_SIZE,uart0_rx_buf,UART0_RX_BUF_SIZE),	//bms <-> front
	#endif
	
	ADD_UART_PORT(USART1,115200 ,DEF_STOP  ,DEF_CHK    ,UART1_IRQ,USART1_IRQn,GPIOA,GPIO_PIN_2 ,GPIOA,GPIO_PIN_3   ,uart1_tx_buf,UART1_TX_BUF_SIZE, uart1_rx_buf,UART1_RX_BUF_SIZE),	//Î´ÓÃ
	ADD_UART_PORT(USART2,115200 ,DEF_STOP  ,DEF_CHK    ,UART2_IRQ,USART2_IRQn,GPIOB,GPIO_PIN_10,GPIOB,GPIO_PIN_11  ,uart2_tx_buf,UART2_TX_BUF_SIZE, uart2_rx_buf,UART2_RX_BUF_SIZE),	//Î´ÓÃ
	ADD_UART_PORT(UART3, 115200 ,DEF_STOP  ,DEF_CHK	   ,UART3_IRQ,UART3_IRQn,GPIOC,GPIO_PIN_10,GPIOC,GPIO_PIN_11   ,uart3_tx_buf,UART3_TX_BUF_SIZE, uart3_rx_buf,UART3_RX_BUF_SIZE),	//Î´ÓÃ
	ADD_UART_PORT(UART4, 115200 ,DEF_STOP  ,DEF_CHK	   ,UART4_IRQ,UART4_IRQn,GPIOC,GPIO_PIN_12,GPIOD,GPIO_PIN_2    ,uart4_tx_buf,UART4_TX_BUF_SIZE, uart4_rx_buf,UART4_RX_BUF_SIZE),	//Î´ÓÃ
};

//=============================================
static uint32_t send_mode[5] = {0,0,0,0,0};	//1:is_wait_mod   0:interrupt


static void bsp_uart_config (uint8_t uart_port,uint32_t baud, uint32_t data_bits,uint32_t stop_bit,uint32_t chk_bit,uint8_t mode)
{

	if(uart_port>=ARRY_ITEMS_NUM(g_uart_init_tab))
	{
		DBG_E("bsp_uart_init err! uart_port:%d",uart_port);
		return ;
	}

	rcu_periph_clock_enable(RCU_AF);

	#if EN_UART0_REMAP
		/*configure dubug GPIO pin remap*/
		gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);
	#endif
	
    /* enable USART, GPIO clock */
    rcu_periph_clock_enable((rcu_periph_enum)g_uart_init_tab[uart_port].uart_tx_port_clk);
    rcu_periph_clock_enable((rcu_periph_enum)g_uart_init_tab[uart_port].uart_rx_port_clk);
    rcu_periph_clock_enable((rcu_periph_enum)g_uart_init_tab[uart_port].uart_clk);



   // nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(g_uart_init_tab[uart_port].uart_irq_src, 0, 0);
   

    
    /* connect port to USARTx_Tx */
    gpio_init(g_uart_init_tab[uart_port].uart_tx_port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, g_uart_init_tab[uart_port].uart_tx_pin);
    /* connect port to USARTx_Rx */
    gpio_init(g_uart_init_tab[uart_port].uart_rx_port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, g_uart_init_tab[uart_port].uart_rx_pin);

    
    /* USARTx baudrate configuration */
    usart_baudrate_set(g_uart_init_tab[uart_port].uart_port, baud);
 
    
    /* configure USART word length */
    usart_word_length_set(g_uart_init_tab[uart_port].uart_port, data_bits);

    
    /* configure USART stop bits */
    usart_stop_bit_set(g_uart_init_tab[uart_port].uart_port, stop_bit);

    
    /* configure USART transmitter */
    usart_transmit_config(g_uart_init_tab[uart_port].uart_port, USART_TRANSMIT_ENABLE);

    
    /* configure USART receiver */
    usart_receive_config(g_uart_init_tab[uart_port].uart_port, USART_RECEIVE_ENABLE);

    
    /* enable USART */
    usart_enable(g_uart_init_tab[uart_port].uart_port);
  
    /* enable the USART interrupt */    
    if(mode==0)
    {
        usart_interrupt_enable(g_uart_init_tab[uart_port].uart_port, USART_INT_RBNE);	//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ú½ï¿½ï¿½ï¿½ï¿½Ð¶ï¿½
        usart_interrupt_enable(g_uart_init_tab[uart_port].uart_port, USART_INT_PERR);
        usart_interrupt_enable(g_uart_init_tab[uart_port].uart_port, USART_INT_ERR);
    }
    else
    {
        usart_interrupt_disable(g_uart_init_tab[uart_port].uart_port, USART_INT_TBE);
        usart_interrupt_disable(g_uart_init_tab[uart_port].uart_port, USART_INT_RBNE);
        usart_interrupt_disable(g_uart_init_tab[uart_port].uart_port, USART_INT_PERR);
        usart_interrupt_disable(g_uart_init_tab[uart_port].uart_port, USART_INT_ERR);
    }

}

void bsp_uart_set(uint8_t uart_port,uint32_t baud, uint32_t data_bits,uint8_t is_wait_mod)
{
    bsp_uart_config(uart_port,baud,
                              data_bits,
                              g_uart_init_tab[uart_port].uart_stop_bits,
                              g_uart_init_tab[uart_port].uart_check_bits,
                              is_wait_mod  //Ä¬ÈÏÖÐ¶ÏÄ£Ê½
                            );
}


void bsp_uart_init(uint8_t uart_port)
{
	//bsp_uart_config(uint8_t uart_port,uint32_t baud, uint32_t data_bits,uint32_t stop_bit,uint8_t mode)
    bsp_uart_config(uart_port,g_uart_init_tab[uart_port].uart_def_baud,
                              g_uart_init_tab[uart_port].uart_data_bits,
                              g_uart_init_tab[uart_port].uart_stop_bits,
                              g_uart_init_tab[uart_port].uart_check_bits,
                              0  //Ä¬ÈÏÖÐ¶ÏÄ£Ê½
                            );
}


int32_t bsp_uart_send(uint8_t uart_port,uint8_t* pbuf,int32_t len)
{
    int32_t real_len = 0;
	//if(len==0){return 0;}
    
    if(uart_port>=ARRY_ITEMS_NUM(g_uart_init_tab))
	{
		DBG_E("bsp_uart_send err!:%d",uart_port);
		return 0;
	}
    
	if(send_mode[uart_port] == 1)
	{
		uint16_t i=0;
		for(i=0; i<len;i++)
		{
			usart_data_transmit(g_uart_init_tab[uart_port].uart_port, pbuf[i]);
			while(usart_flag_get(g_uart_init_tab[uart_port].uart_port, USART_FLAG_TC) != SET);
		}
	}
	else
	{
		//USART_ITConfig( g_uart_init_tab[uart_port].uart_port, USART_IT_TXE, DISABLE);
		real_len = kfifo_push_in(&g_uart_init_tab[uart_port].uart_tx_fifo,pbuf,len);

		if(g_uart_init_tab[uart_port].flag_is_in_sending==0)
		{
			//kfifo_pull_out(&g_uart_init_tab[uart_port].uart_tx_fifo,&g_uart_init_tab[uart_port].send_char,1);
			//USART_SendData(g_uart_init_tab[uart_port].uart_port, g_uart_init_tab[uart_port].send_char);
			g_uart_init_tab[uart_port].flag_is_in_sending=1;
			usart_interrupt_enable(g_uart_init_tab[uart_port].uart_port, USART_INT_TBE);
		}
	}
	return real_len;
}

int32_t bsp_uart_read		(uint8_t uart_port,uint8_t* p_dest,int32_t len	)
{
	int32_t real_len = 0;
    if(uart_port>=ARRY_ITEMS_NUM(g_uart_init_tab))
	{
		DBG_E("bsp_uart_read err!:%d",uart_port);
		return 0;
	}
	real_len = kfifo_pull_out(&g_uart_init_tab[uart_port].uart_rx_fifo,p_dest,len);
	return real_len;
}


void  bsp_uart_set_mode		(uint8_t uart_port,uint32_t mode)
{
	send_mode[uart_port] = mode;
    bsp_uart_config(uart_port,g_uart_init_tab[uart_port].uart_def_baud,
                              g_uart_init_tab[uart_port].uart_data_bits,
                              g_uart_init_tab[uart_port].uart_stop_bits,
                              g_uart_init_tab[uart_port].uart_check_bits,
                              mode  //Ä¬ÈÏÖÐ¶ÏÄ£Ê½
                            );
}



uint32_t uart_rev_cnt = 0;

void uart_irq_action(uint8_t uart_port)
{
    if(uart_port>=ARRY_ITEMS_NUM(g_uart_init_tab))
	{
		DBG_E("uart_irq_action err!:%d",uart_port);
		return ;
	}
    
    
    if( usart_interrupt_flag_get(g_uart_init_tab[uart_port].uart_port, USART_INT_FLAG_TBE) != RESET)
    {
		//if(USART_GetFlagStatus(g_uart_init_tab[uart_port].uart_port, USART_FLAG_TC) == SET)
		{
			if(kfifo_pull_out(&g_uart_init_tab[uart_port].uart_tx_fifo,&g_uart_init_tab[uart_port].send_char,1))
			{
				usart_data_transmit(g_uart_init_tab[uart_port].uart_port, g_uart_init_tab[uart_port].send_char);				
			}
			else
			{
				g_uart_init_tab[uart_port].flag_is_in_sending=0;
				usart_interrupt_disable(g_uart_init_tab[uart_port].uart_port, USART_INT_TBE);
			}
		}
		usart_interrupt_flag_clear(g_uart_init_tab[uart_port].uart_port, USART_INT_FLAG_TBE);
    }
	
	
	if(usart_interrupt_flag_get(g_uart_init_tab[uart_port].uart_port, USART_INT_FLAG_RBNE) != RESET)
	{
		usart_interrupt_flag_clear(g_uart_init_tab[uart_port].uart_port, USART_INT_FLAG_RBNE);
		g_uart_init_tab[uart_port].recv_char= usart_data_receive(g_uart_init_tab[uart_port].uart_port);
		kfifo_push_in(&g_uart_init_tab[uart_port].uart_rx_fifo,&g_uart_init_tab[uart_port].recv_char,1);

		//uart_rev_cnt++;
		//DBG_I("[%d]0x%x",uart_port,g_uart_init_tab[uart_port].recv_char);
	}


	 if (usart_flag_get(g_uart_init_tab[uart_port].uart_port, USART_FLAG_ORERR) == SET)  
	{
	    //USART_ClearITPendingBit(g_uart_init_tab[uart_port].uart_port,USART_IT_ORE);  
        g_uart_init_tab[uart_port].recv_char= usart_data_receive(g_uart_init_tab[uart_port].uart_port);
		kfifo_push_in(&g_uart_init_tab[uart_port].uart_rx_fifo,&g_uart_init_tab[uart_port].recv_char,1);
		usart_flag_clear(g_uart_init_tab[uart_port].uart_port,USART_FLAG_ORERR);  //¶ÁSR
	}
    
    if(usart_flag_get(g_uart_init_tab[uart_port].uart_port, USART_FLAG_PERR) != RESET)
    {//?  @arg USART_IT_PE     : Parity Error interrupt
        usart_flag_clear(g_uart_init_tab[uart_port].uart_port, USART_FLAG_PERR);
    }
    
    if(usart_flag_get(g_uart_init_tab[uart_port].uart_port, USART_FLAG_FERR) != RESET)
    {//?  @arg USART_IT_FE     : Frame Error interrupt
        usart_flag_clear(g_uart_init_tab[uart_port].uart_port, USART_FLAG_FERR);
    }
    
    if(usart_flag_get(g_uart_init_tab[uart_port].uart_port, USART_FLAG_NERR) != RESET)
    {//?  @arg USART_IT_NE     : Noise Error interrupt
        usart_flag_clear(g_uart_init_tab[uart_port].uart_port, USART_FLAG_NERR);
    }
    
    
//    if (USART_GetITStatus(g_uart_init_tab[uart_port].uart_port, USART_IT_TC) == SET)  
//	{  
//	    USART_ClearITPendingBit(g_uart_init_tab[uart_port].uart_port,USART_IT_TC);      
//	}
}

void USART0_IRQHandler(void)
{
    uart_irq_action(0);
}

void USART1_IRQHandler(void)
{
    uart_irq_action(1);
}

void USART2_IRQHandler(void)
{
    uart_irq_action(2);
}

void UART3_IRQHandler(void)
{
    uart_irq_action(3);
}

void UART4_IRQHandler(void)
{
    uart_irq_action(4);
}

