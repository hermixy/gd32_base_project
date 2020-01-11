/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Endpoint routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
extern  uint8_t USART_Rx_Buffer[];
extern uint32_t USART_Rx_ptr_out;
extern uint32_t USART_Rx_length;
extern uint8_t  USB_Tx_State;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

extern uint32_t   _bsp_usb_w_rx_fifo(uint8_t* pbuf,uint32_t len);
extern uint32_t   _bsp_usb_r_tx_fifo(uint8_t* pbuf,uint32_t len);

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback (void)
{
    uint8_t send_cnt = 0;
    uint8_t send_cache[VIRTUAL_COM_PORT_DATA_SIZE]={0};  //±ÜÃâ64×Ö½ÚµÄbug
    
    send_cnt = _bsp_usb_r_tx_fifo(send_cache,VIRTUAL_COM_PORT_DATA_SIZE-1);
    
    UserToPMABufferCopy(send_cache,ENDP1_TXADDR,send_cnt);

    SetEPTxCount(ENDP1, send_cnt);
    SetEPTxValid(ENDP1);   
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
	uint16_t USB_Rx_Cnt;

	uint8_t USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];

	/* Get the received data buffer and update the counter */
	USB_Rx_Cnt = USB_SIL_Read(EP3_OUT, USB_Rx_Buffer);

	/* USB data will be immediately processed, this allow next USB traffic being 
	NAKed till the end of the USART Xfer */
	_bsp_usb_w_rx_fifo(USB_Rx_Buffer,USB_Rx_Cnt);

	/* Enable the receive of data on EP3 */
	SetEPRxValid(ENDP3);
}

/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
#ifdef STM32F10X_CL
void INTR_SOFINTR_Callback(void)
#else
void SOF_Callback(void)
#endif /* STM32F10X_CL */
{
  static uint32_t FrameCount = 0;
  
  if(bDeviceState == CONFIGURED)
  {
    if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
    {
      /* Reset the frame counter */
      FrameCount = 0;
      
        EP1_IN_Callback();
      /* Check the data to be sent through IN pipe */
      //Handle_USBAsynchXfer();
    }
  }  
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

