#include "pid.h"
#include "stdlib.h"

//#include "arm_math.h"
//#include "math.h"

#define RESET_VAL  0.0f

void 	pid_init					(pid_t* pid)
{
	pid->output  		= RESET_VAL;
	pid->integral 		= RESET_VAL;
}
void 	pid_set_kp					(pid_t* pid,float kp)
{
	pid->kp				= kp;
}
void 	pid_set_ki					(pid_t* pid,float ki)
{
	pid->ki				= ki;
}
void 	pid_set_kd					(pid_t* pid,float kd)
{
	pid->kd				= kd;
}

void 	pid_reset_integral		(pid_t* pid)
{
	pid->integral = 0.0f;
}

void 	pid_set_intergral_limit		(pid_t* pid,float integral_limit)
{
	pid->integral_limit = integral_limit;
}
void 	pid_set_tag					(pid_t* pid,float tag)
{
	pid->tag				= tag;
}

void 	pid_set_output_limit		(pid_t* pid,float output_limit)
{
	pid->output_limit 	= output_limit;
}


#if 1
float 	pid_calc					(pid_t* pid,float cur,float dt)
{
	float       integral_temp = 0;;
	
	pid->cur    	= cur;
	pid->err 		= pid->tag - cur;
//	if(fabs(pid->err) < pid->dead_zone)
//	{
//		pid->err = 0.0f;
//	}
	pid->dt 		= dt;
	if(dt!=0)		{pid->d_val  = (pid->err - pid->last_err)/dt;}
	else 			{pid->d_val = 0;}
	
	pid->last_err 	=	pid->err;
	
	
	
	pid->d_val = (pid->coeff_d*pid->d_val) + (1.0f-pid->coeff_d)*pid->d_last;
	pid->d_last = pid->d_val;
	
	//---P + D
	pid->output = (pid->err * pid->kp) + (pid->d_val * pid->kd);
	//pid->output = (pid->err * pid->kp);
	
	//---I
	
	if(fabsf(pid->err<100.0f))//err must less than 1%
	{
		integral_temp = pid->integral +  (pid->err * pid->dt);
	}
	else
	{
		integral_temp = 0.0f;
	}
	
	
	if(integral_temp > pid->integral_limit) 	     	{integral_temp = pid->integral_limit; }
	if(integral_temp < (-1.0f*pid->integral_limit))	 {integral_temp = pid->integral_limit*(-1.0f); }
	
	if(fabsf(integral_temp)<10000.0f)
	{
		pid->integral = integral_temp;
	}

	pid->output += pid->integral * pid->ki;
	
	if(pid->output >  10000.0f) {pid->output =10000.0f; }
	if(pid->output < -10000.0f) {pid->output = -10000.0f;}
	return pid->output;
}

#else

float 	pid_calc_inc					(pid_t* pid,float cur,float dt)
{
	
	pid->cur    	= cur;
	pid->err 		= pid->tag - cur;
	pid->dt         = dt;
	
	pid->d_cur 		= (pid->err - (2*pid->last_err) + pid->prev_last_err) ;

	pid->d_val 		= (pid->coeff_d*pid->d_cur) + (1.0f-pid->coeff_d)*pid->d_val;
	//---P
	pid->pOut =  pid->kp *(pid->err - pid->last_err) ;
	pid->iOut =  pid->ki * pid->err;
	pid->dOut =  pid->kd * (pid->d_val);

	pid->output += pid->pOut + pid->iOut + pid->dOut;
	
	pid->prev_last_err = pid->last_err;
	pid->last_err      = pid->err ;
	
	if(pid->output >  10000.0f) {pid->output =  10000.0f; }
	if(pid->output <= 0.0f)     {pid->output = 0.0f;}
	return pid->output;
}

float 	pid_calc					(pid_t* pid,float cur,float dt)
{
	float       integral_temp = 0;;
	
	pid->cur    	= cur;
	pid->err 		= pid->tag - cur;

	pid->dt 		= dt;
	if(dt!=0)		{pid->d_val  = (pid->err - pid->last_err)/dt;}
	else 			{pid->d_val = 0;}
	
	pid->last_err 	=	pid->err;
	
	pid->d_val = (pid->coeff_d*pid->d_val) + (1.0f-pid->coeff_d)*pid->d_last;
	pid->d_last = pid->d_val;
	
	//---P + D
	pid->pOut =  (pid->err * pid->kp);
	pid->dOut =  (pid->d_val * pid->kd);
	
	//---I
//	if(fabsf(pid->err<200.0f))//err must less than 1%
//	{
//		pid->i_err += (pid->err * pid->dt);
//	}
//	else
//	{
//		pid->i_err = 0.0f;
//	}
	
	pid->i_err += (pid->err * pid->dt);
	
	if(pid->i_err > pid->integral_limit) 	     	 {pid->i_err = pid->integral_limit; }
	if(pid->i_err < (-1.0f*pid->integral_limit))	 {pid->i_err = pid->integral_limit*(-1.0f); }
	
	integral_temp = pid->ki * pid->i_err;
	
	pid->iOut =  integral_temp;

	pid->output = pid->pOut + pid->iOut + pid->dOut;
	
	if(pid->output > 10000.0f) {pid->output =10000.0f; }
	if(pid->output <= 0.0f)    {pid->output = 0.0f;}
	return pid->output;
}

float 	pid_calc_ac					(pid_t* pid,float cur,float dt)
{
	float       integral_temp = 0;;
	
	pid->cur    	= cur;
	pid->err 		= pid->tag - cur;

	pid->dt 		= dt;
	if(dt!=0)		{pid->d_val  = (pid->err - pid->last_err)/dt;}
	else 			{pid->d_val = 0;}
	
	pid->last_err 	=	pid->err;
	
	pid->d_val = (pid->coeff_d*pid->d_val) + (1.0f-pid->coeff_d)*pid->d_last;
	pid->d_last = pid->d_val;
	
	//---P + D
	pid->pOut =  (pid->err * pid->kp);
	pid->dOut =  (pid->d_val * pid->kd);
	
	//---I
//	if(fabsf(pid->err<200.0f))//err must less than 1%
//	{
//		pid->i_err += (pid->err * pid->dt);
//	}
//	else
//	{
//		pid->i_err = 0.0f;
//	}
	
	pid->i_err += (pid->err * pid->dt);
	
	if(pid->i_err > pid->integral_limit) 	     	 {pid->i_err = pid->integral_limit; }
	if(pid->i_err < (-1.0f*pid->integral_limit))	 {pid->i_err = pid->integral_limit*(-1.0f); }
	
	integral_temp = pid->ki * pid->i_err;
	
	pid->iOut =  integral_temp;

	pid->output = pid->pOut + pid->iOut + pid->dOut;
	
	if(pid->output >  10000.0f) 	{pid->output = 10000.0f; }
	if(pid->output < -10000.0f)     {pid->output =-10000.0f;}
	return pid->output;
}


#endif 

//float pid_realise(float speed)//??pid  
//{
//    pid.set_speed = speed;//??????  
//    pid.error = pid.set_speed - pid.actual_speed;
//    float increment_speed;//??
//     
//    increment_speed = pid.kp*(pid.error-pid.error_next)+pid.ki*pid.error+\
//    pid.kd*(pid.error-2*pid.error_next+pid.error_last);//??????  
//    
//    pid.actual_speed+= increment_speed;
//    pid.error_last = pid.error_next;//?????  
//    pid.error_next = pid.error;
//    return pid.actual_speed; 
//    
//}
