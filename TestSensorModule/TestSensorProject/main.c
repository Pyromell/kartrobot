              

#include <avr/io.h>
#include <float.h>
#include "SensorModule_Gyro.h"
#include "SensorModule_IR.h"




int main(void)
{


	SetSensor_IR_IO();
	//SetSensor_Gyro();
	              
	uint16_t result;
	              
	//	Untested code
	//if (ReadSensor_Gyro_SetActiveMode())
	//if (ReadSensor_Gyro_StartAngularConversion())
	//result = ReadSensor_Gyro_ReadResult();
	              
	while (1)
	{
		uint16_t distance = ReadSensor_IR();
		PORTD = distance;
	}
}

