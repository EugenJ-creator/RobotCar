// GPIO.h
// Runs on TM4C123
// Digital input/output from TM4C123 LaunchPad to CC2650
// Jonathan Valvano
// September 23, 2016

/* This example accompanies the books
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
      ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2016
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
      ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2016
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
      ISBN: 978-1466468863, , Jonathan Valvano, copyright (c) 2016

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

#include "../inc/tm4c123gh6pm.h"

void GPIO_Init(void);


// Subroutine to initialize port B pins for input and output (PWM DC IN1)
// PB7 is input 
// Inputs: None
// Outputs: None
// Notes: ...

void PortB_7_init(void);

void PortB_7_enable(void);

void PortB_7_disable(void);
//--------------------------------------------------------------------------------------------------------

// Subroutine to initialize port D pins for input and output (PWM DC IN2)
// PD0 is input 
// Inputs: None
// Outputs: None
// Notes: ...

void PortD_0_init(void);

void PortD_0_enable(void);

void PortD_0_disable(void);
//---------------------------------------------------------------------------------------------------


// Subroutine to initialize port B pins for input and output (PWM DC EN)
// PB7 is input 
// Inputs: None
// Outputs: None
// Notes: ...

void PortB_6_init(void);

void PortB_6_enable(void);

void PortB_6_disable(void);
