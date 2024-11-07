#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "init.c"
#include "drive_functions.c"

#include "uart.c"

int main(void)
{
	PORT_init(); // inits in and outputs
	UART_Init();
	
	uint8_t right_wheel = 0;
	uint8_t left_wheel = 0;
	
	
		
    while (1) 
    {
		north();
		right_wheel = TCNT0 & 0x80;
		left_wheel  = TCNT0 & 0x40;
		PORTD |= right_wheel;
		PORTD |= left_wheel;
	}
}
/*
		west();
		right_wheel = TCNT0 & 0x80;
		left_wheel  = TCNT0 & 0x40;
		
		
		
		for (int i = 0; i < 12; ++i)
		UART_Recive();
				for (int i = 0; i < 12; ++i)
				UART_Transmit(input[i]);
				
		UART_Transmit('H');
		UART_Transmit('e');
		UART_Transmit('l');
		UART_Transmit('l');
		UART_Transmit('o');
		UART_Transmit(',');
		UART_Transmit(' ');
		UART_Transmit('W');
		UART_Transmit('o');
		UART_Transmit('r');
		UART_Transmit('l');
		UART_Transmit('d');
		UART_Transmit('.');

		unsigned char test = 0;
		
		char input[13];
		for (int i = 0; i < 12; ++i)
		input[i] = UART_Recive();
	*/	
		//test = UART_recive();
		//PORTB = test;
		
	