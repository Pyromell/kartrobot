#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.c"

uint16_t sensor_gyro_temp = 0;
uint16_t sensor_gyro = 0;
uint8_t control = 0;
unsigned char sensor = 0;

/***********************************
File Description:
	This file includes all of the interrupt vectors that are used.

Pin Description:
	Pin 14 ISR(USART0_RX) (Receive UART data)
	Pin 15 ISR(USART0_RX) (Send UART confirmation)
  
***********************************/

// This functions receives UART instr on UART0
// The ISR sends an 'U' to indicate that the data was received

ISR(USART0_RX_vect) {
	// Store received data
	com_instr = UDR0;
  	
    // Send a receive confirmation ('R')
	//UDR0 = 'R';
}


ISR(USART1_RX_vect) {
  switch(sensor) {
    case 'G' :
      control++;
      if (control == 1) {
        sensor_gyro_temp = UDR1;
      }
      else if (control >= 2) {
        sensor_gyro_temp = (sensor_gyro_temp << 8);
        sensor_gyro_temp |= UDR1;
        sensor_gyro = sensor_gyro_temp;
        control = 0;
        sensor = 'x';
      }
      break;
	  
    default:
		sensor = UDR1;
		break;
  }  
 
  /*
  // Send a receive confirmation ('R')
	while ( !(UCSR1A & (1 << UDRE1)) ) ;	
	for (int i = 0; i < 99; ++i)
		asm("NOP");
    
	UDR1 = 'U';
	*/
}



uint8_t timer_10_ms = 0;

ISR(TIMER3_COMPA_vect) {
	timer_10_ms++;
}
