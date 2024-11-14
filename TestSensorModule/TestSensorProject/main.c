              

#include <avr/io.h>
#include <float.h>
#include "SensorModule_Gyro.h"
#include "SensorModule_IR.h"




int main(void)
{


	SetSensor_IR_IO();
	SetSensor_Gyro();
	              

	              
	//	Untested code
	if (ReadSensor_Gyro_SetActiveMode())
		PORT_SPI = 0xFF;
       
	while (1)
	{
		uint16_t result = ReadSensor_Gyro_Angular();
	}
}

