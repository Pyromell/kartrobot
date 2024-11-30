              

#include <avr/io.h>
#include <avr/interrupt.h>
#include <float.h>
#include "Gyro.h"
#include "IR.h"
#include "uart.h"
#include "TalkSM.h"
#include "TalkCM.h"
#include "ReflectSensor.h"

int main(void)
 {
	DDRD  = 0b11111111;
	uint8_t errorCode = 0;

	if (!IR_Init())
		errorCode = 0xFF;
	if (!Gyro_Init())
		errorCode = 0xFF - 1;

	UART_Init();
	
	sei();
	
	while (!errorCode)
	{
		Gyro_UpdateBuffer();
		IR_UpdateBuffer();
	}
	
	PORTD = errorCode;
	return errorCode;
}
