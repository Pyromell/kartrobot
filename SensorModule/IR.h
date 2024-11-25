#ifndef SENSORMODULE_IR
#define SENSORMODULE_IR

#include "Gyro.h"
#include <avr/io.h>

void IR_ADC_start(uint8_t sensorPinIndex);
uint8_t IR_Init();
float IR_ConvertADC(uint16_t adcValue);
uint16_t IR_ReadADC(uint8_t sensorIndex);
float IR_ReadDistance_CM(uint8_t sensorIndex);


#endif /* SENSORMODULE_IR_H_ */