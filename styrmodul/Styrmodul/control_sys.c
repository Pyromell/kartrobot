#pragma once
#include <stdbool.h>
#include <avr/io.h>
#include <math.h>

#include "uart.c"

// ir_data index 0-5
enum ir_data_index {
	Sen_RF,
	Sen_LF,
	Sen_F,
	Sen_RB,
	Sen_LB,
	Sen_B
};

// walls index 0-3
enum wall_index {
	Wall_F,
	Wall_B,
	Wall_L,
	Wall_R
};

#define	Kp 16					// higher Kp gives a faster response but can be inaccurate if to high
#define Kd 5					// higher Kd gives a smother transition but disturbance can impact the system if it's to high
#define angle_scale_factor 10  // simply used to scale the output for the switch range case
#define dist_scale_factor 1
#define dist_reference 17		// how far from the wall we will align

// Global variables
bool walls[4] = {0,0,0,0};
	
void lookup_table(const int input) {
	if (input < 10 && input > -10) { // lookup table for input that sets different speeds  Look at the Matlab file for clarification regarding the calculations!!!!!
		table_left_speed  = 1;
		table_right_speed = 1;
	}
	else if (input < 60 && input >= 10) {
		table_left_speed  = 2;
		table_right_speed = 1;
	}
	else if (input < 110 && input >= 60) {
		table_left_speed  = 3;
		table_right_speed = 1;
	}
	else if (input < 800 && input >= 110) {
		table_left_speed  = 3;
		table_right_speed = 1;
	}
	else if (input <= -10 && input > -60) {
		table_left_speed  = 1;
		table_right_speed = 2;
	}
	else if (input <= -60 && input > -110) {
		table_left_speed  = 1;
		table_right_speed = 3;
	}
	else if (input <= -110 && input > -800) {
		table_left_speed  = 1;
		table_right_speed = 3;
	}
	else {
		table_left_speed  = 0;   // this is used to see if something broke
		table_right_speed = 0;
	}
		
}

void evaluate_walls()
{
  const uint8_t min_val = 11;
  const uint8_t max_val = 40;

  // Front wall
  walls[Wall_F] = (min_val <= ir_data[Sen_F] && ir_data[Sen_F] <= max_val);
  
  // Back wall
  walls[Wall_B] = (min_val <= ir_data[Sen_B] && ir_data[Sen_B] <= max_val);
  
  // Left wall
  walls[Wall_L] = ((min_val <= ir_data[Sen_LF] && ir_data[Sen_LF] <= max_val) &&
  (min_val <= ir_data[Sen_LB] && ir_data[Sen_LB] <= max_val));
  
  // Right wall
  walls[Wall_R] = ((min_val <= ir_data[Sen_RF] && ir_data[Sen_RF] <= max_val) &&
  (min_val <= ir_data[Sen_RB] && ir_data[Sen_RB] <= max_val));
  
  if(walls[Wall_R] && walls[Wall_L])
  {
	  if (ir_data[Sen_RF] + ir_data[Sen_RB] < ir_data[Sen_LF] + ir_data[Sen_LB])
		walls[Wall_L] = 0;
	  else
		walls[Wall_R] = 0;
  }
}

void control_system(double angle, uint8_t wall_1, uint8_t wall_2) {
	volatile int16_t output = 0;
	double error = ((wall_1 - wall_2) /15);
	volatile uint8_t average_dist = (wall_1 + wall_2)/2;
	if (error < 0) {
		error = error * -1;
	}
	volatile int16_t alignment = (dist_reference - average_dist) * dist_scale_factor;
	if (walls[Wall_L] && !walls[Wall_R]) {
		alignment = alignment * -1;
	}
	output = angle_scale_factor * angle*( Kp + Kd *error) + alignment;
	//output = angle_scale_factor * angle * ( Kp + (Kd * error) );
	lookup_table(output);
}

/*
  /----\		|\
|||	   ||| 		| \
 |		|		|  \
 |		|		|   \
 |		|	  B |    \C
 |		|		|	  \
|||	   |||   	|      \
 |______|		|_______\
					A
*/         

double trig_angle(uint8_t wall_1, uint8_t wall_2)  {  // calculates the angle
	if (wall_1 != wall_2) {
		double A = (double)wall_1 - (double)wall_2;
		double B = 15.0;				 // distance between side sensors
		return atan(A/B);
	}
	else {
		return 0;
	}
}

/*
Control_system() updates the global variables table_left/right_speed in main.c 
and the interrupt that occurs every 10ms sets the drive controlled_left/right_speed = table_left/right_speed.
This is done to update that data that the drive functions used.
*/					
void control_tech() {
	encountered_wall = 0;
	evaluate_walls();
	

	// if the wheel is kissing the wall then its a problem
	if(11 <= ir_data[Sen_F] && ir_data[Sen_F] <= 16)
	{
  	table_left_speed  = 0;
  	table_right_speed = 0;
    encountered_wall = 1;
	}
	else if (walls[Wall_L]) // control with left sensors
	{
		double angle = trig_angle(ir_data[Sen_LF], ir_data[Sen_LB]);			// calculate angle, negative angle means turn right, positive angle means turn left
		control_system(angle, ir_data[Sen_LF], ir_data[Sen_LB]);
	}
	else if (walls[Wall_R]) // control with left sensors
	{
		double angle = trig_angle(ir_data[Sen_RB], ir_data[Sen_RF]);
		control_system(angle, ir_data[Sen_RB], ir_data[Sen_RF]);		// Inverted order of arguments since each side is inverted logic
	}
	else // IF WE ARE HERE => NO WALL veri bad
	{
		table_left_speed  = 1;
		table_right_speed = 1;
	}
}



