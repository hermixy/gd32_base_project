#ifndef __APP_DEF_PARAM_H__
#define __APP_DEF_PARAM_H__

#include "stdint.h"

#define EN_RECORDER     1
#define EN_SHELL_OUT    1

#define DEF_REC_DAT_FLG			EN_RECORDER
#define DEF_SHELL_BY_PROTOCOL 	EN_SHELL_OUT




#pragma pack(1)
#define APP_COMP_SIZE  32
#define SN_MAX_LEN     12
#define STM32_CPUID_BYTES 12   //??8byte  ????flash ?SRAM???
typedef struct
{
    uint16_t product_type;
    uint16_t product_detail;
    uint8_t app_main_ver;
    uint8_t app_sub_ver;
    uint8_t app_dbg_ver;
    uint8_t app_test_ver;
    uint32_t loader_ver;
    uint8_t  comp_time[APP_COMP_SIZE];
}product_info;

typedef  struct
{
	uint32_t  chk_val;
	uint16_t  product_type;
    uint16_t  product_detail; 
	uint8_t   sn[SN_MAX_LEN];  //uint32_t  cpu_id_chk_val;
	uint8_t   cpu_id[STM32_CPUID_BYTES];
	
}sn_info_t;

#pragma pack()


extern uint8_t enable_rec_dat;

#endif //__APP_DEF_PARAM_H__
