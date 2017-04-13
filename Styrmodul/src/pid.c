#include "pid.h"
#include "stdint.h"
 
 void pid_Init(int16_t p_factor, int16_t d_factor, struct PID_DATA *pid)
 {
	 pid->lastProcessValue = 0;
	 pid->P_Factor = p_factor;
	 pid->D_Factor = d_factor;
	 pid->maxError    = MAX_INT / (pid->P_Factor + 1);
 }
 
 int16_t pid_Controller(int16_t referenceValue, int16_t processValue, struct PID_DATA *pid_st)
 {
	 int16_t errors, p_term, d_term;
	 int32_t ret;

	 errors = referenceValue - processValue;

	 if (errors > pid_st->maxError) {
		 p_term = MAX_INT;
		 } else if (errors < -pid_st->maxError) {
		 p_term = -MAX_INT;
		 } else {
		 p_term = pid_st->P_Factor * errors;
	 }
	 
	 d_term = pid_st->D_Factor * (pid_st->lastProcessValue - processValue);

	 pid_st->lastProcessValue = processValue;

	 ret = (p_term + d_term) / SCALING_FACTOR;
	 if (ret > MAX_INT) {
		 ret = MAX_INT;
		 } else if (ret < -MAX_INT) {
		 ret = -MAX_INT;
	 }

	 return ((int16_t)ret);
 }
