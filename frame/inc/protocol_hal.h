#ifndef __PROTOCOL_HAL_H__
#define __PROTOCOL_HAL_H__

#include <stdint.h>
#include "line_buf.h"
#include "list.h"
#include "x_frame.h"	//ENABLE_PROTCOL

///========link-->protocol
#define REG_LINK(link_id,prio,dev,protocol_ver,buf_size,en_route,en_debug)  \
            PACK_4 uint8_t reg_link_buf_##link_id[buf_size]; \
			link_hal_item    reg_link_##link_id = {#link_id,link_id,prio,dev,dev,protocol_ver,reg_link_buf_##link_id,buf_size,en_route,en_debug};\
			AUTO_REG_ITEM(LINK_##link_id,AUTO_REG_TYPE_LINK,(void*)&reg_link_##link_id)
#define HANDLE_LINK(link_id)  &(__auto_reg_##LINK_##link_id)
#define EXT_LINK(link_id) extern auto_reg_t  (__auto_reg_##LINK_##link_id)


#define REG_PROTOCOL(protocol_id,get_head_len,get_pack_len,check_head,check_pack,pack_data,unpack_data) \
			const protocol_obj_t protocol_id##_obj={#protocol_id,protocol_id,get_head_len,get_pack_len,check_head,check_pack,pack_data,unpack_data};\
			reg_protocol_t       protocol_id##_reg_obj={&protocol_id##_obj};\
			AUTO_REG_ITEM(PROTOCOL_##protocol_id,AUTO_REG_PROTOCOL,(void*)&protocol_id##_reg_obj)
				
#define HANDLE_PROTOCOL(protocol_id)  &(__auto_reg_##PROTOCOL_##protocol_id)
			
#define EXT_PROTOCOL(protocol_id) extern auto_reg_t  (__auto_reg_##PROTOCOL_##protocol_id)


#define ADDR_ROUTE_ITEM(en,dest,tag_link_id) {ROUTE_ADDR,en,0xff,dest,UN_USE_CMD, UN_USE_CMD,tag_link_id,NULL}
#define CMD_ROUTE_ITEM(en,src_link,cmd_set,cmd_id,tag_link_id)  {ROUTE_CMD,en,src_link,0xff,cmd_set, cmd_id,tag_link_id,NULL}

#define REG_ROUTE_FUNC(route_func)  AUTO_REG_ITEM(route_tab,AUTO_REG_ROUTE_CFG,(void*)&route_func)
#define HANDLE_ROUTE_FUNC()  &(__auto_reg_##route_tab)
#define EXT_ROUTE_FUNC() extern auto_reg_t  (__auto_reg_##route_tab)		

#define REG_LINK_ACTION(link_id,cmd_set,cmd_id,func)  \
                action_tab_t  action_##link_id##_##cmd_set##_##cmd_id={cmd_set,cmd_id,func,link_id,#func};\
                AUTO_REG_ITEM(link_id##_##cmd_set##_##cmd_id ,AUTU_REG_TYPE_LINK_ACTION,(void*)&action_##link_id##_##cmd_set##_##cmd_id)
#define HANDLE_LINK_ACTION(link_id,cmd_set,cmd_id,func)  &(__auto_reg_##link_id##_##cmd_set##_##cmd_id)
#define EXT_LINK_ACTION(link_id,cmd_set,cmd_id,func) extern auto_reg_t  (__auto_reg_##link_id##_##cmd_set##_##cmd_id)	


//-----cfg
#define MAX_PROTCOL_PRIO    	4
#define MAX_PACK_LEN         	1024		//PROTOCOL_EF02_PLUS—>512		PROTOCOL_EF02_PLUS->1024
#define MAX_SEND_PACK_LEN 		(MAX_PACK_LEN+20)
#define MAX_RECV_PACK_LEN 		(MAX_PACK_LEN+20)
#define PROTOCOL_RUN_PERIO 		 (10)   //10ms


#define UNPACK_HEAD  0
#define UNPACK_BODY	 1

#define IS_HOST_PACK		(0)
#define IS_ROUTE_PACK		(1)
#define UN_KNOW_PACK		(2)
#define DIRECT_PACK		    (3)
#define ERR_ADDR_PACK		(4)

#define EN_ROUTE		0x01
#define EN_HACK		    0x02   //转发的时候同时获取此包

//-------protocol_hal.c
typedef uint8_t (*pf_opt_route)(uint8_t dest);

typedef struct
{
    uint8_t			route_type; //是否是整个链路路由，还是命名路由
    uint8_t         opt;
	
	uint8_t         src_link_id;
    uint8_t         dest;

    uint8_t			cmd_set;
    uint8_t			cmd_id;

    uint8_t         tag_link_id;
    pf_opt_route	func_opt;
} link_route_t;

typedef struct
{
    uint32_t  link_num;
    list_t    link_head;  //?????????????????
} link_ctrl_t;


typedef struct
{
    link_ctrl_t  		link_ctrl[MAX_PROTCOL_PRIO];
    uint32_t     		real_reg_link_num;
    list_t					protocol_head;
    list_t    			link_all_head;  //?????????
} link_obj_t;

typedef struct
{
    uint8_t 	link_id;
    uint8_t 	src;
    uint8_t 	dest;
    uint8_t 	enc_type;
    uint8_t	 	check_type;
    uint16_t 	cmd_func;
    uint16_t 	cmd_id;
    uint16_t 	data_len;
    uint8_t 	need_ack;
    uint8_t		is_ack;
    uint8_t     ack_type;
    uint8_t 	*pdata;
    uint32_t 	seq; /*????*/
    uint32_t 	time_snap; /*????*/
    uint8_t     is_rw_cmd;
    uint8_t     is_queue;

} hal_frame_info_t;

typedef int32_t         (*pf_action_func)       (hal_frame_info_t *p_frame);
typedef struct
{

    uint8_t         cmd_func;
    uint8_t         cmd_id;
    pf_action_func  action_func;
    uint32_t        link_id;
    const char     *func_name;
    list_t          action_list;
} action_tab_t;


typedef int32_t			(*pf_check_route)       (hal_frame_info_t *p_frame, uint8_t *p_tag_link_id);
typedef pf_action_func  (*pf_user_action_func)  (hal_frame_info_t *p_frame);
typedef int32_t         (*pf_user_action) (hal_frame_info_t *p_frame, action_tab_t *action_tab, uint32_t max_item);

//---REG_LINK
typedef struct
{
    const char             *name;
    uint32_t                link_id;
    uint32_t                prio;       //优先级0-3
    uint32_t                read_dev;
    uint32_t                write_dev;
    uint32_t				protocol_type;
    uint8_t                *p_buf;
    uint16_t                buf_len;
    uint8_t					en_route;
    uint8_t                 en_debug;
	
    uint8_t                 flag_init;
    uint8_t					unpack_step;
    uint32_t				send_seq;
    uint32_t				recv_seq;
    linebuf_t               line_buf_obj;
    list_t                  link_list;
    list_t                  link_all_list;
    list_t                  action_list_head;
} link_hal_item;




//--------protocol
typedef uint16_t  (*pf_get_header_len)	(uint8_t *p_raw_buf,uint32_t len);
typedef uint16_t  (*pf_get_data_len)	(uint8_t *p_raw_buf,uint32_t len);
typedef uint8_t   (*pf_check_header)	(uint8_t *p_raw_buf,uint32_t len);
typedef uint8_t   (*pf_check_data)		(uint8_t *p_raw_buf,uint32_t len);
typedef uint16_t  (*pf_pack_data)		(uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info);
typedef uint16_t  (*pf_unpack_data)	    (uint8_t *p_raw_buf, hal_frame_info_t *p_frame_info);

typedef struct  _protocol_obj_t
{
    const char			*protocol_name;
    uint32_t 			protocol_num;	//type
    pf_get_header_len	get_head_len;
    pf_get_data_len		get_pack_len;
    pf_check_header		check_header;
    pf_check_data		check_data;
    pf_pack_data		pack_data;
    pf_unpack_data		unpack_data;
} protocol_obj_t;

typedef struct _reg_protocol_t
{
    const protocol_obj_t		*protocol_ptr;
    list_t  					protocol_node;
} reg_protocol_t;



void  protocol_init(void);
int32_t protocol_send_by_id(hal_frame_info_t *p_frame_info);
int32_t  protocol_send(hal_frame_info_t *p_frame_info);
void protocol_send_ack(hal_frame_info_t* p_frame_info,uint8_t* pbuf,uint16_t len);

uint32_t judge_link_write_dev(uint32_t debug_dev);
#endif
