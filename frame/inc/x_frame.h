#ifndef __X_FRAME_H__
#define __X_FRAME_H__

#include "x_frame_cfg.h"

#include "debug.h"
#include "auto_reg_module.h"
#include "hal_core.h"
#include "tasks.h"
#include "protocol_hal.h"
#include "shell.h"
#include "check_algorithm.h"
#include "param_cfg.h"
#include "rec_func_entity.h"
#include "fsm.h"
#include "cfm.h"
#include "autotest.h"
#include "flash_part.h"

#define MCU_HEADER	"gd32f3x0.h"

//hal_dev
//  dev is device    drv is driver 
#define ARRY_ITEMS_NUM(n) 		(sizeof(n)/sizeof(n[0]))
	
#define ALIGN(n) __attribute__ 	((aligned (n)))
#define PACK_8  				__attribute__((aligned(8)))
#define PACK_4  				__attribute__((aligned(4)))


#define RET_OK    0
#define ERR_DRV_ID				0xff

//#define INFO_SECTION  __attribute__ ((used,section(".info_section")))

#if USE_REG_SECTION == 1
	#define AUTO_REG_SECTION  __attribute__ ((used,section(".auto_reg_section")))
#else
	#define AUTO_REG_SECTION  
#endif

#define AUTO_REG_ITEM(name,type,p_user_data)  const auto_reg_t   __auto_reg_##name   AUTO_REG_SECTION = {#name,type,p_user_data}





#endif
