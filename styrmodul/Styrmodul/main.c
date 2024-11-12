#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

#include "interrupt.c"
#include "init.c"
#include "drive_functions.c"
#include "uart.c"

// Global variables:
uint8_t wheel_marker_l = 0; // One cog index is one white 'marker' on the wheel
uint8_t wheel_marker_r = 0; 

uint16_t ir_data[6];
uint16_t gyro_data;

/***********************************
Program Description:
	The main file.

Pin Description:
	
	Pin 14 ISR(USART0_RX) (Receive UART data)
	Pin 15 ISR(USART0_RX) (Send UART confirmation)
	
	Pin 18 DIR1 (Direction for left track)
	Pin 19 DIR2 (Direction for right track)
	Pin 20 PWM1 ("Speed" of left track)
	Pin 21 PWM2 ("Speed" of right track)
  
***********************************/

int main(void)
{
  PORT_init();
  PWM_init();
  UART_Init();
  sei();
  
  while(1)
  {
    for (int j = 0; j < 30; ++j)
		for (int i = 0; i < 9999; ++i)
			UART_Transmit('N');
    for (int j = 0; j < 99; ++j)
		for (int i = 0; i < 9999; ++i)
			UART_Transmit('X');

    for (int j = 0; j < 30; ++j)
		for (int i = 0; i < 9999; ++i)
			UART_Transmit('S');

    for (int j = 0; j < 99; ++j)
		for (int i = 0; i < 9999; ++i)
			UART_Transmit('X');
  }
}

/***********************************
	Drive Instruction Manual:
	N: Drive forward
	S: Reverse
	W: Turn left
	E: Turn right
	
	X: Stop
***********************************/


/* rec 2
int main(void)
{
  PORT_init();
  PWM_init();
  UART_Init();
  sei();
  
  while(1)
  {
    drive(drive_instr);
  }
}
*/

/* send 2
int main(void)
{
  PORT_init();
  PWM_init();
  UART_Init();
  sei();
  
  while(1)
  {
    for (int j = 0; j < 30; ++j)
    for (int i = 0; i < 9999; ++i)
    UART_Transmit('N');
    for (int j = 0; j < 99; ++j)
    for (int i = 0; i < 9999; ++i)
    UART_Transmit('X');

    for (int j = 0; j < 30; ++j)
    for (int i = 0; i < 9999; ++i)
    UART_Transmit('S');

    for (int j = 0; j < 99; ++j)
    for (int i = 0; i < 9999; ++i)
    UART_Transmit('X');
  }
}
*/


/* rec
int main(void)
{
  PORT_init();
  PWM_init();
  UART_Init();
  
  sei();
  uint8_t distance = 0;
  
  while(1)
  {
    if(drive_instr != 'X')
    drive_test();
    else
    {
      stop();
      PORTB = drive_instr;
    }
  }
}
*/

/* send
int main(void)
{
  PORT_init();
  PWM_init();
  UART_Init();
  
  sei();
  uint8_t distance = 0;
  
  while(1)
  {
    UART_Transmit('X');
  }
}
*/