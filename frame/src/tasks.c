#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E

#include "x_frame.h"


/*增加事件触发的回调函数*/
volatile uint32_t sys_tick_ms = 0;
uint32_t get_sys_ms(void)
{
    return sys_tick_ms;
}

static int32_t sys_tick_cb       (uint32_t code, uint8_t *pbuf, uint32_t len)
{
    sys_tick_ms+=code;
    return 0;
}
REG_DEV_CB(DRV_ID_SYS_TICK, sys_tick_cb);

void block_ms(uint32_t ms)
{
    uint32_t cur_ms = sys_tick_ms;
    while( (sys_tick_ms - cur_ms) < ms);
}

uint32_t get_sys_s(void)
{
	return sys_tick_ms/1000;
}
uint32_t get_sys_min(void)
{
	return sys_tick_ms/1000/60;
}



#if ENABLE_TASK == 1

static ALIGN(4) task_ctrl_t task_group_data[MAX_TASKS_GROUP] = {0};
static ALIGN(4) task_item_t *task_table[MAX_TASKS_NUM] = {0};

static uint8_t cur_reg_task_num = 0;

#define GET_SYS_MS  get_sys_ms()

void task_init(void)
{
    uint32_t i = 0;
    task_item_t *p_task_item = NULL;
	
	//DBG_I("task init in ok \r\n");
	
    for(i = 0; i < MAX_TASKS_GROUP; i++)
    {
        task_group_data[i].task_num = 0;
        INIT_LIST_HEAD(&task_group_data[i].task_head);
    }
	
    (void)task_table;
    for(i = 0; i < MAX_TASKS_NUM; i++)
    {
        p_task_item = auto_reg_get_func_cont_by_index(AUTO_REG_TYPE_TASK, i);
		if(p_task_item==NULL){return;}
        if( (p_task_item->group < MAX_TASKS_GROUP) ) //unsigned int 绝对会大于0
        {
            task_table[cur_reg_task_num] = p_task_item;
            list_add_tail(&p_task_item->list, &task_group_data[p_task_item->group].task_head);
            task_group_data[p_task_item->group].task_num++;

            /*---------*/
            p_task_item->last_exe_time = 0;
            p_task_item->task_used_time = 0;
            p_task_item->flag_init     = 1;

            cur_reg_task_num++;
            DBG_I("reg_task [%d] : %s is ok!", cur_reg_task_num, p_task_item->func_name);
        }
    }
}
//REG_INIT_MOD(task_init);

void task_sch (uint8_t group)
{
    uint32_t  cur_ms = 0;
    list_t    *cur_node = NULL;
    task_item_t *p_task_item = NULL;
	
	//DBG_I("[%d]0x%p \r\n",group, &task_group_data[group].task_head);
    list_for_each(cur_node, &task_group_data[group].task_head)
    {
        uint32_t cur_perio = 0;
        p_task_item = list_entry(cur_node, task_item_t, list);
        cur_ms = GET_SYS_MS;
        cur_perio = cur_ms - p_task_item->last_exe_time;
        if( (cur_perio >=  p_task_item->perio) || (p_task_item->last_exe_time==0)) //保障第一次也能运行
        {
            p_task_item->last_exe_time = cur_ms;
            p_task_item->exe_func();
        }
    }
}



#if TINY_FUNC_CODE == 0
int32_t list_tasks(uint8_t argc, uint8_t *argv[])
{
    uint32_t  i = 0;
    DBG_I("cur system run %d tasks  (MAX:%d)", cur_reg_task_num,MAX_TASKS_NUM);

    for(i = 0; i < cur_reg_task_num; i++)
    {
        if(task_table[i]->flag_init)
        {
            DBG_I("task[%d]-\t(T:%4d ms) group:%d : %s", i, task_table[i]->perio, task_table[i]->group, task_table[i]->func_name);
        }
    }

    return RET_OK;
}
REG_SHELL_CMD_X(ls_tasks, 0, list_tasks, "list_tasks", "list_tasks.  eg ls_tasks \r\n ");
#endif

#else

void task_init(void)
{
	
}

void task_sch (uint8_t group)
{
	
}

#endif

