#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"


#if ENABLE_FSM == 1
static fsm_core_t fsm_core = {0};

void fsm_core_init(void)
{
    uint8_t i = 0;
    fsm_item_t *p_fsm_item;
    reg_fsm_state_t *p_reg_fsm_state;
    list_t         *p_list_head = NULL;
    list_t          *cur_node = NULL;
    uint8_t         find_fsm_item = 0;

    INIT_LIST_HEAD(&fsm_core.fsm_item_head);
    /*1:加载FSM*/
    for(i = 0; i < 100; i++)
    {
        p_fsm_item = (fsm_item_t *)auto_reg_get_func_cont_by_index(AUTO_REG_FSM, i);

        if(p_fsm_item != NULL)
        {
            INIT_LIST_HEAD(&p_fsm_item->fsm_state_head);
            list_add(&p_fsm_item->fsm_item_node, &fsm_core.fsm_item_head);
            fsm_core.total_fsm_num++;
			
			//--------------goto init state
			p_fsm_item->state_is_change = 1;
			p_fsm_item->next_state = p_fsm_item->init_state;
        }
    }

    /*2:加载FSM  state*/
    for(i = 0; i < 100; i++)
    {
        p_reg_fsm_state = (reg_fsm_state_t *)auto_reg_get_func_cont_by_index(AUTO_REG_FSM_STATE, i);
        find_fsm_item = 0;
        if(p_reg_fsm_state != NULL)
        {
            p_list_head = &fsm_core.fsm_item_head;
            list_for_each(cur_node, p_list_head)
            {
                p_fsm_item = list_entry(cur_node, fsm_item_t, fsm_item_node);
                if(p_fsm_item->fsm_id == p_reg_fsm_state->p_fsm_state->fsm_id)
                {
                    find_fsm_item = 1;
                    break;
                }
            }

            if(find_fsm_item == 1)
            {
                INIT_LIST_HEAD(&p_reg_fsm_state->fsm_state_node);
                list_add(&p_reg_fsm_state->fsm_state_node, &p_fsm_item->fsm_state_head);
            }
        }
    }
}

static fsm_state_t *find_state_by_state_num(fsm_item_t *p_fsm_item, uint32_t state_num)
{
    reg_fsm_state_t *p_reg_fsm_state;
    list_t         *p_list_head = NULL;
    list_t          *cur_node = NULL;
    p_list_head = &p_fsm_item->fsm_state_head;
    list_for_each(cur_node, p_list_head)
    {
        p_reg_fsm_state = list_entry(cur_node, reg_fsm_state_t, fsm_state_node);
        if(p_reg_fsm_state->p_fsm_state->fsm_state_id == state_num)
        {
            return (fsm_state_t *)(p_reg_fsm_state->p_fsm_state);
        }

    }
    return NULL;
}


void fsm_core_run(void)
{
    fsm_item_t *p_fsm_item;
    fsm_state_t *p_fsm_state;
    list_t         *p_list_head = NULL;
    list_t          *cur_node = NULL;
    p_list_head = &fsm_core.fsm_item_head;
    list_for_each(cur_node, p_list_head)
    {
        p_fsm_item = list_entry(cur_node, fsm_item_t, fsm_item_node);
        if( (get_sys_ms() - p_fsm_item->last_exec_time) >= p_fsm_item->fsm_run_time)
        {
            p_fsm_item->last_exec_time = get_sys_ms();
            
            //DBG_I_LIMIT(1000,"fsm:%s  state %d ", p_fsm_item->fsm_name, p_fsm_item->cur_state);
            
            if(p_fsm_item->state_is_change)
            {
                p_fsm_item->state_is_change = 0;
                p_fsm_item->last_state = p_fsm_item->cur_state;
                p_fsm_item->cur_state  = p_fsm_item->next_state;
				
				if(p_fsm_item->fsm_is_ready==1)
				{
					//执行退出函数
					p_fsm_state = find_state_by_state_num(p_fsm_item, p_fsm_item->last_state);
					if(p_fsm_state == NULL)
					{
						DBG_E("fsm:%s  state %d is err", p_fsm_item->fsm_name, p_fsm_item->last_state);
						break;
					}
					if(p_fsm_state->fsm_out_func != NULL)
					{
						p_fsm_state->fsm_out_func(p_fsm_item, p_fsm_item->p_fsm_param_buf, p_fsm_item->fsm_param_len);
					}
				}
                
                //执行进入新状态的函数
                p_fsm_state = find_state_by_state_num(p_fsm_item, p_fsm_item->cur_state);
                if(p_fsm_state == NULL)
                {
                    DBG_E("fsm:%s  state %d is err", p_fsm_item->fsm_name, p_fsm_item->cur_state);
                    break;
                }
                if(p_fsm_state->fsm_in_func != NULL)
                {
                    p_fsm_state->fsm_in_func(p_fsm_item, p_fsm_item->p_fsm_param_buf, p_fsm_item->fsm_param_len);
                }
            }
            else
            {
                //执行稳定状态运行函数
                p_fsm_state = find_state_by_state_num(p_fsm_item, p_fsm_item->cur_state);
                if(p_fsm_state == NULL)
                {
                    DBG_E("fsm:%s  state %d is err", p_fsm_item->fsm_name, p_fsm_item->cur_state);
                    break;
                }
                if(p_fsm_state->fsm_exe_func != NULL)
                {
                    p_fsm_state->fsm_exe_func(p_fsm_item, p_fsm_item->p_fsm_param_buf, p_fsm_item->fsm_param_len);
                }
            }
			p_fsm_item->fsm_is_ready = 1;// 执行第一次之后此标志位ok，避免不能调用out函数

        }
    }
}

REG_TASK(ID_TASK, 1, fsm_core_run);

void fsm_send_event( uint8_t  fsm_id, uint32_t event, uint8_t *pbuf, uint32_t len)
{
    fsm_item_t *p_fsm_item;
    fsm_state_t *p_fsm_state;
    list_t         *p_list_head = NULL;
    list_t          *cur_node = NULL;
    p_list_head = &fsm_core.fsm_item_head;
    list_for_each(cur_node, p_list_head)
    {
        p_fsm_item = list_entry(cur_node, fsm_item_t, fsm_item_node);
        if(p_fsm_item->fsm_id  == fsm_id)
        {
            p_fsm_state = find_state_by_state_num(p_fsm_item, p_fsm_item->cur_state);
            if(p_fsm_state == NULL)
            {
                DBG_E("fsm:%s  state %d is err", p_fsm_item->fsm_name, p_fsm_item->cur_state);
                return;
            }
            p_fsm_item->fsm_param_len = 0;
            if(p_fsm_state->fsm_cond_func != NULL)
            {
                if( p_fsm_state->fsm_cond_func(p_fsm_item, event, pbuf, len) ){p_fsm_item->state_is_change = 1;}
                
            }

            return ;
        }
    }
}


fsm_item_t* fsm_get_obj( uint8_t  fsm_id)

{
    fsm_item_t *p_fsm_item;
    list_t         *p_list_head = NULL;
    list_t          *cur_node = NULL;
    p_list_head = &fsm_core.fsm_item_head;
    list_for_each(cur_node, p_list_head)
    {
        p_fsm_item = list_entry(cur_node, fsm_item_t, fsm_item_node);
        if(p_fsm_item->fsm_id  == fsm_id)
        {
          
            return p_fsm_item;
        }
    }
	return NULL;
}

#endif
