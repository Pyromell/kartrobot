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
#include "SensorModule_Gyro.h"

ISR(USART1_RX_vect) 
{

    unsigned char data = UDR1;
    UART_Transmit_Sen(data);
    if (data == 'G')
    {
        uint16_t gyroData = Gyro_ReadValue();
        UART_Transmit_Sen(gyroData >> 8);
        UART_Transmit_Sen(gyroData);
    }
}