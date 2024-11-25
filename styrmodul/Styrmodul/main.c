#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

#include "interrupt.c"
#include "init.c"
#include "drive_functions.c"
#include "uart.c"
#include "control_sys.c"

/***********************************
Program Description:
	The main file.

Pin Description:
	Pin 14 ISR(USART0_RX) (Receive UART) (com. module)
	Pin 15 ISR(USART0_TX) (Send UART)    (com. module)
	Pin 16 ISR(USART1_RX) (Receive UART) (sen. module)
	Pin 17 ISR(USART1_TX) (Send UART)    (sen. module)
	
	Pin 18 DIR1 (Direction for left track)
	Pin 19 DIR2 (Direction for right track)
	Pin 20 PWM1 ("Speed" for left track)
	Pin 21 PWM2 ("Speed" for right track)
  
***********************************/

/***********************************
Instruction Manual:
   * com_instr:
   * 0:X Stop
   *
   * 1:N Drive forward 40 cm
   * 2:S Drive backward 40 cm
   * 3:E Turn on the spot right (L-track backward, R-track forward)
   * 4:W Turn on the spot left (L-track forward, R-track backward)
   *
   * 5:N once for manual mode
   * 6:S once for manual mode
   * 7:E once for manual mode
   * 8:W once for manual mode

The speed is not adjustable by com. module at the moment
   * current_speed (l & r):
   * 0 = stop
   * 1 - 4 = increasing speed
   * 5 = max speed
***********************************/

int main(void)
{
  PORT_Init();
  UART_Init();
  PWM_Init();
  Interrupt_Init();
  
  uint8_t current_speed_l = 1, current_speed_r = 1;

	while (1)
	{
		UART_Transmit_Sen('G');
		
		for (int j = 0; j < 99; ++j)
		for (int i = 0; i < 9999; ++i)
		asm("NOP");
	}

  while(1)
  {
	switch(com_instr) {
		case 0:
			stop();
			break;
		case 0x01:
			drive_40_cm('N');
			com_instr = 'X';
			break;
		case 0x02:
			drive_40_cm('S');
			com_instr = 'X';
			break;
		case 0x03:
			drive_turn('E');
			com_instr = 'X';
			break;
		case 0x04:
			drive_turn('W');
			com_instr = 'X';
			break;
		case 0x05:
			drive('N',current_speed_l,current_speed_r);
			com_instr = 'X';
			break;
		case 0x06:
			drive('S',current_speed_l,current_speed_r);
			com_instr = 'X';
			break;
		case 0x07:
			drive('E',current_speed_l,current_speed_r);
			com_instr = 'X';
			break;
		case 0x08:
			drive('W',current_speed_l,current_speed_r);
			com_instr = 'X';
			break;
		default:
			stop();
			break;
	}
  }
}