#pragma once
#include "uart.c"

// Global variables
uint8_t wheel_marker_l = 0; // One cog index is one white 'marker' on the wheel
uint8_t wheel_marker_r = 0;

/*
TCNT0 = speed_div_1
TCNT1 = speed_div_8
TCNT2 = speed_div_64
*/

uint8_t speed_select(const uint8_t speed) {
	switch (speed) {
		case 0x00:
			return 0x00;
		case 0x01:
			return TCNT0 & TCNT1; // 25% on, 75% off
		case 0x02:
			return TCNT0; // 50% on, 50 % off
		case 0x03:
			return TCNT0 | TCNT1; // 75% on, 25 % off
		case 0x04:
			return TCNT0 | TCNT1 | TCNT2; // 87,5% on, 12,5% off
		case 0x05:
			return 0xFF; // FULL POWER!
		default:
			return TCNT0; // 50% on, 50 % off
	}
}

void north(const uint8_t speed_left, const uint8_t speed_right)
{
	PORTD = ( (speed_select(speed_left) & 0x80) | ((speed_select(speed_right) >> 1) & 0x40) ) | 0x30;
}

void south(const uint8_t speed_left, const uint8_t speed_right)
{
	PORTD = ( (speed_select(speed_left) & 0x80) | ((speed_select(speed_right) >> 1) & 0x40) ) | 0x00;
}

void west(const uint8_t speed_left, const uint8_t speed_right)
{
	PORTD = ( (speed_select(speed_left) & 0x80) | ((speed_select(speed_right) >> 1) & 0x40) ) | 0x20;
}

void east(const uint8_t speed_left, const uint8_t speed_right)
{
	PORTD = ( (speed_select(speed_left) & 0x80) | ((speed_select(speed_right) >> 1) & 0x40) ) | 0x10;
}

void stop()
{
	PORTD = 0x00;
}

/***********************************
	Drive Instruction Manual:
	N: Drive forward
	S: Reverse
	W: Turn left
	E: Turn right
	
	X: Stop
***********************************/

void drive(uint8_t drive_dir, uint8_t speed_left, uint8_t speed_right)
{
  //UART_Transmit_Instr_Received();
  switch(drive_dir)
  {
    case 'N': north(speed_left, speed_right); break;
    case 'S': south(speed_left, speed_right); break;
    case 'W': west(speed_left, speed_right); break;
    case 'E': east(speed_left, speed_right); break;
    case 'X': stop(); break;
    default: stop(); break;
  }
  //UART_Transmit_Instr_Done();
}


// Drive 40 cm forward
void drive_40_cm(const unsigned char dir)
{
	uint32_t exit_timer = 0; // Exit timer if something goes wrong
	uint32_t exit_timer_2 = 0;
	timer_10_ms = 0;
	
	//UART_Transmit_S(''); // Starting movement
	while(exit_timer_2 < 2)
	{
    if (exit_timer % 100 == 0)
	  if (timer_10_ms >= 1)
	  {
		  //UART_Transmit_Sen('I');
		  timer_10_ms = 0;
	  }
      
		control_tech();
		//main_flow(); // control sys 2
		drive(dir, controlled_left_speed, controlled_right_speed);
		
		exit_timer++;
		if (exit_timer == 1000)
		{
			exit_timer = 0;
			exit_timer_2++;
		}
	}
	//UART_Transmit_S(''); // Movement done
}

// Perform a 90 degrees turn in the specified direction
void drive_turn(const char dir)
{
	int32_t total_angle = 0;
	//int32_t exit_timer = 0;
	
	timer_10_ms = 0;
	sensor_gyro = 0;
	
	if (dir == 'W')
	{
		//UART_Transmit_Sen(''); // Starting movement
		//UART_Transmit_Com(''); // Starting movement
		
		while(total_angle < 7250)
		{
			if (timer_10_ms > 0)
			{
				total_angle += sensor_gyro * timer_10_ms;
				UART_Transmit_Sen('G');
				timer_10_ms = 0;
			}
			drive('W', 2, 2);
		}
		//UART_Transmit_Com(''); // Movement done
	}
	else if(dir == 'E')
	{
		//UART_Transmit_Sen(''); // Starting movement
		//UART_Transmit_Com(''); // Starting movement
		
		while(total_angle < 7250)
		{
			if (timer_10_ms > 0)
			{
				total_angle += sensor_gyro * timer_10_ms;
				UART_Transmit_Sen('G');
				timer_10_ms = 0;
			}
			drive('E', 2, 2);
		}
		//UART_Transmit_Com(''); // Movement done
	}
	else if(dir == 'S')
		{
			//UART_Transmit_Sen(''); // Starting movement
			//UART_Transmit_Com(''); // Starting movement
		
			while(total_angle < 14500)
			{
				if (timer_10_ms > 0)
				{
					total_angle += sensor_gyro * timer_10_ms;
					UART_Transmit_Sen('G');
					timer_10_ms = 0;
				}
				drive('W', 2, 2);
			}
			//UART_Transmit_Com(''); // Movement done
		}
}

// Drive 40 cm in the direction specified by dir.
// Ex: If drive_40_cm_dir('W') is called
// The robot will turn left (90 degrees) and then drive 40 cm
void drive_40_cm_dir(char dir)
{
	switch (dir)
	{
		case 'N':
			drive_40_cm('N');
			break;
		case 'S': // Turn left twice to achieve a pi turn :)
			drive_turn('L');
			drive_turn('L');
			drive_40_cm('N');
			break;
		case 'E':
			drive_turn('R');
			drive_40_cm('N');
			break;
		case 'W':
			drive_turn('L');
			drive_40_cm('N');
			break;
		default:
			break;
	}
}

///////////////////////////////////
// Test functions:
///////////////////////////////////


void drive_test()
{
	drive_40_cm('N');
	for (volatile int j = 0; j < 30; ++j)
		for (volatile int i = 0; i < 9999; ++i)
			stop();
}

void some_kind_of_test_drive()
{
	drive_turn('L');
	drive_turn('L');
	drive_turn('L');
	drive_turn('L');
	
	drive_40_cm_dir('N');
	cli();
	for (volatile int j = 0; j < 30; ++j)
	for (volatile int i = 0; i < 9999; ++i)
	drive('X', 1, 1);
	sei();
	
	drive_40_cm_dir('W');
	cli();
	for (volatile int j = 0; j < 30; ++j)
	for (volatile int i = 0; i < 9999; ++i)
	drive('X', 1, 1);
	sei();
	
	drive_40_cm_dir('E');
	cli();
	for (volatile int j = 0; j < 40; ++j)
	for (volatile int i = 0; i < 9999; ++i)
	drive('X', 1, 1);
	sei();
	drive_40_cm_dir('E');
	cli();
	for (volatile int j = 0; j < 30; ++j)
	for (volatile int i = 0; i < 9999; ++i)
	drive('X', 1, 1);
	sei();
	
	drive_40_cm_dir('E');
	cli();
	for (volatile int j = 0; j < 30; ++j)
	for (volatile int i = 0; i < 9999; ++i)
	drive('X', 1, 1);
	sei();
	
	drive_40_cm_dir('N');
	cli();
	for (volatile int j = 0; j < 30; ++j)
	for (volatile int i = 0; i < 9999; ++i)
	drive('X', 1, 1);
	sei();
	
	drive_turn('L');
	drive_turn('L');
	cli();
	for (volatile int j = 0; j < 30; ++j)
	for (volatile int i = 0; i < 9999; ++i)
	drive('X', 1, 1);
	sei();
}

/* Save station
// Perform a 90 degrees turn in the specified direction
void drive_turn(char dir)
{
	int32_t total_angle = 0;
	
	if (dir == 'L')
	{
		//UART_Transmit_Sen(''); // Starting movement
		//UART_Transmit_Com(''); // Starting movement
		
		while(total_angle < 90000 && TEMP_COUNTER_2 < 2)
		{
			if (timer_10_ms > 0)
			{
				total_angle += sensor_gyro * timer_10_ms;
				UART_Transmit_Sen('G');
				timer_10_ms = 0;
			}
			drive('W', 1, 1);

			TEMP_COUNTER++;
			if (TEMP_COUNTER == 45000)
			{
				TEMP_COUNTER = 0;
				TEMP_COUNTER_2++;
			}
		}
		TEMP_COUNTER = 0;
		TEMP_COUNTER_2 = 0;
		
		//UART_Transmit_Com(''); // Movement done
	}
	else if(dir == 'R')
	{
		//UART_Transmit_Sen(''); // Starting movement
		//UART_Transmit_Com(''); // Starting movement
		
		while(total_angle < 90000 && TEMP_COUNTER_2 < 2)
		{
			if (timer_10_ms > 0)
			{
				total_angle += sensor_gyro * timer_10_ms;
				UART_Transmit_Sen('G');
				timer_10_ms = 0;
			}
			drive('E', 1, 1);

			TEMP_COUNTER++;
			if (TEMP_COUNTER == 45000)
			{
				TEMP_COUNTER = 0;
				TEMP_COUNTER_2++;
			}
		}
		TEMP_COUNTER = 0;
		TEMP_COUNTER_2 = 0;
		
		//UART_Transmit_Com(''); // Movement done
	}
}

*/
