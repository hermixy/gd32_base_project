#ifndef __REC_FUNC_ENTITY_H__
#define __REC_FUNC_ENTITY_H__

#include "x_frame.h"


#define EXT_REC(name)        extern void rec_dat_##name(void)
#define RUN_REC(name)        rec_dat_##name()
#define RUN_REC_NAME(name)   rec_dat_##name

#define EXT_REC_CACHE(name)        extern void rec_cache_dat_##name(void)
#define RUN_REC_CACHE(name)        rec_cache_dat_##name()
#define RUN_REC_CACHE_NAME(name)   rec_cache_dat_##name

//--------------recorder
enum
{
	REC_TYPE_ERR  	= 0,
	REC_TYPE_UINT8	= 1,
	REC_TYPE_INT8	= 2,
	REC_TYPE_UINT16	= 3,
	REC_TYPE_INT16	= 4,
	REC_TYPE_UINT32	= 5,
	REC_TYPE_INT32	= 6,
	REC_TYPE_FLOAT	= 8,
	REC_TYPE_DOUBLE	= 9,
    //REC_TYPE_STRING	= 10,
};

#pragma pack(1)
typedef struct
{
    uint8_t  rec_var_index;
    uint8_t  rec_var_type;
    uint8_t  rec_var_len; //怕string这周依旧不确定长度
    uint8_t  rec_var_name_len;
    uint8_t  rec_var_name[1];
}rec_var_item_t;

typedef struct
{
    uint8_t         pack_cmd_set;
    uint8_t         pack_cmd_id;
    uint8_t         pack_item_num;
    uint8_t         pack_name_len;
    uint8_t         pack_name[1];
}rec_cfg_item_t;
#pragma pack()

typedef void (*pf_void_func)(void);

typedef struct
{
    list_t             rec_init_node;
    pf_void_func       rec_cfg_func;
}rec_node_t;


#define MAX_RECORDER_PACK_LEN 220
#define MAX_REC_ITEM_NUM      50 //max_item_num per pack


const char* get_var_name(const char* var_name);

void add_rec_node(list_t* p_add_node);

#endif     
