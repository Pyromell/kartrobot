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
uint16_t gyro_data = 0;

uint64_t counter = 0;
/***********************************
Program Description:
	The main file.

Pin Description:
	
	Pin 14 ISR(USART0_RX) (Receive UART data)
	Pin 15 ISR(USART0_RX) (Send UART confirmation)
	
	Pin 18 DIR1 (Direction for left track)
	Pin 19 DIR2 (Direction for right track)
	Pin 20 PWM1 ("Speed" for left track)
	Pin 21 PWM2 ("Speed" for right track)
  
***********************************/

int main(void)
{
  PORT_init();
  PWM_init();
  UART_Init();
  //sei();
  
  while(1)
  {
    PORTB = UART_Receive();
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

/* SEND

drive_instr = UART_Receive();
if(drive_instr == 'N')
{
  PORTB = drive_instr;
  drive_instr = 0x00;
}


    for(volatile int i = 0; i < 6000; ++i)
      UART_Transmit('N');
    for(volatile int i = 0; i < 2000; ++i)
      asm("NOP");

    for(volatile int i = 0; i < 6000; ++i)
      UART_Transmit('X');
    for(volatile int i = 0; i < 2000; ++i)
      asm("NOP");
    
    for(volatile int i = 0; i < 6000; ++i)
      UART_Transmit('S');
    for(volatile int i = 0; i < 2000; ++i)
      asm("NOP");

    for(volatile int i = 0; i < 6000; ++i)
      UART_Transmit('X');
    for(volatile int i = 0; i < 2000; ++i)
      asm("NOP");

    for(volatile int i = 0; i < 6000; ++i)
      UART_Transmit('N');

    for(volatile int i = 0; i < 6000; ++i)
      UART_Transmit('X');

    for(volatile int i = 0; i < 6000; ++i)
     UART_Transmit('S');

    for(volatile int i = 0; i < 6000; ++i)
      UART_Transmit('X');
*/