#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E

#include "cfm.h"

#if ENABLE_CFM == 1
static void confirm_zone_item(confirm_state_f_t* p_confirm,float var)
{
	//uint8_t flg = 0;
	
	//DBG_I("dir:%d var:%f  in:%f  out_%f",p_confirm->judge_dir,var,p_confirm->in_level,p_confirm->out_level);
	switch(p_confirm->state)
	{
		case 0: {p_confirm->cur_ms = get_sys_ms(); p_confirm->state  = 1;}
		
		case 1: 
				{	
					if(p_confirm->judge_dir == LARGER_THAN_IN_LEVEL)
					{
						if(var>=(p_confirm->in_level) )
						{
							if( (p_confirm->cur_ms+p_confirm->confirm_in_ms) <= get_sys_ms())
							{
								p_confirm->state  = 2;
								p_confirm->in_func();
								p_confirm->cur_ms = get_sys_ms();
							}
						}
						else
						{
							p_confirm->cur_ms = get_sys_ms();
						}
					}
					else
					{
						if(var<=(p_confirm->in_level) )
						{
							if( (p_confirm->cur_ms+p_confirm->confirm_in_ms) <= get_sys_ms())
							{
								p_confirm->state  = 2;
								p_confirm->in_func();
								p_confirm->cur_ms = get_sys_ms();
							}
						}
						else
						{
							p_confirm->cur_ms = get_sys_ms();
						}
					}
					
				}
				break;
		case 2:
				{
					if(p_confirm->judge_dir == LARGER_THAN_IN_LEVEL)
					{
						if(var<=(p_confirm->out_level))
						{
							if( (p_confirm->cur_ms+p_confirm->confirm_out_ms) <= get_sys_ms())
							{
								p_confirm->state  = 1;
								p_confirm->out_func();
								p_confirm->cur_ms = get_sys_ms();
							}
						}
						else
						{
							p_confirm->cur_ms = get_sys_ms();
						}
					}
					else
					{
						if(var>=(p_confirm->out_level))
						{
							if( (p_confirm->cur_ms+p_confirm->confirm_out_ms) <= get_sys_ms())
							{
								p_confirm->state  = 1;
								p_confirm->out_func();
								p_confirm->cur_ms = get_sys_ms();
							}
						}
						else
						{
							p_confirm->cur_ms = get_sys_ms();
						}
					}
				}
				break;
	   default: break;
	}
}


void confirm_zone_chk(confirm_state_f_t* p_confirm,uint32_t tab_cnt,float var)
{
	uint32_t i = 0;
	for(i=0;i<tab_cnt;i++)
	{
		confirm_zone_item(&p_confirm[i],var);
	}
}

#endif
