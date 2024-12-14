#ifndef SENSORMODULE_IR
#define SENSORMODULE_IR

#include "Gyro.h"
#include <avr/io.h>
#include <stdlib.h>

uint8_t IR_Init();
//	returns true if successfull
uint8_t IR_UpdateBuffer();
//  returns the most recent value
uint16_t IR_ReadValue(uint8_t sensorIndex);

#endif /* SENSORMODULE_IR_H_ */