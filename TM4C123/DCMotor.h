
#include <stdint.h>
#include "BSP.h"


#define PERIOD_DC 1600  //   , 80 * 50 mikrosec  =  20KHZ (80 takts per 1 mikro second)
#define SPEED_NULL 1  // 1 for Initialisation  

// ------------Control the Period of PWM for DC Motor------------
// Joystick is not stable, so there is a small offset. 
// Position Zero is between 0 and 200
// Input: speed is an integer , that indikates how many takts has duty cycle (max 125 = 50 mikrisec  = 20 KHz, 1 mikrosec = 2,5 Takts )
// Output: Void
void DCforward(uint32_t speed , uint32_t direction);


// ------------Control the Period of PWM for DC Motor------------
// Joystick is not stable, so there is a small offset. 
// Position Zero is between 0 and 200
// Input: speed is an integer , that indikates how many takts has duty cycle (max 125 = 50 mikrisec  = 20 KHz, 1 mikrosec = 2,5 Takts )
// Output: Void
void DCbackward(uint32_t speed , uint32_t direction);
