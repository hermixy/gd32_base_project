#ifndef __CFM_H__
#define __CFM_H__

#include "x_frame.h"

//------------------delay confirm

#define REG_CFM_TAB(name,...)   confirm_state_f_t   confirm_state_tab_##name[]={\
							__VA_ARGS__ }; uint32_t confirm_state_tab_cnt_##name = sizeof(confirm_state_tab_##name)/sizeof(confirm_state_f_t);		

		
#define RUN_CFM(name,var) confirm_zone_chk(confirm_state_tab_##name,confirm_state_tab_cnt_##name,var)	
	

#define PTR_CFM_TAB(name)	 confirm_state_tab_##name
	
							

#define LARGER_THAN_IN_LEVEL 	0
#define LESS_THAN_IN_LEVEL 		1

typedef struct
{
	uint8_t			judge_dir;   //0--lager than in_level  1-less than in_level
	float			in_level;
	uint32_t		confirm_in_ms;
	pf_void_func	in_func;
	
	float			out_level;
	uint32_t		confirm_out_ms;
	pf_void_func	out_func;
	
	uint8_t 		state;   //0-not init  1-normnal  2-abnormal
	uint32_t    	cur_ms;
	
}confirm_state_f_t;

void confirm_zone_chk(confirm_state_f_t* p_confirm,uint32_t tab_cnt,float var);

#endif //__CFM_H__
