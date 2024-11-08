#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "init.c"
#include "drive_functions.c"

#include "uart.c"

int main(void)
{
	PORT_init();
	PWM_init();
	UART_Init();
	
	uint8_t right_wheel = 0;
	uint8_t left_wheel = 0;

	north();
	for (int i = 0; i < 99999; ++i)
	{
		right_wheel = speed_select(0) & 0x80;
		left_wheel = speed_select(0) & 0x40;
		PORTD |= right_wheel;
		PORTD |= left_wheel;
	}
		
    while (1) 
    {
		
		PORTD &= 0x0;
		
		/*
		south();
		for (int i = 0; i < 99999; ++i)
		{
			right_wheel = speed_select(1) & 0x80;
			left_wheel = speed_select(1) & 0x40;
			PORTD |= right_wheel;
			PORTD |= left_wheel;
		}
		
		for (int i = 0; i < 99999999999; ++i)
		{
			PORTD = 0x00;
			PORTD = 0x00;
		}
		*/
		/*
		north();
		for (int i = 0; i < 999; ++i)
		{
			right_wheel = speed_select(2) & 0x80;
			left_wheel = speed_select(2) & 0x40;
			PORTD |= right_wheel;
			PORTD |= left_wheel;
		}
		
		for (int i = 0; i < 999999; ++i)
		{
			PORTD = 0x00;
			PORTD = 0x00;
		}
		
		south();
		for (int i = 0; i < 999999; ++i)
		{
			right_wheel = speed_select(3) & 0x80;
			left_wheel = speed_select(3) & 0x40;
			PORTD |= right_wheel;
			PORTD |= left_wheel;
		}
		
		for (int i = 0; i < 999999; ++i)
		{
			PORTD = 0x00;
			PORTD = 0x00;
		}
		*/
	}
}