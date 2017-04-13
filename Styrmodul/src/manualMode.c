/*
 * manualMode.c
 *
 * Created: 4/7/2017 11:44:01 AM
 *  Author: hjaek237
 */ 

#include <servo.h>
#include <avr/interrupt.h>


int manualInstruction[4] = {0,0,0,0}; // right,forward,left,reverse

/*
 * This method performs the manual mode of driving
 * The manual instructions are forward, left, right, reverse stored in array manualInstruction
 */
void manualMode(void)
{
	cli(); //disable interrupts
	
	// This if-statement controls the steering
	if (manualInstruction[0]==1) // Right key is pressed
	{
		if (manualInstruction[2]==1) // Conflict with left key
		{
			setServo(STRAIGHT);
		} 
		else 
		{
			setServo(MAXRIGHT);
		}
	}
	else if (manualInstruction[2]==1) // Only left key is pressed
	{
		setServo(MAXLEFT);
	}
	else
	{ 
		setServo(STRAIGHT);
	}
	
	// This if-statement controls the speed
	if (manualInstruction[1]==1) // Up key is pressed 
	{
		if (manualInstruction[3]==1) // Conflict with down key
		{
			setESC(NEUTRAL);	
		}
		else 
		{
			setESC(MANUAL_FORWARD);
		}
	}
	else if (manualInstruction[3]==1)
	{
		setESC(MANUAL_REVERSE);
	}
	
	else
	{
		setESC(NEUTRAL);
	}
	
	sei(); // enable global interrupts
}