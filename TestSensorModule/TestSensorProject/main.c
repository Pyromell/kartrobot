              

#include <avr/io.h>
#include <float.h>
#include "SensorModule_Gyro.h"
#include "SensorModule_IR.h"




int main(void)
{


	SetSensor_IR_IO();
	SetSensor_Gyro();
	              
	uint16_t result = 3;
	              
	//	Untested code
	if (ReadSensor_Gyro_SetActiveMode());
	
		        
	while (1)
	{
		result = ReadSensor_Gyro_Angular();
		//PORTD = result;
		//uint16_t distance = ReadSensor_IR();

	}
}

