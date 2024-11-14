              

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
		
	DDRA |= 0b00000011;
	
	while (!errorCode)
	{

		uint16_t answer;
		answer = ReadSensor_Gyro_ExecuteInstruction(0b10010100);
		if (~answer & (1 << 15))
		{
			for (uint32_t i = 0; i < 500; i++)
			asm("NOP");
			answer = ReadSensor_Gyro_ExecuteInstruction(0b10000000);
			answer >>= 1;
			answer &= 0b0000011111111111; 
			int16_t deegress = ReadSensor_Gyro_Convert_Int(answer);
			PORTD = deegress;
			if (deegress < 0)
			{
				deegress >>= 8;
				//	1000 0000 0000 0000 --> 0000 0000 0000 0010
				deegress |= 0b00000010;
			}
			else
				deegress >>= 8;
			PORTA = deegress;
			
		}	
	}
	
	PORTD = errorCode;
	return errorCode;
}

