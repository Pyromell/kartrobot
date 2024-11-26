#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.c"

int16_t sensor_gyro_temp = 0;
int16_t sensor_gyro = 0;
uint8_t byte_nr = 0;
unsigned char sensor = 0;
uint8_t IR_DATA[6];

/***********************************
File Description:
	This file includes all of the interrupt vectors that are used.

Pin Description:
	Pin 14 ISR(USART0_RX) (Receive UART data)
	Pin 15 ISR(USART0_RX) (Send UART confirmation)
  
***********************************/

// This functions receives UART instr on UART0
// The ISR sends an 'U' to indicate that the data was received

ISR(USART0_RX_vect) {
	// Store received data
	com_instr = UDR0;
  	
    // Send a receive confirmation ('R')
	//UDR0 = 'R';
}


ISR(USART1_RX_vect) {
  byte_nr++;
  
  switch(sensor) {
    case 'G' :
      if (byte_nr == 1) {
        sensor_gyro_temp = UDR1;
      }
      else if (byte_nr >= 2) {
        sensor_gyro_temp = (sensor_gyro_temp << 8);
        sensor_gyro_temp |= UDR1;
        sensor_gyro = sensor_gyro_temp;
        sensor = 'x';
      }
      break;
	 case 'I' :
		switch(byte_nr) {
			case 1:
				IR_DATA[0] = UDR1;
				break;
			case 2:
				IR_DATA[1] = UDR1;
				break;
			case 3:
				IR_DATA[2] = UDR1;
				break;
			case 4:
				IR_DATA[3] = UDR1;
				break;
			case 5:
				IR_DATA[4] = UDR1;
				break;
			case 6:
				IR_DATA[5] = UDR1;
				sensor = 'x';
				break;
			default:
				sensor = 'x';
				break;
		}
    default:
		sensor_gyro_temp = 0;
		sensor = UDR1;
		byte_nr = 0;
		break;
  }  
 
  /*
  // Send a receive confirmation ('R')
	while ( !(UCSR1A & (1 << UDRE1)) ) ;	
	for (int i = 0; i < 99; ++i)
		asm("NOP");
    
	UDR1 = 'U';
	*/
}



uint8_t timer_10_ms = 0;

ISR(TIMER3_COMPA_vect) {
	
	timer_10_ms++;
}
