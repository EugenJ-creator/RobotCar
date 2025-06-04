#include <stdint.h>
#include "PWM.h"

#define PERIOD_BUZZER 2000  //  5 takt in 1 mikrsec * 333 mikrosec (3KHZ)  =  1666
#define VOLUME_50 833  // 50% volume for Initialisation  


void BuzzerOn(uint32_t volume);


void BuzzerOff(void);
