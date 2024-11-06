

#include <avr/io.h>
#include <float.h>


#define DDR_SPI DDRB
#define PIN_SPI_CS 4
#define PIN_SPI_MOSI 5
#define PIN_SPI_MISO 6
#define PIN_SPI_SCLK 7


void ADC_start()
{
	ADCSRA |= (1<<ADSC);
}

void SetSensor_IR_IO()
{
	PORTA = 0b00010010;
	DDRA  = 0b00110110;
	// bit 7-6 vref, 5 left or right shifted, 4-0 MUX
	ADMUX = 0b11100000;
	
	//bit 7 enable, 6 start conversion, 4 ADC done flag (Read only)
	ADCSRA =0b10000000;
	
}

float ReadSensor_IR_ToDistance(uint16_t adcValue)
{
	float v_out = ((float)(adcValue)/1023.0)*5;
	
	float distance = 27.86/(v_out-0.42);
	return distance;
}


uint16_t ReadSensor_IR_ToDistance_Temp(uint16_t value)
{
	if (value < 10) value = 10;
	return ((67870.0/ (value-3.0))- 40.0);
}

uint16_t ReadSensor_IR()
{
	ADC_start();
	
	//	0b00010000 & b10000000
	//	waits until ADC convertion is done,
	//	indictated by the ADEN
	//	then reads the value
	while( !((1<<ADEN) & ADCSRA) );
	
	uint8_t ADC_High = ADCH;
	uint8_t ADC_Low	= ADCL;
	
	//uint16_t ADC = ((uint16_t)(ADC_High) + (uint16_t)(ADC_Low));
	return ReadSensor_IR_ToDistance_Temp((uint16_t)(ADC_High) + (uint16_t)(ADC_Low));

}

void SetSensor_Gyro()
{

	//	0b101110000
	DDR_SPI = (1 << PIN_SPI_SCLK) | (1 <<  PIN_SPI_MOSI) | (0 <<  PIN_SPI_MISO) | (1 <<  PIN_SPI_CS);
	
	//	double check the CPOL and CPHA relationship, page 168 for ATMega1284P
	SPCR = (1 << SPE) | (0 << DORD) | (0 << CPOL) | (0 << CPHA);
	
}

uint16_t ReadSensor_Gyro_Answer()
{
	uint16_t data = (uint16_t)SPDR;		//	read the Byte
	data <<= 8;
	while (!(SPSR & (1 << SPIF)));		//	wait
	data |= (uint16_t)SPDR;
	while (!(SPSR & (1 << SPIF)));
	return data;
}

void ReadSensor_Gyro_Receive(uint8_t* data, uint8_t length)
{
	for (uint8_t i = 0; i < length;)
	{
		data[i++] = SPDR;						//	read the Byte
		while (!(SPSR & (1 << SPIF)));		//	wait
	}	
}


void ReadSensor_Gyro_TransmitByte(uint8_t data)
{

	SPDR = data;						//	write the Byte
	while (!(SPSR & (1 << SPIF)));		//	wait
	
}
void ReadSensor_Gyro_TransmitData(uint8_t* data, uint8_t length)
{
	for (uint8_t i = 0; i < length;)
	{
		SPDR = data[i++];						//	write the Byte
		while (!(SPSR & (1 << SPIF)));		//	wait
	}
}

void ReadSensor_Gyro_ReadInstruction(uint8_t instruction, uint8_t* data)
{
	ReadSensor_Gyro_TransmitByte(instruction);	//	send 1 Byte instruction
	ReadSensor_Gyro_Receive(data, 2);			//	receive 2 Bytes of data
	
}

//	return true if successfull
uint8_t ReadSensor_Gyro_SetActiveMode()
{
	ReadSensor_Gyro_TransmitByte(0b10010100);
	uint16_t answer = ReadSensor_Gyro_Answer();
	return !(uint8_t)!((1 << 15) & answer);
}

//	returns true if successfull
uint8_t ReadSensor_Gyro_StartAngularConversion()
{
	ReadSensor_Gyro_TransmitByte(0b10010100);
	uint16_t answer = ReadSensor_Gyro_Answer();
	return !(uint8_t)!((1 << 15) & answer);
}

//	returns true if successfull
uint8_t ReadSensor_Gyro_StartTemperatureConversion()
{
	ReadSensor_Gyro_TransmitByte(0b10011100);
	uint16_t answer = ReadSensor_Gyro_Answer();
	return (uint8_t)!((1 << 15) & answer);
}

uint16_t ReadSensor_Gyro_ReadResult()
{
	ReadSensor_Gyro_TransmitByte(0b10000000);		//	send instruction
	uint16_t answer = ReadSensor_Gyro_Answer();		//	read answer
	

	while ((answer & (1 << 13)) == 0)	//	while EOC is not set
	{
		if (answer & ((1 << 15)))		//	if at any point, the command is rejected, return immediately
			return 0xFFFF;
		answer = ReadSensor_Gyro_Answer();	//	
	}
	
	
	//	return only the bits 11 down to 1, shifted once to the right
	return (answer >> 1) & ((1 << 11) - 1);
}

int main(void)
{


	/*
		ADC init
		ADSC start conversion bit i ADCSRA
		ADMUX internal vref 
	*/
	SetSensor_IR_IO();
	SetSensor_Gyro();
	
	uint16_t result;
	
	//	Untested code
	if (ReadSensor_Gyro_SetActiveMode())
	if (ReadSensor_Gyro_StartAngularConversion())
		result = ReadSensor_Gyro_ReadResult();
	
	int cnt = 0;
	cnt = 10;
	cnt = result;

    while (1) 
    {
		
		uint16_t distance = ReadSensor_IR();
    }
}

