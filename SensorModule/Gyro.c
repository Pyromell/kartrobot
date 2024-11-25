#include "Gyro.h"

#define DDR_SPI DDRB
#define PORT_SPI PORTB

#define PIN_SPI_CS 4
#define PIN_SPI_MOSI 5
#define PIN_SPI_MISO 6
#define PIN_SPI_SCLK 7

#define Gyro_CalibrationIterations 20
#define Gyro_StandingStillValues_Capacity 8

#define Gyro_dataBuffer_length 3

//	global variables
//		mainly used for calibration
int16_t  Gyro_standingStillValues[Gyro_StandingStillValues_Capacity];
uint8_t  Gyro_standingStillValues_length;
int16_t  Gyro_standingStillAvg;

int16_t Gyro_dataBuffer[Gyro_dataBuffer_length];
uint16_t Gyro_dataBuffer_readIndex = 0;
uint16_t Gyro_dataBuffer_writeIndex = 1;

//	store data in the buffer
void Gyro_WriteValue(int16_t gyroData)
{	
	Gyro_dataBuffer[Gyro_dataBuffer_writeIndex] = gyroData;
	Gyro_dataBuffer_readIndex = Gyro_dataBuffer_writeIndex;
	if (++Gyro_dataBuffer_writeIndex >= Gyro_dataBuffer_length)
		Gyro_dataBuffer_writeIndex = 0;
}

//	read data from the buffer
uint16_t Gyro_ReadValue()
{
	return Gyro_dataBuffer[Gyro_dataBuffer_readIndex];
}

void Gyro_ChipSelect()
{
	PORT_SPI &= ~(1 << PIN_SPI_CS);
}

void Gyro_ChipUnselect()
{
	PORT_SPI |= (1 << PIN_SPI_CS);
}

uint16_t Gyro_ExecuteInstruction(uint8_t instruction)
{
	
	uint8_t inData = 0;
	uint16_t answer = 0;
	
	Gyro_ChipSelect();
	
	//	send instruction and read dummy byte
	SPDR = instruction;				//	send the instruction code
	while (!(SPSR & (1 << SPIF)));	//	wait

	//	send dummy byte and receive high byte
	SPDR = 0;
	while (!(SPSR & (1 << SPIF)));
	inData = SPDR;
	answer |= inData;
	answer <<= 8;
	
	//	send dummy byte and receive low byte
	SPDR = 0;
	while (!(SPSR & (1 << SPIF)));
	inData = SPDR;
	answer |= inData;
	
	Gyro_ChipUnselect();
	
	return answer;
}

//	adc value
int16_t Gyro_ADCToInt(uint16_t adcValue)
{
	
	int32_t value = (adcValue);
	value *= 300;
	value /= 1024;
	value -= 300;
	return (int16_t)value;	//	--> -300, 0, 300

}

//	adc value 
float Gyro_ADCToFloat(uint16_t adcValue)
{
	
	int16_t angularRate = adcValue - 1024;

	//	+1024 = +300 �/s						+1.0 = +300 �/s 
	//	0	  =  0	 �/s			--->		 0.0 =  0	�/s		
	//	-1024 = -300 �/s						-1.0 = -300 �/s
	float rotationDegressPerSeconds = ((float)(angularRate) / 1024.0f) * 300.0f;

	return rotationDegressPerSeconds;
	
}

//	returns true if successful
uint8_t Gyro_UpdateBuffer()
{
	
	uint8_t success = 0;
	
	uint16_t answer = Gyro_ExecuteInstruction(0b10010100);
	if (~answer & (1 << 15))
	{
		for (uint32_t i = 0; i < 500; i++)
			asm("NOP");
		answer = Gyro_ExecuteInstruction(0b10000000);
		answer >>= 1;
		answer &= 0b0000011111111111;
		int16_t deegress = Gyro_ADCToInt(answer);
		
		uint8_t deegressSetToZero = 0;
		//for (uint8_t i = 0; i < Gyro_standingStillValues_length; i++)
			//if (deegress == Gyro_standingStillValues[i])
			//{
				//deegress = 0;
				//deegressSetToZero = 1;
			//}
		
		if (deegressSetToZero == 0)
			deegress -= Gyro_standingStillAvg;
			
		Gyro_WriteValue(deegress);
		success = 1;
		
	}
	
	return success;
	
	
}


//	returns true if successfull
uint8_t Gyro_Calibrate()
{
	//	initialize variables
	uint16_t finishedCalibration = 1;
	Gyro_standingStillAvg = 0;
	Gyro_standingStillValues_length = 0;
	uint16_t valuesFrequency[Gyro_StandingStillValues_Capacity];
	for (uint8_t i = 0; i < Gyro_StandingStillValues_Capacity; i++)
	{
		Gyro_standingStillValues[i] = 0x0000;
		valuesFrequency[i] = 0;
		
	}
	
	//	Fetch samples
	for (uint16_t i = 0; i < Gyro_CalibrationIterations; i++)
	{
		
		//	Fetch one sample
		Gyro_UpdateBuffer();
		int16_t degress = Gyro_ReadValue();
		if (degress == 0xFF)
		{
			//	if instruction rejected, return abort
			finishedCalibration = 0;
			break;
		}
		else
		{
			//	register the value
			for (uint8_t i = 0; i < Gyro_StandingStillValues_Capacity; i++)
			{
				
				if (valuesFrequency[i] == 0)
				{
					valuesFrequency[i] = 1;
					Gyro_standingStillValues[i] = degress;
					Gyro_standingStillValues_length++;
					break;
				}
				else if (Gyro_standingStillValues[i] == degress)
				{
					valuesFrequency[i]++;	
					break;
				}

				
			}
		}
		
	}
	if (finishedCalibration == 0)
		return 0;
	
	//	Calculate avg and decide what values will be defaulted to 0
	int32_t total = 0;
	for (uint8_t i = 0; i < Gyro_standingStillValues_length; i++)
	{
		total += (int32_t)(Gyro_standingStillValues[i]) * (int32_t)(valuesFrequency[i]);
	}
	total /= Gyro_CalibrationIterations;
	Gyro_standingStillAvg = total;
	
	return finishedCalibration;
}

//	returns true if successful
uint8_t Gyro_Init()
{
	
	//	0b101110000
	DDR_SPI = (1 << PIN_SPI_SCLK) | (1 <<  PIN_SPI_MOSI) | (0 <<  PIN_SPI_MISO) | (1 <<  PIN_SPI_CS);
	Gyro_ChipSelect();
	
	//	double check the CPOL and CPHA relationship, page 168 for ATMega1284P
	SPCR = (1 << SPE) | (1 << MSTR) | (0 << DORD) | (0 << CPOL) | (0 << CPHA); //| (1<<SPR1) | (1 << SPR0);
	
	
	uint16_t answer = Gyro_ExecuteInstruction(0b10010100);
	for (uint32_t i = 0; i < 500; i++)
		asm("NOP");
	
	if (answer & (1 << 15))	//	if answer has a leading 1 then the instruction was rejected
		return 0;
		
	if (!Gyro_Calibrate())
		return 0;


	
	
	return 1;
	
}
