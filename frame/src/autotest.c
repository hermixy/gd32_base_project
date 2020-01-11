#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E

#include "autotest.h"

#if ENABLE_AUTOTEST == 1
typedef struct
{
	list_t					head;
	uint8_t					cur_step;
	uint8_t 				total;
	uint32_t				err_code;
	uint8_t					next_item_flg;
	uint8_t					stop_test_flg;
	list_t*					cur_exe_node;
	auto_test_ctrl_t* 		ctrl_func_set;
}auto_test_t;


#define AUTO_TEST_FSM_ID   0xf0

enum
{
	AUTO_TEST_INIT = 0,
	AUTO_TEST_WORK,
	AUTO_TEST_CHANGE_ITEM,
	AUTO_TEST_FINISH_TEST,
	AUTO_TEST_NG,
	AUTO_TEST_OK,
	AUTO_TEST_END,
};


enum
{
	AUTO_TEST_EV_INIT_OK = 1,
	AUTO_TEST_EV_ITEM_INIT_OK,
	AUTO_TEST_EV_TEST_ITEM_OK,
	AUTO_TEST_EV_TEST_ITEM_NG,
	AUTO_TEST_EV_TEST_NEXT_ITEM,
	AUTO_TEST_EV_TEST_ALL_OK,
	AUTO_TEST_EV_TEST_ALL_NG,
	AUTO_TEST_EV_TEST_FINISH,
	AUTO_TEST_EV_TEST_RE_START,
};


static auto_test_t auto_test_obj={0};


void auto_test_init(void)
{
	uint8_t i = 0;
	uint8_t j = 0;
	auto_test_item_t * p_test_item=NULL;
	auto_test_item_t * p_cur_test_item;
	list_t*     p_cur_node;
	
	(void)p_cur_test_item;
	
	
	INIT_LIST_HEAD(&auto_test_obj.head);
	
	
	for(i=0;i<250;i++)
	{
		p_test_item = (auto_test_item_t*)auto_reg_get_func_cont_by_index(AUTO_REG_AUTOTEST, i);
		if(p_test_item==NULL){break;}
		
		auto_test_obj.total++;
		
		
		p_cur_node = auto_test_obj.head.next;
		for(j=0;j<250;j++)
		{
			if(p_cur_node == &auto_test_obj.head )
			{
				
				list_add_tail(&p_test_item->node, p_cur_node);
				break;
			}
			
			p_cur_test_item = list_entry(p_cur_node,auto_test_item_t,node);
			
			if(p_cur_test_item->p_cfg->test_index < p_test_item->p_cfg->test_index)
			{
				p_cur_node = p_cur_node->next;
			}
			else
			{
				list_add_tail(&p_test_item->node, p_cur_node);
				break;
			}
		}

	}		
	
	
	//-------------生成执行的顺序代码
	i = 0;
	list_for_each_entry(p_cur_test_item,&auto_test_obj.head,node)
	{
		i++;
		p_cur_test_item->seq_num = i;
	}
	
	
	//-------------加载测试调用函数
	auto_test_obj.ctrl_func_set = (auto_test_ctrl_t*)auto_reg_get_func_cont_by_index(AUTO_REG_AUTOTEST_CTRL, 0);
	
	auto_test_obj.cur_step = 0;
}
REG_INIT_MOD(auto_test_init);

void auto_test_emit_result(uint32_t err_code)
{
	auto_test_obj.err_code = err_code;
	if(err_code==0)
	{
		auto_test_obj.next_item_flg  = 1;
		fsm_send_event(AUTO_TEST_FSM_ID,AUTO_TEST_EV_TEST_ITEM_OK,NULL,0);
	}
	else
	{
		auto_test_obj.stop_test_flg  = 1;
		fsm_send_event(AUTO_TEST_FSM_ID,AUTO_TEST_EV_TEST_ITEM_NG,NULL,0);
	}
	
}


void auto_test_finish_test(void)
{
	fsm_send_event(AUTO_TEST_FSM_ID,AUTO_TEST_EV_TEST_FINISH,NULL,0);
}

void auto_test_re_start_test(void)
{
	fsm_send_event(AUTO_TEST_FSM_ID,AUTO_TEST_EV_TEST_RE_START,NULL,0);
}

//---------------auto test  fsm


/*                             not end
                         +------<------+               +---------------o---------------+  
                        /              |               |             OK                |
	   o---------------o---------------o---------------o---------------o---------------x
     init             work        change_item       finish           NG               END

*/

//---------------------------init

static void auto_test_init_in	(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
	
	//auto_test_init();	//重新切换状态机会导致重新初始化
	
	if(auto_test_obj.ctrl_func_set!=NULL)  //在第一次就去判断，避免后面每次都去判断
	{
		if(auto_test_obj.ctrl_func_set->auto_test_in_func!=NULL) {auto_test_obj.ctrl_func_set->auto_test_in_func();}
		
		if(auto_test_obj.head.next!=&auto_test_obj.head)
		{
			auto_test_obj.cur_exe_node = auto_test_obj.head.next;
			
			DBG_I("---- auto_test wait start ... ----");
		}
		else
		{
			DBG_E_LIMIT(1000," err ,auto test item is  NULL ====");
		}
	}
	else
	{
		DBG_E_LIMIT(1000," err ,auto test set is  NULL +++");
	}
}

static void auto_test_init_in_exe	(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
	if(auto_test_obj.ctrl_func_set->auto_test_start_func!=NULL)
	{
		if(auto_test_obj.ctrl_func_set->auto_test_start_func())
		{
			fsm_send_event(AUTO_TEST_FSM_ID,AUTO_TEST_EV_INIT_OK,NULL,0);
			
			DBG_I("---- auto_test start ----------!");
		}
	}
}

static uint32_t auto_test_init_chk_func	(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len)
{
	if(event == AUTO_TEST_EV_INIT_OK){ p_fsm->next_state = AUTO_TEST_WORK; return 1;}
	return 0;
}


//-----------------work
static void auto_test_work_in	(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
	auto_test_item_t * p_cur_test_item = NULL;
	p_cur_test_item = list_entry(auto_test_obj.cur_exe_node,auto_test_item_t,node);
	
	
	if(auto_test_obj.ctrl_func_set->auto_test_exe_func!=NULL) {auto_test_obj.ctrl_func_set->auto_test_exe_func(p_cur_test_item,0,p_cur_test_item->seq_num,auto_test_obj.total);}
	
	if(p_cur_test_item!=NULL)
	{
		if(p_cur_test_item->p_cfg->in_func!=NULL){p_cur_test_item->p_cfg->in_func();}
	}
	//fsm_send_event(AUTO_TEST_FSM_ID,AUTO_TEST_EV_INIT_OK,NULL,0);
		
}

static void auto_test_work_in_exe	(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
	auto_test_item_t * p_cur_test_item;
	p_cur_test_item = list_entry(auto_test_obj.cur_exe_node,auto_test_item_t,node);
	
	if(auto_test_obj.ctrl_func_set->auto_test_exe_func!=NULL) {auto_test_obj.ctrl_func_set->auto_test_exe_func(p_cur_test_item,0,p_cur_test_item->seq_num,auto_test_obj.total);}
	
	if(p_cur_test_item!=NULL)
	{
		if(p_cur_test_item->p_cfg->exe_func!=NULL){p_cur_test_item->p_cfg->exe_func();}
	}	
}



static uint32_t auto_test_work_chk_func	(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len)
{
	if(event == AUTO_TEST_EV_TEST_ITEM_OK){ p_fsm->next_state = AUTO_TEST_CHANGE_ITEM; return 1;}
	
	if(event == AUTO_TEST_EV_TEST_ITEM_NG){ p_fsm->next_state = AUTO_TEST_NG; return 1;}
	
	return 0;
}


//-------------------change item
static void auto_test_change_item_in	(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
	if(auto_test_obj.cur_exe_node->next!=&auto_test_obj.head)
	{
		auto_test_obj.cur_exe_node = auto_test_obj.cur_exe_node->next;
		fsm_send_event(AUTO_TEST_FSM_ID,AUTO_TEST_EV_TEST_NEXT_ITEM,NULL,0);
	}
	else
	{
		fsm_send_event(AUTO_TEST_FSM_ID,AUTO_TEST_EV_TEST_ALL_OK,NULL,0);
	}	
}

static uint32_t auto_test_change_item_chk_func	(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len)
{
	if(event == AUTO_TEST_EV_TEST_NEXT_ITEM){ p_fsm->next_state = AUTO_TEST_WORK; return 1;}
	
	if(event == AUTO_TEST_EV_TEST_ALL_OK){ p_fsm->next_state = AUTO_TEST_OK; return 1;}
	
	return 0;
}



//-------------------TEST OK
static void auto_test_OK_exe	(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
	
	if(auto_test_obj.ctrl_func_set->auto_test_OK_func!=NULL) {auto_test_obj.ctrl_func_set->auto_test_OK_func(NULL,0,0,0);}
	
}

static uint32_t auto_test_OK_chk_func	(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len)
{
	if(event == AUTO_TEST_EV_TEST_FINISH){ p_fsm->next_state = AUTO_TEST_END; return 1;}
	
	return 0;
}

//-------------------TEST NG
static void auto_test_NG_exe	(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
	if(auto_test_obj.ctrl_func_set->auto_test_NG_func!=NULL) {auto_test_obj.ctrl_func_set->auto_test_NG_func(NULL,auto_test_obj.err_code,0,0);}

}

static uint32_t auto_test_NG_chk_func	(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len)
{
	if(event == AUTO_TEST_EV_TEST_FINISH){ p_fsm->next_state = AUTO_TEST_END; return 1;}
	
	return 0;
}

//----end
static void auto_test_end_in	(fsm_item_t *p_fsm, uint8_t *pbuf, uint32_t len)
{
	DBG_I("----------auto_test_end-----");
	auto_test_re_start_test();
}

static uint32_t auto_test_end_chk_func	(fsm_item_t *p_fsm, uint32_t event, uint8_t *pbuf, uint32_t len)
{
	if(event == AUTO_TEST_EV_TEST_RE_START){ p_fsm->next_state = AUTO_TEST_INIT; return 1;}
	
	return 0;
}


//--------FSM_ID    run_time	param_len	start_state
REG_FSM(AUTO_TEST_FSM_ID, 1, 	0			,AUTO_TEST_INIT );
//------------FSM_ID			state					in_func					exe_func    				out_func		event_func
REG_FSM_STATE(AUTO_TEST_FSM_ID, AUTO_TEST_INIT,			auto_test_init_in    	,auto_test_init_in_exe  	,NULL			,auto_test_init_chk_func  		);
REG_FSM_STATE(AUTO_TEST_FSM_ID, AUTO_TEST_WORK,			auto_test_work_in		,auto_test_work_in_exe		,NULL			,auto_test_work_chk_func   		);
REG_FSM_STATE(AUTO_TEST_FSM_ID, AUTO_TEST_CHANGE_ITEM,	auto_test_change_item_in,NULL					    ,NULL			,auto_test_change_item_chk_func	);
REG_FSM_STATE(AUTO_TEST_FSM_ID, AUTO_TEST_NG,			NULL    				,auto_test_NG_exe			,NULL			,auto_test_NG_chk_func  		);
REG_FSM_STATE(AUTO_TEST_FSM_ID, AUTO_TEST_OK,			NULL    				,auto_test_OK_exe			,NULL			,auto_test_OK_chk_func       	);
REG_FSM_STATE(AUTO_TEST_FSM_ID, AUTO_TEST_END,			auto_test_end_in    	,NULL      					,NULL			,auto_test_end_chk_func	     	);



void show_auto_test_item(void)
{
	auto_test_item_t * p_cur_test_item;
	DBG_I("\r\n[AUTO_TEST]  --- show test item --- total:%d",auto_test_obj.total);
	list_for_each_entry(p_cur_test_item,&auto_test_obj.head,node)
	{
		DBG_RAW("[%2d  seq:%d]:%s\r\n",p_cur_test_item->p_cfg->test_index,p_cur_test_item->seq_num,p_cur_test_item->p_cfg->test_name);
	}
	
}

int32_t show_auto_test_item_cmd(uint8_t argc, uint8_t *argv[])
{
	show_auto_test_item();
	return 0;
}
REG_SHELL_CMD(show_test_item,0,show_auto_test_item_cmd, "display all auto test items","show_test_item ");


//=======================================
#define JSON_BUF_SIZE   512
static uint8_t json_buf[JSON_BUF_SIZE]={0};
static uint32_t  json_index  = 0;


void create_test_ctrl_info(const char* proj_name,const char* board_name,const char* step_name,const char* id_info)
{
	 uint32_t len = 0;
	 json_index = 0 ;
	 len = snprintf((char*)&json_buf[json_index],(JSON_BUF_SIZE-json_index-1),"{\"P\":\"%s\",",proj_name);
	 json_index+=len;
	
	 len = snprintf((char*)&json_buf[json_index],(JSON_BUF_SIZE-json_index-1),"\"B\":\"%s\",",board_name);
	 json_index+=len;
	
	 len = snprintf((char*)&json_buf[json_index],(JSON_BUF_SIZE-json_index-1),"\"S\":\"%s\",",step_name );
	 json_index+=len;
	
	 len = snprintf((char*)&json_buf[json_index],(JSON_BUF_SIZE-json_index-1),"\"ID\":\"%s\",\"DATA\":{" ,id_info);
	 json_index+=len;
	
}

void add_test_case_info(const char* key_name,float key_val)
{
	uint32_t len = 0;
	len = snprintf((char*)&json_buf[json_index],(JSON_BUF_SIZE-json_index-1),"\"%s\":\%.3f,",key_name,key_val);
	json_index+=len;
}

void end_test_case_info()
{
    if(json_index>0){json_index--;}
	json_buf[json_index]='}';
	json_index++;
	json_buf[json_index]='}';
	json_index++;
}

uint8_t* get_test_json_ptr(void)
{
    return json_buf;
}

uint32_t get_test_json_len(void)
{
    return json_index;
}

#endif //ENABLE_AUTOTEST
