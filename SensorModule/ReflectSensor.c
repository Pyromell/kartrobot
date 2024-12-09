#include "ReflectSensor.h"
#include <limits.h>

//	global variables
uint16_t dataBuffer_left;
uint16_t dataBuffer_right;
uint16_t dataBuffer_leftStored;
uint16_t dataBuffer_rightStored;

uint8_t leftWasHigh;
uint8_t rightWasHigh;
#define HIGHVOLTAGE (1024/2)


//	returns true if successful
uint8_t ReflectSensor_Init()
{
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
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
	if (left < HIGHVOLTAGE)
	{
		if (leftWasHigh == 0)
		{
			dataBuffer_left++;
			leftWasHigh = 1;		
		}
	}
	else
		leftWasHigh = 0;
		
	if (right < HIGHVOLTAGE)	//if white passes sensor it counts up (white = low ADC value)
	{
		if (rightWasHigh == 0)
		{	
			dataBuffer_right++;
			rightWasHigh = 1;		
		}
	}
	else		// has to be high (not on white) before adding another step
		rightWasHigh = 0;
		
}

void ReflectSensor_StoreValue()
{
	dataBuffer_leftStored = dataBuffer_left;
	dataBuffer_rightStored = dataBuffer_right;
}

// 50 240 34 50
//	  240
//		 12

//			255 - 240 + 12
//		Rel 12
uint16_t ReflectSensor_GetValue_Right_Rel()
{
	if (dataBuffer_right >= dataBuffer_rightStored)
	{
		uint16_t stepsSinceStored = dataBuffer_right - dataBuffer_rightStored;
		return stepsSinceStored;
	}
	else
	{
		uint16_t stepsSinceStored = UINT16_MAX - dataBuffer_rightStored + dataBuffer_right;
		return stepsSinceStored;		
	}
}


uint16_t ReflectSensor_GetValue_Left_Rel()
{
	if (dataBuffer_left >= dataBuffer_leftStored)
	{
		uint16_t stepsSinceStored = dataBuffer_left - dataBuffer_leftStored;
		return stepsSinceStored;
	}
	else
	{
		uint16_t stepsSinceStored = UINT16_MAX - dataBuffer_leftStored + dataBuffer_left;
		return stepsSinceStored;
	}	
}