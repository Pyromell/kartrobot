#pragma once
#include <avr/io.h>

// Defines
#define UBRR 51 // This equals 19200 bps

/***********************************
File Description:
	This file Handles the UART without the use of ISR

Pin Description:
	Pin 14 ISR(USART0_RX) (Receive UART) (com. module)
	Pin 15 ISR(USART0_TX) (Send UART)    (com. module)
	Pin 16 ISR(USART1_RX) (Receive UART) (sen. module)
	Pin 17 ISR(USART1_TX) (Send UART)    (sen. module)
  
***********************************/

void UART_Init(void) {
	// UART w/ Communication module
	UBRR0 = UBRR;
	/* Enable receiver and transmitter and interrupt(RXCIEn) */
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
	
	// UART w/ Sensor module
	UBRR1 = UBRR;
	/* Enable receiver and transmitter and interrupt(RXCIEn) */
	UCSR1B = (1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1);
	/* Set frame format: 8data, 2stop bit */
	UCSR1C = (1 << USBS1) | (3 << UCSZ10);
}

// Communications module
void UART_Transmit_Com(const unsigned char data) {
	while ( !(UCSR0A & (1 << UDRE0)) ) ; // wait for empty transmit buffer
	UDR0 = data; // put data into buffer
	while(TXC0 == 1) ; // wait for the hardware to set its done flag
}

// Sensor module
void UART_Transmit_Sen(const unsigned char data) {
	while ( !(UCSR1A & (1 << UDRE1)) ) ; // wait for empty transmit buffer
	UDR1 = data; // put data into buffer
	while(TXC1 == 1) ; // wait for the hardware to set its done flag
}

// This function sends a '0x0A' for RECEIVED over UART0
// This should be called each time we receive a drive instr. to indicate to
// the communication module that the drive module is currently doing a drive
// function.
void UART_Transmit_Instr_Received()
{
	while ( !(UCSR1A & (1 << UDRE1)) ) ; // wait for empty transmit buffer
	UDR0 = 0x0A; // put data into buffer
	while(TXC0 == 1) ;
}

// This function sends a '0x0B' for DONE over UART0
// This should be called each time a move instr id completed to indicate to the
// communication module that the drive module is ready for a new drive instr
void UART_Transmit_Instr_Done()
{
	while ( !(UCSR1A & (1 << UDRE1)) ) ; // wait for empty transmit buffer
	UDR0 = 0x0B; // put data into buffer
	while(TXC0 == 1) ;
}
