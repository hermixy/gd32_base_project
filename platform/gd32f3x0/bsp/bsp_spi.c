#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_spi.h"


#define ARRAYSIZE         8*5

#define WS2812_L		0xC0
#define WS2812_H		0xFC

// g&r&b
#define WS2812_G	1
#define WS2812_R	2
#define WS2812_B	3
#define WS2812_GB	4
#define WS2812_GR	5
#define WS2812_RB	6
#define WS2812_GRB	7

//#define CRU_WS2812_COLOR	WS2812_RB

#if CRU_WS2812_COLOR==WS2812_G
uint8_t spi2_send_array[ARRAYSIZE] = {	WS2812_H,WS2812_H,WS2812_H,WS2812_H,  WS2812_H,WS2812_H,WS2812_H,WS2812_H, \
										WS2812_L,WS2812_L,WS2812_L,WS2812_L,  WS2812_L,WS2812_L,WS2812_L,WS2812_L, \
										WS2812_L,WS2812_L,WS2812_L,WS2812_L,  WS2812_L,WS2812_L,WS2812_L,WS2812_L, \
};
#elif CRU_WS2812_COLOR==WS2812_R
uint8_t spi2_send_array[ARRAYSIZE] = {	WS2812_L,WS2812_L,WS2812_L,WS2812_L,  WS2812_L,WS2812_L,WS2812_L,WS2812_L, \
										WS2812_H,WS2812_H,WS2812_H,WS2812_H,  WS2812_H,WS2812_H,WS2812_H,WS2812_H, \
										WS2812_L,WS2812_L,WS2812_L,WS2812_L,  WS2812_L,WS2812_L,WS2812_L,WS2812_L, \
};
#elif CRU_WS2812_COLOR==WS2812_B
uint8_t spi2_send_array[ARRAYSIZE] = {	WS2812_L,WS2812_L,WS2812_L,WS2812_L,  WS2812_L,WS2812_L,WS2812_L,WS2812_L, \
										WS2812_L,WS2812_L,WS2812_L,WS2812_L,  WS2812_L,WS2812_L,WS2812_L,WS2812_L, \
										WS2812_H,WS2812_H,WS2812_H,WS2812_H,  WS2812_H,WS2812_H,WS2812_H,WS2812_H, \
};
#elif CRU_WS2812_COLOR==WS2812_GB
uint8_t spi2_send_array[ARRAYSIZE] = {	WS2812_H,WS2812_H,WS2812_H,WS2812_H,  WS2812_H,WS2812_H,WS2812_H,WS2812_H, \
										WS2812_L,WS2812_L,WS2812_L,WS2812_L,  WS2812_L,WS2812_L,WS2812_L,WS2812_L, \
										WS2812_H,WS2812_H,WS2812_H,WS2812_H,  WS2812_H,WS2812_H,WS2812_H,WS2812_H, \
};
#elif CRU_WS2812_COLOR==WS2812_GR
uint8_t spi2_send_array[ARRAYSIZE] = {	WS2812_H,WS2812_H,WS2812_H,WS2812_H,  WS2812_H,WS2812_H,WS2812_H,WS2812_H, \
										WS2812_H,WS2812_H,WS2812_H,WS2812_H,  WS2812_H,WS2812_H,WS2812_H,WS2812_H, \
										WS2812_L,WS2812_L,WS2812_L,WS2812_L,  WS2812_L,WS2812_L,WS2812_L,WS2812_L, \
};
#elif CRU_WS2812_COLOR==WS2812_RB
uint8_t spi2_send_array[ARRAYSIZE] = {	WS2812_L,WS2812_L,WS2812_L,WS2812_L,  WS2812_L,WS2812_L,WS2812_L,WS2812_L, \
										WS2812_H,WS2812_H,WS2812_H,WS2812_H,  WS2812_H,WS2812_H,WS2812_H,WS2812_H, \
										WS2812_H,WS2812_H,WS2812_H,WS2812_H,  WS2812_H,WS2812_H,WS2812_H,WS2812_H, \
};
#elif CRU_WS2812_COLOR==WS2812_RB
uint8_t spi2_send_array[ARRAYSIZE] = {	WS2812_H,WS2812_H,WS2812_H,WS2812_H,  WS2812_H,WS2812_H,WS2812_H,WS2812_H, \
										WS2812_H,WS2812_H,WS2812_H,WS2812_H,  WS2812_H,WS2812_H,WS2812_H,WS2812_H, \
										WS2812_H,WS2812_H,WS2812_H,WS2812_H,  WS2812_H,WS2812_H,WS2812_H,WS2812_H, \
};
#endif


#define MAX_GRB_LED_NUM		10
#define RSV_ARRAY_SIZE		280	//byte	333us(reset>280us)
#define MAX_GRB_ARRAY_SIZE	(8*3*MAX_GRB_LED_NUM+RSV_ARRAY_SIZE)	//Ô¤Áô3byte,×÷Îª¸´Î»
uint8_t spi2_send_array[MAX_GRB_ARRAY_SIZE] = {0};

#pragma pack(1) 
typedef struct
{
	uint8_t 	g;
	uint8_t 	r;
	uint8_t 	b;
} ws2812_grb_dat_t;
#pragma pack() 

ws2812_grb_dat_t  ws2812_grb_dat[MAX_GRB_LED_NUM] = {0};


static void rcu_config(void);
static void gpio_config(void);
static void dma_config(void);
static void spi_config(void);

/*
	PB3->SPI2_SCK, PB4->SPI2_MISO, PB5->SPI2_MOSI
*/
static void bsp_spi_init(void)
{
	/* peripheral clock enable */
    rcu_config();
    /* GPIO configure */
    gpio_config();
    /* DMA configure */
    dma_config();
    /* SPI configure */
    spi_config();
    
	
	/* SPI enable */
    spi_enable(SPI2);
	dma_channel_enable(DMA1, DMA_CH1);
	dma_interrupt_enable(DMA1, DMA_CH1, DMA_INT_FTF);
	spi_i2s_interrupt_enable(SPI2, SPI_I2S_INT_TBE);
	
	/* SPI DMA enable */
    spi_dma_enable(SPI2, SPI_DMA_TRANSMIT);
}
REG_INIT_MOD(bsp_spi_init);


void DMA1_Channel1_IRQHandler(void)
{
	if(dma_interrupt_flag_get(DMA1, DMA_CH1, DMA_INT_FLAG_FTF) != RESET)
	{
		dma_interrupt_flag_clear(DMA1, DMA_CH1, DMA_INT_FLAG_FTF);
		
		DBG_I_LIMIT(1000, "DMA1_Channel1_IRQHandler")
	}
}

//void SPI2_IRQHandler(void)
//{
//	if(spi_i2s_interrupt_flag_get(SPI2, SPI_I2S_INT_FLAG_TBE) != RESET)
//	{
//		DBG_I_LIMIT(1000, "SPI2_IRQHandler")
//	}
//}

static void write_ws2812_by_byte(uint8_t *pbuf, uint8_t byte)
{
	uint8_t i = 0;
	uint8_t val = byte;
	
	for(i = 0;i < 8;i++)
	{
		if(val&0x80)	//hi_bit
		{
			pbuf[i] = WS2812_H;
		}
		else
		{
			pbuf[i] = WS2812_L;
		}
		val<<=1;
	}
}
static void write_ws2812_by_grb(uint8_t *pbuf, ws2812_grb_dat_t *p_ws2812_grb_dat)
{
	uint8_t i = 0;
	for(i=0;i<MAX_GRB_LED_NUM;i++)
	{
		write_ws2812_by_byte(&pbuf[24*i], 	 p_ws2812_grb_dat[i].g);	//0		24	48
		write_ws2812_by_byte(&pbuf[24*i+8],  p_ws2812_grb_dat[i].r);	//8		32
		write_ws2812_by_byte(&pbuf[24*i+16], p_ws2812_grb_dat[i].b);	//16	40
	}
	
	memset(&pbuf[MAX_GRB_ARRAY_SIZE-RSV_ARRAY_SIZE], 0, RSV_ARRAY_SIZE);
}

static void ws2812_spi_test_func(void)
{
	static uint8_t cnt = 0;
	
	cnt++;
	cnt = cnt%2;
	
	if(cnt==0)
	{
		ws2812_grb_dat[0].g = 0xff;
		ws2812_grb_dat[1].r = 0xff;
		ws2812_grb_dat[2].b = 0xff;
		
		//---
		ws2812_grb_dat[3].g = 0xff;
		ws2812_grb_dat[4].r = 0xff;
		ws2812_grb_dat[5].b = 0xff;
		
		//---
		ws2812_grb_dat[6].g = 0xff;
		ws2812_grb_dat[7].r = 0xff;
		ws2812_grb_dat[8].b = 0xff;
		
		ws2812_grb_dat[9].g = 0xff;
	}
	else
	{
		ws2812_grb_dat[0].g = 0x0;
		ws2812_grb_dat[1].r = 0x0;
		ws2812_grb_dat[2].b = 0x0;
		
		//---
		ws2812_grb_dat[3].g = 0x0;
		ws2812_grb_dat[4].r = 0x0;
		ws2812_grb_dat[5].b = 0x0;
		
		//---
		ws2812_grb_dat[6].g = 0x0;
		ws2812_grb_dat[7].r = 0x0;
		ws2812_grb_dat[8].b = 0x0;
		
		ws2812_grb_dat[9].g = 0x0;
	}
	
	//---

	
	write_ws2812_by_grb(&spi2_send_array[0], &ws2812_grb_dat[0]);
	
//	write_ws2812_by_byte(&spi2_send_array[0],  0x20);
//	write_ws2812_by_byte(&spi2_send_array[8],  0x20);
//	write_ws2812_by_byte(&spi2_send_array[16], 0xff);

}
REG_TASK(3, 500, ws2812_spi_test_func);




/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_DMA1);
    rcu_periph_clock_enable(RCU_SPI2);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void gpio_config(void)
{
    /* SPI0 GPIO config:SCK/PA5, MISO/PA6, MOSI/PA7 */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_3|GPIO_PIN_5);
    //gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void dma_config(void)
{
    dma_parameter_struct dma_init_struct;
    
    /* SPI0 transmit dma config:DMA0,DMA_CH2 */
    dma_deinit(DMA1, DMA_CH1);
    dma_struct_para_init(&dma_init_struct);
    
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI2);
    dma_init_struct.memory_addr  = (uint32_t)spi2_send_array;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_LOW;
    dma_init_struct.number       = MAX_GRB_ARRAY_SIZE;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA1, DMA_CH1, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_enable(DMA1, DMA_CH1);
    dma_memory_to_memory_disable(DMA1, DMA_CH1);
	
	#if 0
    /* SPI2 receive dma config:DMA1,DMA_CH0 */
    dma_deinit(DMA1, DMA_CH0);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI2);
    dma_init_struct.memory_addr  = (uint32_t)spi2_receive_array;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA1, DMA_CH0, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA1, DMA_CH0);
    dma_memory_to_memory_disable(DMA1, DMA_CH0);
	#endif
}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi_config(void)
{
    spi_parameter_struct spi_init_struct;
    /* deinitilize SPI and the parameters */
    spi_i2s_deinit(SPI2);
    spi_struct_para_init(&spi_init_struct);

    /* SPI2 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_8;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI2, &spi_init_struct);

}

