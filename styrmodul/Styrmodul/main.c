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
  
  unsinged char current_status = 'X';
  uint8_t current_speed = 1;
  uint8_t manual_mode = 1;
  /* M = manual mode
   * A = auto mode
   */ 
  /* current_status:
   * X = Stop
   * N = Forward
   * S = Backward
   * W = Turn on the spot left (L-track backward, R-track forward)
   * E = Turn on the spot right (L-track forward, R-track backward)
   */
  /* current_speed
   * 0 = stop
   * 1 - 4 = increasing speed
   * 5 = max speed
   * unspecified = 2
   */
  while(1)
  {
	  switch(com_instr)
	  {
		  case 'M':
			  manual_mode = 1;
			  break;
		  case 'A':
			  manual_mode = 0;
			  break;
		  case 'X':
			  current_status = 'X';
			  break;
		  case 'N':
			  current_status = 'N';
			  break;
		  case 'S':
			  current_status = 'S';
			  break;
		  case 'W':
			  current_status = 'W';
			  break;
		  case 'E':
			  current_status = 'E';
			  break;
		  case '0':
			  current_speed = 0;
			  break;
		  case '1':
			  current_speed = 1;
			  break;
		  case '2':
			  current_speed = 2;
			  break;
		  case '3':
			  current_speed = 3;
			  break;
		  case '4':
			  current_speed = 4;
			  break;
		  case '5':
			  current_speed = 5;
			  break;
		  default:
			  current_status = 'X';
			  com_instr = 'X';
			  break;
	  }


	  }
	  if (manual_mode == 1)
	  {
		  drive(current_status, current_speed, current_speed);
	  }
	  else
	  {

	  }
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
