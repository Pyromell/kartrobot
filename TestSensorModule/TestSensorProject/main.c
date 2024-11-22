              

#include <avr/io.h>
#include <avr/interrupt.h>
#include <float.h>
#include "SensorModule_Gyro.h"
#include "SensorModule_IR.h"
#include "uart.h"



int main(void)
{
	DDRD  = 0b11111111;
	
	float res = IR_ConvertADC(220);

	uint8_t errorCode = 0;
	
	if (res > 0.3f)
		errorCode = 2;

	if (!IR_Init())
		errorCode = 0xFF;
	if (!Gyro_Init())
		errorCode = 0xFF - 1;

	UART_Init();
		
	DDRA |= 0b00000011;
	
	sei();
	
	while (!errorCode)
	{
		Gyro_UpdateBuffer();
	}
	
	PORTD = errorCode;
	return errorCode;
}


//	Styrmodule comms
//		1 byte -	Request data id
//			-	G	:	Get gyro data
//				3 bytes
//					1	:	'G'
//					2	:	high byte of Gyro data
//					3	:	low byte of gyro data

