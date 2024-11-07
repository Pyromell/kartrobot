/*
 * Styrmodul.c
 *
 * Created: 2024-11-06 09:22:57
 * Author : 46704
 */ 

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "init.c"
#include "drive_functions.c"
#include "uart.c"


int main(void)
{
	PORT_init();				// inits in and outputs
	UART_init();
	uint8_t right_wheel = 0;
	uint8_t left_wheel = 0;
	unsigned char test = 0;
	sei();
    while (1) 
    {
		//west();
		//right_wheel = TCNT0 & 0x80;
		//left_wheel  = TCNT0 & 0x40;
		//PORTD |= right_wheel;
		//PORTD |= left_wheel;
		//test = UART_recive();
		//PORTB = test;
		PORTB = 0x01;
		UART_transmit('a');
		PORTB = 0x00;
	}
}

