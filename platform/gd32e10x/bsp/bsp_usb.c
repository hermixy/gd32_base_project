#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_usb.h"
#include "x_frame.h"

#include "usb_lib.h"
#include "usb_pwr.h"
#include "usb_istr.h"


#define USB_FIFO_SIZE  512
uint8_t usb_tx_buf[USB_FIFO_SIZE]={0};	
uint8_t usb_rx_buf[USB_FIFO_SIZE]={0};	
static kfifo_t					usb_tx_fifo={"usb_tx_fifo",USB_FIFO_SIZE,usb_tx_buf,0,0};
static kfifo_t					usb_rx_fifo={"usb_rx_fifo",USB_FIFO_SIZE,usb_rx_buf,0,0};


static void usb_hw_init(void);
static void usb_func_init(void);

void bsp_usb_init           (void)
{
    usb_hw_init();  
    usb_func_init();
    DBG_I("init usb!!----------------");
}

int32_t bsp_usb_write       (uint8_t* pbuf,int32_t len)
{
    return kfifo_push_in(&usb_tx_fifo,pbuf,len);
}

int32_t bsp_usb_read		(uint8_t* p_dest,int32_t len)
{
    return kfifo_pull_out(&usb_rx_fifo,p_dest,len);
}

void  bsp_usb_set_mode		(uint32_t mode)
{
    
}


//RET_TYPE usb_r(uint8_t argc, uint8_t *argv[])
//{
//    uint8_t read_temp[128]={0};
//    uint32_t read_len = bsp_usb_read(0,read_temp,128);
//    DBG_I("read:[%d]",read_len);
//    show_buf(read_temp,read_len);
//	return RET_OK;
//}

//REG_SHELL_CMD(usb_r		    ,0,usb_r	,"usb_r"		, "usb_r ");


//给 usb irq 驱动增加 fifo读取和写入接口
uint32_t   _bsp_usb_w_rx_fifo(uint8_t* pbuf,uint32_t len)
{
    return kfifo_push_in(&usb_rx_fifo,pbuf,len);
}

uint32_t   _bsp_usb_r_tx_fifo(uint8_t* pbuf,uint32_t len)
{
    return kfifo_pull_out(&usb_tx_fifo,pbuf,len);
}

//--------------inter  bsp
static void usb_plug_in(uint8_t is_plug_in);
static void usb_hw_init(void)
{
    //clk
    GPIO_InitTypeDef	GPIO_InitStructure;
    EXTI_InitTypeDef 	EXTI_InitStructure;
    
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); 
    
    
    usb_plug_in(0);
    
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    
    EXTI_ClearITPendingBit(EXTI_Line18);
    EXTI_InitStructure.EXTI_Line 	= EXTI_Line18;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    //gpio  如果有控制usb 链接和端口的IO需要也初始化
    DBG_I("usb_hw_init!!!!!!!!");
}

static void USB_IRQ_Config(void);
static void usb_func_init(void)
{
    //usb clk
    /* USBCLK = PLLCLK = 48 MHz */
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

    /* Enable USB clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
    
    //usb interrupt 
    USB_IRQ_Config();
    
    //usb core init
    USB_Init();
}


static void USB_IRQ_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Enable the USB interrupt */

  NVIC_InitStructure.NVIC_IRQChannel					= USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority 		= 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd 				= ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
    
  /* Enable the USB Wake-up interrupt */
  NVIC_InitStructure.NVIC_IRQChannel 					= USBWakeUp_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd 				= ENABLE;
  NVIC_Init(&NVIC_InitStructure);      
}


static void usb_delay(uint32_t cnt)
{
    volatile uint32_t i = 0;
    for(i=0;i<cnt;i++);
}
static void usb_plug_in(uint8_t is_plug_in)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    /* Pull down PA12 to create USB disconnect pulse */

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); 

    GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_OD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_12);
    usb_delay(100000);
    GPIO_SetBits(GPIOA, GPIO_Pin_12);
}


//-------------------------IRQ action


void USB_LP_CAN1_RX0_IRQHandler(void)
{
   USB_Istr();
}


void USBWakeUp_IRQHandler(void)
{
  /* Initiate external resume sequence (1 step) */
  Resume(RESUME_EXTERNAL);  
  EXTI_ClearITPendingBit(EXTI_Line18);
}




