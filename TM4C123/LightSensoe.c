// Implemented by Evgheni Jaruc

#include "ADCT0.h"
#include "LightSensor.h"
#include "../inc/tm4c123gh6pm.h"


void EnableInterrupts(void);




void AD_Init(void){
	
	ADC0_InitTimer0ATriggerSeq3(0, 8000000); // ADC channel 0, 10 Hz sampling
	EnableInterrupts();
}

