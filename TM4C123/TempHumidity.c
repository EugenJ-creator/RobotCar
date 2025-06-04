// Implemented by Evgheni Jaruc

#include <stdint.h>
#include "I2C3.h"
#include "TempHumidity.h"


#define I2C3_MASTER_MCS_R       (*((volatile unsigned long *)0x40023004))   //  0x4002X004  X is the number of IIC Modul

//unsigned short rawDataShort = 0;             // 16-bit data straight from thermometer
//uint32_t rawDataLong = 0;             // 16-bit data straight from thermometer

uint8_t rawData[6]; // Array with received Raw data, First Byte we are not using

float temperature = 0;
float humidity =0;



// Trigger temp and humidity measurement
// Inputs:  none
// Outputs: none
void TempHumiditySensor_Start(void){
	
//	I2C_Send1(0x38, 0xBA); //Soft Reset
//	Delay(4166667); 
	
//	I2C_Send1(0x38, 0xE1); //Initialization
//	Delay(4166667); 
	
	I2C3_Send3(0x38, 0xAC, 0x33, 0x00);  // Send request to measure the temp and humidity
	I2C3_Send1(0x38, 0);                     // use command 0 to set pointer to temperature
}
	

// Read temp and humidity data from the sensor
// Input: Temp & Humidity is pointer to store
// Output: Void
// Assumes: TempHumiditySensor_Start() has been called
void TempHumiditySensor_End(uint8_t *tempHumidityData)	{

	I2C3_Recv6(0x38, tempHumidityData);            // rawData[0} MSB     rawData[4} LSB     temperature data stored in 12 or 13 MSBs (Figure 7.XX chapter7-10-3.ulb, reply 0x1660, 22.375 C)


////Calculate humidity, byte1, byte2, 4bits MSB from byte 3
//	uint32_t h = tempHumidityData[1];
//	h <<= 8;
//	h |= tempHumidityData[2];
//	h <<= 4;
//	h |= tempHumidityData[3] >> 4;
//  humidity = ((float)h * 100) / 0x100000;   
//	//tempHumidityData[0] = humidity;
//	
////Calculate temp , 4bits LSB from byte 3, byte4, byte5 
//	uint32_t tdata = tempHumidityData[3] & 0x0F;
//	tdata <<= 8;
//	tdata |= tempHumidityData[4];
//	tdata <<= 8;
//	tdata |= tempHumidityData[5];
//	temperature = ((float)tdata * 200 / 0x100000) - 50;
//	//tempHumidityData[1] = temperature;

//  return temperature;
}


