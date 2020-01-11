#ifndef __TASKS_H__
#define __TASKS_H__

#include <stdint.h>
#include "list.h"

///---------task
#define REG_TASK(group,perio,task_name)  \
			task_item_t    task_##task_name  = {#task_name,group,perio,task_name};\
			AUTO_REG_ITEM(TASK_##task_name,AUTO_REG_TYPE_TASK,(void*)&task_##task_name)
#define HANDLE_TASK(task_name)  &(__auto_reg_##TASK_##task_name)

#define EXT_TASK(task_name)   extern auto_reg_t (__auto_reg_##TASK_##task_name)

///-----power on init
#define REG_INIT_MOD(func)  AUTO_REG_ITEM(func,AUTO_REG_INIT_MOD,(void*)func)



#define VH_TASK   0
#define HI_TASK   1
#define LW_TASK   2
#define ID_TASK   3

#define MAX_TASKS_GROUP         4   /*very hi    hi   low  idle*/
#define MAX_TASKS_NUM           32   

#define DRV_ID_SYS_TICK         0x0   

//----device opts
typedef int32_t (*pf_dev_init)		 (void);
typedef int32_t (*pf_dev_write)	     (uint8_t *pSrc     , uint32_t len  , uint32_t offset, uint32_t *real_w_bytes);
typedef int32_t (*pf_dev_read)		 (uint8_t *pDest    , uint32_t len  , uint32_t offset, uint32_t *real_r_bytes);
typedef int32_t (*pf_dev_ctrl)		 (uint32_t ctrl_cmd , uint8_t *pDest, uint32_t len   , uint32_t *real_c_bytes);
typedef int32_t (*pf_dev_close)	     (void);
typedef int32_t (*pf_dev_push)       (uint32_t code, uint8_t *pbuf, uint32_t len);

struct  _dev_push_t
{
    const char     *cb_func_name;
    uint32_t        dev_id;
    pf_dev_push     cb_func;
};

struct _dev_obj_
{
    const char 	*dev_name;
    uint32_t		dev_id;           

    uint32_t        is_auto_init;

    pf_dev_init		dev_init;
    pf_dev_write	dev_write;
    pf_dev_read		dev_read;
    pf_dev_ctrl		dev_ctrl;
    pf_dev_close	dev_close;
    struct _dev_push_t     *p_dev_push_obj;
    uint32_t		dev_status;
    uint32_t        dev_flag;        
};

#define DEV_FLAG_REG    (0x1<<0)
#define DEV_FLAG_INIT   (0x1<<1)
#define DEV_READY       (DEV_FLAG_REG|DEV_FLAG_INIT)
#define DEV_AUTO_INIT   (0x01)

typedef struct _dev_obj_ dev_obj_t;
typedef struct _dev_push_t dev_push_t;

//------task
typedef void (*pf_exe_func)(void);

typedef struct
{
    uint32_t  task_num;
    list_t    task_head;
} task_ctrl_t;


typedef struct
{
    const char     *func_name;
    uint32_t        group;
    uint32_t        perio;
    pf_exe_func     exe_func;

    uint32_t        last_exe_time;
    uint32_t        task_used_time;
    uint8_t         flag_init;
    uint8_t         flag_sem;
    list_t          list;
} task_item_t;



uint32_t get_sys_ms(void);
void task_init(void);
void task_sch (uint8_t group);

uint32_t get_sys_s(void);
uint32_t get_sys_min(void);

#endif
