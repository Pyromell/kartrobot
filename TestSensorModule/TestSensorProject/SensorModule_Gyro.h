#ifndef SENSORMODULE_GYRO_H_
#define SENSORMODULE_GYRO_H_

#define DDR_SPI DDRB
#define PIN_SPI_CS 4
#define PIN_SPI_MOSI 5
#define PIN_SPI_MISO 6
#define PIN_SPI_SCLK 7



void SetSensor_Gyro()
{

	//	0b101110000
	DDR_SPI = (1 << PIN_SPI_SCLK) | (1 <<  PIN_SPI_MOSI) | (0 <<  PIN_SPI_MISO) | (1 <<  PIN_SPI_CS);
	
	//	double check the CPOL and CPHA relationship, page 168 for ATMega1284P
	SPCR = (1 << SPE) | (1 << MSTR) | (0 << DORD) | (0 << CPOL) | (0 << CPHA); //| (1<<SPR1) | (1 << SPR0);
}

uint16_t ReadSensor_Gyro_Answer()
{
	while (!(SPSR & (1 << SPIF)));
	uint16_t data = (uint16_t)SPDR;		//	read the Byte
	//PORTD = data;
	data <<= 8;
	//while (!(SPSR & (1 << SPIF)));		//	wait
	data |= (uint16_t)SPDR;
	//PORTD = data;
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
	answer &= (1 << 15);
	if (answer)
		return 0;
	else
		return 1;
}

//	returns true if successfull
uint8_t ReadSensor_Gyro_StartAngularConversion()
{
	ReadSensor_Gyro_TransmitByte(0b10010100);
	uint16_t answer = ReadSensor_Gyro_Answer();
	answer &= (1 << 15);
	if (answer)
		return 0;
	else
		return 1;
}

//	returns true if successfull
uint8_t ReadSensor_Gyro_StartTemperatureConversion()
{
	ReadSensor_Gyro_TransmitByte(0b10011100);
	uint16_t answer = ReadSensor_Gyro_Answer();
	return (uint8_t)!((1 << 15) & answer);
}

uint16_t ReadSensor_Gyro_GetADCValue()
{
	ReadSensor_Gyro_TransmitByte(0b10000000);		//	send instruction
	return ReadSensor_Gyro_Answer();
}

int16_t oldValue = 0;
int16_t cnt = 0;
int16_t ReadSensor_Gyro_Angular()
{

	//	I don't know why, but for some reason uncommenting this line of code
	//	makes the value read be constant, which is weird, idealy we would wan't
	//	to start the conversion before we try to get the ADC value
	//ReadSensor_Gyro_StartAngularConversion();
	while(1)
	{
		for (uint32_t i = 0; i < 50000; i++)
		{
			asm("NOP");	
		}			
			
		uint16_t answerValue = ReadSensor_Gyro_GetADCValue();
		uint16_t headerValue = answerValue & 0b1010000000000000;
		uint16_t adcValue    = answerValue & 0b0000111111111111;
			
		if (headerValue                    & 0b1000000000000000)	//	check if command was accepted, if not, restart conversion
			ReadSensor_Gyro_StartAngularConversion();
		else if (headerValue               & 0b0010000000000000)	//	check EOC
		{
			if (oldValue != adcValue)
			{	//	this simply test that we get new values, which we do
				//	the output of the gyroscope is not very stable
				
				oldValue = adcValue;
				cnt++;
				PORTD = adcValue;
			}
			return adcValue;
		}
	}

}




#endif /* SENSORMODULE_GYRO_H_ */