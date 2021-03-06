#include "stdint.h"
#include "pid.h"
#include "asf.h"

#define K_P 1.00
#define K_D 0.00

struct GLOBAL_FLAGS {
	//! True when PID control loop should run one time
	uint8_t pidTimer : 1;
	uint8_t dummy : 7;
	} gFlags = {0, 0};
		
struct PID_DATA pidData;

/*! \brief Sampling Time Interval
 *
 * Specify the desired PID sample time interval
 * With a 8-bit counter (255 cylces to overflow), the time interval value is calculated as follows:
 * TIME_INTERVAL = ( desired interval [sec] ) * ( frequency [Hz] ) / 255
 */

#define TIME_INTERVAL 157 //EXAMPLE //TODO

ISR(TIMER0_OVF_vect)
{
	static uint16_t i = 0;

	if (i < TIME_INTERVAL) {
		i++;
		} else {
		gFlags.pidTimer = 1;
		i               = 0;
	}
}

void Init(void)
{
	pid_Init(K_P * SCALING_FACTOR, K_D * SCALING_FACTOR, &pidData);

	// Set up timer, enable timer/counter 0 overflow interrupt
	TCCR0B = (1 << CS00); // clock source to be used by the Timer/Counter clkI/O
	TIMSK0 = (1 << TOIE0);
	TCNT0  = 0;
}
int16_t Get_Reference(void) //TODO
{
	return 8; 
}

int16_t Get_Measurement(void) //TODO
{
	return 4; 
}

void Set_Input(int16_t inputValue) //TODO
{
	;
}

void main(void)
{
	int16_t referenceValue, measurementValue, inputValue;
	
	// Configure Power reduction register to enable the Timer0 module
	// Atmel START code by default configures PRR to reduce the power consumption.
	sei();

	while (1) {
		// Run PID calculations once every PID timer timeout
		if (gFlags.pidTimer == TRUE) {
			referenceValue   = Get_Reference();
			measurementValue = Get_Measurement();

			inputValue = pid_Controller(referenceValue, measurementValue, &pidData);

			Set_Input(inputValue);

			gFlags.pidTimer = FALSE;
		}
	}
}
