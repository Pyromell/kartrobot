#pragma once

#include <avr/io.h>
#include <math.h>

#define	Kp 10					// higher Kp gives a faster response but can be inaccurate if to high
#define Kd 5					// higher Kd gives a smother transition but disturbance can impact the system if it's to high
#define max_correction 30		// we use angle if the average side distance is within min_correction < average_side_dist < max_correction
#define min_correction 20		// if we are outside this interval then that means that the robotic need to control based in distance, in order to straighten out to the middle:ish of the square
#define turn_left_value 20		// turn_left/right_value are used in the lookup table for speeds.
#define turn_right_value -20 
#define control_scale_factor 10  // simply used to scale the output for the switch range case


// the lookup table needs to calibrated!!!!!!!!!!!!!!!!!!


	
	void lookup_table(const int output) {
		if (output < 10 && output < -10) {		// lookup table for output that sets different speeds  Look at the Matlab file for clarification regarding the calculations!!!!!
			table_left_speed  = 2;
			table_right_speed = 2;
		}
		else if (output < 40 && output >= 10) {
			table_left_speed  = 3;
			table_right_speed = 2;
		}
		else if (output < 80 && output >= 40) {
			table_left_speed  = 4;
			table_right_speed = 2;
		}
		else if (output < 400 && output >= 80) {
			table_left_speed  = 5;
			table_right_speed = 2;
		}
		else if (output <= -10 && output > -40) {
			table_left_speed  = 2;
			table_right_speed = 3;
		}
		else if (output <= -40 && output > -80) {
			table_left_speed  = 2;
			table_right_speed = 4;
		}
		else if (output <= -80 && output > -400) {
			table_left_speed  = 2;
			table_right_speed = 5;
		}
		else {
			table_left_speed  = 0;   // this is used to see if something broke
			table_right_speed = 0;
		}
		
	}

	int IR_validation() {
		if (IR_DATA[Sen_LF] >= 10 && IR_DATA[Sen_LF] <= 80 && IR_DATA[Sen_LB] >= 10 && IR_DATA[Sen_LB] <= 80) {		// checks that distance is within the interval 10 <= x <= 80
			return 1;
		}
		else if (IR_DATA[Sen_RF] >= 10 && IR_DATA[Sen_RF] <= 80 && IR_DATA[Sen_RB] >= 10 && IR_DATA[Sen_RB] <= 80) {
			return 2;
		}
		else
		return 3;
	}

void control_system(double angle, int wall_1, int wall_2) {
	int output = 0;
	double average_side_dist = (wall_1 + wall_2)/2;
	double error = ((wall_1 - wall_2) /15);
	if (error < 0) {
		error = error * -1;
	}
	// min/max_correction and Kp/Kd is defined at the top of this document
	if (average_side_dist < max_correction && average_side_dist > min_correction) {		// Output is compared to a table to set the speed
		output = control_scale_factor * angle*( Kp + Kd *error);						// Negative output = turn right and the the higher the output, the higher speed should be selected
	}																					// Positive output =	turn left and the the higher the output, the higher speed should be selected
	else if (average_side_dist <= max_correction) {										// if we are very far right in the square then we need to force it to align more to the middle
		output = turn_left_value;
	}
	else if (average_side_dist <= min_correction) {										// if we are very far left in the square then we need to force it to align more to the middle
		output = turn_right_value;
	}
	else {
		output = 0;
	}
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


double trig_angle(int wall_1, int wall_2)  {  // calculates the angle
	if (wall_1 != wall_2) {
		double A = (double)wall_1 - (double)wall_2;
		double B = 15.0;				 // distance between side sensors
		return atan(A/B);
	}
	else {
		return 0;
	}
}
/*					Control_system() updates the global variables table_left/right_speed in main.c 
					and the interrupt that occurs every 10ms sets the drive controlled_left/right_speed = table_left/right_speed.
					This is done to update that data that the drive functions used.
*/					
void control_tech() {												
	double angle = 0;
	int control_method = IR_validation();
	switch(control_method) {
		case 1:  // control with left sensors
			angle = trig_angle(IR_DATA[Sen_LF], IR_DATA[Sen_LB]);			// calculate angle, negative angle means turn right, positive angle means turn left
			control_system(angle, IR_DATA[Sen_LF], IR_DATA[Sen_LB]);
			break;
		case 2:  // control with right sensors
			angle = trig_angle(IR_DATA[Sen_RB], IR_DATA[Sen_RF]);
			control_system(angle, IR_DATA[Sen_RB], IR_DATA[Sen_RB]);		// Inverted order of arguments since each side is inverted logic 
			break;
		default:
			control_system(0, 25, 25);										// No valid data. Keep driving forward
	}
}



