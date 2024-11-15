#ifndef SENSORMODULE_GYRO_H_
#define SENSORMODULE_GYRO_H_

#define DDR_SPI DDRB
#define PORT_SPI PORTB

#define PIN_SPI_CS 4
#define PIN_SPI_MOSI 5
#define PIN_SPI_MISO 6
#define PIN_SPI_SCLK 7

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
	//PORTD = inData;
	answer |= inData;
	
	Gyro_ChipUnselect();
	
	return answer;
}

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
		
	if (answer & (1 << 15))
		return 0;
	else
		return 1;
	
}

//	adc value
uint16_t Gyro_ADCToInt(uint16_t adcValue)
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

	//	+1024 = +300 °/s						+1.0 = +300 °/s 
	//	0	  =  0	 °/s			--->		 0.0 =  0	°/s		
	//	-1024 = -300 °/s						-1.0 = -300 °/s
	float rotationDegressPerSeconds = ((float)(angularRate) / 1024.0f) * 300.0f;

	return rotationDegressPerSeconds;
	
}



#endif /* SENSORMODULE_GYRO_H_ */