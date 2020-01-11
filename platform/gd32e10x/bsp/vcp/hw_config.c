#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_istr.h"
#include "hw_config.h"
#include "usb_pwr.h"


ErrorStatus HSEStartUpStatus;
EXTI_InitTypeDef EXTI_InitStructure;
USART_InitTypeDef USART_InitStructure;

uint8_t  USART_Rx_Buffer [USART_RX_DATA_SIZE]; 
uint32_t USART_Rx_ptr_in = 0;
uint32_t USART_Rx_ptr_out = 0;
uint32_t USART_Rx_length  = 0;

uint8_t  USB_Tx_State = 0;
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Extern variables ----------------------------------------------------------*/
extern __IO uint8_t PrevXferComplete;
extern LINE_CODING linecoding;

/* Private function prototypes -----------------------------------------------*/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Private functions ---------------------------------------------------------*/


void Enter_LowPowerMode(void)
{
	bDeviceState=SUSPENDED;
}

void Leave_LowPowerMode(void)
{
	DEVICE_INFO *pInfo=&Device_Info;
	if (pInfo->Current_Configuration!=0)bDeviceState=CONFIGURED; 
	else bDeviceState = ATTACHED; 
} 
	

void USB_Cable_Config (FunctionalState NewState)
{ 

}


void Get_SerialNum(void)
{
	uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
	Device_Serial0 = *(uint32_t*)(0x1FFFF7E8);
	Device_Serial1 = *(uint32_t*)(0x1FFFF7EC);
	Device_Serial2 = *(uint32_t*)(0x1FFFF7F0);
	Device_Serial0 += Device_Serial2;
	if (Device_Serial0 != 0)
	{
		IntToUnicode (Device_Serial0,&Virtual_Com_Port_StringSerial[2] , 8);
		IntToUnicode (Device_Serial1,&Virtual_Com_Port_StringSerial[18], 4);
	}
} 

void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
	uint8_t idx = 0;
	for( idx = 0 ; idx < len ; idx ++)
	{
		if( ((value >> 28)) < 0xA )
		{
			pbuf[ 2* idx] = (value >> 28) + '0';
		}
		else
		{
			pbuf[2* idx] = (value >> 28) + 'A' - 10; 
		} 
		value = value << 4; 
		pbuf[ 2* idx + 1] = 0;
	}
}
