#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"



#if ENABLE_DBG == 1

uint32_t flag_enable_cpf = 0;

debug_ctrl_t debug_ctrl_obj = {0};

void debug_dev_init(pf_debug_init_drv  pf_debug_init)
{
	uint8_t i = 0;
	debug_obj_t *p_debug_obj = NULL;
	
	INIT_LIST_HEAD(&debug_ctrl_obj.all_debug_head);	//初始化之前不能打印
	
	for(i = 0;i < MAX_DRV_NUM;i++)
	{
		p_debug_obj = auto_reg_get_func_cont_by_index(AUTO_REG_TYPE_DBG_DEV, i);	//shell中也寻找了一遍
		
		if(p_debug_obj != NULL)
		{
			list_add_tail(&p_debug_obj->debug_node, &debug_ctrl_obj.all_debug_head);
			pf_debug_init(p_debug_obj->debug_dev);	//调试驱动类型的初始化
			debug_ctrl_obj.debug_num++;
		}
		else
		{
			break;
		}
	}
}
//REG_INIT_MOD(debug_dev_init);	//在进入while(1)前初始化，如果之前有调用打印则会hardfault, 所以采用hal_init中直接调用

void list_debug_dev(void)
{
	uint8_t i = 1;
	list_t *cur_node = NULL;
	list_t *tag_list_head = &debug_ctrl_obj.all_debug_head;
	debug_obj_t *p_dev_obj = NULL;
	
	DBG_I("-------list all debug dev	num:%d", debug_ctrl_obj.debug_num);
	list_for_each(cur_node, tag_list_head)
    {
        p_dev_obj = list_entry(cur_node, debug_obj_t, debug_node);
		//DBG_I("[%d] %s", p_dev_obj->debug_dev, sg_sys_hal.p_dev_tab[p_dev_obj->debug_dev]->dev_name);
		dbg_i_debug_dev_info(i++, p_dev_obj->debug_dev);
    }
}
REG_INIT_MOD(list_debug_dev);

volatile uint8_t lock = 0;
void dbg(uint8_t enable_protocol, uint8_t log_type,uint8_t log_window,char *fmt, ...)
{
    va_list ap;
    uint16_t len = 0;
    uint8_t str[DEBUG_BUF_SIZE];
    debug_obj_t *p_dev_obj = NULL;
    list_t  *cur_node = NULL;
    //list_t *tag_list_head = &shell_obj.shell_dev_head;
	list_t *tag_list_head = &debug_ctrl_obj.all_debug_head;
	
    if(lock == 1)
    {
        return;
    }
//    if(shell_obj.shell_init_flag == 0)
//    {
//        return;
//    }

    lock = 1;
    va_start(ap, fmt);
	
    len = vsnprintf((char*)&str[2], DEBUG_BUF_SIZE - 1, fmt, ap);
    str[len+2] = '\0';
    len = len + 1;
	
	
	//hal_app_write(DRV_RTT, (uint8_t *)&str[2], len, 0, NULL);	//先只考虑从RTT打印
	
	#if 0
	{
		uint8_t i = 0;
		debug_obj_t *p_debug_obj = NULL;
		
		for(i = 0;i < MAX_DRV_NUM;i++)
		{
			p_debug_obj = auto_reg_get_func_cont_by_index(AUTO_REG_TYPE_DBG_DEV, i);
			
			if(p_debug_obj != NULL)
			{
				hal_app_write(p_debug_obj->debug_dev, (uint8_t *)&str[2], len, 0, NULL);
			}
		}
	}
	#else
	//-----hardfault（原因 链表头没有初始化）
    list_for_each(cur_node, tag_list_head)
    {
        p_dev_obj = list_entry(cur_node, debug_obj_t, debug_node);
		
		if(judge_link_write_dev(p_dev_obj->debug_dev) == 0)	//如果同一个设备注册为DEBUG和LINK，则DEBUG不输出
		{
			hal_app_write(p_dev_obj->debug_dev, (uint8_t *)&str[2], len, 0, NULL);
		}
    }
	#endif
    
#if DISABLE_PROTOCOL_SHELL==0
    {
		hal_frame_info_t  shell_frame_info={0};
        if(enable_protocol_shell_out>0)
        {
			str[0]=log_type;
			str[1]=log_window;
            shell_frame_info.link_id  =  LINK_SHELL_ID;
            shell_frame_info.cmd_func =  COMM_CMD_SET;
			shell_frame_info.cmd_id   =  CMD_ID_SHELL_OUT;  
            shell_frame_info.src      =  HOST_ADDR;
            shell_frame_info.dest     =  PC_ADDR;
            shell_frame_info.pdata    =  (uint8_t *)str;
            shell_frame_info.data_len =  len-1;    //协议传输不用后面的\0    add log_type  log_win  sub 0x0d  0x0a
            protocol_send_by_id(&shell_frame_info);
        }
    }
#endif 

    lock = 0;
    va_end(ap);
}

//void dbg_func_test(void)
//{
//	dbg(0,0,0, "dbg_func_test---");
//}
//REG_TASK(1, 1000, dbg_func_test);

#else

void debug_dev_init(pf_debug_init_drv  pf_debug_init)
{
	
}

#endif
