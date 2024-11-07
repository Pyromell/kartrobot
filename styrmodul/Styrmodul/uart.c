#pragma once
#include <avr/io.h>
#include <util/delay.h>

void UART_init(void) {
	unsigned int baud = 9600;
	
	// UART med Kommunikationsmodul
	UBRR0H = (unsigned char)(baud >> 8);
	UBRR0L = (unsigned char)baud;
	
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
	
	
	// UART med sensormodul
	UBRR1H = (unsigned char)(baud >> 8);
	UBRR1L = (unsigned char)baud;
	
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	UCSR1C = (1 << USBS1) | (3 << UCSZ10);
}

unsigned char UART_recive(void) {
	while ( !UCSR0A & (1 << RXC0) )
	;
	return UDR0;
}

void UART_transmit(unsigned char data) {

	// wait for empty transmit buffer
	while ( !(UCSR0A) & (1 << UDRE0) )
	;
	// put data into buffer
	UDR0 = data;
	
	while ( !(UCSR0A) & (1 << TXC0) )
	;
}

// 9600 bau
// 8900 pak
