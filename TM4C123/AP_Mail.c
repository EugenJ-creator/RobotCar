// LaunchPad.c
// Runs on TM4C123


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

////------------LaunchPad_Init------------
//// Initialize Switch input and LED output
//// Input: none
//// Output: none
//void LaunchPad_Init(void){
//  SYSCTL_RCGCGPIO_R |= 0x00000020;  // 1) activate clock for Port F
//  while((SYSCTL_PRGPIO_R&0x20) == 0){};// allow time for clock to stabilize
//  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
//  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
//  // only PF0 needs to be unlocked, other bits can't be locked
//  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out
//  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
//  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
//  GPIO_PORTF_DATA_R = 0;            // LEDs off
//}


////------------LaunchPad_Input------------
//// Input from Switches 
//// Input: none
//// Output: 0x00 none
////         0x01 Button1
////         0x02 Button2
////         0x03 both Button1 and Button2
//uint8_t LaunchPad_Input(void){
//	uint32_t data;
//	// read PF4,PF0 inputs
//	data = ~GPIO_PORTF_DATA_R; // convert to positive logic
//	// shift bits so switches are in bits 1,0
//  return (((data&0x10)>>3)|((data&0x01)));   
//}

////------------LaunchPad__Output------------
//// Output to LaunchPad LEDs 
//// Input: 0 off, bit0=red,bit1=blue,bit2=green
//// Output: none
//void LaunchPad_Output(uint8_t data){  // write three outputs bits of P2
//  GPIO_PORTF_DATA_R = data<<1;
//}

