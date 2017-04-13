/*
 * servo.c
 *
 * Created: 4/7/2017 11:27:27 AM
 *  Author: hjaek237
 */ 



#include <asf.h>
#include <avr/interrupt.h>
#include <avr/io.h>

// This file includes methods for controlling the servo
void setServo(int counterServo);
void setESC(int counterESC);
void pwmInit(void);



// Declaration of variables and constants for PWM
int MAXLEFT = 3060;
int MAXRIGHT = 2200;
int STRAIGHT = 2870;
int NEUTRAL = 2764;			// 1.5 ms
int MANUAL_FORWARD = 2840;
int MANUAL_REVERSE = 2664;


/*
Configuration setup for PWM signals in Timer/Counter 1
Pins 19(OC1A) and 18(OC1B) setup for output for ESC and steering servo respectively
TCCR1A,
*/
void pwmInit(void)
{
	DDRD = (1<<DDD5)|(1<<DDD4);
	TCCR1A = (1<<WGM11)|(1<<COM1A1)|(1<<COM1B1);
	ICR1 = 36863;
	TCCR1B = (1<<CS11)|(1<<WGM12)|(1<<WGM13);
}

/*
1843 => 1ms very high reverse
2690 => 1.46 ms high reverse
2765 => 1.5 ms
*/
void setESC(int counterEsc){

	if(counterEsc >= 2700 || counterEsc <= 2950)
	{
		OCR1A = counterEsc;
		} else {
		OCR1A = NEUTRAL;
	}
}

/* This method takes a counter value and sets the servo pulse width if valid value.
 * @parameter int counterServo is a value between [2200,3060]
 *
 * During testing the following was found:
 *
 * 1843 => 1 ms too much right
 * 2000 => 1.08 ms too much right
 * 2100 => 1.14 ms too much right
 *
 * APPROX MAX RIGHT: 2200 => 1.196 ms
 *
 *
 * 2690 => 1.459 ms STRAIGHT
 * 2764 => 1.5 ms slight left/center
 *
 * APPROX MAX LEFT: 3060 => 1.66 ms
 *
 * 3086 => 1.68 ms too much left
 * 3386 => 1.86 ms too much left
 * 3586 => 1.96 ms too much left
 * 3686 => 2.0 ms too much left
*/
void setServo (int counterServo)
{
	if (counterServo <= MAXRIGHT ){
		
		OCR1B =MAXRIGHT;
		
	}	else if (counterServo >= MAXLEFT){

		OCR1B = MAXLEFT;
		
	} else {
		
		OCR1B = counterServo;	
	}
}