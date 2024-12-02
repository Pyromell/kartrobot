#include "ReflectSensor.h"

//	global variables
uint16_t dataBuffer_left;
uint16_t dataBuffer_right;
uint8_t leftWasHigh;
uint8_t rightWasHigh;
#define HIGHVOLTAGE (1024/2)


//	returns true if successful
uint8_t ReflectSensor_Init()
{
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	dataBuffer_left = 0;
	dataBuffer_right = 0;
	leftWasHigh = 0;
	rightWasHigh = 0;
	return 1;
}

uint16_t ReflectSensor_GetValue_Left()
{
	return dataBuffer_left;
}
uint16_t ReflectSensor_GetValue_Right()
{
	return dataBuffer_right;
}

void ReflectSensor_ADC_start(uint8_t sensorPinIndex)
{
	ADMUX = (ADMUX & 0b11100000) | sensorPinIndex;
	ADCSRA |= (1 << ADSC);
}


uint16_t ReflectSensor_ReadADC(uint8_t sensorIndex)
{
	ReflectSensor_ADC_start(sensorIndex);

	//	waits until ADC conversion is done,
	//	indicated by the ADEN
	while( !((1<<ADIF) & ADCSRA) );
	
	ADCSRA |= (1<<ADIF);


	uint16_t adcValue = ADC;
	return adcValue;

}

void ReflectSensor_Update()
{
	//	double check these pin values
	uint16_t left = ReflectSensor_ReadADC(6);
	uint16_t right = ReflectSensor_ReadADC(7);
	if (left > HIGHVOLTAGE)
	{
		if (leftWasHigh == 0)
			dataBuffer_left++;
		leftWasHigh = 1;
	}
	if (right > HIGHVOLTAGE)
	{
		if (rightWasHigh == 0)
			dataBuffer_left++;
		rightWasHigh = 1;		
	}
		
}
