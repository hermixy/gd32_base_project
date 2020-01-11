#ifndef __PID_H__
#define __PID_H__


#include "stdint.h"


typedef struct
{
	float       tag;
	float       cur;
	float 		err;
	float		last_err;
	float		prev_last_err;
	float       i_err;
	float 		d_val;
	float		d_last;
	float       d_cur;
	float		dt;
	float		kp;
	float		ki;
	float		kd;
	float       pOut;
	float       iOut;
	float       dOut;
	float    	integral;
	float    	integral_limit;
	float    	output_limit;
	float       output;
	float		coeff_d;
	float 		dead_zone;
}pid_t;

void 	pid_init					(pid_t* pid);
void 	pid_set_kp					(pid_t* pid,float kp);
void 	pid_set_ki					(pid_t* pid,float ki);
void 	pid_set_kd					(pid_t* pid,float kd);
void 	pid_set_tag					(pid_t* pid,float tag);
void 	pid_set_intergral_limit		(pid_t* pid,float integral_limit);
void 	pid_set_output_limit		(pid_t* pid,float output_limit);
void 	pid_reset_integral			(pid_t* pid);
float 	pid_calc					(pid_t* pid,float cur,float dt);

#endif	// 
