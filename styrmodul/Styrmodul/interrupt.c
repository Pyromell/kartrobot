#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.c"

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
	drive_instr = UDR0;
  	
    // Send a receive confirmation ('R')
	while ( !(UCSR0A & (1 << UDRE0)) ) ;	
	for (int i = 0; i < 99; ++i)
		asm("NOP");
    
	UDR0 = 'U';			
}

ISR(TIMER3_COMPA_vect) {
	PORTB = 0xAA;
	PORTB = 0x00;
}