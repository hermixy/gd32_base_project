#ifndef __DEBUG_H__
#define __DEBUG_H__

//#include <stdint.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdarg.h>		//printf  va_list
//#include "line_buf.h"
//#include "list.h"

#include "x_frame.h"


#define REG_DEBUG(dev_id)                               \
				static uint8_t      debug_lb_buf_##dev_id[DEBUG_BUF_SIZE]={0 }; \
				static linebuf_t     debug_lb_##dev_id = {NULL,debug_lb_buf_##dev_id,DEBUG_BUF_SIZE,0,0};\
				static debug_obj_t   debug_ch_##dev_id = {dev_id, \
														 &debug_lb_##dev_id, \
														}; \
				AUTO_REG_ITEM(DBG_DEV_##dev_id  ,AUTO_REG_TYPE_DBG_DEV          ,(void*)&debug_ch_##dev_id)
#define HANDLE_DEBUG(dev_id)  &(__auto_reg_##DBG_DEV_##dev_id)

#define EXT_DEBUG(dev_id) extern auto_reg_t  (__auto_reg_##DBG_DEV_##dev_id)


typedef struct
{
    uint32_t	debug_dev;
    linebuf_t	*p_line_buf;
	
	list_t   	debug_node;			//dbg			--dbg.c
	list_t   	shell_debug_node;	//shell_node	--shell.c
} debug_obj_t;

typedef struct
{
	uint32_t  debug_num;
    list_t    all_debug_head;
} debug_ctrl_t;
extern debug_ctrl_t debug_ctrl_obj;


extern uint32_t flag_enable_cpf;

#define USER_KEIL 0

#if USER_KEIL == 1
    #define __FILE_NAME__	__MODULE__
#else
    #define __FILE_NAME__	filename(__FILE__)
#endif

//extern uint32_t flag_enable_cpf ;

#define COMPILER_VERSION_5	0
#define COMPILER_VERSION_6	1
#define COMPILER_VERSION	COMPILER_VERSION_5

#if (COMPILER_VERSION == COMPILER_VERSION_6)
	#define filename(x) strrchr(x,'/')?strrchr(x,'/')+1:x
#elif (COMPILER_VERSION == COMPILER_VERSION_5)
	#define filename(x) strrchr(x,'\\')?strrchr(x,'\\')+1:x
#endif

#define _PREFIX_I	"[INFO]: "
#define _PREFIX_W	"[WARN]: "
#define _PREFIX_E	"[ERRO]: "
#define _APPEND_FIX "\r\n"
#define _FILE_INFO	"[%-5d][%8s][%8s][%-3d]"
#define _FILE_INFO_ARG	SYS_MS_FUNC,__FILE_NAME__,__FUNCTION__,__LINE__

#define ENABLE_PROTOCOL  	0
#define DISABLE_PROTOCOL 	0

#define FIX_PARAM           ENABLE_PROTOCOL

enum
{
	log_info_id = 0x0,
	log_warn_id ,
	log_err_id ,
	log_debug_id ,
};

#define DEF_WIND  0

typedef uint32_t (*pf_debug_init_drv)(uint32_t driver_id);

void debug_dev_init(pf_debug_init_drv  pf_debug_init);
extern uint32_t get_sys_ms(void);
void dbg(uint8_t enable_protocol, uint8_t log_type,uint8_t log_window,char *fmt, ...);
void protocol_dbg(uint8_t enable_protocol, char *fmt, ...);

#define NOP_FUNC do{}while(0)

#define PRINTF_FUNC dbg//SEGGER_RTT_printf

#define SYS_MS_FUNC  get_sys_ms()
	
	
#define DBG_RAW(fmt,args...) 		PRINTF_FUNC(ENABLE_PROTOCOL,log_info_id,DEF_WIND,fmt,##args)
#define DBG_RAW_SER(fmt,args...) 	PRINTF_FUNC(DISABLE_PROTOCOL,log_info_id,DEF_WIND,fmt,##args)
#define PRF(fmt,args...) 			PRINTF_FUNC(ENABLE_PROTOCOL, log_info_id,DEF_WIND,fmt _APPEND_FIX,##args)

#define RUN_BY_LIMIT_BLOCK(ms, BLOCK)		\
{											\
	static uint32_t last_exec_ms = 0;		\
	uint32_t cur_ms_tick = SYS_MS_FUNC;		\
	if(cur_ms_tick >= last_exec_ms+ms)		\
	{										\
		last_exec_ms = cur_ms_tick;			\
		BLOCK;								\
	}										\
}

#define RUN_BY_LIMIT_FUNC(ms, func)		RUN_BY_LIMIT_BLOCK(ms, func())

#define COLOR_RED  "\033[1;31;40m"
#define COLOR_GRE  "\033[1;32;40m"
#define COLOR_BLU  "\033[1;34;40m"
#define COLOR_WHI  "\033[1;37;40m"
#define COLOR_YEL  "\033[1;33;40m"
#define COLOR_END  "\033[0m"


#if DISABLE_DBG_I
	#undef 			DEF_DBG_I
#endif 
#if DISABLE_DBG_W
	#undef 			DEF_DBG_W
#endif 
#if DISABLE_DBG_E
	#undef 			DEF_DBG_E
#endif 

#if ENABLE_DBG == 0			
	#undef 			DEF_DBG_I		
	#undef 			DEF_DBG_W
	#undef 			DEF_DBG_E
#endif 


#ifdef DEF_DBG_I
#define DBG_I(fmt,args...) 								  	do{ if(flag_enable_cpf==0) {PRINTF_FUNC(FIX_PARAM,	log_info_id,DEF_WIND,_FILE_INFO 	_PREFIX_I	fmt _APPEND_FIX,	_FILE_INFO_ARG,##args);}\
                                                                    else                   {PRINTF_FUNC(FIX_PARAM,log_info_id,DEF_WIND,	"%s" _FILE_INFO 	_PREFIX_I	fmt COLOR_END _APPEND_FIX,	COLOR_GRE,_FILE_INFO_ARG,##args);}}while(0)
#define DBG_I_LIMIT(ms,fmt,args...) RUN_BY_LIMIT_BLOCK(ms,	DBG_I(fmt, ##args))
#else
#define DBG_I(fmt,args...) 				NOP_FUNC
#define DBG_I_LIMIT(ms,fmt,args...)		NOP_FUNC
#endif

#ifdef DEF_DBG_W
#define DBG_W(fmt,args...) 									do{ if(flag_enable_cpf==0) {PRINTF_FUNC(FIX_PARAM,log_warn_id,DEF_WIND,	_FILE_INFO 	_PREFIX_W	fmt _APPEND_FIX,	_FILE_INFO_ARG,##args);}\
                                                                    else                   {PRINTF_FUNC(FIX_PARAM,log_warn_id,DEF_WIND,	"%s" _FILE_INFO 	_PREFIX_W	fmt COLOR_END _APPEND_FIX,	COLOR_YEL,_FILE_INFO_ARG,##args);}}while(0)
#define DBG_W_LIMIT(ms,fmt,args...) RUN_BY_LIMIT_BLOCK(ms,	DBG_W(fmt, ##args))
#else
#define DBG_W(fmt,args...) 				NOP_FUNC
#define DBG_W_LIMIT(ms,fmt,args...)		NOP_FUNC
#endif

#ifdef DEF_DBG_E
#define DBG_E(fmt,args...) 									do{ if(flag_enable_cpf==0) {PRINTF_FUNC(FIX_PARAM,log_err_id,DEF_WIND,	_FILE_INFO 	_PREFIX_E	fmt _APPEND_FIX,	_FILE_INFO_ARG,##args);}\
                                                                    else                   {PRINTF_FUNC(FIX_PARAM,log_err_id,DEF_WIND,	"%s" _FILE_INFO 	_PREFIX_E	fmt COLOR_END _APPEND_FIX,	COLOR_RED,_FILE_INFO_ARG,##args);}}while(0)
#define DBG_E_LIMIT(ms,fmt,args...) RUN_BY_LIMIT_BLOCK(ms,	DBG_E(fmt, ##args))
#else
#define DBG_E(fmt,args...) 				NOP_FUNC
#define DBG_E_LIMIT(ms,fmt,args...)		NOP_FUNC
#endif


#endif //__DEBUG_H__
