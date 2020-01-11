#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
//#include "debug.h"
#include "x_frame.h"

#include "bsp_systick.h"

#if ENABLE_SYS_TICK_DRV
static int32_t systick_init		(void)
{
	bsp_sys_tick_init();
	return RET_OK;
}

REG_DEV(    
            DRV_SYSTICK,                /*driver id         */
            DEV_AUTO_INIT,              /*driver auto init  */
            systick_init,                 /*driver init func  */
            NULL,                       /*driver write func */
            NULL,                       /*driver read  func */
            NULL,                       /*driver ctrl  func */
            NULL                        /*driver close func */
        );  
#endif









