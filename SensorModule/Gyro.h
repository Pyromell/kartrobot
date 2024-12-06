#ifndef SENSORMODULE_GYRO
#define SENSORMODULE_GYRO

#include <avr/io.h>

//	read the most recent value gathered from the Gyro
uint16_t Gyro_ReadValue();

//	returns true if successfull
uint8_t Gyro_UpdateBuffer();
//	returns true if successfull
uint8_t Gyro_Calibrate();
//	returns true if successfull
uint8_t Gyro_Init();

uint8_t shouldCalibrate = 0;

#endif /* SENSORMODULE_GYRO */