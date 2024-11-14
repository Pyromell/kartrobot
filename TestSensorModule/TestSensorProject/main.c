              

#include <avr/io.h>
#include <float.h>
#include "SensorModule_Gyro.h"
#include "SensorModule_IR.h"




int main(void)
{
	DDRD  = 0b11111111;

	uint8_t errorCode = 0;

	if (!SetSensor_IR_IO())
		errorCode = 0xFF;
	if (!SetSensor_Gyro())
		errorCode = 0xFF - 1;
	
	while (!errorCode)
	{

		uint16_t answer;
		answer = ReadSensor_Gyro_ExecuteInstruction(0b10010100);
		if (~answer & (1 << 15))
		{
			for (uint32_t i = 0; i < 500; i++)
			asm("NOP");
			answer = ReadSensor_Gyro_ExecuteInstruction(0b10000000);
		}	
	}
	
	PORTD = errorCode;
	return errorCode;
}

