#pragma once
#include <avr/io.h>


void UART_Init( void )
{
	unsigned int BAUD = 9600;
	
	UBRR0H = (unsigned int)(BAUD >> 8);
	UBRR0L = (unsigned int)BAUD;
	
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<USBS0)|(1<<UCSZ00);
}

void UART_Transmit( unsigned char data)
{
	while ( !( UCSR0A & (1<<UDRE0)));
	
	UDR0 = data;
}