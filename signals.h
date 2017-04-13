/*
 * signals.h
 *
 * Created: 4/11/2017 11:17:37 AM
 *  Author: hjaek237
 */ 


#ifndef SIGNALS_H_
#define SIGNALS_H_

typedef struct Input_signals {
	//! Last process value, used to find derivative of process value.
	int16_t lateral_distance;
};

typedef struct Output_signals {
	// Input to Servo
	int16_t servoSignal;
	// Input to ESC
	int16_t escSignal;
	};



#endif /* SIGNALS_H_ */