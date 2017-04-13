#include <asf.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define F_CPU 14745600

int pidController(double d_lateral);

double error = 0;
double error_prior = 0;
double KP = 1; 
double KD = 0; 
double v_ref = 0; 
double v = 0;
double derivative = 0; 
double dt = 1;	 
double output = 0;
static double d_ref = 25.0;

double distance = 0;
double notDetected = 2;
double detected = 1; 
double close = 0.5;
double cruise;
double low;
double lowrwe4w;

int pidController(double d_lateral)
{
		error = d_ref - d_lateral;
		derivative = (error-error_prior)/dt;
		output = KP*error + KD*derivative;
		error_prior = error;
		
		return (int) output;	
}	



/**
 * \file
 *
 * \Styrmodulen
 *
 */

#define F_CPU 14745600

#include <asf.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <testmanual.h>
#include "servo.h"
#include "manualMode.h"
#include <uart_buffer.h>
#include "FLC.h"


// from facebook

void USART0_init(unsigned int baud_setting);

// Headers for methods
void SPI_slaveInit(void);

void carInit(void);



int elapsedSeconds = 0;
int SPI_FLAG = 0;

// Declaration of input signals
int carInitialized = 0;
int drivingMode[6] = {0,0,0,0,0,0}; // if index i is set to 1 => driving mode i as specified by {courtyard,road,intersection,parkingLot,garage,manual}
double speed = 0;
int speedCounter = 0;
int speedInterrupt = 0;
double circleArch = 0.053;


int PID_Control(double d_lateral);

double error = 0;
double error_prior = 0;
double KP = 1;
double KD = 2; ///KD > KP always!!!
double derivative = 0;
double dt = 1;
double output = 0;
double d_ref = 25.0; // cm to the right side line markings

int PID_Control(double d_lateral)
{
	error = d_ref - d_lateral;
	derivative = (error - error_prior)/dt;
	output = KP*error + KD*derivative;
	error_prior = error;
	
	if (output > 370)
	{
		output = 370;
	}
	else if (output < -490)
	{
		output = -490;
	}
	return (int)output;
}




ISR(INT0_vect)
{
	
	if (speedCounter == 0)
	{
		TCCR3A = (1<<CS31)|(1<<CS30);
	}
	speedInterrupt ++;
	if (speedInterrupt ==2)
	{
		speedCounter = TCNT3;
		speed = 0.053*230400.0/speedCounter;
		TCCR3A = (0<<CS31)|(0<<CS30);
	}
}


/*
 * SPI port definitions
 * PB7 SCK (Master clock)
 * PB6 Slave output (MISO)
 * PB5 Slave input (MOSI)
 * PB4 Slave select (SS)
 */
void SPI_slaveInit(void)
{
	DDRB = (1<<DDB6); // Set MISO output, all others input
	SPCR = (1<<SPE)|(1<<SPIE); // Enables SPI
}



void carInit(void)
{
	pwmInit();
	SPI_slaveInit();
	setESC(NEUTRAL);
	setServo(STRAIGHT);
	_delay_ms(5000);
}





void USART0_init(unsigned int baud_setting)
{
	UBRR0 = baud_setting;
	//
	UCSR0B = (1<<RXEN0) | (0<<TXEN0) | (0<<RXCIE0) | (0<<TXCIE0)| (0<<UDRIE0);
	//Set frame format: 8data, 2 stop bit
	UCSR0C = (1<<USBS0) | (3<<UCSZ00);
	
	
	//Setting baud rate
	UBRR0 = baud_setting;
}







int main (void)
{
	initializeTest();
	
	
	
	
	fuzzy();
	
	
	int clock = TCNT3;
	
	/*
	 *
	 
	 DDRA = 0xFF;
	unsigned char baud_setting =  7;
	USART0_init(baud_setting);
	int i= 0;
	carInit();
	
	
	unsigned char commando;
	while(1)
	{
		


		while ( !(UCSR0A & (1<<RXC0)) )
		;	
		
		commando = UDR0;
		
		PORTA |= (1<<PORTA1);
		testmanual(commando);
			

	}	
	 */
	
	

}
