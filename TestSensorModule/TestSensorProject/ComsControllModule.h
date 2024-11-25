#pragma once

/***********************************
Pin Description:
	Pin 14 ISR(USART0_RX) (Receive UART) (com. module)
	Pin 15 ISR(USART0_TX) (Send UART)    (com. module)
***********************************/

#define MODULE_INDENTIFIER 1
#define COMMAND_VALUE_IDENTIFY 255
#define COMMAND_VALUE_REQUEST_IR 253


ISR(USART0_RX_vect) 
{

	uint8_t data = UART_Receive_Com();
	if (data == COMMAND_VALUE_IDENTIFY)
	{
		UART_Transmit_Com(MODULE_INDENTIFIER);
	}
	else if (data == COMMAND_VALUE_REQUEST_IR)
	{
		
	}

}