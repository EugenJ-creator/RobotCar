// PWM.h
// Runs on TM4C123
// Use PWM0/PB6 and PWM1/PB7 to generate pulse-width modulated outputs.
// Daniel Valvano
// March 28, 2014
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
  Program 6.7, section 6.3.2

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */



// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/2 
//                = 80 MHz/2 = 40 MHz (in this example)
// Output on PB7/M0PWM1
void PWM0B_Init(uint16_t period, uint16_t duty);

// change duty cycle of PB7
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0B_Duty(uint16_t duty);

//-------------------------------------------------------------------------------------------




// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/16 
//                = 80 MHz/16 = 5 MHz (in this example)
// Output on PB7/M0PWM1
void PWM0_0_B_Init(uint16_t period, uint16_t duty);
// change duty cycle of PB7
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_0_B_Duty(uint16_t duty);

void PWM0_0_B_enable(void);

void PWM0_0_B_disable(void);

void PWM0_0_B_Deactivate(void);

//---------------------------------------------------------------------------------------------


////---------------BSP-  DC Motor Speed PWM Init Pins PB4
//// period is 16-bit number of PWM clock cycles in one period (3<=period)
//// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
//// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
////                = BusClock/16 
////                = 80 MHz/16 = 5 MHz (in this example)
//// Output on PB4/M0PWM2
//void PWM0_1_A_Init(uint16_t period, uint16_t duty);
//	
//// change duty cycle of PB4
//// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
//void PWM0_1_A_Duty(uint16_t duty); // 6) count value when output rises

//	
//void PWM0_1_A_enable(void);      // enable PB4/M0PWM2


//void PWM0_1_A_disable(void);         // disable PB4/M0PWM2

//----------------------------------------------------------------------


//---------------BSP-  DC Motor Speed TCC Init Pins PB4
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
//          TIMER  = 80 MHz = 5 MHz (in this example)
// Output on PB4/ TIMER with PWM
void TIMER_1_A_PWM_Init(uint16_t period, uint16_t duty);   

void TIMER_1_A_PWM_Duty(uint16_t duty);

void TIMER_1_A_PWM_enable(void);         // enable PB4 PWM


void TIMER_1_A_PWM_disable(void);         // disable PB4 PWM


// deactivate PB4/PWM
void TIMER_1_A_PWM_Deactivate(void);      // disable PB4/PWM
	



//---------------------------------------------------------------------------------------
//---------------BSP-  DC Motor Speed TCC Init Pins 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/32 
//                = 80 MHz/32 = 2.5 MHz (in this example)
// Output on PD0/M0PWM6
void PWM0_3_A_Init(uint16_t period, uint16_t duty);
	
// change duty cycle of PD0
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_3_A_Duty(uint16_t duty);     // 6) count value when output rises


void PWM0_3_A_enable(void);        // enable PD0/M0PWM6


void PWM0_3_A_disable(void);       // disable PD0/M0PWM6

void PWM0_3_A_Deactivate(void);   // deactivate PD0/M0PWM6

