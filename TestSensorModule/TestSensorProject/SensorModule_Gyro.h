#ifndef SENSORMODULE_GYRO_H_
#define SENSORMODULE_GYRO_H_

#define DDR_SPI DDRB
#define PORT_SPI PORTB

#define PIN_SPI_CS 4
#define PIN_SPI_MOSI 5
#define PIN_SPI_MISO 6
#define PIN_SPI_SCLK 7



void SetSensor_Gyro()
{

	//	0b101110000
	DDR_SPI = (1 << PIN_SPI_SCLK) | (1 <<  PIN_SPI_MOSI) | (0 <<  PIN_SPI_MISO) | (1 <<  PIN_SPI_CS);
	PORT_SPI |= (1 << PIN_SPI_CS);	//	set 
	
	//	double check the CPOL and CPHA relationship, page 168 for ATMega1284P
	SPCR = (1 << SPE) | (1 << MSTR) | (0 << DORD) | (0 << CPOL) | (0 << CPHA); //| (1<<SPR1) | (1 << SPR0);
}

void ReadSensor_Gyro_Select()
{
	PORT_SPI &= ~(1 << PIN_SPI_CS);
}

void ReadSensor_Gyro_Unselect()
{
	PORT_SPI |= (1 << PIN_SPI_CS);
}

uint16_t ReadSensor_Gyro_Answer()
{
	while (!(SPSR & (1 << SPIF)));
	uint16_t data = (uint16_t)SPDR;		//	read the Byte
	PORTD = data;
	data <<= 8;
	//while (!(SPSR & (1 << SPIF)));		//	wait
	data |= (uint16_t)SPDR;
	PORTD = data;
	return data;
}


void ReadSensor_Gyro_TransmitByte(uint8_t data)
{

	SPDR = data;						//	write the Byte
	while (!(SPSR & (1 << SPIF)));		//	wait
	
}

//	return true if successfull
uint8_t ReadSensor_Gyro_SetActiveMode()
{
	
	ReadSensor_Gyro_Select();
	ReadSensor_Gyro_TransmitByte(0b10010100);
	uint16_t answer = ReadSensor_Gyro_Answer();
	ReadSensor_Gyro_Unselect();
	answer &= (1 << 15);
	if (answer)
		return 0;
	else
		return 1;
}

//	returns true if successfull
uint8_t ReadSensor_Gyro_StartAngularConversion()
{
	ReadSensor_Gyro_Select();
	ReadSensor_Gyro_TransmitByte(0b10010100);
	uint16_t answer = ReadSensor_Gyro_Answer();
	ReadSensor_Gyro_Unselect();
	answer &= (1 << 15);
	if (answer)
		return 0;
	else
		return 1;
}

//	returns true if successfull
uint8_t ReadSensor_Gyro_StartTemperatureConversion()
{
	ReadSensor_Gyro_Select();
	ReadSensor_Gyro_TransmitByte(0b10011100);
	uint16_t answer = ReadSensor_Gyro_Answer();
	ReadSensor_Gyro_Unselect();
	return (uint8_t)!((1 << 15) & answer);
}

uint16_t ReadSensor_Gyro_GetADCValue()
{
	ReadSensor_Gyro_Select();
	ReadSensor_Gyro_TransmitByte(0b10000000);		//	send instruction
	uint16_t result = ReadSensor_Gyro_Answer();
	ReadSensor_Gyro_Unselect();
	return result;
}

int16_t ReadSensor_Gyro_Angular()
{

	//	I don't know why, but for some reason uncommenting this line of code
	//	makes the value read be constant, which is weird, idealy we would wan't
	//	to start the conversion before we try to get the ADC value
	ReadSensor_Gyro_StartAngularConversion();
	while(1)
	{
		for (uint32_t i = 0; i < 50; i++)
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
			//PORTD = adcValue;
			return adcValue;
		}
	}

}




#endif /* SENSORMODULE_GYRO_H_ */