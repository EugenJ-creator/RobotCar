


#include <stdint.h>
#include "PWM.h"
#include "../inc/tm4c123gh6pm.h"

///  Devider will be aplied for all PWMs!!!!!!!!!!!!!!!!!!!!!!!!

// ------------BSP_StepMotorIitPins_End------------
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/32 
//                = 80 MHz/16 = 5 MHz (in this example)
// Devider will the same for all PWM Modules
// Output on PB6/M0PWM0
void PWM0_0_A_Init(uint16_t period, uint16_t duty){
//	volatile unsigned long delay;              //2-3 takts delay , volatile to not be optimized
//  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0 Modul
//  SYSCTL_RCGCGPIO_R |= 0x02;            // 2) activate port B
//	delay = SYSCTL_RCGCGPIO_R;            // allow time to finish activating
//  //while((SYSCTL_PRGPIO_R&0x02) == 0){};
//  GPIO_PORTB_AFSEL_R |= 0x40;           // enable alt funct on PB6
//  GPIO_PORTB_PCTL_R &= ~0x0F000000;     // configure PB6 as PWM0
//  GPIO_PORTB_PCTL_R |= 0x04000000;
//  GPIO_PORTB_AMSEL_R &= ~0x40;          // disable analog functionality on PB6
//  GPIO_PORTB_DEN_R |= 0x40;             // enable digital I/O on PB6
//  //SYSCTL_RCC_R = 0x00100000 |           // 3) use PWM divider
//  //   (SYSCTL_RCC_R & (~0x00070000));   //    configure for /32 divider
//	//	SYSCTL_RCC_R &= ~SYSCTL_RCC_USEPWMDIV;
//	SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
//  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
////  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_32;  //    configure for /32 divider	
//	SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_16;
//			
//  //PWM0_0_CTL_R = 0;                     // 4) re-loading down-counting mode
//	PWM0_0_CTL_R &= ~(1<<0) ;                     // 4) disable Generator
//	PWM0_0_CTL_R &= ~(1<<1) ;                     // 4) re-loading down-counting mode
//  PWM0_0_GENA_R = (PWM_0_GENA_ACTCMPAD_ONE|PWM_0_GENA_ACTLOAD_ZERO);                 // high on LOAD, low on CMPA down
//  // PB6 goes low on LOAD
//  // PB6 goes high on CMPA down

//	PWM0_0_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
//  PWM0_0_CMPA_R = duty - 1;             // 6) count value when output rises
//  PWM0_0_CTL_R |= 0x00000001;           // 7) start PWM0
  
	
	SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // 2) activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_AFSEL_R |= 0x40;           // enable alt funct on PB6
  GPIO_PORTB_PCTL_R &= ~0x0F000000;     // configure PB6 as PWM0
  GPIO_PORTB_PCTL_R |= 0x04000000;
  GPIO_PORTB_AMSEL_R &= ~0x40;          // disable analog functionality on PB6
  GPIO_PORTB_DEN_R |= 0x40;             // enable digital I/O on PB6
//  SYSCTL_RCC_R = 0x00100000 |           // 3) use PWM divider
//      (SYSCTL_RCC_R & (~0x000E0000));   //    configure for /2 divider
  PWM0_0_CTL_R = 0;                     // 4) re-loading down-counting mode
  PWM0_0_GENA_R = 0xC8;                 // low on LOAD, high on CMPA down
  // PB6 goes low on LOAD
  // PB6 goes high on CMPA down
  PWM0_0_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_0_CMPA_R = duty - 1;             // 6) count value when output rises
  PWM0_0_CTL_R |= 0x00000001;           // 7) start PWM0
  PWM0_ENABLE_R |= 0x00000001;          // enable PB6/M0PWM0
	

}

void PWM0_0_A_enable(void){
  PWM0_ENABLE_R |= 0x00000001;          // enable M0PWM0 PB6
}

void PWM0_0_A_disable(void){
  PWM0_ENABLE_R &= (~0x00000001);          // disable M0PWM0 PB6
}


// change duty cycle of PB6
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_0_A_Duty(uint16_t duty){
  PWM0_0_CMPA_R = duty - 1;             // 6) count value when output rises
}


// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/2 
//                = 80 MHz/2 = 40 MHz (in this example)
// Output on PB7/M0PWM1
void PWM0_0_B_Init(uint16_t period, uint16_t duty){
  volatile unsigned long delay;
  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // 2) activate port B
  delay = SYSCTL_RCGCGPIO_R;            // allow time to finish activating
  GPIO_PORTB_AFSEL_R |= 0x80;           // enable alt funct on PB7
  GPIO_PORTB_PCTL_R &= ~0xF0000000;     // configure PB7 as M0PWM1
  GPIO_PORTB_PCTL_R |= 0x40000000;
  GPIO_PORTB_AMSEL_R &= ~0x80;          // disable analog functionality on PB7
  GPIO_PORTB_DEN_R |= 0x80;             // enable digital I/O on PB7
//  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
//  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
//  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_2;  //    configure for /2 divider
  PWM0_0_CTL_R = 0;                     // 4) re-loading down-counting mode
  PWM0_0_GENB_R = (PWM_0_GENB_ACTCMPBD_ONE|PWM_0_GENB_ACTLOAD_ZERO);
  // PB7 goes low on LOAD
  // PB7 goes high on CMPB down
  PWM0_0_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_0_CMPB_R = duty - 1;             // 6) count value when output rises
  PWM0_0_CTL_R |= 0x00000001;           // 7) start PWM0
  PWM0_ENABLE_R |= 0x00000002;          // enable PB7/M0PWM1
}
// change duty cycle of PB7
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_0_B_Duty(uint16_t duty){
  PWM0_0_CMPB_R = duty - 1;             // 6) count value when output rises
}

void PWM0_0_B_enable(void){
  PWM0_ENABLE_R |= 0x00000002;          // enable PB7/M0PWM1
}

void PWM0_0_B_disable(void){
  PWM0_ENABLE_R &= (~0x00000002);          // disable PB7/M0PWM1
}








//---------------BSP-  DC Motor Speed TCC Init Pins 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/32 
//                = 80 MHz/32 = 2.5 MHz (in this example)
// Devider will the same for all PWM Modules
// Output on PB4/M0PWM1
void PWM0_1_A_Init(uint16_t period, uint16_t duty){
  volatile unsigned long delay;              //2-3 takts delay , volatile to not be optimized
  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0 Modul
  SYSCTL_RCGCGPIO_R |= 0x02;            // 2) activate port B
  delay = SYSCTL_RCGCGPIO_R;            // allow time to finish activating
  GPIO_PORTB_AFSEL_R |= 0x10;           // enable alt funct on PB4
  GPIO_PORTB_PCTL_R &= ~0x000F0000;     // configure PB4 as M0PWM2
  GPIO_PORTB_PCTL_R |= 0x00040000;
  GPIO_PORTB_AMSEL_R &= ~0x10;          // disable analog functionality on PB4
  GPIO_PORTB_DEN_R |= 0x10;             // enable digital I/O on PB4
//	SYSCTL_RCC_R = 0x00100000 |           // 3) use PWM divider
//      (SYSCTL_RCC_R & (~0x00070000));   //    configure for /32 divider
  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
  //SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_32;  //    configure for /32 divider
	SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_16;
  //PWM0_1_CTL_R = 0;                     // 4) re-loading down-counting mode
	PWM0_1_CTL_R &= ~(1<<0) ;                     // 4) disable Generator
	PWM0_1_CTL_R &= ~(1<<1) ;                     // 4) re-loading down-counting mode
  PWM0_1_GENA_R = (PWM_1_GENA_ACTCMPAD_ONE|PWM_1_GENA_ACTLOAD_NONE);  
  // PB7 goes high on LOAD
  // PB7 goes low on CMPB down
  PWM0_1_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_1_CMPA_R = duty - 1;             // 6) count value when output rises
  PWM0_1_CTL_R |= 0x00000001;           // 7) start PWM1
	PWM0_ENABLE_R |= 0x00000004;          // enable PB4/M0PWM2
//  PWM0_ENABLE_R &= (~0x00000004);          // disable PB4/M0PWM2
}


void PWM0_1_A_Deactivate(void){
	PWM0_1_CTL_R &= ~(1<<0) ;                     // 4) disable Generator
  PWM0_1_GENA_R = (PWM_1_GENA_ACTCMPAD_NONE|PWM_1_GENA_ACTLOAD_ZERO);  
  PWM0_ENABLE_R &= (~0x00000004);          // disable PB4/M0PWM2
}
// change duty cycle of PB4
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_1_A_Duty(uint16_t duty){
  PWM0_1_CMPA_R = duty - 1;             // 6) count value when output rises
}

void PWM0_1_A_enable(void){
  PWM0_ENABLE_R |= 0x00000004;          // enable PB4/M0PWM2
}

void PWM0_1_A_disable(void){
  PWM0_ENABLE_R &= (~0x00000004);          // disable PB4/M0PWM2
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

//---------------BSP-  DC Motor Speed TCC Init Pins 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/32 
//                = 80 MHz/32 = 2.5 MHz (in this example)
// Devider will the same for all PWM Modules
// Output on PC4/M0PWM6
void PWM0_3_A_Init(uint16_t period, uint16_t duty){
  volatile unsigned long delay;              //2-3 takts delay , volatile to not be optimized
  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0 Modul
  SYSCTL_RCGCGPIO_R |= 0x04;            // 2) activate port C
  delay = SYSCTL_RCGCGPIO_R;            // allow time to finish activating
  GPIO_PORTC_AFSEL_R |= 0x10;           // enable alt funct on PC4
  GPIO_PORTC_PCTL_R &= ~0x000F0000;     // configure PC4 as M0PWM6
  GPIO_PORTC_PCTL_R |= 0x00040000;
  GPIO_PORTC_AMSEL_R &= ~0x10;          // disable analog functionality on PC4
  GPIO_PORTC_DEN_R |= 0x10;             // enable digital I/O on PC4
//	SYSCTL_RCC_R = 0x00100000 |           // 3) use PWM divider
//      (SYSCTL_RCC_R & (~0x00070000));   //    configure for /32 divider
  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
  //SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_32;  //    configure for /32 divider
	SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_16;  //    configure for /16 divider
  //PWM0_1_CTL_R = 0;                     // 4) re-loading down-counting mode
	PWM0_3_CTL_R &= ~(1<<0) ;                     // 4) disable Generator
	PWM0_3_CTL_R &= ~(1<<1) ;                     // 4) re-loading down-counting mode
  PWM0_3_GENA_R = (PWM_3_GENA_ACTCMPAD_ONE|PWM_3_GENA_ACTLOAD_NONE);  
  // PB7 goes high on LOAD
  // PB7 goes low on CMPB down
  PWM0_3_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_3_CMPA_R = duty - 1;             // 6) count value when output rises
  PWM0_3_CTL_R |= 0x00000001;           // 7) start PWM3
  PWM0_ENABLE_R &= (~0x00000040);          // disable PC4/M0PWM6
}


//void PWM0_3_A_Deactivate(void){
//	PWM0_1_CTL_R &= ~(1<<0) ;                     // 4) disable Generator
//  PWM0_1_GENA_R = (PWM_1_GENA_ACTCMPAD_NONE|PWM_1_GENA_ACTLOAD_ZERO);  
//  PWM0_ENABLE_R &= (~0x00000004);          // disable PB4/M0PWM2
//}
// change duty cycle of PC4
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0_3_A_Duty(uint16_t duty){
  PWM0_3_CMPA_R = duty - 1;             // 6) count value when output rises
}

void PWM0_3_A_enable(void){
  PWM0_ENABLE_R |= 0x00000040;          // enable PC4/M0PWM6
}

void PWM0_3_A_disable(void){
  PWM0_ENABLE_R &= (~0x00000040);          // disable PC4/M0PWM6
}


////---------------BSP-  DC Motor Speed TCC Init Pins 
//// period is 16-bit number of PWM clock cycles in one period (3<=period)
//// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
//// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
////                = BusClock/32 
////                = 80 MHz/32 = 2.5 MHz (in this example)
//// Devider will the same for all PWM Modules
//// Output on PB5/M0PWM3
//void PWM0_1_B_Init(uint16_t period, uint16_t duty){
//  volatile unsigned long delay;              //2-3 takts delay , volatile to not be optimized
//  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0 Modul
//  SYSCTL_RCGCGPIO_R |= 0x02;            // 2) activate port B
//  delay = SYSCTL_RCGCGPIO_R;            // allow time to finish activating
//  GPIO_PORTB_AFSEL_R |= 0x20;           // enable alt funct on PB5
//  GPIO_PORTB_PCTL_R &= ~0x00F00000;     // configure PB5 as M0PWM3
//  GPIO_PORTB_PCTL_R |= 0x00400000;
//  GPIO_PORTB_AMSEL_R &= ~0x20;          // disable analog functionality on PB5
//  GPIO_PORTB_DEN_R |= 0x20;             // enable digital I/O on PB5
////	SYSCTL_RCC_R = 0x00100000 |           // 3) use PWM divider
////      (SYSCTL_RCC_R & (~0x00070000));   //    configure for /32 divider
//  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
//  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
//  //SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_32;  //    configure for /32 divider
//	SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_16;   //    configure for /16 divider
//  //PWM0_1_CTL_R = 0;                     // 4) re-loading down-counting mode
//	PWM0_1_CTL_R &= ~(1<<0) ;                     // 4) disable Generator
//	PWM0_1_CTL_R &= ~(1<<1) ;                     // 4) re-loading down-counting mode
//  PWM0_1_GENB_R = (PWM_1_GENB_ACTCMPAD_ONE|PWM_1_GENB_ACTLOAD_ZERO);  
//  // PB7 goes high on LOAD
//  // PB7 goes low on CMPB down
//  PWM0_1_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
//  PWM0_1_CMPB_R = duty - 1;             // 6) count value when output rises
//  PWM0_1_CTL_R |= 0x00000001;           // 7) start PWM0_1 generator
//  PWM0_ENABLE_R &= (~0x00000008);          // disable PB5/M0PWM3
//}
//// change duty cycle of PB5
//// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
//void PWM0_1_B_Duty(uint16_t duty){
//  PWM0_1_CMPB_R = duty - 1;             // 6) count value when output rises
//}

//void PWM0_1_B_enable(void){
//  PWM0_ENABLE_R |= 0x00000008;          // enable PB5/M0PWM3
//}

//void PWM0_1_B_disable(void){
//  PWM0_ENABLE_R &= (~0x00000008);          // disable PB5/M0PWM3
//}




//---------------BSP-  DC Motor Speed TCC Init Pins 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/32 
//                = 80 MHz/32 = 2.5 MHz (in this example)
// Output on PF2/M1PWM6
void PWM1_3_A_Init(uint16_t period, uint16_t duty){

	volatile unsigned long delay;              //2-3 takts delay , volatile to not be optimized
  SYSCTL_RCGCPWM_R |= 0x02;             // 1) activate PWM1 Modul
  SYSCTL_RCGCGPIO_R |= 0x20;            // 2) activate port F
	SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_32;  //    configure for /32 divider
  GPIO_PORTF_AFSEL_R |= 0x4;           // enable alt funct on PF2
  GPIO_PORTF_PCTL_R &= ~0x00000F00;     // configure PB4 as M1PWM6
  GPIO_PORTF_PCTL_R |= 0x00000500;			//  M1PWM6
  GPIO_PORTF_AMSEL_R &= ~0x4;          // disable analog functionality on PF2
  GPIO_PORTF_DEN_R |= 0x4;             // enable digital I/O on PF2
	
	PWM1_3_CTL_R &= ~(1<<0) ;                     // 4) disable Generator
	PWM1_3_CTL_R &= ~(1<<1) ;                     // 4) re-loading down-counting mode
  //PWM1_3_GENA_R = (PWM_3_GENA_ACTCMPBD_ONE|PWM_3_GENA_ACTLOAD_ZERO);  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		PWM1_3_GENA_R = (PWM_3_GENA_ACTCMPAD_ZERO|PWM_3_GENA_ACTLOAD_ONE);
  // PB7 goes low on LOAD
  // PB7 goes high on CMPB down
  PWM1_3_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM1_3_CMPA_R = duty - 1;             // 6) count value when output rises
  PWM1_3_CTL_R |= 0x00000001;           // 7) start PWM1
  //PWM1_ENABLE_R |= 0x40;          // enable PF2/M1PWM6, Bit nr.6
	}
	
	// change duty cycle of PF2
	// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
	void PWM1_3_A_Duty(uint16_t duty){
  PWM1_3_CMPA_R = duty - 1;             // 6) count value when output rises
}
	
	void PWM1_3_A_enable(void){
  PWM1_ENABLE_R |= 0x00000040;          // enable PB2/M1PWM6
}

void PWM1_3_A_disable(void){
  PWM1_ENABLE_R &= (~0x00000040);          // disable PB2/M1PWM6
}
	
	
//---------------BSP-  DC Motor Speed TCC Init Pins 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/16 
//                = 80 MHz/16 = 5 MHz (in this example)
// Output on PA6/M1PWM2
void PWM1_1_A_Init(uint16_t period, uint16_t duty){

	volatile unsigned long delay;              //2-3 takts delay , volatile to not be optimized
  SYSCTL_RCGCPWM_R |= 0x02;             // 1) activate PWM1 Modul
  SYSCTL_RCGCGPIO_R |= 0x01;            // 2) activate port A
	//SYSCTL_RCC_R &= ~SYSCTL_RCC_USEPWMDIV;
	
	SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
//  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_32;  //    configure for /32 divider
	SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_16;
	
  GPIO_PORTA_AFSEL_R |= 0x40;           // enable alt funct on PA6
  GPIO_PORTA_PCTL_R &= ~0x0F000000;     // configure PA6 as M1PWM2
  GPIO_PORTA_PCTL_R |= 0x05000000;			//  M1PWM2
  GPIO_PORTA_AMSEL_R &= ~0x40;          // disable analog functionality on PA6
  GPIO_PORTA_DEN_R |= 0x40;             // enable digital I/O on PA6
	
	PWM1_1_CTL_R &= ~(1<<0) ;                     // 4) disable Generator (For A and B the same)
	PWM1_1_CTL_R &= ~(1<<1) ;                     // 4) re-loading down-counting mode
  //PWM1_3_GENA_R = (PWM_3_GENA_ACTCMPBD_ONE|PWM_3_GENA_ACTLOAD_ZERO);  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		PWM1_1_GENA_R = (PWM_1_GENA_ACTCMPAD_ZERO|PWM_1_GENA_ACTLOAD_ONE);
  // PB7 goes low on LOAD
  // PB7 goes high on CMPB down
  PWM1_1_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM1_1_CMPA_R = duty - 1;             // 6) count value when output rises
  PWM1_1_CTL_R |= 0x00000001;           // 7) start M1PWM2
	PWM1_ENABLE_R |= 0x00000004;          // enable PA6/M1PWM2
  //PWM1_ENABLE_R &= (~0x00000004);          // disable PA6/M1PWM2
	}
	
	// change duty cycle of PA6
	// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
	void PWM1_1_A_Duty(uint16_t duty){
  PWM1_1_CMPA_R = duty - 1;             // 6) count value when output rises
}
	
	void PWM1_1_A_enable(void){
  PWM1_ENABLE_R |= 0x00000004;          // enable PA6/M1PWM2
}

void PWM1_1_A_disable(void){
  PWM1_ENABLE_R &= (~0x00000004);          // disable PA6/M1PWM2
}
