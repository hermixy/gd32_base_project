#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_pwm.h"
#include "x_frame.h"


#if ENABLE_PWM_DRV==1
static int32_t drv_pwm_init		(void)
{
    bsp_pwm_init();
	return RET_OK;
}


int32_t drv_pwm_write		(uint8_t *pSrc, uint32_t len	, uint32_t offset, uint32_t *p_real)
{
    uint32_t pwm_val = 0;
    memcpy(&pwm_val,pSrc,len);
    bsp_pwm_write(offset,pwm_val);
    return 0;
}


static int32_t drv_pwm_read		(uint8_t *dest, uint32_t len	, uint32_t offset, uint32_t *p_real)
{
    return 0;
}


int32_t drv_pwm_ctrl		(uint32_t ctrl_cmd, uint8_t *pDest, uint32_t len, uint32_t *p_real)
{
	return RET_OK;
}

REG_DEV(
	DRV_PWM,
	DEV_AUTO_INIT,
	drv_pwm_init,
	drv_pwm_write,
	drv_pwm_read,
	drv_pwm_ctrl,
	NULL                             /*driver close func */
);
#endif
