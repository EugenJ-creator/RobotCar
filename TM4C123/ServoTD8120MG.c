// Implemented by Evgheni Jaruc

#include "tm4c123gh6pm.h"
//#include "timer.h"
#include "ServoTD8120MG.h"
#include <stdint.h>
#include "PWM.h"
#include "os.h"
#include "Profile.h"
#include "Texas.h"
#include "CortexM.h"
#include "Math.h"


int32_t absolute(int32_t value);

//int32_t lastPosition;

//int32_t static referenz;
int32_t Duty;




// ------------Control the Period of PWM for Stearing Servo Motor------------
// Trigger pulse in mikrosec, offset = 890 Mikrosec, 90 Degree = 1340 Mikrosec , Pulse width = 1565
// Input: degree is an integer 0..180 , 0 Right, 180 Left, that indicates how many takts has duty cycle (min = 890, max = 1760 )
// Output: Void

void Angle(uint32_t degree){
		//DisableInterrupts();



			if (degree>50)
			{
				Duty = (((degree-50) * 405)/50)+1145;
			} 
			else if (degree<50)
			{
				Duty = ((degree * 305)/50)+840;
			}
			else if (degree==50)
			{
				Duty = 1145;
			}
			
			
			
			
		
			PWM0_3_A_disable();
			// New Duty Cycle
			PWM0_3_A_Duty( (5*Duty));
			// Activate PWM Signal
			PWM0_3_A_enable();
				//EnableInterrupts();	
	}
		

// delay in microsec, target in degrees
	
	
	
void servoupdownarm(uint32_t target , uint32_t delay){
	
	
	Angle(target);


}
								



