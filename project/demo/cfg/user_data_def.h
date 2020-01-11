#ifndef __USER_DATA_DEF_H__
#define __USER_DATA_DEF_H__
#include "x_frame.h"

extern uint8_t enable_dbg_mode;

#define FSM_ID_PD_STATE 	1
enum ef03_pd_state
{
	FSM_POWER_ON_STATE  = 0 ,
	FSM_POWER_OFF_STATE,
};

enum
{
	EV_POWER_ON = 0,
	EV_POWER_OFF,
};




typedef struct
{
	uint8_t 	beep_state;
} ctrl_t;

typedef struct
{
	ctrl_t 	ctrl;
} ef_state_t;

extern ef_state_t ef_state;
	
#endif //__USER_DATA_DEF_H__
