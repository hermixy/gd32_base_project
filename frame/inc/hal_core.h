#ifndef __HAL_CORE_H__
#define __HAL_CORE_H__

#include <stdint.h>

///-------device
#define REG_DEV(id,is_auto_init,init,write,read,ctrl,close) \
                static dev_obj_t device_##id = {#id,id,is_auto_init,init,write,read,ctrl,close,NULL,0,0};\
                AUTO_REG_ITEM(DRIVER_##id      ,AUTO_REG_TYPE_DRIVER           ,(void*)&device_##id)
#define HANDLE_DEV(id)  &(__auto_reg_##DRIVER_##id)
#define EXT_DEV(id)  extern auto_reg_t (__auto_reg_##DRIVER_##id)

#define REG_DEV_CB(dev_id,func)     \
                                    dev_push_t    dev_cb_##func  = {#func,dev_id,func};\
                                    AUTO_REG_ITEM(CB_##dev_id##func,AUTO_REG_TYPE_DRIVER_PUSH,(void*)& dev_cb_##func)
#define HANDLE_DEV_CB(dev_id,func)  &(__auto_reg_##CB_##dev_id##func)
#define EXT_DEV_CB(dev_id,func)  extern auto_reg_t (__auto_reg_##CB_##dev_id##func)



#define MAX_DRV_NUM 32
typedef struct _hal_obj_t
{
    const char 	*hal_name;
    const char 	*hal_version;
    const char 	*hal_platform_info;
    struct _dev_obj_      *p_dev_tab[MAX_DRV_NUM + 1];
}hal_obj_t;


int32_t	hal_init(void);
int32_t  hal_app_write	        (uint32_t dev_handle, uint8_t *pbuf, uint32_t len, uint32_t offset, uint32_t *p_real_bytes);
int32_t  hal_app_read		    (uint32_t dev_handle, uint8_t *pbuf, uint32_t len, uint32_t offset, uint32_t *p_real_bytes);
int32_t  hal_app_ctrl		    (uint32_t dev_handle, uint32_t ctrl_cmd, uint8_t *pDest, uint32_t len, uint32_t *p_real_bytes);
void hal_dev_notify(uint32_t dev_id, uint32_t code, uint8_t *pbuf, uint32_t len);

void dbg_i_debug_dev_info(uint8_t cnt, uint8_t debug_dev_num);

#endif
