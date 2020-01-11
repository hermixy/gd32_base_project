#ifndef __PARAM_CFG_H__
#define __PARAM_CFG_H__

#include "x_frame.h"

//=================PARAM_CFG
#define REG_CFG_ITEM(var_name,var,type,attr,def,min,max) \
        const param_item_cfg_t  param_cfg_##var_name= {#var_name,type,attr,sizeof(var),def,min,max,&var};\
        param_item_t            param_##var_name = {&param_cfg_##var_name};\
        AUTO_REG_ITEM(param_item_##var_name,AUTO_REG_TYPE_PARAM_CFG,(void*)&param_##var_name)

#define REG_CFG_ITEM_CB(var_name,var,type,attr,def,min,max,cb_func) \
        const param_item_cfg_t  param_cfg_##var_name= {#var_name,type,attr,sizeof(var),def,min,max,&var,cb_func};\
        param_item_t            param_##var_name = {&param_cfg_##var_name};\
        AUTO_REG_ITEM(param_item_##var_name,AUTO_REG_TYPE_PARAM_CFG,(void*)&param_##var_name)

#define HANDLE_CFG_ITEM(var)  &(__auto_reg_##param_item_##var_name)
#define EXT_CFG_ITEM(var) extern auto_reg_t  (__auto_reg_##param_item_##var_name)


//-------param cfg
enum
{
    eu_int8 	= 0x10,
    eu_uint8 	= 0x01,
    eu_int16 	= 0x12,
    eu_uint16 	= 0x03,
    eu_int32 	= 0x14,
    eu_uint32 	= 0x05,
    eu_fp32 	= 0x06,
};

enum
{
    attr_r		= 1,
    attr_w		= 2,
};

#define PARM_RW    (attr_r|attr_w)
#define PARM_R     (attr_r)
#define PARM_W     (attr_w)

//1-内存运行数据结构

typedef struct
{
    list_t          param_head;
    uint32_t        param_count;
} param_core_t;

typedef union
{
    uint8_t         u8_val;
    int8_t         i8_val;
    uint16_t        u16_val;
    int16_t        i16_val;
    uint32_t        u32_val;
    int32_t        i32_val;
    float           fp32_val;
} value_contain_t;


typedef void (*pf_save_func)(uint32_t addr, uint32_t len);
typedef void (*pf_user_cb_func)(void);
typedef struct
{
    const char	*name;
    uint8_t			type;
    uint8_t 		attr;
    uint8_t  		size;
    float			def_val;
    float			min_val;
    float			max_val;
    void		    *p_param_val_addr;
	pf_user_cb_func user_cb_func;               
} param_item_cfg_t;

//之所以加一层是因为param_item_cfg_t 基本是不变得内存，节省内存从36bytes->16
typedef struct
{
    const param_item_cfg_t *p_item_cfg;
    //pf_save_func    save_func;
    list_t          param_node;
    uint32_t        hash_value;
} param_item_t;

#pragma  pack(1)
//2-调参软件通信数据结构
typedef struct
{
    uint8_t         status;
    uint32_t        item_num;
    uint8_t			type;
    uint8_t 		attr;
    uint8_t		    size;
    float			cur_val;
    float			def_val;
    float			min_val;
    float			max_val;
    uint8_t         name[1];   //因为字符串是变长的
} protocol_param_item_t;

typedef struct
{
    uint8_t     status;
    uint32_t    total_item_num;
} ack_param_cnt_t;

typedef struct
{
    int8_t     status;
    uint32_t    wr_index;
    float       ret_val;
} ack_param_wr_t;

typedef struct
{
    uint32_t  wr_index;
    float     val;
} param_wr_t;

#pragma pack()

//3-存储参数数据结构
typedef struct
{
    uint32_t        hash_value;//crc16+sum16
    uint8_t			type;
    uint8_t 		attr;
    uint16_t		size;
    float			cur_val;
    float			def_val;
    float			min_val;
    float			max_val;
} save_param_item_t;


#endif //__PARAM_CFG_H__
