#pragma once

/***********************************
Pin Description:
	Pin 16 ISR(USART1_RX) (Receive UART) (sen. module)
	Pin 17 ISR(USART1_TX) (Send UART)    (sen. module)
***********************************/

//	Styrmodule comms
//		1 byte -	Request data id
//			-	G	:	Get gyro data
//				3 bytes
//					1	:	'G'
//					2	:	high byte of Gyro data
//					3	:	low byte of gyro data

#include "uart.h"
#include "Gyro.h"
#include "IR.h"
#include "ReflectSensor.h"

ISR(USART1_RX_vect) 
{
    volatile unsigned char inData = UDR1;
    UART_Transmit_SM(inData);
    if (inData == 'G')
    {
        volatile uint16_t gyroData = Gyro_ReadValue();
        UART_Transmit_SM(gyroData >> 8);
        UART_Transmit_SM(gyroData);
    }
    else if (inData == 'I')
    {
		for (uint8_t i = 0; i < 6; i++)
		{
			volatile uint8_t IRdata = IR_ReadValue(i);
			UART_Transmit_SM(IRdata);
		}
    }
	else if (inData == 'R')
	{
		volatile uint16_t leftValue = ReflectSensor_GetValue_Left();
		volatile uint16_t rightValue = ReflectSensor_GetValue_Right();
		UART_Transmit_SM(leftValue >> 8);
		UART_Transmit_SM(leftValue);
		UART_Transmit_SM(rightValue >> 8);
		UART_Transmit_SM(rightValue);
	}
	else if (inData == 'T')
	{
		ReflectSensor_StoreValue();
	}
	else if (inData == 'U')
	{
		volatile uint16_t leftValue = ReflectSensor_GetValue_Left_Rel();
		volatile uint16_t rightValue = ReflectSensor_GetValue_Right_Rel();
		UART_Transmit_SM(leftValue >> 8);
		UART_Transmit_SM(leftValue);
		UART_Transmit_SM(rightValue >> 8);
		UART_Transmit_SM(rightValue);
	}
	else if (inData == 'V')
	{
		volatile uint16_t leftValue = ReflectSensor_GetValue_Left_Rel();
		volatile uint16_t rightValue = ReflectSensor_GetValue_Right_Rel();
		UART_Transmit_SM(leftValue >> 8);
		UART_Transmit_SM(leftValue);
		UART_Transmit_SM(rightValue >> 8);
		UART_Transmit_SM(rightValue);
		ReflectSensor_StoreValue();
	}

		
}