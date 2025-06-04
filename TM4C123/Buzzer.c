
// Implemented by Evgheni Jaruc

#include <stdint.h>
#include "Buzzer.h"
#include "PWM.h"



// ------------Control the Period of PWM for Buzzer------------


// Input: volume is an integer , that indikates how many takts has duty cycle (max 2500 = 1 msec = 1 KHz, 1 mikrosec = 2,5 Takts )
// Output: Void
void BuzzerOn(uint32_t volume){
	
	// New Duty Cycle
	PWM0_0_B_Duty(volume-1);
	// Activate PWM Signal
	PWM0_0_B_enable();

}	

// ------------Deactivate the buzzer------------
// Output: Void
void BuzzerOff(){
	
	PWM0_0_B_disable();
}
