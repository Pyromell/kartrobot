#pragma once
#include <avr/io.h>
#include <time.h>

// Defines
#define UBRR 103 // This equals 9600 bps

// Global variables
uint8_t drive_instr	= 0x00;
uint8_t transmit_data = 0x00;

/***********************************
File Description:
	This file Handles the UART without the use of ISR

Pin Description:
	Pin 14 ISR(USART0_RX) (Receive UART data)
	Pin 15 ISR(USART0_RX) (Send UART data)
  
***********************************/

void UART_Init(void) {
	// UART w/ Communication module
	UBRR0 = UBRR;
	/* Enable receiver and transmitter */
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); // RXCIEn allows for interrupt to be generated
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
	
	// UART w/ Sensor module
	UBRR1 = UBRR;
	/* Enable receiver and transmitter */
	UCSR1B = (1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1); // RXCIEn allows for interrupt to be generated
	/* Set frame format: 8data, 2stop bit */
	UCSR1C = (1 << USBS1) | (3 << UCSZ10);
}

unsigned char UART_Recive(void) {
	while ( !(UCSR0A & (1 << RXC0)) )
	;
	return UDR0;
}

void UART_Transmit(const unsigned char* data) {
	// wait for empty transmit buffer
	while ( !(UCSR0A & (1 << UDRE0)) ) ;
	
	for (int i = 0; i < 99; ++i)
		asm("NOP");
		
	// put data into buffer
	UDR0 = data;
}


// This function sends a 'R' over UART0
// This should be called each time a move instr id completed to indicate to the communication module that
// the drive module is ready for a new drive instr
void UART_Transmit_Instr_Done()
{
	UART_Transmit('R');
}