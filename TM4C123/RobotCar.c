// RobotCar.c
// Implemented by Evgheni Jaruc

#include <stdint.h>
#include "BSP.h"
#include "PWM.h"
#include "Profile.h"
#include "Texas.h"
#include "../inc/CortexM.h"
#include "os.h"
#include "ServoTD8120MG.h"
#include "DCMotor.h"
#include "Math.h"
#include "../inc/BSP.h"
#include "../inc/UART0.h"
#include "../inc/Profile.h"
#include "../inc/AP.h"
#include "AP_Services.h"
#include "PLL.h"
#include "GPIO_DIF.h"
#include "Buzzer.h"
#include "I2C3.h"
#include "I2C1.h"
#include "TempHumidity.h"
#include "GyroAccelMag.h"
#include "LightSensor.h"
#include "RobotCar.h"
#include "ADCT0.h"
#include "CanTransmitReceive.h"

#define THREADFREQ 1000   // frequency in Hz of round robin scheduler

//---------------- Global variables shared between tasks ----------------





//  Global parameters
uint32_t SpeedWheels;    // 0...255
uint32_t DirectionWheels = 0;    // 0-Forward/1-back
uint32_t AngleWheels;    // 0...180
//uint32_t SpeedAngleWheels;    // 0..9 Sensitivity
uint32_t BuzzerVolume = 10;  // 0...1600   Initially will have 10 power
uint32_t BuzzerVolumeUpdatedNotNull = 10;  // 0...1600   Initially will have 10 power
uint32_t selectedOptions; // 8 bit   0 bit - tempomat, ...
uint8_t tempHumidityData[6];  // Data Array for bluetooth notification
//int16_t magnetometerData[3];  // x, y, z   little indian
uint8_t magnetometerData[6];  // x, y, z   little indian
int16_t magnetometerData2[3];  // x, y, z   little indian
uint8_t speedPhotoData[3];   // Sensor Speed data
uint32_t HighBeamPower = 100;  // By default Power is 100  (0..250)


uint32_t ControlledSpeed = 0;  // Flag,  if speed is in tempomat modus
uint32_t WarningBlinking = 0;  // Flag,  if warnining blinking is triggered
uint32_t LightAuto = 0;        // Flag,  if auto light is triggered
uint32_t HightBeam = 0;        // Flag,  if High Beam is triggered
uint32_t ParkingLight = 0;     // Flag,  if Parking Light is triggered
uint32_t BuzzerIsOn = 0;       // Flag,  if buzzer is triggered
uint32_t StartAuto = 0;  			 // Flag,  if start is triggered

uint32_t StearingDirection = 0;  //  0 - not blinking, 1- left blinking, 2 -right blinking
uint32_t DCDirection = 0;      //   Rear led is off when 0

extern uint32_t lightIntensity;   //   Light intensity , Measured all 10 HZ
extern uint32_t Period ;              // (1/clock) units
uint32_t lastLightStatus = 0; // Last measured intenity. 0 - light, 1- dark
uint32_t LastWarningBlinkingState = 0; //   Last warning state
uint32_t LastParkingLightState = 0; //   Last Parking Light state
uint32_t LastHighBeamState = 0; //   Last High Beam state
uint32_t LastLightAuto = 0; //   Last Light auto state
uint32_t LastSignalState = 0;  //  Last Backwards signal state
uint32_t LastStartState = 0;   //  Last start state

// semaphores
int32_t NewBluetoothDirectionData;  // true when new Speed Value comes from Bluetooth Client
int32_t NewBluetoothSpeedData;  // true when new Speed Value comes from Bluetooth Client
//int32_t NewBluetoothAngleData;  // true when new Angle Value comes from Bluetooth Client
int32_t AnglePositionChanged; 	// true after triggering new angle position
int32_t BackwardDCPositionChanged; 	// true after triggering new DC Backward position
//int32_t NewBluetoothBuzzerData; // true when new Buzzer volume comes from Bluetooth Client
int32_t NewTempHumidityData; 		// true when new Temp&Humidity Sensor data are ready


int32_t CANmutex; // exclusive access to CAN
int32_t Buzzermutex; // exclusive access to Buzzer

uint8_t SendTempHumidityFlag=0;
uint8_t SendMagnetometerFlag=0;

uint8_t pui8MsgDataT[2];

uint8_t  LeftBlinkingCANObject[] = {2, 3, 1, 4, 0 };
uint8_t  NoBlinkingCANObject[] = {2, 3, 0, 4, 0 };
uint8_t  RightBlinkingCANObject[] = {2, 4, 1, 3, 0 };

uint8_t  WarningBlinkingOnCANObject[] = {4, 5, 1 };
uint8_t  WarningBlinkingOffCANObject[] = {4, 5, 0 };

uint8_t  BackwardLightOnCANObject[] = {1, 2, 1 };
uint8_t  BackwardLightOffCANObject[] = {1, 2, 0 };

uint8_t  DayLightOnCANObject[] = {3, 2, 1 };
uint8_t  DayLightOffCANObject[] = {3, 2, 0 };

uint8_t  LedPWMAutoLightOnCANObject[] = {5, 1, 6 };
uint8_t  LedPWMAutoLightOffCANObject[] = {5, 0, 0 };

uint8_t  LedPWMHighBeamOnCANObject[] = {5, 1, 100 };   // DC PWM  0-255
uint8_t  LedPWMHighrBeamOffCANObject[] = {5, 0, 0 };


void BackwardPeriodicalSignal(void);

//------------ end of Global variables shared between tasks -------------






//*****************************************************************************
//
// This function provides a 1 second delay using a simple polling method.
//
//*****************************************************************************
void
SimpleDelay(void)
{
    //
    // Delay cycles for 1 second
    //
    SysCtlDelay(80000000/3);
}

//---------------- Task0 dummy function ----------------
// *********Task0*********
// Main thread scheduled by OS round robin preemptive scheduler
// Task0 checks for Bluetooth incoming frames but never blocks or sleeps
// Inputs:  none
// Outputs: none
uint32_t Count0;
void Task0(void){ 
	Count0 = 0;
  while(1){
    Count0++;
		AP_BackgroundProcess();
		
		
//		if (SendTempHumidityFlag){
//			if (AP_SendNotification(0)){   // Send data for 0 Characteristic , Temp&Humidity
//				SendTempHumidityFlag = 0;
//			}
//		}
		
//		if (SendMagnetometerFlag){
//			if (AP_SendNotification(1)){   // Send data for 1 Characteristic , Magnetometer
//				SendMagnetometerFlag = 0;
//			}
//		}


		WaitForInterrupt();
	}
}
/* ****************************************** */
/*          End of Task0 Section              */
/* ****************************************** */




// Main thread scheduled by OS round robin preemptive scheduler
// Task1: Controls rear light if car is running backwards
// Inputs:  none
// Outputs: none
void Task1(void){

	
  while(1){


			OS_Wait(&BackwardDCPositionChanged);
			OS_Sleep(5000);     // waits about 1 sec after what Backward DC position was changed
			if (Period==0){  // If still speed of wheels is Null, make Back Led off
				if (DCDirection!=0) {
				OS_Wait(&CANmutex);
				CAN_Send(BackwardLightOffCANObject , 3);
				OS_Signal(&CANmutex);
				BSP_PeriodicTask_StopB();    //  Deaktivate Backward signal	Timer
				if (BuzzerIsOn == 0){
					BuzzerOff();                 //  Make buzzer off
				}
				DCDirection = 0;
				}
			}
		



  }
}


// Main thread scheduled by OS round robin preemptive scheduler
// Task2: Controls warning light, parking light, high beam light
// Inputs:  none
// Outputs: none
void Task2(void){

  while(1){
		
//------------------------------------------Auto light-----------------------------	
	


	
	
//------------------------------------------ Warning light ------------------------------
	
	if (WarningBlinking!= LastWarningBlinkingState){
			
		if (WarningBlinking ==1){
			OS_Wait(&CANmutex);	
			CAN_Send(WarningBlinkingOnCANObject, 3);
			OS_Signal(&CANmutex);
		} else if (WarningBlinking ==0){
			OS_Wait(&CANmutex);	
			CAN_Send(WarningBlinkingOffCANObject, 3);
			OS_Signal(&CANmutex);
		}
		LastWarningBlinkingState = WarningBlinking;
	}
	
	
//--------------------------------------------- Parking Light --------------------------------	


	
	if (ParkingLight!= LastParkingLightState){
		
		if (ParkingLight ==1){
			OS_Wait(&CANmutex);	
			CAN_Send(DayLightOnCANObject, 3);
			OS_Signal(&CANmutex);
		} else if (ParkingLight ==0){
			OS_Wait(&CANmutex);	
			CAN_Send(DayLightOffCANObject, 3);
			OS_Signal(&CANmutex);
		}
		LastParkingLightState = ParkingLight;
	}
	
	
	
//----------------------------------------------- High Beam -----------------------------------	

	
	if (HightBeam!= LastHighBeamState){
			
			if (HightBeam ==1){
				OS_Wait(&CANmutex);	
				CAN_Send(LedPWMHighBeamOnCANObject, 3);
				OS_Signal(&CANmutex);
				
			} else if (HightBeam ==0){
				OS_Wait(&CANmutex);	
				CAN_Send(LedPWMHighrBeamOffCANObject, 3);
				OS_Signal(&CANmutex);
				lastLightStatus = 0;  //Dark is now, led can be switched On now
			}
			LastHighBeamState = HightBeam;
		}


			
				
	//	OS_Sleep(2000);     // waits about 2 sec 

	}
}

// Main thread scheduled by OS round robin preemptive scheduler
// Task3 Disconnect PWM for servo if no action for 3 seconds
// Inputs:  none
// Outputs: none
void Task3(void){

		while(1){
			OS_Wait(&AnglePositionChanged);
			OS_Sleep(3000);     // waits about 3 sec after what wheel position was changed
			if (SpeedWheels==0){  // If still speed of wheels is Null, disconnect PWM for stearing
				PWM0_3_A_disable();
			}
//		WaitForInterrupt();
  }

}


// Main thread scheduled by OS round robin preemptive scheduler
// Task4 Make nothing
// Inputs:  none
// Outputs: none
void Task4(void){

  while(1){
		

		
//		WaitForInterrupt();
  }
}

// Main thread scheduled by OS round robin preemptive scheduler
// Task5: Read Temp and Humidity sensor data
// Inputs:  none
// Outputs: none
void Task5(void){


		while(1){
			//OS_Wait(&NewBluetoothBuzzerData); // Waint intil sensor data are ready
			TempHumiditySensor_Start();
			OS_Sleep(250);     // waits about 250 msec	
			TempHumiditySensor_End(tempHumidityData);
			SendTempHumidityFlag = 1;
			//OS_Signal(&NewTempHumidityData);  // Signal that Temp&Humidity sensor data are ready 
			OS_Sleep(1000);     // waits about 1 sec for new measurement
		}
	}

// Main thread scheduled by OS round robin preemptive scheduler
	// Task6: Read magnetometer data
// Inputs:  none
// Outputs: none
void Task6(void){

  while(1){

		// Send notification with index null
		if (ReadMagnetometerData(magnetometerData, magnetometerData2)==0){
			SendMagnetometerFlag = 1;
		}
		OS_Sleep(400);     // waits about 1 sec	
		
	}
	
		
//		WaitForInterrupt();

}

// Main thread scheduled by OS round robin preemptive scheduler
// Task2: Triggering Light depending on the light sensor
// Inputs:  none
// Outputs: none
void Task7(void){


	// Hysterese , initilizing
	if (lightIntensity<20){
		lastLightStatus = 1; //Dark is now
//		OS_Wait(&CANmutex);	
//		CAN_Send(DayLightOnCANObject, 3);
//		OS_Signal(&CANmutex);
	} 
	else if (lightIntensity>50){
		lastLightStatus = 0; //Light is now
//		OS_Wait(&CANmutex);	
//		CAN_Send(DayLightOffCANObject, 3);
//		OS_Signal(&CANmutex);
	} 
	
  while(1){


		
			// Hysterese, loop
		if ((lightIntensity<20)&&(lastLightStatus == 0)){
			if (LightAuto&&!HightBeam){
				OS_Wait(&CANmutex);	
				CAN_Send(LedPWMAutoLightOnCANObject, 3);
				OS_Signal(&CANmutex);
			}
			lastLightStatus = 1; //Light is now
		} else if ((lightIntensity>50)&&(lastLightStatus == 1)){
			if (LightAuto&&!HightBeam){
				OS_Wait(&CANmutex);	
				CAN_Send(LedPWMAutoLightOffCANObject, 3);
				OS_Signal(&CANmutex);
			}
			lastLightStatus = 0;  //Dark is now
		}
		
		if (LightAuto!= LastLightAuto){
			
			if (LightAuto == 0){
				if (!HightBeam){
					OS_Wait(&CANmutex);	
					CAN_Send(LedPWMAutoLightOffCANObject, 3);
					OS_Signal(&CANmutex);
				}
				LastLightAuto = 0;
				
			} else if (LightAuto == 1){
					LastLightAuto = 1;
					lastLightStatus = 0;  //Dark is now
			}
		}
		
	
  }
}

// Main thread scheduled by OS round robin preemptive scheduler
// Task8 does nothing but never blocks or sleeps
// Inputs:  none
// Outputs: none
void Task8(void){

  while(1){
		//int i= 1;
//		WaitForInterrupt();
  }
}

// Main thread scheduled by OS round robin preemptive scheduler
// Task2 does nothing but never blocks or sleeps
// Inputs:  none
// Outputs: none
void Task9(void){

  while(1){
		//int i= 1;
//		WaitForInterrupt();
  }
}

// *********Task0*********
// Periodic event thread runs in real time at 1000 Hz
// Do nothing
// Inputs:  none
// Outputs: none
void Task10(void){
  //static int32_t soundSum = 0;
  

}



//------------LaunchPad__Output------------
// Set new value of Stearing angle from bluetooth client to launchpad
// Input: none
// Output: none
void Bluetooth_Write_Stear_Angle_Value(){  // write angle

		servoupdownarm(AngleWheels, 0); // 0..100

	if (WarningBlinking==0){	
		
		OS_Wait(&CANmutex);	
	
		if (AngleWheels > 60){
				if (StearingDirection!=1){

					CAN_Send(LeftBlinkingCANObject, 5);
					StearingDirection = 1;
				} 
			} else if (AngleWheels < 40){
				if (StearingDirection!=2){

					CAN_Send(RightBlinkingCANObject, 5);
					StearingDirection = 2;
				} 
			} else if ((AngleWheels >=40)&&(AngleWheels <=60)){
				if (StearingDirection!=0){

					CAN_Send(NoBlinkingCANObject, 5);
					StearingDirection = 0;
				} 
			}

	
			OS_Signal(&CANmutex);
	
			}
	
		if (SpeedWheels==0){
			OS_Signal(&AnglePositionChanged);  // Signal that Wheel position was changed
		}
		
	

  
}


void Bluetooth_Read_Stear_Angle_Value(){
}






// Set new value of Motor Speed from bluetooth client to launchpad
// 
// Input: none
// Output: none
void Bluetooth_Write_Speed_Forward_Motor_Value(){  
		if (!ControlledSpeed) {
			DCforward(SpeedWheels *6 , 0);  //between(0..250)* 6  for range (0..4000)  Forward Direction
		} else {
			setDesiredSpeed(SpeedWheels, 0);
		}
		if (DCDirection!=0){
			OS_Wait(&CANmutex);
			CAN_Send(BackwardLightOffCANObject, 3);
			OS_Signal(&CANmutex);
			BSP_PeriodicTask_StopB();    //  Deaktivate Backward signal	Timer
			BuzzerOff();                 //  Make buzzer off
			DCDirection = 0;
		}
		
				
		
}





// Set new value of Motor Speed from bluetooth client to launchpad
// 
// Input: none
// Output: none
void Bluetooth_Write_Speed_Backward_Motor_Value(){  // write angle
    if (!ControlledSpeed) {
			DCbackward(SpeedWheels *6 , 0);  //between(0..250)* 6  for range (0..4000)  Backward Direction
		} else {
			setDesiredSpeed(SpeedWheels, 1);
		}
		
		if (DCDirection!=1){
			OS_Wait(&CANmutex);	
			CAN_Send(BackwardLightOnCANObject, 3);
			OS_Signal(&CANmutex);
			BSP_PeriodicTask_InitB( &BackwardPeriodicalSignal, FREQUENCY_REAR_SIGNAL, FREQUENCY_REAR_SIGNAL);  // Init timer for Bacward signal  
			BuzzerOn(BuzzerVolume);
			LastSignalState = 1;
			DCDirection = 1;
		}

		
		OS_Signal(&BackwardDCPositionChanged);
}




void Bluetooth_Read_Speed_Forward_Motor_Value(){
}

void Bluetooth_Read_Speed_Backward_Motor_Value(){
}


void Bluetooth_Read_Options_Value(){
}

// Set new value of Options from bluetooth client to launchpad
// 
// Input: none
// Output: none
void Bluetooth_Write_Options_Value(){
	
	ControlledSpeed = selectedOptions&0x1;
	WarningBlinking = (selectedOptions&0x2)>>1;
	LightAuto = (selectedOptions&0x4)>>2;
	HightBeam = (selectedOptions&0x8)>>3;
	ParkingLight = (selectedOptions&0x10)>>4;
	StartAuto = (selectedOptions&0x20)>>5;
	
	//   Put wheels in the center
	if (LastStartState == 0){
		if (StartAuto == 1){
			servoupdownarm(50, 0); // 0..100     Middle
			LastStartState = 1;
			if (SpeedWheels==0){
			OS_Signal(&AnglePositionChanged);  // Signal that Wheel position was changed
		}
		}  else if (StartAuto == 0){
			
		}
	} else if (LastStartState == 1){
		if (StartAuto == 0){
			
			
			LastStartState = 0;
		} else if (StartAuto == 1){
			
		}
	}
}





// Input: none
// Output: none
void Bluetooth_Write_Wheel_Direction_Value(){
	
	// Change direction only if motor is stopped
//	if (SpeedWheels==0){
//		DirectionWheels = CharacteristicList[0].pt[0];
//	}
}

void Bluetooth_Reed_Wheel_Direction_Value(){
	
}




//------------LaunchPad__Output------------
// Set new value of Buzzer Valume from bluetooth client to launchpad
// Input: none
// Output: none
void Bluetooth_Write_Buzzer_Value(){  // write angle

	if (BuzzerVolume!=0){
			BuzzerVolumeUpdatedNotNull = BuzzerVolume;
			OS_Wait(&Buzzermutex);
			BuzzerOn(BuzzerVolume);
			OS_Signal(&Buzzermutex);
			BuzzerIsOn = 1;
		} else {
			BuzzerOff();
			BuzzerIsOn = 0;
		}
	
	
  //OS_Signal(&NewBluetoothBuzzerData);  // Set Semaphore NewSpeedData

}


void Bluetooth_Read_Buzzer_Value(){
}



// Input: none
// Output: none
void Bluetooth_Write_TempHumidity_Value(){  // write temp , humidity


}





// Input: none
// Output: none
void Bluetooth_Read_TempHumidity_Value(){  // write temp , humidity



}


// Input: none
// Output: none
void Bluetooth_Write_Magnet_Value(){  
	
}

// Bluetooth client read speed , 3 bytes
// Input: none
// Output: none
void Bluetooth_Read_SpeedSensor_Value(){
	
	speedPhotoData[2] =  Period >> 16 & 0xFF;
	speedPhotoData[1] =  Period >> 8 & 0xFF;
	speedPhotoData[0] =  Period  & 0xFF;

}


// Input: none
// Output: none
void Bluetooth_Write_SpeedSensor_Value(){
	
}



// Input: none
// Output: none
void Bluetooth_Read_Magnet_Value(){  
	
}



// Set new value of HighBeamPower from bluetooth client to launchpad

// Input: none
// Output: none
void Bluetooth_Write_HighBeamPower_Value(){
	
	LedPWMHighBeamOnCANObject[2] = HighBeamPower;
	
	if (HightBeam ==1){
				OS_Wait(&CANmutex);	
				CAN_Send(LedPWMHighBeamOnCANObject, 3);
				OS_Signal(&CANmutex);
			} 
	
}


// Data from UART are Little Endian
// Input: 
// Output: none
void Bluetooth_Read_HighBeamPower_Value(){  
	
}

// Periodical Task for bacward Signal
// Input: none
// Output: none
void BackwardPeriodicalSignal(){
	
if (BuzzerIsOn == 0)	{
	if (LastSignalState == 1){
		BuzzerOff();
		LastSignalState = 0;
	} else if (LastSignalState == 0){
		OS_Wait(&Buzzermutex);
		BuzzerOn(BuzzerVolumeUpdatedNotNull);
		OS_Signal(&Buzzermutex);

		LastSignalState = 1;
		}
	}
		
}


void Bluetooth_Init(void){volatile int r;
	
  //EnableInterrupts();
  UART0_OutString("\n\rRobot car bluetooth initialisation \n\r");
  r = AP_Init(); 
  GetStatus();  // optional
  GetVersion(); // optional
  AddService(0xFFE0); 

	AddCharacteristic(0xFFE1,1,&AngleWheels,0x03,0x0A,"Angle",&Bluetooth_Read_Stear_Angle_Value,&Bluetooth_Write_Stear_Angle_Value);   //  write current Angle value, 1 byte
	AddCharacteristic(0xFFE2,1,&SpeedWheels,0x03,0x0A,"SpeedFoward",&Bluetooth_Read_Speed_Forward_Motor_Value,&Bluetooth_Write_Speed_Forward_Motor_Value);   //  write current Speed value, 1 bytes
	AddCharacteristic(0xFFE3,1,&SpeedWheels,0x03,0x0A,"SpeedBackward",&Bluetooth_Read_Speed_Backward_Motor_Value,&Bluetooth_Write_Speed_Backward_Motor_Value);   //  write current Speed value, 1 bytes
	AddCharacteristic(0xFFE4,1,&BuzzerVolume,0x03,0x0A,"Buzzer",&Bluetooth_Read_Buzzer_Value,&Bluetooth_Write_Buzzer_Value);   //  write current Speed value, 1 bytes
	
	AddCharacteristic(0xFFE5,6,&tempHumidityData[0],0x03,0x0A,"TempHumidity",&Bluetooth_Read_TempHumidity_Value,&Bluetooth_Write_TempHumidity_Value);   //  write current Speed value, 1 bytes
	AddCharacteristic(0xFFE6,6,&magnetometerData[0],0x03,0x0A,"Magnetometer",&Bluetooth_Read_Magnet_Value,&Bluetooth_Write_Magnet_Value);   //  write current Speed value, 1 bytes
	
	AddCharacteristic(0xFFE7,3,&speedPhotoData[0],0x03,0x0A,"SpeedPhotoSensor",&Bluetooth_Read_SpeedSensor_Value,&Bluetooth_Write_SpeedSensor_Value);   //  write current Speed value, 1 bytes
	AddCharacteristic(0xFFE8,1,&selectedOptions,0x03,0x0A,"SelectedFunctions",&Bluetooth_Read_Options_Value,&Bluetooth_Write_Options_Value);   //  write options, 1 bytes
	
	AddCharacteristic(0xFFE9,1,&HighBeamPower,0x03,0x0A,"HighBeamPower",&Bluetooth_Read_HighBeamPower_Value,&Bluetooth_Write_HighBeamPower_Value);   //  write current HighBeamPower value, 1 bytes
	
//	AddNotifyCharacteristic(0xFFE5,6,&tempHumidityData[0],"TempHumidity",&Bluetooth_Write_TempHumidity_Value);
//	AddNotifyCharacteristic(0xFFE6,6,&magnetometerData[0],"Magnetometer",&Bluetooth_Write_Magnet_Value);
	
  RegisterService();
  StartAdvertisement();
  GetStatus();
  //DisableInterrupts(); // optional
}




int main(void){
	PLL_Init();                      // bus clock at 80 MHz
	
	AD_Init();  // initilize AD converter

	GPIO_DC_Init();	//  initialize pins for forward and backward dierction

  OS_Init();
  //Profile_Init();  // initialize the 7 hardware profiling ,   pins can conflict, check
 

  OS_InitSemaphore(&NewBluetoothSpeedData, 0);  // 0 means no data
	//OS_InitSemaphore(&NewBluetoothAngleData, 0);  // 0 means no data
	OS_InitSemaphore(&AnglePositionChanged, 0);  // 0 means no data
	//OS_InitSemaphore(&NewBluetoothBuzzerData, 0);  // 0 means no data
	OS_InitSemaphore(&NewTempHumidityData, 0);  // 0 means no data
	OS_InitSemaphore(&CANmutex, 1); // 1 means free
	OS_InitSemaphore(&Buzzermutex, 1); // 1 means free
	
	
	// Task 6 should run every 1000ms
  //OS_AddPeriodicEventThread(&Task6, 1000);  //  Only for prio 1 Events that should be quickly executed. Not thread!!. It is a Task event.
	
  OS_AddThreads(&Task0,0, &Task1,0, &Task2,0, &Task3,0, &Task4,0, &Task5,0, &Task6,0, &Task7,0, &Task8,0, &Task9,0);


	
	// Input:  Period(Takts), DutyCicle(Takts)
	// initialize PWM0_3_A, 5 Takt in 1 mikros * 2500 mikros , Servo steering  
	PWM0_3_A_Init((5*PERIOD_SERVO), (5*POSITION_NULL));   // PC4       
	
	OS_FIFO_Init();                 // initialize FIFO used to send data between Tasks. Implement if need. 
	
	// Initialize TIMER 1A for PWM, 20 KHz Period, DC Motor Control, 80 Takt in 1 mikros *  20 microsec (period 50 khz)  = 1600
	TIMER_1_A_PWM_Init(PERIOD_DC, SPEED_NULL);
	
	// Input:  Period(Takts), DutyCicle(Takts)
	// initialize PWM0_0A, 3 KHz Period, Buzzer , 50% volume
	PWM0_0_B_Init(PERIOD_BUZZER, VOLUME_50);
	
	
	UART0_Init();     //  will affect PWM0_0_A
	Bluetooth_Init();
	
	Timer3_Init(4000000); // initialize timer3 (20 Hz, 50ms)
	PeriodMeasure_Init();  // Initialize Edge triggered capture mode interrupt
	EnableInterrupts();
	
	I2C3_Init(); // Init I2C Module3 for Temp Humidity Sensor
	I2C1_Init(); // Init I2C Module1 for Gyroskop Accel magnetometer Sensor
	
	if (Init_MPU9250() == 1){   //Initialize Magnetometer 
		// Notify user:  for example with Led:    Initialization of magnetometer couldn't be done
	}
	
	PortF_2_init();  // Initialize port F2   , Led on board 
	PortF_1_init();  // Initialize port F2   , Led on board 
	
	CAN_Init();
	


  OS_Launch(BSP_Clock_GetFreq()/THREADFREQ); // doesn't return, interrupts enabled in here
  return 0;             // this never executes
}


