#ifndef SENSORMODULE_GYRO_H_
#define SENSORMODULE_GYRO_H_

#define DDR_SPI DDRB
#define PORT_SPI PORTB

#define PIN_SPI_CS 4
#define PIN_SPI_MOSI 5
#define PIN_SPI_MISO 6
#define PIN_SPI_SCLK 7

void ReadSensor_Gyro_Select()
{
	PORT_SPI &= ~(1 << PIN_SPI_CS);
}

void ReadSensor_Gyro_Unselect()
{
	PORT_SPI |= (1 << PIN_SPI_CS);
}

uint16_t ReadSensor_Gyro_ExecuteInstruction(uint8_t instruction)
{
	
	uint8_t inData = 0;
	uint16_t answer = 0;
	
	ReadSensor_Gyro_Select();
	
	//	send instruction and read dummy byte
	SPDR = instruction;				//	send the instruction code
	while (!(SPSR & (1 << SPIF)));	//	wait
	//inData = SPDR;
	
	
	//	send dummy byte and receive high byte
	SPDR = 0;
	while (!(SPSR & (1 << SPIF)));
	inData = SPDR;
	//PORTD = inData;
	answer |= inData;
	answer <<= 8;
	
	//	send dummy byte and receive low byte
	SPDR = 0;
	while (!(SPSR & (1 << SPIF)));
	inData = SPDR;
	PORTD = inData;
	answer |= inData;
	
	ReadSensor_Gyro_Unselect();
	
	return answer;
}

uint8_t SetSensor_Gyro()
{

	//	0b101110000
	DDR_SPI = (1 << PIN_SPI_SCLK) | (1 <<  PIN_SPI_MOSI) | (0 <<  PIN_SPI_MISO) | (1 <<  PIN_SPI_CS);
	ReadSensor_Gyro_Select();
	
	//	double check the CPOL and CPHA relationship, page 168 for ATMega1284P
	SPCR = (1 << SPE) | (1 << MSTR) | (0 << DORD) | (0 << CPOL) | (0 << CPHA); //| (1<<SPR1) | (1 << SPR0);
	
	
	uint16_t answer = ReadSensor_Gyro_ExecuteInstruction(0b10010100);
	for (uint32_t i = 0; i < 500; i++)
		asm("NOP");
		
	if (answer & (1 << 15))
		return 0;
	else
		return 1;
	
}



#endif /* SENSORMODULE_GYRO_H_ */