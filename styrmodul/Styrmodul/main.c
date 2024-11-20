#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

// Global variables:
uint8_t wheel_marker_l = 0; // One cog index is one white 'marker' on the wheel
uint8_t wheel_marker_r = 0;

uint16_t ir_data[6];
uint16_t gyro_data = 0;

uint64_t counter = 0;

#include "interrupt.c"
#include "init.c"
#include "drive_functions.c"
#include "uart.c"
#include "control_sys.c"

/***********************************
Program Description:
	The main file.

Pin Description:
	
	Pin 14 ISR(USART0_RX) (Receive UART data)
	Pin 15 ISR(USART0_TX) (Send UART confirmation)
	Pin 16 ISR(USART1_RX) (Receive UART data)
	Pin 17 ISR(USART1_TX) (Send UART confirmation)
	
	Pin 18 DIR1 (Direction for left track)
	Pin 19 DIR2 (Direction for right track)
	Pin 20 PWM1 ("Speed" for left track)
	Pin 21 PWM2 ("Speed" for right track)
  
***********************************/

int main(void)
{
  PORT_init();
  UART_Init();
  PWM_init();
  sei();
  
  while(1)
  {
    some_kind_of_test_drive();
  }
}
/*
void styr()
{
  UART_Transmit_Sen('G'); // Gyro data is requested
  
}

void sen()
{
  while(1) {
    if (data = 'G') {
        transmitt('G');
        transmitt('degrees');
        transmitt('degrees >> 8');
    }
    
    
  }
}

switch()
{
  case 'G':
    transmitt('G');
    transmitt('degrees');
    transmitt('degrees >> 8');
  case 'I':
    transmitt('I');
    transmitt(some_kind_of_ir_data0);
    transmitt(some_kind_of_ir_data1);
    transmitt(some_kind_of_ir_data2);
    transmitt(some_kind_of_ir_data3);
}
*/