#pragma once

#include <avr/io.h>
#include <math.h>

#include "interrcupt.c"
#include "control_sys.c"




void control_side() { 
	double angle = 0;
	int control_method = 0;
	switch(control_method) {
		case 1:  // left
			
			angle = trig_angle(IR_DATA[Sen_LF], IR_DATA[Sen_LB]);		// calculate angle, negative angle means turn right, positive angle means turn left
			control_system(angle, IR_DATA[Sen_LF], IR_DATA[Sen_LB]);
			break;
		case 2:  // right
			angle = trig_angle(IR_DATA[Sen_RB], IR_DATA[Sen_RF]);
			control_system(angle, IR_DATA[Sen_RB], IR_DATA[Sen_RB]);     // Inverted order of arguments since each side is inverted logic 
			break;
		default:														 // No valid data. Keep driving forward
	}
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
double trig_angle(int wall_1, int wall_2)  {  // 
	double A = wall_1 - wall_2;
	double B = 15.0;   // distance between side sensors
	double angle =  atan(A/B);
	
}

void control_system(double angle, int wall_1, int wall_2) {
	double Kp = 0;	// higher Kp gives a faster response but can be inaccurate if to high
	double Kd = 0;	// higher Kd gives a smother transition but disturbance can impact the system if it's to high
	double output = 0;
	double error = ((wall_1 - wall_2) /15)
	if (error < 0) {
		error = error * -1;
	}
	// Output is compared to a table to set the speed
	// Negative output = turn right and the the higher the output, the higher speed should be selected
	// Positive output =	turn left and the the higher the output, the higher speed should be selected
	output = angle*( Kp + Kd *error);
	
}

int IR_validation(int control_method) {
	if (IR_DATA[Sen_LF] >= 10 && IR_DATA[Sen_LF] <= 80 && IR_DATA[Sen_LB] >= 10 && IR_DATA[Sen_LB] <= 80) {		// checks that distance is within the interval 10 <= x <= 80
		control_method = 1;
	}
	else if (IR_DATA[Sen_RF] >= 10 && IR_DATA[Sen_RF] <= 80 && IR_DATA[Sen_RB] >= 10 && IR_DATA[Sen_RB] <= 80) {
		control_method = 2;
	}
	else 
		control_method = 3;
}