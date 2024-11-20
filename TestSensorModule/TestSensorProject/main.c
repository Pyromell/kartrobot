              

#include <avr/io.h>
#include <float.h>
#include "SensorModule_Gyro.h"
#include "SensorModule_IR.h"




int main(void)
{
	DDRD  = 0b11111111;
	

	uint8_t errorCode = 0;

	if (!IR_Init())
		errorCode = 0xFF;
	if (!Gyro_Init())
		errorCode = 0xFF - 1;

	UART_Init();
		
	DDRA |= 0b00000011;
	
	while (!errorCode)
	{
		int16_t deegress = Gyro_FetchRotation();
		unsigned char data = UART_Receive_Sen();
		if (data == 'R')
			UART_Transmit_Sen('D');
		else
			UART_Transmit_Sen('O');
		//deegress >>= 8;
	}
	
	PORTD = errorCode;
	return errorCode;
}

