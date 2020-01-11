#ifndef __APP_CFG_H__
#define __APP_CFG_H__

//#define SYS_TICK_PERIO  20  //ms
//-------debug
#define DEBUG_BUF_SIZE 128
#define TINY_FUNC_CODE     0    //1-disable shell cmd item  0-enable shell cmd item


#define PC_LINK_ID			1
#define PC_LINK_PROTOCOL	PROTOCOL_EF02_PLUS


#define DISABLE_DBG_I           0
#define DISABLE_DBG_W           0
#define DISABLE_DBG_E           0
#define DISABLE_PROTOCOL_SHELL  0


#define LINK_SHELL_ID           PC_LINK_ID
#define REC_LINK_ID             PC_LINK_ID

#define REC_TIME_STAMP          get_sys_ms()

#endif //__APP_CFG_H__

