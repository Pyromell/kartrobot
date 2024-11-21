#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.c"

uint16_t sensor_data_temp = 0;
uint16_t sensor_data = 0;
uint8_t control = 0;
unsigned char sensor = 0;
uint8_t data_complete = 0;

/***********************************
File Description:
	This file includes all of the interrupt vectors that are used.

Pin Description:
	Pin 14 ISR(USART0_RX) (Receive UART data)
	Pin 15 ISR(USART0_RX) (Send UART confirmation)
  
***********************************/

// This functions receives UART instr on UART0
// The ISR sends an 'U' to indicate that the data was received

ISR(USART0_RX_vector) {
	// Receive instruction data
	while ( !(UCSR0A & (1 << RXC0)) ) ;
	// Store received instr
	com_instr = UDR0;
  	
    // Send a receive confirmation ('R')
	while ( !(UCSR0A & (1 << UDRE0)) ) ;	
	for (int i = 0; i < 99; ++i)
		asm("NOP");
    
	UDR0 = 'U';
	
}

ISR(USART1_RX_vector) {
	// Store received instr
  switch(sensor) {
    case 'G' :
      control++;
      if (control == 1) {
        sensor_data_temp = UDR1;
        data_complete = 0;
      }
      else if (control >= 2) {
        sensor_data_temp = (sensor_data << 8);
        sensor_data_temp |= UDR1;
        sensor_data = sensor_data_temp;
        data_complete = 1;
        control = 0;
        sensor = 'x';
      }
      break;
    default:
    sensor = UDR1;
    data_complete = 0;
  }  
    
  // Send a receive confirmation ('R')
	while ( !(UCSR1A & (1 << UDRE1)) ) ;	
	for (int i = 0; i < 99; ++i)
		asm("NOP");
    
	UDR1 = 'U';
	
}

ISR(TIMER3_COMPA_vect) {
	UART_Transmit_Sen('G');
}
