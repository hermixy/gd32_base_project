#include "avg_filter.h"
#include "stdlib.h"



float move_average_run(move_avg_obj_t* move_obj,float push_data)
{
    float ret = 0;
	float min_val = 0;
	float max_val = 0;
    uint32_t i = 0;
    uint32_t deep = 0;
    move_obj->pavg_buf[move_obj->cur_write_index] = push_data;
    move_obj->cur_write_index++;
    if(move_obj->cur_write_index>=move_obj->buf_deep){move_obj->cur_write_index = 0;move_obj->flag_is_full=1;}

	/*不足一个深度的时候，计算当前实际有的平均值*/
    if(move_obj->flag_is_full)	{ deep = move_obj->buf_deep; }  
    else						{ deep = move_obj->cur_write_index;}
    
	min_val = move_obj->pavg_buf[0];
	max_val = move_obj->pavg_buf[0];
	if(deep<3){return 0;}
    for(i=0;i<deep;i++)			
	{
		if(move_obj->pavg_buf[i]<min_val){min_val = move_obj->pavg_buf[i];}
		if(move_obj->pavg_buf[i]>max_val){max_val = move_obj->pavg_buf[i];}
		ret+=move_obj->pavg_buf[i]; 
	}
	
	ret = ret - min_val - max_val;

    return ret/(deep-2);
}


float move_avg_clean(move_avg_obj_t* move_obj)
{
	move_obj->cur_write_index = 0;
	move_obj->flag_is_full = 0;
	return 1;
}


float move_avg_reinit(move_avg_obj_t* move_obj,float init_val)
{
	move_obj->cur_write_index = 3;
	move_obj->flag_is_full = 0;
    
    move_obj->pavg_buf[0] = init_val;
    move_obj->pavg_buf[1] = init_val;
    move_obj->pavg_buf[2] = init_val;
    
	return 1;
}
