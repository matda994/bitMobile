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

int pidController(double d_lateral)
{
		error = d_ref - d_lateral;
		derivative = (error-error_prior)/dt;
		output = KP*error + KD*derivative;
		error_prior = error;
		
		return (int) output;	
}	



int main(void)
{
	//Feedback loop 
	

	return 1;
}
