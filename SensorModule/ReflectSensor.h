#pragma  once

#include <avr/io.h>

uint8_t ReflectSensor_Init();
uint16_t ReflectSensor_GetValue_Left();
uint16_t ReflectSensor_GetValue_Right();
void  ReflectSensor_Update();