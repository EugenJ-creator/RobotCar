

#include <stdint.h>
#include "I2C3.h"




// Trigger temp and humidity measurement
// Inputs:  none
// Outputs: none
void TempHumiditySensor_Start(void);




// Read temp and humidity data from the sensor
// Input: Temp & Humidity is pointer to store
// Output: Void
// Assumes: TempHumiditySensor_Start() has been called
void TempHumiditySensor_End(uint8_t *tempHumidityData);
