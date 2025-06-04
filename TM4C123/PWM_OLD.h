#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"


// ------------BSP_StepMotorIitPins_End------------
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/32 
//                = 80 MHz/32 = 2,5 MHz (in this example)
// Output on PB6/M0PWM0
void PWM0_0_A_Init(uint16_t period, uint16_t duty);
	

void PWM0_0_A_enable(void);         // enable M0PWM0


void PWM0_0_A_disable(void);        // disable M0PWM0



// change duty cycle of PB6
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_0_A_Duty(uint16_t duty);   // 6) count value when output rises
//------------------------------------------------------------------------------------------

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/16 
//                = 80 MHz/2 = 40 MHz (in this example)
// Output on PB7/M0PWM1
void PWM0_0_B_Init(uint16_t period, uint16_t duty);
// change duty cycle of PB7
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_0_B_Duty(uint16_t duty);

void PWM0_0_B_enable(void);

void PWM0_0_B_disable(void);


//---------------------------------------------------------------------------------------------



//---------------BSP-  DC Motor Speed TCC Init Pins 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/32 
//                = 80 MHz/32 = 2.5 MHz (in this example)
// Output on PB4/M0PWM2
void PWM0_1_A_Init(uint16_t period, uint16_t duty);
	
void PWM0_1_A_Deactivate(void);   // deactivate PB4/M0PWM2


// change duty cycle of PB4
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_1_A_Duty(uint16_t duty);     // 6) count value when output rises


void PWM0_1_A_enable(void);        // enable PB4/M0PWM2


void PWM0_1_A_disable(void);       // disable PB4/M0PWM2



//---------------BSP-  DC Motor Speed TCC Init Pins 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/32 
//                = 80 MHz/32 = 2.5 MHz (in this example)
// Output on PC4/M0PWM6
void PWM0_3_A_Init(uint16_t period, uint16_t duty);
	
void PWM0_3_A_Deactivate(void);   // deactivate PB4/M0PWM2


// change duty cycle of PB4
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_3_A_Duty(uint16_t duty);     // 6) count value when output rises


void PWM0_3_A_enable(void);        // enable PB4/M0PWM2


void PWM0_3_A_disable(void);       // disable PB4/M0PWM2

//---------------BSP-  DC Motor Speed TCC Init Pins 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/32 
//                = 80 MHz/32 = 2.5 MHz (in this example)
// Output on PB5/M0PWM3
void PWM0_1_B_Init(uint16_t period, uint16_t duty);
	



// change duty cycle of PB5
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_1_B_Duty(uint16_t duty);     // 6) count value when output rises


void PWM0_1_B_enable(void);        // enable PB5/M0PWM3


void PWM0_1_B_disable(void);       // disable PB5/M0PWM3


//---------------BSP-  DC Motor Speed TCC Init Pins 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/32 
//                = 80 MHz/32 = 2.5 MHz (in this example)
// Output on PF2/M1PWM6
void PWM1_3_A_Init(uint16_t period, uint16_t duty);
	

// change duty cycle of PF2
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM1_3_A_Duty(uint16_t duty); // 6) count value when output rises

void PWM1_3_A_enable(void);        // enable PB2/M1PWM6


void PWM1_3_A_disable(void);          // disable PB2/M1PWM6



//---------------BSP-  DC Motor Speed TCC Init Pins 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/32 
//                = 80 MHz/32 = 2.5 MHz (in this example)
// Output on PA6/M1PWM2
void PWM1_1_A_Init(uint16_t period, uint16_t duty);
	
// change duty cycle of PA6
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM1_1_A_Duty(uint16_t duty); // 6) count value when output rises

	
void PWM1_1_A_enable(void);      // enable PA6/M1PWM2


void PWM1_1_A_disable(void);         // disable PA6/M1PWM2


