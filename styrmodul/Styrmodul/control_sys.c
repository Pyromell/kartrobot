#pragma once
#include <stdbool.h>

#include "interrupt.c"

// ir_data index 0-5
enum ir_data_index {
	Sen_RF,
	Sen_LF,
	Sen_F,
	Sen_RB,
	Sen_LB,
	Sen_B
};

// walls & wall_relation & wall_angle index 0-3
enum wall_index {
	Wall_F,
	Wall_B,
	Wall_L,
	Wall_R
};

// wall_relation values 0-3
enum wall_relation_values {
	close,
	good,
	far,
	invalid
};

// wall_angle values 0-3
enum wall_angle_values {
	parallel,
	into,
	away
	//invalid
};

volatile uint8_t ir_data_2[6] = {60,0,0,11,0,0};
bool walls_2[4] = {0,0,0,0}; // Do we have walls?
volatile uint8_t wall_relation[4] = {invalid, invalid, invalid, invalid}; // How close are we to a wall?
volatile uint8_t wall_angle[4] = {invalid, invalid, invalid, invalid}; // What is our angle to the wall?

char dir = 'X'; // N, S, W, E
uint8_t speed_left = 0, speed_right = 0; // Speed setting: 0 off, 1 lowest, 6 highest

void evaluate_walls_2();
void dist_to_wall();
void angle_to_wall();
void calculate_trajectory();

// When new data is received. Functions to be executed:
void main_flow()
{
	evaluate_walls_2();
	dist_to_wall();
	angle_to_wall();
	calculate_trajectory();
}


// Function that evaluates if ir_data have detected a wall
void evaluate_walls_2()
{
	const uint8_t min_val = 11;
	const uint8_t max_val = 60;

	// Front wall
	walls_2[Wall_F] = (min_val <= ir_data_2[Sen_F] && ir_data_2[Sen_F] <= max_val);
	
	// Back wall
	walls_2[Wall_B] = (min_val <= ir_data_2[Sen_B] && ir_data_2[Sen_B] <= max_val);
	
	// Left wall
	walls_2[Wall_L] = ((min_val <= ir_data_2[Sen_LF] && ir_data_2[Sen_LF] <= max_val) &&
	(min_val <= ir_data_2[Sen_LB] && ir_data_2[Sen_LB] <= max_val));
	
	// Right wall
	walls_2[Wall_R] = ((min_val <= ir_data_2[Sen_RF] && ir_data_2[Sen_RF] <= max_val) &&
	(min_val <= ir_data_2[Sen_RB] && ir_data_2[Sen_RB] <= max_val));
	
	walls_2[Wall_B] = 0;
	walls_2[Wall_F] = 0;
}

// Detect if the robot is too far away, good distance, or too close to the wall
// This will be used in conjunction with the angle of the robot.
// Currently it's only: close, good, far. This will prob. be changed to a variable?
void dist_to_wall()
{
	const uint8_t distance_close = 19;
	const uint8_t distance_far = 21;

	// Front wall
	wall_relation[Wall_F] = invalid;
	if (ir_data_2[Sen_F] > distance_far) {
		// Far away
		wall_relation[Wall_F] = far;
	} else if (ir_data_2[Sen_F] < distance_close ) {
		// Too close
		wall_relation[Wall_F] = close;
	} else {
		// Good distance
		wall_relation[Wall_F] = good;
	}
		
	// Back wall
	wall_relation[Wall_B] = invalid;
	if (ir_data_2[Sen_B] > distance_far) {
		// Far away
		wall_relation[Wall_B] = far;
	} else if (ir_data_2[Sen_B] < distance_close ) {
		// Too close
		wall_relation[Wall_B] = close;
	} else {
		// Good distance
		wall_relation[Wall_B] = good;
	}
	
	// Left wall
	wall_relation[Wall_L] = invalid;
	if (ir_data_2[Sen_LF] > distance_far && ir_data_2[Sen_LB] > distance_far) {
		// Far away
		wall_relation[Wall_L] = far;
	} else if (ir_data_2[Sen_LF] < distance_close && ir_data_2[Sen_LB] < distance_close) {
		// Too close
		wall_relation[Wall_L] = close;
	} else {
		// Good distance
		wall_relation[Wall_L] = good;
	}
		
	// Right wall
	wall_relation[Wall_R] = invalid;
	if (ir_data_2[Sen_RF] > distance_far && ir_data_2[Sen_RB] > distance_far) {
		// Far away
		wall_relation[Wall_R] = far;
	} else if (ir_data_2[Sen_RF] < distance_close && ir_data_2[Sen_RB] < distance_close) {
		// Too close
		wall_relation[Wall_R] = close;
	} else {
		// Good distance
		wall_relation[Wall_R] = good;
	}
}

// See end of file for ascii drawings for the different scenarios
// Detect the relative angle to left and right walls
void angle_to_wall()
{
	uint8_t valid_error = 2;
	// Left wall
	if (walls_2[Wall_L])
	{
		if ( (ir_data_2[Sen_LF] <= (ir_data_2[Sen_LB] + valid_error)) && (ir_data_2[Sen_LF] >= (ir_data_2[Sen_LB] - valid_error)) )
			// Parallel to the wall
			wall_angle[Wall_L] = parallel;
		else if (ir_data_2[Sen_LF] >= ir_data_2[Sen_LB])
			// angling away from the wall.
			wall_angle[Wall_L] = away;
		else if (ir_data_2[Sen_LF] < ir_data_2[Sen_LB])
			// angling into the wall.
			wall_angle[Wall_L] = into;
	}
	
	// Right wall
	if (walls_2[Wall_R])
	{		
		if ( (ir_data_2[Sen_RF] <= (ir_data_2[Sen_RB] + valid_error)) && (ir_data_2[Sen_RF] >= (ir_data_2[Sen_RB] - valid_error)) )
			// Parallel to the wall
			wall_angle[Wall_R] = parallel;
		else if (ir_data_2[Sen_RF] >= ir_data_2[Sen_RB])
			// angling away from the wall.
			wall_angle[Wall_R] = away;
		else if (ir_data_2[Sen_RF] < ir_data_2[Sen_RB])
			// angling into the wall.
			wall_angle[Wall_R] = into;
	}
}

void set_trajectory(const char new_dir, const uint8_t new_left_speed, const uint8_t new_right_speed) {
	dir = new_dir;
    table_left_speed = new_left_speed;
    table_right_speed = new_right_speed;
}

// Calculate where we should go, based on: if there is a wall, our relation and angle to it.
void calculate_trajectory()
{
	if (walls_2[Wall_L])
	{
		if (wall_relation[Wall_L] == close)
		{
			if (wall_angle[Wall_L] == parallel)
				set_trajectory('N', 1, 0);
			else if (wall_angle[Wall_L] == into)
				set_trajectory('E', 1, 1);
			else if (wall_angle[Wall_L] == away)
				set_trajectory('N', 1, 1);
		}
		else if (wall_relation[Wall_L] == good)
		{
			if (wall_angle[Wall_L] == parallel)
				set_trajectory('N', 1, 1);
			else if (wall_angle[Wall_L] == into)
				set_trajectory('N', 1, 0);
			else if (wall_angle[Wall_L] == away)
				set_trajectory('N', 0, 1);
		}
		else if (wall_relation[Wall_L] == far)
		{
			if (wall_angle[Wall_L] == parallel)
				set_trajectory('N', 0, 1);
			else if (wall_angle[Wall_L] == into)
				set_trajectory('N', 1, 1);
			else if (wall_angle[Wall_L] == away)
				set_trajectory('W', 1, 1);
		}
	}
	
	if (walls_2[Wall_R])
	{
		if (wall_relation[Wall_R] == close)
		{
			if (wall_angle[Wall_R] == parallel)
				set_trajectory('N', 0, 1);
			else if (wall_angle[Wall_R] == into)
				set_trajectory('W', 1, 1);
			else if (wall_angle[Wall_R] == away)
				set_trajectory('N', 1, 1);
		}
		else if (wall_relation[Wall_R] == good)
		{
			if (wall_angle[Wall_R] == parallel)
				set_trajectory('N', 1, 1);
			else if (wall_angle[Wall_R] == into)
				set_trajectory('N', 0, 1);
			else if (wall_angle[Wall_R] == away) 
				set_trajectory('N', 1, 0);
		}
		else if (wall_relation[Wall_R] == far)
		{
			if (wall_angle[Wall_R] == parallel)
				set_trajectory('N', 1, 0);
			else if (wall_angle[Wall_R] == into)
				set_trajectory('N', 1, 1);
			else if (wall_angle[Wall_R] == away)
				set_trajectory('E', 1, 1);
		}
	}
}


// unused
//const uint16_t arctan_table[32] = {};
/*
(mm) (rad)
arctan(l / h) = ang
0:  l = 155, h = 10,  arctan(155 / 10)  = 1.50636949 rad
1:  l = 155, h = 20,  arctan(155 / 20)  = 1.44247310 rad
2:  l = 155, h = 30,  arctan(155 / 30)  = 1.37961187 rad
3:  l = 155, h = 40,  arctan(155 / 40)  = 1.31824205 rad
4:  l = 155, h = 50,  arctan(155 / 50)  = 1.25875421 rad
5:  l = 155, h = 60,  arctan(155 / 60)  = 1.20146267 rad
6:  l = 155, h = 70,  arctan(155 / 70)  = 1.14660192 rad
7:  l = 155, h = 80,  arctan(155 / 80)  = 1.09432891 rad
8:  l = 155, h = 90,  arctan(155 / 90)  = 1.04472991 rad
9:  l = 155, h = 100, arctan(155 / 100) = 0.99783018 rad
10: l = 155, h = 110, arctan(155 / 110) = 0.95360494 rad
11: l = 155, h = 120, arctan(155 / 120) = 0.91199029 rad
12: l = 155, h = 130, arctan(155 / 130) = 0.87289350 rad
13: l = 155, h = 140, arctan(155 / 140) = 0.83620187 rad
14: l = 155, h = 150, arctan(155 / 150) = 0.80179014 rad
15: l = 155, h = 160, arctan(155 / 160) = 0.76952648 rad
16: l = 155, h = 170, arctan(155 / 170) = 0.73927705 rad
17: l = 155, h = 180, arctan(155 / 180) = 0.71090937 rad
18: l = 155, h = 190, arctan(155 / 190) = 0.68429479 rad
19: l = 155, h = 200, arctan(155 / 200) = 0.65931007 rad
20: l = 155, h = 210, arctan(155 / 210) = 0.63583842 rad
21: l = 155, h = 220, arctan(155 / 220) = 0.61377010 rad
22: l = 155, h = 230, arctan(155 / 230) = 0.59300256 rad
23: l = 155, h = 240, arctan(155 / 240) = 0.57344053 rad
24: l = 155, h = 250, arctan(155 / 250) = 0.55499573 rad
25: l = 155, h = 260, arctan(155 / 260) = 0.53758665 rad
26: l = 155, h = 270, arctan(155 / 270) = 0.52113814 rad
27: l = 155, h = 280, arctan(155 / 280) = 0.50558105 rad
28: l = 155, h = 290, arctan(155 / 290) = 0.49085178 rad
29: l = 155, h = 300, arctan(155 / 300) = 0.47689187 rad
30: l = 155, h = 310, arctan(155 / 310) = 0.46364761 rad
31: l = 155, h = 320, arctan(155 / 320) = 0.45106966 rad
*/
//#define Scale_Fac 16

// unused
/*
uint16_t arctan(uint16_t x)
{
	if (x < 0 || x > 16)
		return 0; // Invalid range
	return arctan_table[x];
}
*/


// unused
/*
void calculate_angle(const char side)
{
	uint16_t h = 0; // Height (mm)
	const uint16_t l = 200; // Length (mm)
	uint16_t ang = 0; // Angle (rad)
	
	if(side == 'R')	
	{
		if(ir_data_2[Sen_RF] > ir_data_2[Sen_RB])
			h = ir_data_2[Sen_RF] - ir_data_2[Sen_RB];
		else
			h = ir_data_2[Sen_RB] - ir_data_2[Sen_RF];
		
		ang = arctan(l / h);
	}
	if(side == 'L')
	{
		if(ir_data_2[Sen_LF] > ir_data_2[Sen_LB])
			h = ir_data_2[Sen_LF] - ir_data_2[Sen_LB];
		else
			h = ir_data_2[Sen_LB] - ir_data_2[Sen_LF];
		
		ang = arctan(l / h);
	}	
}
*/

/*
// angling away from the wall.
                                      ││            
                                     ││             
┌──────────────────┐                ││              
│                  │                │               
│         Sensor ──┼──`````````````│                
│                  │       :      ││                
│                  │       :      │                 
│                  │       :     │                  
│                  │       :    ││                  
│                  │       :    │                   
│                  │       :   │                    
│                  │       :  │                     
│                  │       : ││                     
│         Sensor ──┼──``````││                      
│                  │        │                       
└──────────────────┘       ││                       
                          ││                        
                          │                         
                         ││                         
                         │                          


// angling into the wall. 
                               │                    
                               ││                   
┌──────────────────┐            │                   
│                  │            ││                  
│                ──┼──```````````│                  
│                  │            :│                  
│                  │            : │                 
│                  │            : │                 
│                  │            : │                 
│                  │            :  │                
│                  │            :  │                
│                  │            :  │                
│                  │            :   │               
│                ──┼──``````````````││              
│                  │                 │              
└──────────────────┘                 ││             
                                      │             
                                      ││            


// Detected a husknut (we left the current wall)
┌──────────────────┐                                
│                  │                                
│                ──┼──``````````````````````        
│                  │                                
│                  │                                
│                  │                                
│                  │                                
│                  │                                
│                  │                                
│                  │            ┌───────────────    
│                  │            │                   
│                ──┼──``````````│                   
│                  │            │                   
└──────────────────┘            │                   
                                │                   
                                │                   


// Detected a husknut (we found a new wall)
                                │                   
                                │                   
                                │                   
┌──────────────────┐            │                   
│                  │            │                   
│                ──┼──``````````│                   
│                  │            │                   
│                  │            │                   
│                  │            └───────────────────
│                  │                                
│                  │                                
│                  │                                
│                  │                                
│                  │                                
│                ──┼──```````````````````````       
│                  │                                
└──────────────────┘                                
*/