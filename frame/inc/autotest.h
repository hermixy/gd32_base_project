#ifndef __AUTOTEST_H__
#define __AUTOTEST_H__

#include "x_frame.h"

//----------------------------------------auto test
#define REG_AUTO_TEST_ITEM(name,index,in_func,exe_func) \
        const auto_test_cfg_t  auto_test_cfg_##name= {#name,index,in_func,exe_func};\
        auto_test_item_t       auto_test_item_##name = {& auto_test_cfg_##name,1};\
        AUTO_REG_ITEM(auto_test_item_##name,AUTO_REG_AUTOTEST,(void*)&auto_test_item_##name)
			
#define REG_AUTO_TEST_CTRL(in_func,exe_func,start_func,NG_func,OK_func) \
        const auto_test_ctrl_t  auto_test_ctrl_obj= {in_func,exe_func,start_func,NG_func,OK_func};\
        AUTO_REG_ITEM(auto_test_ctrl_obj,AUTO_REG_AUTOTEST_CTRL,(void*)&auto_test_ctrl_obj)		
		
	

//----------------- auto test mod
typedef  struct
{
	const char* 		test_name;
	uint8_t				test_index;
	pf_void_func		in_func;
	pf_void_func		exe_func;	
}auto_test_cfg_t;

typedef struct
{
	const auto_test_cfg_t* 	p_cfg;
	uint8_t					enable;
	uint8_t					seq_num;
	list_t					node;
}auto_test_item_t;


typedef void (*pf_auto_test_exe)(auto_test_item_t* p_item,uint32_t err_code,uint8_t cur_step,uint8_t total);
typedef uint8_t (*pf_auto_test_start_exe)(void);

typedef struct
{
	pf_void_func			auto_test_in_func;
	pf_auto_test_exe		auto_test_exe_func;
	pf_auto_test_start_exe	auto_test_start_func;
	pf_auto_test_exe		auto_test_NG_func;
	pf_auto_test_exe		auto_test_OK_func;
}auto_test_ctrl_t;


void auto_test_emit_result(uint32_t err_code);
void auto_test_finish_test(void);
		
void 		create_test_ctrl_info	(const char* proj_name,const char* board_name,const char* step_name,const char* id_info);
void 		add_test_case_info		(const char* key_name,float key_val);
void 		end_test_case_info		(void);
uint8_t* 	get_test_json_ptr		(void);
uint32_t 	get_test_json_len		(void);


#endif //__PARAM_CFG_H__
