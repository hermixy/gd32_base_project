#ifndef __AVG_FILTER_H__
#define __AVG_FILTER_H__


#include "x_frame.h"

typedef struct
{
    float       *pavg_buf;
    uint32_t    buf_deep;
    uint32_t    cur_write_index;
    uint8_t     flag_is_full;
}move_avg_obj_t;

float move_avg_clean(move_avg_obj_t* move_obj);
float move_average_run(move_avg_obj_t* move_obj,float push_data);
float move_avg_reinit(move_avg_obj_t* move_obj,float init_val);

#define REG_MOVE_AVG_OBJ(name,deep) float avg_buf_##name[deep]={0}; move_avg_obj_t move_avg_obj_##name={avg_buf_##name,deep,0,0};
#define HANDLE_MOVE_AVG(name)  &move_avg_obj_##name  


#endif	//__AVG_FILTER_H__
