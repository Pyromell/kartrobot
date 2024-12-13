#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

volatile uint8_t table_left_speed = 0;
volatile uint8_t table_right_speed = 0;
volatile uint8_t controlled_left_speed = 0;
volatile uint8_t controlled_right_speed = 0;
volatile uint8_t exit_forward = 0;

#include "init.c"
#include "interrupt.c"
#include "control_sys.c"
#include "drive_functions.c"
#include "uart.c"
/*
If we should have some kind of fully variable speed,
these are the registers used:
OCRnA / OCRnB
*/

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
   
   * 9:S Turn around on the spot (L-track backward, R-track forward)

The speed is not adjustable by com. module at the moment
   * current_speed (l & r):
   * 0 = stop
   * 1 - 4 = increasing speed
   * 5 = max speed
***********************************/
void n()
{
  drive_40_cm('N');
	//calibrate_F();
}
void s()
{
  drive_40_cm('S');
  //calibrate_B();
}
void e()
{
  calibrate_angle_complete();
  drive_turn('E');
}
void w()
{
  calibrate_angle_complete();
  drive_turn('W');
}

int main(void)
{
  PORT_Init();
  UART_Init();
  PWM_Init();
  Interrupt_Init();

	uint8_t current_speed_l = 1, current_speed_r = 1;
    
/*
  drive_40_cm('S');
  n();
  n();
  w();
  n();
  s();
  s();
  s();
  n();
  n();
  e();
  n();
  n();
  e();
  n();
  n();
  s();
  s();
  e();
  n();
  n();
  n();
  n();
*/
  while(1)
  {
	  switch(com_instr) {
		  case 0:
			  stop();
			  com_instr = 'X';
			  UART_Transmit_Instr_Done();
			  break;

		  case 0x01:
			  drive_40_cm('N');

        com_instr = 'X';
			  UART_Transmit_Instr_Done();
			  break;

		  case 0x02:
			  drive_40_cm('S');

        com_instr = 'X';
			  UART_Transmit_Instr_Done();
			  break;

		  case 0x03:
			  calibrate_angle_complete();
			  drive_turn('E');

			  com_instr = 'X';
        UART_Transmit_Instr_Done();
			  break;

		  case 0x04:
			  calibrate_angle_complete();
        drive_turn('W');

			  com_instr = 'X';
			  UART_Transmit_Instr_Done();
			  break;

		  case 0x05:
			  for (int i = 0; i < 1000; ++i)
			  drive('N',current_speed_l,current_speed_r);
			  com_instr = 'X';
			  break;

		  case 0x06:
			  for (int i = 0; i < 1000; ++i)
			  drive('S',current_speed_l,current_speed_r);
        calibrate_B();
			  com_instr = 'X';
			  break;

		  case 0x07:
			  for (int i = 0; i < 1000; ++i)
			  drive('E',current_speed_l,current_speed_r);
	          com_instr = 'X';
			  break;

		  case 0x08:
			  for (int i = 0; i < 100; ++i)
			  drive('W',current_speed_l,current_speed_r);
			  com_instr = 'X';
			  break;

		  case 0x09:
			  calibrate_angle();
        for (volatile int j = 0; j < 30; ++j)
          for (volatile int i = 0; i < 9999; ++i)
            stop();
        calibrate_angle();
        for (volatile int j = 0; j < 30; ++j)
          for (volatile int i = 0; i < 9999; ++i)
            stop();
			  drive_turn('S');

			  com_instr = 'X';
			  UART_Transmit_Instr_Done();
			  break;

		  default:
			  stop();
			  break;
	  }
  }
}
/*
while (1)
    {
      drive_40_cm('N');
      drive_40_cm('S');
      calibrate_angle_complete();
      drive_turn('E');
      calibrate_angle_complete();
      drive_turn('W');
    }
*/