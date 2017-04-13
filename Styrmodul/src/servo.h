#ifndef SERVO_H_
#define SERVO_H_

int MAXLEFT;
int MAXRIGHT;
int STRAIGHT;
int NEUTRAL;			// 1.5 ms
int MANUAL_FORWARD;
int MANUAL_REVERSE;


void setServo(int counterServo);
void setESC(int counterESC);
void pwmInit(void);



#endif


