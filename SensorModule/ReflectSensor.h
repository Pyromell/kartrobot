#pragma  once

#include <avr/io.h>

uint8_t ReflectSensor_Init();
uint16_t ReflectSensor_GetValue_Left();
uint16_t ReflectSensor_GetValue_Right();
void  ReflectSensor_Update();
//	new stuff
void ReflectSensor_StoreValue();
uint16_t ReflectSensor_GetValue_Left_Rel();
uint16_t ReflectSensor_GetValue_Right_Rel();