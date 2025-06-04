// Implemented by Evgheni Jaruc

#include <stdint.h>
#include "BSP.h"
#include "ServoTD8120MG.h"
#include "PWM.h"
#include "GPIO_DIF.h"
#include "DCMotor.h"
// ------------Control the Period of PWM for DC Motor------------

// Output: Void
// Input: speed 5 Takt in 1 mikros * 250 mikros = 20kHZ, 

void DCforward(uint32_t speed , uint32_t direction){
	
	if (speed == 0) {
		TIMER_1_A_PWM_disable();   // Deaktivate PWM for DC Motor if velocity is 0
		PortB_2_disable();  //  DC IN1 LOW
		PortB_3_disable();  //  DC IN2 LOW

	} else {

		PortB_2_enable();  //  DC IN1 HIGH
		PortB_3_disable();  //  DC IN2 LOW
		
		TIMER_1_A_PWM_disable();
		// New Duty Cycle
		TIMER_1_A_PWM_Duty(speed);
		// Activate PWM Signal
		TIMER_1_A_PWM_enable();

	}
	
	
	
}	
// ------------Control the Period of PWM for DC Motor------------
// 
// Output: Void	
// Input: speed 5 Takt in 1 mikros * 250 mikros = 20kHZ, 
void DCbackward(uint32_t speed , uint32_t direction){

if (speed == 0) {
		TIMER_1_A_PWM_disable();   // Deaktivate PWM for DC Motor if velocity is 0
		PortB_2_disable();  //  DC IN1 LOW
		PortB_3_disable();  //  DC IN2 LOW

	} else {

		PortB_2_disable();  //  DC IN1 LOW
		PortB_3_enable();  //  DC IN2 HIGH
		
		TIMER_1_A_PWM_disable();
		// New Duty Cycle
		TIMER_1_A_PWM_Duty(speed);
		// Activate PWM Signal
		TIMER_1_A_PWM_enable();
	}
	

}
