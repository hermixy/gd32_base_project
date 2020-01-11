#ifndef __X_FRAME_CFG_H__
#define __X_FRAME_CFG_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>		//printf  va_list

#include "x_frame_typedefine.h"     
#include "app_driver_cfg.h"
#include "app_cfg.h"
#include "protocol_cmd_cfg.h"
#include "app_def_param.h"
#include "user_data_def.h"


#define ENABLE_PROTCOL  		1
#define ENABLE_PROTCOL_ROUTE	0

//----------default	
#define ENABLE_DBG              1
#define ENABLE_TASK             1

//-----
#define ENABLE_RECORDER			1
#define ENABLE_PARAM_CFG		1
#define ENABLE_FSM				1
#define ENABLE_CFM				0
#define ENABLE_FLASH_PART		0

#define ENABLE_AUTOTEST			0	//未使用关闭，测试未注册item会导致hardfault



#define USE_REG_SECTION         1 //1-从section加载   0-从数组加载


#define X_FRAME_VER            "V0.2.0.0"
#define X_FRAME_NAME           "EF HAL platform"
#define X_FRAME_PLATFORM       "GD32 platform"

#endif

