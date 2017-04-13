/*
 * setServo.c
 *
 * Created: 3/31/2017 1:33:19 PM
 *  Author: hjaek237
 */ 

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */

#define F_CPU 50000
#include <asf.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
int pulseWidthEsc =0;
int pulseWidthServo = 0; 


void setServo (int pulseWidthEsc, int pulseWidthServo)
{

	DDRD = (1<<DDD5)|(1<<DDD4);															// Port D, bit 5 is set for PWM output port to ESC (OC1A, pin 19)
																						// Port D, Bit 4 is set for PWM output port to steering servo (OC1B, pin 18)
	TCCR1A = (1<<COM1A1)|(1<<COM1A0)|(1<<COM1B0)|(1<<COM1B1)|(1<<WGM11);				// TCCR0A Register is set to Fast PWM mode with counter 00H-FFH. Clear OCA on compare match and set OCA at bottom. 
		
	ICR1 = 1999;																		// The IRC1 sets the period of the counter as ICR1 = F_CPU /50 where 1/50=0.02 is the period period of the PWM
	
	
	OCR1A = ICR1 - pulseWidthEsc;
	OCR1B = ICR1 - pulseWidthServo;
	
	
	TCCR1B = (1<<CS10)|(1<<WGM12)|(1<<WGM13);	
		

}
