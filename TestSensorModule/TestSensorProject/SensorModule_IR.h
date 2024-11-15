#ifndef SENSORMODULE_IR_H_
#define SENSORMODULE_IR_H_


void IR_ADC_start()
{
	ADCSRA = 0b11000000;
}

uint8_t IR_Init()
{
	PORTA = 0b00010010;
	DDRA  = 0b00110110;
	// bit 7-6 vref, 5 left or right shifted, 4-0 MUX
	ADMUX = 0b11100000;
	
	//bit 7 enable, 6 start conversion, 4 ADC done flag (Read only)
	ADCSRA =0b10000000;
	
	
	return 1;
	
}

float IR_ToDistance(uint16_t adcValue)
{
	float v_out = ((float)(adcValue)/1023.0)*5;
	
	float distance = 27.86/(v_out-0.42);
	return distance;
}


uint16_t IR_ToDistance_Temp(uint16_t value)
{
	if (value < 10) value = 10;
	return ((67870.0/ (value-3.0))- 40.0);
}



uint16_t IR_Read()
{
	IR_ADC_start();

	//	0b00010000 & b10000000
	//	waits until ADC convertion is done,
	//	indictated by the ADEN
	//	then reads the value
	while( !((1<<ADIF) & ADCSRA) );
	
	ADCSRA |= (1<<ADIF);

	uint16_t ADC_l = ADCL;
	uint16_t ADC_h = ADCH;
	

	ADC_h = ADC_h << 2;
	ADC_l = ADC_l >> 6;
	
	uint16_t ADC_done = ADC_h + ADC_l;
	return ADC_done;//ReadSensor_IR_ToDistance_Temp(ADC_done);

}




#endif /* SENSORMODULE_IR_H_ */