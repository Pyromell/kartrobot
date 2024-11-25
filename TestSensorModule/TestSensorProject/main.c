              

#include <avr/io.h>
#include <avr/interrupt.h>
#include <float.h>
#include "SensorModule_Gyro.h"
#include "SensorModule_IR.h"
#include "uart.h"
#include "ComsStyrModule.h"
#include "ComsControllModule.h"

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
