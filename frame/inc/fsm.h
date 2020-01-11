#ifndef __FSM_H__
#define __FSM_H__

#include "x_frame.h"


//-------------FSM

#define REG_FSM(fsm_id,fsm_run_time,max_param_len,init_state)  \
            static uint8_t       fsm_##fsm_id##_buf[1+max_param_len]={0} ;   \
			static fsm_item_t    fsm_item_##fsm_id = {#fsm_id,fsm_id,fsm_run_time,init_state,max_param_len,fsm_##fsm_id##_buf};\
			AUTO_REG_ITEM(fsm_item_##fsm_id,AUTO_REG_FSM,(void*)&fsm_item_##fsm_id)
				
#define HANDLE_REG_FSM(fsm_id)  &(__auto_reg_##fsm_item_##fsm_id)
#define EXT_REG_FSM(fsm_id) extern auto_reg_t  (__auto_reg_##fsm_item_##fsm_id)	
				
#define FSM_OBJ_PTR(fsm_id)       (&fsm_item_##fsm_id)
			
#define REG_FSM_STATE(fsm_id,state_id,in_func,exe_func,out_func,cond_func)  \
            const fsm_state_t       fsm_state##fsm_id##state_id={fsm_id,state_id,in_func,exe_func,out_func,cond_func} ;   \
			reg_fsm_state_t         reg_fsm_state##fsm_id##state_id = {&fsm_state##fsm_id##state_id};\
			AUTO_REG_ITEM(reg_fsm_state##fsm_id##state_id,AUTO_REG_FSM_STATE,(void*)&reg_fsm_state##fsm_id##state_id)
				
#define HANDLE_FSM_STATE(fsm_id,state_id)  &(__auto_reg_##reg_fsm_state##fsm_id##state_id)
#define EXT_FSM_STATE(fsm_id,state_id) extern auto_reg_t  (__auto_reg_##reg_fsm_state##fsm_id##state_id)					


//---------------fsm
typedef struct  _fsm_core_t
{
    uint32_t       total_fsm_num;
    list_t         fsm_item_head;

} fsm_core_t;

#define FSM_MAX_PARAM_SIZE  32

typedef struct _fsm_item_t
{
    const char     *fsm_name;
    uint8_t         fsm_id;
    uint32_t        fsm_run_time;
	uint8_t 		init_state;
    uint32_t        fsm_param_len_max;
    uint8_t        *p_fsm_param_buf;

    uint32_t        last_exec_time;
    uint32_t        fsm_param_len;
    uint8_t         state_is_change;
    uint8_t         last_state;
    uint8_t         cur_state;
    uint8_t         next_state;
    list_t          fsm_item_node;
    list_t          fsm_state_head;
    uint8_t         fsm_is_ready;
    uint8_t         fsm_state_num;


} fsm_item_t;

typedef void (*pf_fsm_in_func)  (fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len);
typedef void (*pf_fsm_exe_func) (fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len);
typedef void (*pf_fsm_out_func) (fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len);
typedef uint32_t (*pf_fsm_cond_func)(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len);

typedef struct _fsm_state_t
{
    uint8_t             fsm_id;
    uint32_t            fsm_state_id;
    pf_fsm_in_func      fsm_in_func;
    pf_fsm_exe_func     fsm_exe_func;
    pf_fsm_out_func     fsm_out_func;
    pf_fsm_cond_func    fsm_cond_func;
} fsm_state_t;

typedef struct _reg_fsm_state_t
{
    const fsm_state_t  *p_fsm_state;
    list_t              fsm_state_node;
} reg_fsm_state_t;


void fsm_core_init(void);
void fsm_core_run(void);
void fsm_send_event( uint8_t  fsm_id, uint32_t event, uint8_t *pbuf, uint32_t len);
fsm_item_t* fsm_get_obj( uint8_t  fsm_id);


#endif //__FSM_H__
