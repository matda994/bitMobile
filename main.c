#include <asf.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define F_CPU 14745600

void pidController(void);

double error = 0;
double error_prior = 0;
double KP = 1; 
double KD = 0; 
double v_ref = 0; 
double v = 0;
double derivative = 0; 
double dt = 1;	 
double output = 0;
double bias = 0; //maybe not include

int main(void)
{
	//Feedback loop
	
	while(1)
	{
		//TODO: reset timer
		//TODO: maybe write code to escape loop on receiving keyboard interrupt?
		error = v_ref - v; 
		derivative = error/dt;
		output = KP*error + KD*derivative + bias;
		error_prior = error;
		
		//setSomething(output) OR setSomething(converter(output)); //input clock cycles or convert clock cycles to velocity
		
		//TODO: "sleep until timer counted to dt" to Synchronize rate
		
	}
	return 1;
}
