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
	PORTD = ((speed_select(speed_left) & 0x80) | ((speed_select(speed_right) >> 1) & 0x40) ) | 0x30;
}

void south(const uint8_t speed_left, const uint8_t speed_right)
{
	PORTD = ((speed_select(speed_left) & 0x80) | ((speed_select(speed_right) >> 1) & 0x40) ) | 0x00;
}

void west(const uint8_t speed_left, const uint8_t speed_right)
{
	PORTD = ((speed_select(speed_left) & 0x80) | ((speed_select(speed_right) >> 1) & 0x40) ) | 0x20;
}

void east(const uint8_t speed_left, const uint8_t speed_right)
{
	PORTD = ((speed_select(speed_left) & 0x80) | ((speed_select(speed_right) >> 1) & 0x40) ) | 0x10;
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
  switch(drive_dir)
  {
    case 'N': north(speed_left, speed_right); break;
    case 'S': south(speed_left, speed_right); break;
    case 'W': west(speed_left, speed_right); break;
    case 'E': east(speed_left, speed_right); break;
    case 'X': stop(); break;
    default: stop(); break;
  }
}


// Drive 40 cm forward
void drive_40_cm(const unsigned char dir)
{	
	ir_recived = 0;
	UART_Transmit_Sen('I');
	while (ir_recived == 0) {}   // wait for new IR DATA
	
	timer_10_ms = 0;
	uint16_t reflex_l_start = reflex_l;
	uint16_t reflex_r_start = reflex_r;
	
	while( (((reflex_l_start + 20) > reflex_l) || ((reflex_r_start + 20) > reflex_r)))
	{
		if (timer_10_ms > 3)
		{
			UART_Transmit_Sen('I');
			timer_10_ms = 0;
		}

		if(11 <= ir_data[Sen_F] && ir_data[Sen_F] <= 16 && dir == 'N') {
    	for (volatile int j = 0; j < 30; ++j)
	      for (volatile int i = 0; i < 9999; ++i)
	        asm("NOP");
      break;
		}

		control_tech(dir);
		if (dir == 'S') {
		  drive(dir, controlled_right_speed, controlled_left_speed);			// reversed for going backwards
		}
		else {
		  drive(dir, controlled_left_speed, controlled_right_speed);
		}
	}
}

// Perform a 90 degrees turn in the specified direction
void drive_turn(const char dir)
{
	int16_t old_gyro = 0;
	int32_t total_angle = 0; 
	timer_10_ms = 0;
	sensor_gyro = 0;
	UART_Transmit_Sen('G');
	while (sensor_gyro == 0) {}	 // we wait for the first gyro data
	old_gyro = sensor_gyro;			 // the first data will be the error of the gyro

	if (dir == 'W')
	{	
		while(total_angle < 7600)
		{
			if (timer_10_ms > 0)
			{
				total_angle += (sensor_gyro - old_gyro) * timer_10_ms;  // this is a scaled but calibrated angle 
				UART_Transmit_Sen('G');
				timer_10_ms = 0;
			}
			drive('W', 2, 2);
		}
	}
	else if(dir == 'E')
	{
		while(total_angle < 7600)
		{
			if (timer_10_ms > 0)
			{
				total_angle -= (sensor_gyro - old_gyro) * timer_10_ms;
				UART_Transmit_Sen('G');
				timer_10_ms = 0;
			}
			drive('E', 2, 2);
		}
	}
	else if(dir == 'S')
		{
			while(total_angle < 15200)
			{
				if (timer_10_ms > 0)
				{
					total_angle += (sensor_gyro - old_gyro) * timer_10_ms;
					UART_Transmit_Sen('G');
					timer_10_ms = 0;
				}
				drive('W', 2, 2);
			}
		}
	for (volatile int j = 0; j < 30; ++j)
	  for (volatile int i = 0; i < 9999; ++i)
	    stop();
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
	drive_turn('W');
	for (volatile int j = 0; j < 30; ++j)
	for (volatile int i = 0; i < 9999; ++i)
	stop();
	drive_turn('E');
	for (volatile int j = 0; j < 30; ++j)
	for (volatile int i = 0; i < 9999; ++i)
	stop();
}