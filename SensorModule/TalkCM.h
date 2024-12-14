#pragma once

#include "Gyro.h"
#include "IR.h"
#include "ReflectSensor.h"
#include <avr/io.h>


/***********************************
Pin Description:
	Pin 14 ISR(USART0_RX) (Receive UART) (com. module)
	Pin 15 ISR(USART0_TX) (Send UART)    (com. module)
***********************************/

#define MODULE_INDENTIFIER 1
#define COMMAND_VALUE_IDENTIFY 255
#define COMMAND_VALUE_REQUEST_Gyro 254
#define COMMAND_VALUE_REQUEST_IR 253
#define COMMAND_VALUE_REQUEST_Reflect 252
#define COMMAND_VALUE_REQUEST_Gyro_Calibrate 251
#define COMMAND_VALUE_REQUEST_Gyro_Calibrate_Status 250

ISR(USART0_RX_vect) 
{
	volatile uint8_t commandData = UDR0;
	if (commandData == COMMAND_VALUE_IDENTIFY)
	{
		UART_Transmit_CM(MODULE_INDENTIFIER);
	}
	else if (commandData == COMMAND_VALUE_REQUEST_Gyro)
	{
        volatile uint16_t gyroData = Gyro_ReadValue();
        UART_Transmit_CM(gyroData >> 8);
        UART_Transmit_CM(gyroData);	
		
	}	
	else if (commandData == COMMAND_VALUE_REQUEST_IR)
	{


		//IR_ReadValue() är den normala live IR datan
		//IR_ReadValueMedian() medianvärdet på 20 senaste 
		for (uint8_t i = 0; i < 6; i++)
		{
			volatile uint8_t IRdata = IR_ReadValueMedian(i);
			UART_Transmit_CM(IRdata);
		}		
			
	}
	else if (commandData == COMMAND_VALUE_REQUEST_Reflect)
	{
		volatile uint16_t leftValue = ReflectSensor_GetValue_Left();
		volatile uint16_t rightValue = ReflectSensor_GetValue_Right();
		UART_Transmit_CM(leftValue >> 8);
		UART_Transmit_CM(leftValue);
		UART_Transmit_CM(rightValue >> 8);
		UART_Transmit_CM(rightValue);			
	}
}