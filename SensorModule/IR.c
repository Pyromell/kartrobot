#include "IR.h"

#define VREF 5.0f
//	ADC conversion value is 10 bits
#define ADC_MaxValue 1024

#define IR_PORT PORTA
#define IR_PIN0 0	//	front-right
#define IR_PIN1 1	//	front left
#define IR_PIN2 2	//	front middle
#define IR_PIN3 3	//	back right
#define IR_PIN4 4	//	back left
#define IR_PIN5 5	//	back middle

#define IR_dataBuffer_length 6
volatile int16_t IR_dataBuffer[IR_dataBuffer_length];
volatile int16_t IR_dataBuffer_adc[IR_dataBuffer_length];

uint8_t IR_medianIndex = 0;
volatile int16_t IR_median[IR_dataBuffer_length];
volatile int16_t IR_medianBuffer[6][20];


int16_t compare (const void* a, const void* b)
{
	return (*(int16_t*)a - *(int16_t*)b);
}

void IR_WriteValue(uint8_t sensorIndex, uint8_t IRdata)
{	
	IR_dataBuffer[sensorIndex] = IRdata;
	IR_medianBuffer[sensorIndex][IR_medianIndex] = IRdata;
	
	int16_t temp_arr [20];
	for (uint8_t i = 0 ; i <20 ; i++)
		temp_arr[i] = IR_medianBuffer[sensorIndex][i];
	
	int n = sizeof(temp_arr) / sizeof(temp_arr[0]);
	
	qsort(temp_arr,n,sizeof(int16_t),compare);
	
	if (sensorIndex == 5)
	{
		IR_medianIndex++;
		if(IR_medianIndex > 19)
			IR_medianIndex = 0;
	}

	
	IR_median[sensorIndex] = temp_arr[10];
	
}
uint16_t IR_ReadValue(uint8_t sensorIndex)
{
	return IR_dataBuffer[sensorIndex];
}

uint16_t IR_ReadValueMedian(uint8_t sensorIndex)
{
	return IR_median[sensorIndex];
}


uint8_t IR_Init()
{
	PORTA = 0b00000000;
	DDRA = 0b00000000;
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	return 1;
}

void IR_ADC_start(uint8_t sensorPinIndex)
{
	ADMUX = (ADMUX & 0b11100000) | sensorPinIndex;
	ADCSRA |= (1 << ADSC);
}


/*
	6cm = 3.15
	8cm = 2.75
	10cm = 2.3
	15cm = 1.65
	20cm = 1.3
	25cm = 1.08
	30cm = 0.92
	40cm = 0.75
	50cm = 0.60
	60cm = 0.50
	70cm = 0.45
	80cm = 0.40
	100cm = 0.30	(made up value)
	
	ADC gives 10 bits
	VREF = 5.0f ?	
*/

#define IR_TableLength 13
uint8_t IR_DistanceTable[IR_TableLength]	= {	6,		8,		10,		15,		20,		25,		30,		40,		50,		60,		70,		80,		120}; 
float IR_VoltageTable[IR_TableLength]		= {	3.15f,	2.75f,	2.3f,	1.65f,	1.3f,	1.08f,	0.92f,	0.75f,	0.60f,	0.50f,	0.45f,	0.40f,	0.30f};

float IR_ConvertADC(uint16_t adcValue)
{
	float adc = (float)adcValue;
	float voltage = (adc * VREF) / ADC_MaxValue;
	
	for (uint8_t i = 1; i < IR_TableLength; i++)
	{
		
		float voltagePoint = IR_VoltageTable[i];
		if (voltagePoint < voltage)
		{
			
			//	load the values from the table
			uint8_t lowDistanceValue = IR_DistanceTable[i - 1];
			uint8_t highDistanceValue = IR_DistanceTable[i];
			float lowDistanceVoltage = IR_VoltageTable[i - 1];
			float highDistanceVoltage = IR_VoltageTable[i];
			
			//	calculate which point is closest to the given voltage, from a scale of 1.0 to 0.0
			float totalVoltageDifference = (lowDistanceVoltage - voltage) + (voltage - highDistanceVoltage);
			float lowDistanceMultiplier  = 1.0f - ((lowDistanceVoltage - voltage) / totalVoltageDifference);
			float highDistanceMultiplier = 1.0f - ((voltage - highDistanceVoltage) / totalVoltageDifference);
			
			//	distance can be found on the line between the two closest points
			float distance = (lowDistanceMultiplier * lowDistanceValue) + (highDistanceMultiplier * highDistanceValue);
			return distance;
			
			
		}
	}
	
	return 0.0f;
}



uint16_t IR_ReadADC(uint8_t sensorIndex)
{
	IR_ADC_start(sensorIndex);

	//	waits until ADC conversion is done,
	//	indicated by the ADEN
	while( !((1<<ADIF) & ADCSRA) );

	ADCSRA |= (1<<ADIF);

	uint16_t adcValue = ADC;
	return adcValue;

}

float IR_ReadDistance_CM(uint8_t sensorIndex)
{
	uint16_t adcValue = IR_ReadADC(sensorIndex);
	IR_dataBuffer_adc[sensorIndex] = adcValue;
	float distanceCM = IR_ConvertADC(adcValue);
	return distanceCM;
}

//	NOTE:
//		Only one pin can undergo a ADC convertion at a time,
//		we will loop and convert the adc values of all 6 pins 
//		used to communicate with the IR sensor
//		Consider replacing this function with a system that can convert
//		asynchronously. Running this function wastes a lot of time.
//		ALSO this function is not optimized in order to avoid bugs.
//		The ADC to CM conversion should idealy be done while waiting for the 
//		next conversion.
//	TLDR:	THIS IS BAD FOR PERFORMANCE PLS FIX 
uint8_t IR_UpdateBuffer()
{
	for (uint8_t sensorIndex = 0; sensorIndex < 6; sensorIndex++)
	{
		float cm = IR_ReadDistance_CM(sensorIndex);
		uint8_t result = (uint8_t)cm;
		if (result == 0)
			result = 255;
		IR_WriteValue(sensorIndex, result);
	}

	return 1;
}
