
#include "GPIO.h"
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>




void GPIO_Init(void){
	PortB_7_init();   // PWM DC IN2  Init
	PortD_0_init();   // PWM DC IN1  Init
	//PortB_6_init();  // DC EN  Init
	PortB_7_disable();   //B7 GND
	//PortB_6_disable();   // DC Disable
}

// Subroutine to initialize port B pins for input and output (PWM DC IN1)
// PB7 is input 
// Inputs: None
// Outputs: None
// Notes: ...

void PortB_7_init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;      // 1) B clock
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize     
  GPIO_PORTB_AMSEL_R &= ~0x80;        // 2) disable analog function
  GPIO_PORTB_PCTL_R &= ~0xF0000000;   // 3) GPIO clear bit PCTL  
  GPIO_PORTB_DIR_R |= 0x80;          // 4.2) PB7 output  
  GPIO_PORTB_AFSEL_R &= ~0x80;        // 5) no alternate function
//  GPIO_PORTF_PUR_R |= 0x10;          // 6) enable pullup resistor on PF4       
  GPIO_PORTB_DEN_R |= 0x80;          // 7) enable digital pin B7
}

void PortB_7_enable(void){
	GPIO_PORTB_DATA_R |=	0x80;
}

void PortB_7_disable(void){
	GPIO_PORTB_DATA_R &=	~0x80;
}
//--------------------------------------------------------------------------------------------------------

// Subroutine to initialize port D pins for input and output (PWM DC IN2)
// PD0 is input 
// Inputs: None
// Outputs: None
// Notes: ...

void PortD_0_init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000008;      // 1) D clock
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize     
  GPIO_PORTD_AMSEL_R &= ~0x1;        // 2) disable analog function
  GPIO_PORTD_PCTL_R &= ~0x0000000F;   // 3) GPIO clear bit PCTL  
  GPIO_PORTD_DIR_R |= 0x1;          // 4.2) PD0 output  
  GPIO_PORTD_AFSEL_R &= ~0x1;        // 5) no alternate function
//  GPIO_PORTF_PUR_R |= 0x10;          // 6) enable pullup resistor on PF4       
  GPIO_PORTD_DEN_R |= 0x1;          // 7) enable digital pin D0
}

void PortD_0_enable(void){
	GPIO_PORTD_DATA_R |=	0x1;
}

void PortD_0_disable(void){
	GPIO_PORTD_DATA_R &=	~0x1;
}
//---------------------------------------------------------------------------------------------------


// Subroutine to initialize port B pins for input and output (PWM DC EN)
// PB7 is input 
// Inputs: None
// Outputs: None
// Notes: ...

void PortB_6_init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;      // 1) B clock
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize     
  GPIO_PORTB_AMSEL_R &= ~0x40;        // 2) disable analog function
  GPIO_PORTB_PCTL_R &= ~0x0F000000;   // 3) GPIO clear bit PCTL  
  GPIO_PORTB_DIR_R |= 0x40;          // 4.2) PB7 output  
  GPIO_PORTB_AFSEL_R &= ~0x40;        // 5) no alternate function
//  GPIO_PORTF_PUR_R |= 0x10;          // 6) enable pullup resistor on PF4       
  GPIO_PORTB_DEN_R |= 0x40;          // 7) enable digital pin B7
}

void PortB_6_enable(void){
	GPIO_PORTB_DATA_R |=	0x40;
}

void PortB_6_disable(void){
	GPIO_PORTB_DATA_R &=	~0x40;
}




