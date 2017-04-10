#ifndef PID_H_
#define PID_H_

#include "stdint.h"

#define SCALING_FACTOR 128

typedef struct PID_DATA {
	//! Last process value, used to find derivative of process value.
	int16_t lastProcessValue;
	//! The Proportional tuning constant, multiplied with SCALING_FACTOR
	int16_t P_Factor;
	//! The Derivative tuning constant, multiplied with SCALING_FACTOR
	int16_t D_Factor;
	//! Maximum allowed error, avoid overflow
	int16_t maxError;
} pidData_t;

#define MAX_INT INT16_MAX

#define FALSE 0
#define TRUE 1

void pid_Init(int16_t p_factor, int16_t d_factor, struct PID_DATA *pid);
int16_t pid_Controller(int16_t setPoint, int16_t processValue, struct PID_DATA *pid_st);

#endif