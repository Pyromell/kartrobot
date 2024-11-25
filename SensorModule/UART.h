#pragma once

#include <avr/io.h>

// Defines
#define UBRR 103 // This equals 9600 bps

// Global variables
uint8_t com_instr = 0x00;
uint8_t transmit_data = 0x00;

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
unsigned char UART_Receive_CM(void) {
	while ( !(UCSR0A & (1 << RXC0)) )
	;
	return UDR0;
}

// Communications module
void UART_Transmit_CM(const unsigned char data) {
	// wait for empty transmit buffer
	while ( !(UCSR0A & (1 << UDRE0)) ) ;
	
	//for(int i = 0; i < 99; ++i)
	//	asm("NOP");
	
	// put data into buffer
	UDR0 = data;

	// wait for the hardware to set its done flag
	while(TXC0 == 1) ;
}

// SensorModule/StyrModule
unsigned char UART_Receive_SM(void) {
	while ( !(UCSR1A & (1 << RXC1)) )
	;
	return UDR1;
}

// SensorModule/StyrModule
void UART_Transmit_SM(const unsigned char data) {
	// wait for empty transmit buffer
	while ( !(UCSR1A & (1 << UDRE1)) ) ;
	// put data into buffer
	UDR1 = data;
	// wait for the hardware to set its done flag
	while(TXC1 == 1) ;
}
