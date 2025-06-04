// GPIO.c
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
 
 Modified for RobotCar Project by Evgheni Jaruc
 */

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "GPIO_DIF.h"
#include "pid_ctrl.h"
#include "DCMotor.h"

// Legend    TI part number
// CC2650BP  BOOSTXL-CC2650MA
// CC2650LP  LAUNCHXL-CC2650
// MSP432    MSP-EXP432P401R 
// TM4C123   EK-TM4C123GXL
// MKII      BOOSTXL-EDUMKII

#define NVIC_EN0_INT22          0x00400000  // Interrupt 22 enable
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define TIMER_TAMR_TACMR        0x00000004  // GPTM TimerA Capture Mode
#define TIMER_TAMR_TAMR_CAP     0x00000003  // Capture mode
#define TIMER_CTL_TAEN          0x00000001  // GPTM TimerA Enable
#define TIMER_CTL_TAEVENT_POS   0x00000000  // Positive edge
#define TIMER_CTL_TAEVENT_NEG   0x00000004  // Negative edge
#define TIMER_CTL_TAEVENT_BOTH  0x0000000C  // Both edges
#define TIMER_IMR_CAEIM         0x00000004  // GPTM CaptureA Event Interrupt
                                            // Mask
#define TIMER_ICR_CAECINT       0x00000004  // GPTM CaptureA Event Interrupt
                                            // Clear
#define TIMER_TAILR_TAILRL_M    0x0000FFFF  // GPTM TimerA Interval Load
                                            // Register Low
#define TIMER_TBILR_TBILRL_M    0x0000FFFF  // GPTM TimerB Interval Load
                                            // Register Low
extern uint32_t ControlledSpeed;  // Flag,  if speed is in tempomat modus
uint32_t CountInterrupt;
uint32_t Period = 0;              // (1/clock) units
uint32_t PeriodTemp = 0;          // (1/clock) units
uint32_t First;               // Timer0A first edge
uint32_t Last = 0;            // Timer3A last edge
uint32_t DoneRising;                // set each rising
uint32_t edgeIndex = 0;
uint32_t lastPeriods[50];    

//---------PID-------
#define PERIOD_MIN    89459  // Period min and max frequency of DC Motor, have to be multiplyed with 12.5 ns
#define PERIOD_MAX    626223  // Period max and min frequency of DC Motor, have to be multiplyed with 12.5 ns
pidctl_t PID;
float measuredSpeed = 0;
float desiredSpeed = 0;
float E = 0;  // Actual Error 
float O = 0;  // Controller outpup
uint32_t direction = 0; //Direction of motor

#ifdef DEFAULT
// Option 4) Use this option with CC2650BP without an MKII 
// Two board stack: CC2650BP+TM4C123 
// Acceptable projects:
//     VerySimpleApplicationProcessor_4C123
//     ApplicationProcessor_4C123
// This runs with the default version of SNP that ships on the BOOSTXL-CC2650MA
// signal  TM4C123                          CC2650BP       comment
//  3V3    J1.1  3.3V                       J1.1  3.3V     Power from TM4C123 to CC2650BP 
//  GND    J2.20 ground                     J2.20 ground   Connect ground together
//  NRESET J4.35 TM4C123 PC6, MSP432 P6.7   J4.35          Reset from TM4C123 to CC2650BP  
//  TX     J1.3  TM4C123 PB0, MSP432 P3.2   J1.3  DIO0_TXD UART from CC2650BP to TM4C123  
//  RX     J1.4  TM4C123 PB1, MSP432 P3.3   J1.4  DIO1_RXD UART from TM4C123 to CC2650BP
//  MRDY   J1.2  TM4C123 PB5, MSP432 P6.0   J1.2  IOID_7   Master ready from TM4C123 to CC2650BP 
//  SRDY   J2.19 TM4C123 PB2, MSP432 P2.5   J2.19 IOID_8   Slave ready from CC2650BP to TM4C123 

//------------GPIO_Init------------
// Initialize MRDY (out), SRDY (in), RESET (out) GPIO pins
// Input: none
// Output: none
void GPIO_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x0006;     // activate clock for Ports B and C
  while((SYSCTL_PRGPIO_R&0x06) != 0x06){};// wait for clocks to stabilize

  GPIO_PORTB_AFSEL_R &= ~0x04;     // SRDY is PB2, J2.19
  GPIO_PORTB_AMSEL_R &= ~0x04;     // disable analog functionality on PB2
  GPIO_PORTB_DEN_R |= 0x04;        // enable digital I/O on PB2
  GPIO_PORTB_PUR_R |= 0x04;        // enable pullup on PB2
  GPIO_PORTB_DIR_R &= ~0x04;       // input on PB2
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFFF0FF);

  GPIO_PORTC_AFSEL_R &= ~0x40;     // J4.35 Reset to CC2650 {TM4C123 PC6, MSP432 P6.7}
  GPIO_PORTC_AMSEL_R &= ~0x40;     // disable analog functionality on PC6
  GPIO_PORTC_DEN_R |= 0x40;        // enable digital I/O on PC6
  GPIO_PORTC_DIR_R |= 0x40;        // output on PC6
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xF0FFFFFF);

  GPIO_PORTB_AFSEL_R &= ~0x20;     // J1.2 MRDY {TM4C123 PB5, MSP432 P6.0}
  GPIO_PORTB_AMSEL_R &= ~0x20;     // disable analog functionality on PB5
  GPIO_PORTB_DEN_R |= 0x20;        // enable digital I/O on PB5
  GPIO_PORTB_DIR_R |= 0x20;        // output on PB5
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFF0FFFFF);
    
  SetMRDY();        //   MRDY=1  
  
  ClearReset();     // RESET=0    
}
#else
// These three options require either reprogramming the CC2650LP/CC2650BP or using a 7-wire tether
// These three options allow the use of the MKII I/O boosterpack
// Option 1) The CC2650BP is tethered to the TM4C123 using 7 wires (no reprogramming CC2650 needed)
// Two board stack: TM4C123+MKII  <---7 wires--->  CC2650BP
// signal  TM4C123                          CC2650BP       comment
//  3V3    J1.1  3.3V                       J1.1  3.3V     Power from TM4C123 to CC2650BP 
//  GND    J2.20 ground                     J2.20 ground   Connect ground together
//  NRESET J4.35 TM4C123 PC6, MSP432 P6.7   J4.35          Reset from TM4C123 to CC2650BP  
//  TX     J1.3  TM4C123 PB0, MSP432 P3.2   J1.3  DIO0_TXD UART from CC2650BP to TM4C123 
//  RX     J1.4  TM4C123 PB1, MSP432 P3.3   J1.4  DIO1_RXD UART from TM4C123 to CC2650BP
//  MRDY   J2.14 TM4C123 PB6, MSP432 P1.7   J1.2  IOID_7   Master ready from TM4C123 to CC2650BP 
//  SRDY   J2.12 TM4C123 PA3, MSP432 P5.2   J2.19 IOID_8   Slave ready from CC2650BP to TM4C123  

// Option 2) This version also works with a custom version of the SNP loaded onto the CC2650BP
// Program into CC2650BP: simple_np_cc2650bp_uart_pm_xsbl_mooc_custom.hex
// Three board stack: CC2650BP+TM4C123+MKII (BOOSTXL-CC2650MA, EK-TM4C123GXL, and BOOSTXL-EDUMKII)
// signal  TM4C123                          CC2650LP       comment
//  3V3    J1.1  3.3V                       J1.1  3.3V     Power from MSP432 to CC2650BP 
//  GND    J2.20 ground                     J2.20 ground   Connect ground together
//  NRESET J4.35 TM4C123 PC6, MSP432 P6.7   J4.35          Reset from TM4C123 to CC2650BP  
//  TX     J1.3  TM4C123 PB0, MSP432 P3.2   J1.3  DIO0_TXD UART from CC2650BP to TM4C123 
//  RX     J1.4  TM4C123 PB1, MSP432 P3.3   J1.4  DIO1_RXD UART from TM4C123 to CC2650BP
//  MRDY   J2.14 TM4C123 PB6, MSP432 P1.7   J1.14 DIO8     Master ready from TM4C123 to CC2650BP 
//  SRDY   J2.12 TM4C123 PA3, MSP432 P5.2   J2.12 DIO14    Slave ready from CC2650BP to TM4C123 

// Option 3) This version also works with a custom version of the SNP loaded onto the CC2650LP
// Program into CC2650LP: simple_np_cc2650lp_uart_pm_xsbl_mooc_custom.hex
// Remove Rx and Tx jumpers on CC2650LP
// Optional: remove two LED jumpers on Red, Green LEDs on CC2650 LaunchPad
// Place insulting tape over the set of 11 jumpers in middle, before stacking
// Three board stack: CC2650LP+TM4C123+MKII (LAUNCHXL-CC2650, EK-TM4C123GXL, and BOOSTXL-EDUMKII)
// signal  TM4C123                          CC2650LP       comment
//  3V3    J1.1  3.3V                       J1.1  3.3V     Power from TM4C123 to CC2650LP 
//  GND    J2.20 ground                     J2.20 ground   Connect ground together
//  NRESET J4.35 TM4C123 PC6, MSP432 P6.7   J4.35          Reset from TM4C123 to CC2650LP  
//  TX     J1.3  TM4C123 PB0, MSP432 P3.2   J1.3  DIO3_TXD UART from CC2650LP to TM4C123 
//  RX     J1.4  TM4C123 PB1, MSP432 P3.3   J1.4  DIO2_RXD UART from TM4C123 to CC2650LP
//  MRDY   J2.14 TM4C123 PB6, MSP432 P1.7   J1.14 DIO12    Master ready from TM4C123 to CC2650LP 
//  SRDY   J2.12 TM4C123 PA3, MSP432 P5.2   J2.12 DIO14    Slave ready from CC2650LP to TM4C123 

//------------GPIO_Init------------
// Initialize MRDY (out), SRDY (in), RESET (out) GPIO pins
// Input: none
// Output: none
void GPIO_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x0007;     // activate clock for Ports A B and C
  while((SYSCTL_PRGPIO_R&0x07) != 0x07){};// wait for clocks to stabilize

  GPIO_PORTA_AFSEL_R &= ~0x08;     // SRDY   J2.12 TM4C123 PA3, MSP432 P5.2}
  GPIO_PORTA_AMSEL_R &= ~0x08;     // disable analog functionality on PA3
  GPIO_PORTA_DEN_R |= 0x08;        // enable digital I/O on PA3
  GPIO_PORTA_PUR_R |= 0x08;        // enable pullup on PA3
  GPIO_PORTA_DIR_R &= ~0x08;       // input on PA3
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFF0FFF);

  GPIO_PORTC_AFSEL_R &= ~0x40;     // J4.35 Reset to CC2650 {TM4C123 PC6, MSP432 P6.7}
  GPIO_PORTC_AMSEL_R &= ~0x40;     // disable analog functionality on PC6
  GPIO_PORTC_DEN_R |= 0x40;        // enable digital I/O on PC6
  GPIO_PORTC_DIR_R |= 0x40;        // output on PC6
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xF0FFFFFF);

  GPIO_PORTB_AFSEL_R &= ~0x40;     // MRDY {J2.14 TM4C123 PB6, MSP432 P1.7}
  GPIO_PORTB_AMSEL_R &= ~0x40;     // disable analog functionality on PB6
  GPIO_PORTB_DEN_R |= 0x40;        // enable digital I/O on PB6
  GPIO_PORTB_DIR_R |= 0x40;        // output on PB6
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF);
    
  SetMRDY();        //   MRDY=1  
  
  ClearReset();     // RESET=0    
  
}
#endif

//---------------------------------------------------------------------------------


void GPIO_DC_Init(void){
	PortB_2_3_init();   // PWM DC IN1 IN2 Init
	
	
	// Motor Stop
	PortB_2_disable();   //  IN1 LOW
	PortB_3_disable();   //  IN2 LOW
}



// Subroutine to initialize port B pins for input and output (PWM DC IN1-IN2)
// PB2-3 is output 
// Inputs: None
// Outputs: None
// Notes: ...

void PortB_2_3_init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;      // 1) B clock
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize  
	GPIO_PORTB_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port B
	GPIO_PORTB_CR_R = 0x3F;           // allow changes to PB2-3
  GPIO_PORTB_AMSEL_R &= ~0xC;        // 2) disable analog function
  GPIO_PORTB_PCTL_R &= ~0x0000FF00;   // 3) GPIO clear bit PCTL  
  GPIO_PORTB_DIR_R |= 0xC;          // 4.2) PB2-3 output  
  GPIO_PORTB_AFSEL_R &= ~0xC;        // 5) no alternate function
//  GPIO_PORTF_PUR_R |= 0x;          // 6) enable pullup resistor on PF4       
  GPIO_PORTB_DEN_R |= 0xC;          // 7) enable digital pin B2B3
}

void PortB_2_enable(void){
	GPIO_PORTB_DATA_R |=	0x4;
}

void PortB_2_disable(void){
	GPIO_PORTB_DATA_R &=	~0x4;
}



void PortB_3_enable(void){
	GPIO_PORTB_DATA_R |=	0x8;
}

void PortB_3_disable(void){
	GPIO_PORTB_DATA_R &=	~0x8;
}



//  Init for Blue Led

void PortF_2_init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000020;         // activate port F
	GPIO_PORTF_DIR_R |= 0x04;                // make PF2 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;             // disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;                // enable digital I/O on PF2
                                           // configure PF2 as GPIO
	GPIO_PORTF_DR4R_R|= 0x04; 							 // Driver 2 mA
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;                  // disable analog functionality on PF
  GPIO_PORTF_DATA_R &= ~0x04;              // turn off LED
}


void ledToggleBlue(void){
	GPIO_PORTF_DATA_R ^= 0x04;             // toggle LED
}

//  Init for Red Led

void PortF_1_init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000020;         // activate port F
	GPIO_PORTF_DIR_R |= 0x02;                // make PF2 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x02;             // disable alt funct on PF1
  GPIO_PORTF_DEN_R |= 0x02;                // enable digital I/O on PF1
                                           // configure PF2 as GPIO
	GPIO_PORTF_DR4R_R|= 0x02; 							 // Driver 4 mA
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFFF0F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;                  // disable analog functionality on PF
  GPIO_PORTF_DATA_R &= ~0x02;              // turn off LED
}


void ledToggleRed(void){
	GPIO_PORTF_DATA_R ^= 0x02;             // toggle LED
}


//---------------------------- Measure Period between rising edge-------------------------
// max period is (2^24-1)*12.5ns = 209.7151ms!!!!!!!
// min period determined by time to run ISR, which is about 1us
void PeriodMeasure_Init(void){
  SYSCTL_RCGCTIMER_R |= 0x2;// activate timer1    
//  SYSCTL_RCGCGPIO_R |= 0x22;       // activate port B and port F
//                                   // allow time to finish activating
	
	SYSCTL_RCGCGPIO_R |= 0x2;        // activate port B
                                   // allow time to finish activating
	
  First = 0;                       // first will be wrong
  DoneRising = 0;                        // set on subsequent
  GPIO_PORTB_DIR_R &= ~0x20;       // make PB5 in
  GPIO_PORTB_AFSEL_R |= 0x20;      // enable alt funct on PB5
  GPIO_PORTB_DEN_R |= 0x20;        // enable digital I/O on PB5
                                   // configure PB5 as T1CCP1
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFF0FFFFF)+0x00700000;
  GPIO_PORTB_AMSEL_R &= ~0x20;     // disable analog functionality on PB5
//  GPIO_PORTF_DIR_R |= 0x04;        // make PF2 out (PF2 built-in blue LED)
//  GPIO_PORTF_AFSEL_R &= ~0x04;     // disable alt funct on PF2
//  GPIO_PORTF_DEN_R |= 0x04;        // enable digital I/O on PF2
//                                   // configure PF2 as GPIO
//  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
//  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
  TIMER1_CTL_R &= ~TIMER_CTL_TBEN; // disable timer1B during setup
  TIMER1_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
                                   // configure for 24-bit capture mode
  TIMER1_TBMR_R = (TIMER_TBMR_TBCMR|TIMER_TBMR_TBMR_CAP);
                                   
//  TIMER1_CTL_R &= ~(TIMER_CTL_TBEVENT_POS|0xC);  // rising edge
	TIMER1_CTL_R |= TIMER_CTL_TBEVENT_NEG;  // configure for falling edge event, capture mod
  TIMER1_TBILR_R = TIMER_TBILR_TBILRL_M;// start value, 16 bit value FFFF
  TIMER1_TBPR_R = 0xFF;            // activate prescale, creating 24-bit, 256 Clocks , Max.time 209.7152 ms
  TIMER1_IMR_R |= TIMER_IMR_CBEIM; // enable capture match interrupt
  TIMER1_ICR_R = TIMER_ICR_CBECINT;// clear timer1B capture match flag
  TIMER1_CTL_R |= TIMER_CTL_TBEN;  // enable timer1B 16-b, +edge timing, interrupts
                                   // Timer1B=priority 2
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFF0FFFFF)|0x00400000; //  bits  23-21    , 010 (2)
  NVIC_EN0_R = NVIC_EN0_INT22;     // enable interrupt 22 in NVIC, bit 22 in en0
}
void TIMER1B_Handler(void){
//  PF2 = PF2^0x04;  // toggle PF2
//  PF2 = PF2^0x04;  // toggle PF2
  TIMER1_ICR_R = TIMER_ICR_CBECINT;// acknowledge timer1B capture match
//	TIMER3_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER3A timeout
	
	// Apply Chebyshev filter, Aktiv low pass filter or with OPa, 5 or 4 order low pass filter 
	// Apply digital filter, for 100ms
	// Cut off to change to 1.5 kHz
	//Max Speed car : T = 88979, 0,94 ms
	//Lowest speed: T=623983,  5,72 ms
	//   First edge will will not considered for time priod, is skipped . Only afre second Period can be measured
		if (First!=0){ 
			// Check if period smoller then 0,83 milsec then this can be noise. Not calculate
			if (((First - TIMER1_TBR_R)&0xFFFFFF) > 66400) {
				PeriodTemp = (First - TIMER1_TBR_R)&0xFFFFFF;// 24 bits, 12.5ns resolution

					lastPeriods[edgeIndex] = PeriodTemp;
					edgeIndex ++;
			}
		}
		First = TIMER1_TBR_R;            // setup for next

		DoneRising = 1;
//  PF2 = PF2^0x04;  // toggle PF2
}

//--------------------------------------------------------------------------------------------------------------------------


// ***************** Timer3_Init ****************
// Activate Timer3 interrupts to run user task periodically
// Inputs:  period in units (1/clockfreq), 50ms
// Outputs: none
void Timer3_Init( unsigned long period){
  SYSCTL_RCGCTIMER_R |= 0x08;   // 0) activate TIMER3
//  PeriodicTask = task;          // user function
  TIMER3_CTL_R = 0x00000000;    // 1) disable TIMER3A during setup
  TIMER3_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER3_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER3_TAILR_R = period-1;    // 4) reload value
  TIMER3_TAPR_R = 0;            // 5) bus clock resolution
  TIMER3_ICR_R = 0x00000001;    // 6) clear TIMER3A timeout flag
  TIMER3_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 51, interrupt number 35
  NVIC_EN1_R = 1<<(35-32);      // 9) enable IRQ 35 in NVIC
  TIMER3_CTL_R = 0x00000001;    // 10) enable TIMER3A
	
	pidctl_reset(PID, E)  
}
//  Will be executed in 50 ms
void TIMER3A_Handler(void){
  TIMER3_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER3A timeout
//  (*PeriodicTask)();                // execute user task
	if (First == Last){
		// Reset Count of edges is speed == 0
		First= 0;
		Last = 0; 
		Period = 0;
		measuredSpeed = 0;
		if (ControlledSpeed){
			
			
			if (desiredSpeed!=0){
				E = desiredSpeed - measuredSpeed;
				pidctl(PID, E , O)
				
				if (direction == 0){
					DCforward(O , 0);
				} else if (direction == 1) {
					DCbackward(O , 0);
				}
			}
			
		}
	} else {
		uint32_t sum = 0;
		
			for (int i = 0; i < edgeIndex; i++){
				sum = sum + lastPeriods[i];
				Period = sum/edgeIndex;
			}
			edgeIndex = 0; 
			
//		uint32_t znamenateli = (((1<<24) / ( PERIOD_MIN )) - ((1<<24) / ( PERIOD_MAX )));
//		uint32_t cesliteli = ((((1<<24)*250) / ( PeriodTemp )) - (((1<<24)*250) / ( PERIOD_MAX )));
//		measuredSpeed = cesliteli/znamenateli;    //  
		
		if (ControlledSpeed){
			measuredSpeed = 204800000 / Period;   // RPS  * 256
			E = (float)(desiredSpeed - measuredSpeed);
			pidctl(PID, E , O)
			
			if (direction == 0){
				if (O>0){
					DCforward(O , 0);
				}
			} else if (direction == 1) {
				if (O>0){
					DCbackward(O , 0);
				}
			}
		}
	}
	Last = First;
}

// set Desired Speed
void setDesiredSpeed(uint32_t DesiredSpeed, uint32_t Direction){
	desiredSpeed = DesiredSpeed*2;
	direction = Direction;
}













