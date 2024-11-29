#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>

#include "control_sys.c"
#include "uart.c"
int16_t sensor_gyro_temp = 0;
int16_t sensor_gyro = 0;
uint8_t byte_nr = 0;
unsigned char sensor = 0;
volatile uint8_t IR_DATA[6];

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

void fetch_gyro(const uint8_t index) {
	if (index == 1) {
		sensor_gyro_temp = UDR1;
	}
	else if (index >= 2) {
		sensor_gyro_temp = (sensor_gyro_temp << 8);
		sensor_gyro_temp |= UDR1;
		sensor_gyro = sensor_gyro_temp;
		sensor = 'x';       // sensor is 'G' or 'I'
	}
}

void fetch_IR_data(uint8_t index) {
	IR_DATA[index -1] = UDR1;
	if (index == 6) {
		sensor = 'x';
	}
}

	

ISR(USART1_RX_vect) {
  switch(sensor) {
    case 'G' :
		fetch_gyro(byte_nr);
		byte_nr++;
		break;
	case 'I' :
		fetch_IR_data(byte_nr);
		byte_nr++;
		break;
    default:
		sensor_gyro_temp = 0;
		sensor = UDR1;
		byte_nr = 1;
		break;
  }  
}



uint8_t timer_10_ms = 0;

ISR(TIMER3_COMPA_vect) {
	controlled_left_speed = table_left_speed;		// updates the speed from control_tech to the speed that the drive functions use
	controlled_right_speed = table_right_speed;
	
	timer_10_ms++;
}
