#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.c"

// Global variables
uint8_t byte_nr = 0; // the index of the received byte in a sequence of bytes
uint8_t com_instr = 0x00; // received instr. from the Com. module
unsigned char sensor = 0;

volatile int16_t sensor_gyro_temp = 0;
volatile int16_t sensor_gyro = 0;
volatile uint8_t ir_data[6] = {0,0,0,0,0,0};
uint16_t reflex_l = 0;
uint16_t reflex_r = 0;
volatile uint8_t ir_recived = 0;


/***********************************
File Description:
	This file includes all of the interrupt vectors that are used.

Pin Description:
	Pin 14 ISR(USART0_RX) (Receive UART) (com. module)
	Pin 15 ISR(USART0_TX) (Send UART)    (com. module)
	Pin 16 ISR(USART1_RX) (Receive UART) (sen. module)
	Pin 17 ISR(USART1_TX) (Send UART)    (sen. module)
***********************************/

ISR(USART0_RX_vect) {
	com_instr = UDR0;
	UART_Transmit_Instr_Received();
	if (com_instr == 0) {
		force_stop = 1;
	}
}

void fetch_gyro(const uint8_t index) {
	if (index == 1) {
		sensor_gyro_temp = UDR1;
	}
	else if (index >= 2) {
		sensor_gyro_temp = (sensor_gyro_temp << 8);
		sensor_gyro_temp |= UDR1;
		sensor_gyro = sensor_gyro_temp;
		if (sensor_gyro == 0) {
		sensor_gyro = 1;
		} 
		sensor = 'x';       // sensor is 'G' or 'I'
	}
}

void fetch_IR(const uint8_t index) {
	ir_data[index -1] = UDR1;

	if (index > 5) {
		sensor = 'x';
		UART_Transmit_Sen('R');
	}
}

void fetch_reflex(const uint8_t index) {
  switch (index)
  {
    case 1:
      reflex_l = UDR1;
      break;
    case 2:
      reflex_l = (reflex_l << 8);
      reflex_l |= UDR1;
      break;
    case 3:
      reflex_r = UDR1;
      break;
    case 4:
      reflex_r = (reflex_r << 8);
      reflex_r |= UDR1;
      sensor = 'x';
	    ir_recived = 1;
      break;
    default:
      sensor = 'x';
      break;
  }
}

ISR(USART1_RX_vect) {
  switch(sensor) {
    case 'G' :
		fetch_gyro(byte_nr);
		byte_nr++;
		break;
	case 'I' :
		fetch_IR(byte_nr);
		byte_nr++;
		break;
  case 'R' :
		fetch_reflex(byte_nr);
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
