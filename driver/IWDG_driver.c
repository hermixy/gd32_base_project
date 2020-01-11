#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_IWDG.h"
#include "x_frame.h"

#if ENABLE_IWDG_DRV
static int32_t drv_IWDG_init		(void)
{
   // bsp_IWDG_init();
    return 0;
}



int32_t drv_IWDG_ctrl		(uint32_t ctrl_cmd, uint8_t *pDest, uint32_t len, uint32_t *p_real)
{
	if(ctrl_cmd==RELOAD_IWDT)
	{
		bsp_IWDG_reload();
	}
	
	
    return 0;
}
REG_DEV(
    DRV_IWDG,                        /*driver id         */
    DEV_AUTO_INIT,                   /*driver auto init  */
    drv_IWDG_init,                   /*driver init func  */
    NULL,                            /*driver write func */
    NULL,                            /*driver read  func */
    drv_IWDG_ctrl,                   /*driver ctrl  func */
    NULL                             /*driver close func */
);
#endif



