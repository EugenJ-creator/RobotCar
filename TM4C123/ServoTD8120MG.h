
#define PERIOD_SERVO 2500  //  Mikrosec
#define POSITION_NULL 1340
#define NULLPOSITION 90
#include <stdint.h>

// ------------Control the Period of PWM for Stearing Servo Motor------------
// Trigger pulse in mikrosec, offset = 890 Mikrosec, 90 Degree = 1340 Mikrosec , Pulse width = 1565
// Input: degree is an integer 0..180 , 0 Right, 180 Left, that indicates how many takts has duty cycle (min = 890, max = 1760 )
// Output: Void
void Angle(uint32_t degree);
void servoupdownarm(uint32_t target , uint32_t delay);
