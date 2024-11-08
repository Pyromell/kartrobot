#pragma once
#include <avr/io.h>
#include <time.h>

//This might not be needed?
//#define F_CPU 16000000UL

#define UBRR 103 // This equals 9600 bps

void UART_Init(void) {
	// UART w/ Communication module
	UBRR0 = UBRR;
	/* Enable receiver and transmitter */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
	
	// UART w/ Sensor module
	UBRR1 = UBRR;
	/* Enable receiver and transmitter */
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	/* Set frame format: 8data, 2stop bit */
	UCSR1C = (1 << USBS1) | (3 << UCSZ10);
}

unsigned char UART_Recive(void) {
	while ( !(UCSR0A & (1 << RXC0)) )
	;
	return UDR0;
}

void UART_Transmit(unsigned char data) {
	// wait for empty transmit buffer
	while ( !(UCSR0A & (1 << UDRE0)) ) ;
	
	for (int i = 0; i < 99; ++i)
		asm("NOP");
		
	// put data into buffer
	UDR0 = data;
}