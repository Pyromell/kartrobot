#ifndef SENSORMODULE_IR
#define SENSORMODULE_IR

#define VREF 5.0f
//	ADC conversion value is 10 bits
#define ADC_MaxValue 1024

#define IR_PORT PORTA
#define IR_PIN0 0
#define IR_PIN1 1
#define IR_PIN2 2
#define IR_PIN3 3
#define IR_PIN4 4
#define IR_PIN5 5


void IR_ADC_start(uint8_t sensorPinIndex)
{
	ADMUX &= 11100000;
	ADMUX |= 11100000 & sensorPinIndex;
	ADCSRA = 0b11000000;
}

uint8_t IR_Init()
{
	PORTA = 0b00000000;
	DDRA  = 0b00000000;
	// bit 7-6 vref, 5 left or right shifted, 4-0 MUX
	ADMUX = 0b11100000;
	
	//bit 7 enable, 6 start conversion, 4 ADC done flag (Read only)
	ADCSRA = 0b10000000;
	
	
	return 1;
	
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
	
	ADC gives 10 bits
	VREF = 5.0f ?	
*/

#define IR_TableLength 12
uint8_t IR_DistanceTable[IR_TableLength]	= {	6,		8,		10,		15,		20,		25,		30,		40,		50,		60,		70,		80}; 
float IR_VoltageTable[IR_TableLength]		= {	3.15f,	2.75f,	2.3f,	1.65f,	1.3f,	1.08f,	0.92f,	0.75f,	0.60f,	0.50f,	0.45f,	0.40f};

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

	uint16_t ADC_l = ADCL;
	uint16_t ADC_h = ADCH;
	

	ADC_h = ADC_h << 2;
	ADC_l = ADC_l >> 6;
	
	uint16_t ADC_done = ADC_h + ADC_l;
	return ADC_done;//ReadSensor_IR_ToDistance_Temp(ADC_done);

}

float IR_ReadDistance_CM(uint8_t sensorIndex)
{
	uint16_t adcValue = IR_ReadADC(sensorIndex);
	float distanceCM = IR_ConvertADC(adcValue);
	return distanceCM;
}


#endif /* SENSORMODULE_IR_H_ */