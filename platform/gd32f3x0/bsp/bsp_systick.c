#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_systick.h"


void SysTick_Handler(void)
{
    hal_dev_notify(DRV_ID_SYS_TICK,1,NULL,0);
}


void bsp_sys_tick_init(void)
{
	if (SysTick_Config(SystemCoreClock / 1000))	//1ms
	{ 
		/* Capture error */ 
		while (1);
	}
	NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void system_reset(void)
{
    __disable_irq();
    NVIC_SystemReset();  //唤醒系统后应自动重启初始化
}

uint8_t get_cpu_id(uint8_t* pbuf)
{
    
    #define START_ADDR         0x1ffff7e8//0x1ffff7AC(M0) //0x1ffff7e8(M3)   //0x1fff7a10(M4)  //27 00 2d 00 03 51 4d 52 38 34 34 20
    uint8_t i = 0;
	if(pbuf==NULL){return 0;}
    for(i=0; i<STM32_CPUID_BYTES; i++)
    {
        pbuf[i] = *(volatile uint8_t *)(START_ADDR+i);
    }
    DBG_I("cpu id:");
    show_buf(pbuf,STM32_CPUID_BYTES);
	
    return STM32_CPUID_BYTES;
}

int32_t shell_get_cpu_id(uint8_t argc, uint8_t *argv[])
{
	uint8_t cpu_id[STM32_CPUID_BYTES];
	get_cpu_id(cpu_id);
    return RET_OK;
}
REG_SHELL_CMD(cpu_id, 0, shell_get_cpu_id, "shell_get_cpu_id", "shell_get_cpu_id");


#if  1
void jump_to_app(uint32_t app_address)
{
    typedef void (*_func)(void);
    
    __disable_irq();
    
    /* MCU peripherals re-initial. */

    /* reset systick */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    /* disable and clean up all interrupts. */
    {
    	int i;
	    for(i = 0; i < 8; i++)
	    {
	        /* disable interrupts. */
	        NVIC->ICER[i] = 0xFFFFFFFF;

	        /* clean up interrupts flags. */
	        NVIC->ICPR[i] = 0xFFFFFFFF;
	    }

	    /* Set new vector table pointer */
	    SCB->VTOR = app_address;

	    /* reset register values */
	    //__set_BASEPRI(0);
	    __set_FAULTMASK(0);

	    /* set up MSP and switch to it */
	    //__set_PSP(*(uint32_t*)app_address);
	    //__set_CONTROL(0);
	    __set_MSP(*(uint32_t*)app_address);

	    /* ensure what we have done could take effect */
	    //__ISB();
    }

	
    /* never return */
    ((_func)(*(uint32_t*)(app_address + 4)))();
}
#else

typedef  void (*pFunction)(void);

void block_ms_a(void)
{
	volatile uint32_t i ,j= 0;
	for(i=0;i<100;i++)
	{
		for(j=0;j<10000;j++)
		{
			;;;
		}
	}
}

void jump_to_app(uint32_t app_address)
{
    uint32_t JumpAddress;
    pFunction  Jump_To_Application;
    /* Check Vector Table: Test if user code is programmed starting from address 
    "APPLICATION_ADDRESS" */
    //d_printfhex32((*(__IO uint32_t*)APPLICATION_ADDRESS));d_printf("\n");
	
	DBG_I("-----------app_address:0x%x   sp:0x%x",app_address,*(__IO uint32_t*) app_address);
	block_ms_a();
	if (((*(__IO uint32_t*)app_address) & 0x2FFE0000) == 0x20000000)
	{
		__disable_irq();
		JumpAddress = *(__IO uint32_t*) (app_address +4);
		//d_printfhex32(JumpAddress);d_printf("\n");
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
	   // MSR_MSP(*(__IO uint32_t*) app_address);
		__set_MSP(*(__IO uint32_t*) app_address);
		Jump_To_Application();
	}
}
#endif 

