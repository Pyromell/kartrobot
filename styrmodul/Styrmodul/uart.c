#pragma once
#include <avr/io.h>
#include <time.h>

#define F_CPU 16000000UL
#define UBRR 103
//#define MYUBRR FCPU/8/BAUD-1

void UART_Init(void) {
	// UART med Kommunikationsmodul
	UBRR0 = UBRR;
	
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
	
	// UART med sensormodul
	UBRR1 = UBRR;
	
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	UCSR1C = (1 << USBS1) | (3 << UCSZ10);
}

unsigned char UART_Recive(void) {
	while ( !UCSR0A & (1 << RXC0) )
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