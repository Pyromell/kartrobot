#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "init.c"
#include "drive_functions.c"

#include "uart.c"

//Global variables:

uint8_t wheel_marker_l = 0; // One cog index is one white 'marker' on the wheel
uint8_t wheel_marker_r = 0; 

uint16_t ir_data[6];
uint16_t gyro_data;

int main(void)
{
	PORT_init();
	PWM_init();
	UART_Init();
	
	uint8_t distance = 0;
    while (1) 
    {
		drive();
	}
}