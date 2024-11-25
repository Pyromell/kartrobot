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

ISR(USART1_RX_vect) 
{

    unsigned char inData = UDR1;
    UART_Transmit_SM(inData);
    if (inData == 'G')
    {
        uint16_t gyroData = Gyro_ReadValue();
        UART_Transmit_SM(gyroData >> 8);
        UART_Transmit_SM(gyroData);
    }
    else if (inData == 'I')
    {
        uint8_t IRdata = IR_ReadValue();
        UART_Transmit_SM(IRdata);
    }
}