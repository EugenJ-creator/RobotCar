// PWM.c
// Runs on TM4C123
// Use PWM0/PB6 and PWM1/PB7 to generate pulse-width modulated outputs.
// Implemented by Evgheni Jaruc
#include <stdint.h>
#include "PWM.h"
#include "../inc/tm4c123gh6pm.h"



//---------------BSP-  Buzzer  TCC Init Pins 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/16 
//                = 80 MHz/16 = 5 MHz (in this example)
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
  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_16;  //    configure for /16 divider
  PWM0_0_CTL_R = 0;                     // 4) re-loading down-counting mode
  PWM0_0_GENB_R = (PWM_0_GENB_ACTCMPBD_ONE|PWM_0_GENB_ACTLOAD_ZERO);
  // PB7 goes low on LOAD
  // PB7 goes high on CMPB down
  PWM0_0_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_0_CMPB_R = duty - 1;             // 6) count value when output rises
  PWM0_0_CTL_R |= 0x00000001;           // 7) start PWM0
	PWM0_ENABLE_R &= (~0x00000002);          // disable PB7/M0PWM1
  //PWM0_ENABLE_R |= 0x00000002;          // enable PB7/M0PWM1
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

// deactivate PD0/M0PWM6
void PWM0_0_B_Deactivate(void){   
	PWM0_0_CTL_R &= ~(1<<0) ;                     //  disable Generator
  PWM0_ENABLE_R &= (~0x00000002);          // disable PB7/M0PWM1
}




////------------------------------------------------------------------------------------------------------------------------------------
////---------------BSP-  DC Motor
//// period is 16-bit number of PWM clock cycles in one period (3<=period)
//// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
//// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
////                = BusClock/16 
////                = 80 MHz/16 = 5 MHz (in this example)

//// Output on PB4/M0PWM2

//void PWM0_1_A_Init(uint16_t period, uint16_t duty){
//  volatile unsigned long delay;
//  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0
//  SYSCTL_RCGCGPIO_R |= 0x02;            // 2) activate port B
//  delay = SYSCTL_RCGCGPIO_R;            // allow time to finish activating
//  GPIO_PORTB_AFSEL_R |= 0x10;           // enable alt funct on PB4
//  GPIO_PORTB_PCTL_R &= ~0x000F0000;     // configure PB4 as M0PWM2
//  GPIO_PORTB_PCTL_R |=  0x00040000;
//  GPIO_PORTB_AMSEL_R &= ~0x10;          // disable analog functionality on PB4
//  GPIO_PORTB_DEN_R |= 0x10;             // enable digital I/O on PB4
//  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
//  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
//  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_16;  //    configure for /16 divider
//  PWM0_1_CTL_R = 0;                     // 4) re-loading down-counting mode
//  PWM0_1_GENA_R = (PWM_1_GENA_ACTCMPAD_ONE|PWM_1_GENA_ACTLOAD_ZERO);
//  // PB4 goes low on LOAD
//  // PB4 goes high on CMPB down
//  PWM0_1_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
//  PWM0_1_CMPA_R = duty - 1;             // 6) count value when output rises
//  PWM0_1_CTL_R |= 0x00000001;           // 7) start PWM0
//	PWM0_ENABLE_R &= (~0x00000004);          // disable PB4/M0PWM2
//  //PWM0_ENABLE_R |= 0x00000004;          // enable PB4/M0PWM2
//}
//// change duty cycle of PB4
//// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
//void PWM0_1_A_Duty(uint16_t duty){
//  PWM0_1_CMPA_R = duty - 1;             // 6) count value when output rises
//}

//void PWM0_1_A_enable(void){
//  PWM0_ENABLE_R |= 0x00000004;          // enable PB4/M0PWM2
//}

//void PWM0_1_A_disable(void){
//  PWM0_ENABLE_R &= (~0x00000004);          // disable PB4/M0PWM2
//}

//// deactivate PB4/M0PWM2
//void PWM0_1_A_Deactivate(void){   
//	PWM0_1_CTL_R &= ~(1<<0) ;                     //  disable Generator
//  PWM0_ENABLE_R &= (~0x00000004);          // disable PB4/M0PWM2
//}


//------------------------------------------------------------------------------------------


//---------------BSP-  DC Motor Speed TCC Init Pins 
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/16 
//                = 80 MHz/16 = 5 MHz (in this example)
// Devider will the same for all PWM Modules
// Output on PC4/M0PWM6
void PWM0_3_A_Init(uint16_t period, uint16_t duty){
  volatile unsigned long delay;              //2-3 takts delay , volatile to not be optimized
  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0 Modul
  SYSCTL_RCGCGPIO_R |= 0x04;            // 2) activate port C
  delay = SYSCTL_RCGCGPIO_R;            // allow time to finish activating
  GPIO_PORTC_AFSEL_R |= 0x10;            // enable alt funct on PC4
  GPIO_PORTC_PCTL_R &= ~0x000F0000;     // configure PC4 as M0PWM6
  GPIO_PORTC_PCTL_R |=  0x00040000;
  GPIO_PORTC_AMSEL_R &= ~0x10;          // disable analog functionality on PC4
  GPIO_PORTC_DEN_R |= 0x10;             // enable digital I/O on PD0

  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
 	SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_16;  //    configure for /16 divider

	PWM0_3_CTL_R &= ~(1<<0) ;                     // 4) disable Generator
	PWM0_3_CTL_R &= ~(1<<1) ;                     // 4) re-loading down-counting mode
  //PWM0_3_GENA_R = (PWM_3_GENA_ACTCMPAD_ONE|PWM_3_GENA_ACTLOAD_ZERO);
	PWM0_3_GENA_R = (PWM_3_GENA_ACTCMPAD_ZERO|PWM_3_GENA_ACTLOAD_ONE);
  // PB7 goes high on LOAD
  // PB7 goes low on CMPB down
  PWM0_3_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_3_CMPA_R = duty - 1;             // 6) count value when output rises
  PWM0_3_CTL_R |= 0x00000001;           // 7) start PWM3
	//PWM0_ENABLE_R |= 0x00000040;          // enable PC4/M0PWM6
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

// deactivate PD0/M0PWM6
void PWM0_3_A_Deactivate(void){   
	PWM0_3_CTL_R &= ~(1<<0) ;                     //  disable Generator
  PWM0_ENABLE_R &= (~0x00000040);          // disable PC4/M0PWM6
}


// Output on PB4/M0PWM2

void TIMER_1_A_PWM_Init(uint16_t period, uint16_t duty){
  volatile unsigned long delay;
	
	SYSCTL_RCGCTIMER_R |= 0x02;      // activate timer1
  SYSCTL_RCGCGPIO_R |= 0x0002;     // activate port B
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};// ready?
	GPIO_PORTB_AFSEL_R |= 0x10;           // enable alt funct on PB4	
  GPIO_PORTB_DEN_R |= 0x10;             // enable digital I/O on PB4
																			// configure PB4 as T1CCP0
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFF0FFFF)+0x00070000;
  GPIO_PORTB_AMSEL_R &= ~0x10;          // disable analog functionality on PB4
	//GPIO_PORTB_DR4R_R	|= 0x10;           // Output Pad 4-mA Drive Enable
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN; // disable timer1A during setup
  TIMER1_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
                                   // configure for alternate (PWM) mode
  TIMER1_TAMR_R = (TIMER_TAMR_TAAMS|TIMER_TAMR_TAMR_PERIOD);
  TIMER1_TAILR_R = period-1;       // timer start value
  TIMER1_TAMATCHR_R = period-duty-1; // duty cycle = high/period
  //TIMER1_CTL_R |= TIMER_CTL_TAEN;  // enable timer1A 16-b, PWM	
}
// change duty cycle of PB4
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void TIMER_1_A_PWM_Duty(uint16_t duty){
  TIMER1_TAMATCHR_R = TIMER1_TAILR_R-duty; // duty cycle = high/period
}

void TIMER_1_A_PWM_enable(void){
  TIMER1_CTL_R |= TIMER_CTL_TAEN;          // enable PB4 PWM
}

void TIMER_1_A_PWM_disable(void){
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN;          // disable PB4 PWM
}

// deactivate PB4/PWM
void TIMER_1_A_PWM_Deactivate(void){   
	
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN;          // disable PB4/PWM
}



