#pragma once
#include <stdbool.h>

// ir_data index
#define Sen_F 0
#define Sen_B 1
#define Sen_LF 2
#define Sen_LB 3
#define Sen_RF 4
#define Sen_RB 5

// walls & wall_relation & wall_angle index
#define Wall_F 0
#define Wall_B 1
#define Wall_L 2
#define Wall_R 3

// wall_relation values
#define close 0
#define good 1
#define far 2
#define invalid 3

// wall_angle values
#define parallel 0
#define into 1
#define away 2
#define invalid 3

uint16_t ir_data[6] = {0,0,0,0,0,0};

bool walls[4] = {0,0,0,0}; // Do we have walls?
uint16_t wall_relation[4] = {invalid, invalid, invalid, invalid}; // How close are we to a wall?
uint16_t wall_angle[4] = {invalid, invalid, invalid, invalid}; // What is our angle to the wall?

char dir = 'X'; // N, S, W, E
uint8_t speed_left = 0, speed_right = 0; // Speed setting: 0 off, 1 lowest, 6 highest

bool ir_data_validation(); // This function is not complete
void evaluate_walls();
void dist_to_wall();
void angle_to_wall();
void calculate_trajectory();

// When new data is received. Functions to be executed:
void main_flow()
{
	evaluate_walls();
	dist_to_wall();
	angle_to_wall();
	calculate_trajectory();
	drive(dir, speed_left, speed_right);
}

// not finished
bool ir_data_validation(uint16_t data)
{
	uint16_t min_val = 0x0000;
	uint16_t max_val = 0xFFFF;
	if(data > min_val && data < max_val)
		return true;
	return false;
}

// Function that evaluates if ir_data have detected a wall
void evaluate_walls()
{
	// Front wall
	if (ir_data_validation(ir_data[Sen_F]))
		walls[Wall_F] = 1;
	else
		walls[Wall_F] = 0;
	
	// Back wall
	if (ir_data_validation(ir_data[Sen_B]))
		walls[Wall_B] = 1;
	else
		walls[Wall_B] = 0;
	
	// Left wall
	if (ir_data_validation(ir_data[Sen_LF]) || ir_data_validation(ir_data[Sen_LB]))
		walls[Wall_L] = 1;
	else
		walls[Wall_L] = 0;
	
	// Right wall
	if (ir_data_validation(ir_data[Sen_RF]) || ir_data_validation(ir_data[Sen_RB]))
		walls[Wall_R] = 1;
	else
		walls[Wall_R] = 0;
}

// Detect if the robot is too far away, good distance, or too close to the wall
// This will be used in conjunction with the angle of the robot.
// Currently it's only: close, good, far. This will prob. be changed to a variable?
void dist_to_wall()
{
	uint16_t distance_400mm = 400;
	uint16_t distance_395mm = 395;
	uint16_t distance_405mm = 405;	

	// Front wall
	if (walls[Wall_F] && ir_data[Sen_F] > distance_400mm )
		// Far away
		wall_relation[Wall_F] = far;
	else if (walls[Wall_F] && ( distance_395mm < ir_data[Sen_F] && ir_data[Sen_F] < distance_405mm ))
		// Good distance
		wall_relation[Wall_F] = good;
	else if (walls[Wall_F] && ir_data[Sen_F] < distance_400mm )
		// Too close
		wall_relation[Wall_F] = close;
	else if (!walls[Wall_F])
		// No wall
		wall_relation[Wall_F] = invalid;
		
	// Back wall
	if (walls[Wall_B] && ir_data[Sen_B] > distance_400mm )
		// Far away
		wall_relation[Wall_B] = far;
	else if (walls[Wall_B] && ( distance_395mm < ir_data[Sen_B] && ir_data[Sen_B] < distance_405mm ))
		// Good distance
		wall_relation[Wall_B] = good;
	else if (walls[Wall_B] && ir_data[Sen_B] < distance_400mm )
		// Too close
		wall_relation[Wall_B] = close;
	else if (!walls[Wall_B])
		// No wall
		wall_relation[Wall_B] = invalid;
	
	// Left wall
	if (walls[Wall_L] && ir_data[Sen_LF] > distance_400mm && ir_data[Sen_LB] > distance_400mm )
		// Far away
		wall_relation[Wall_L] = far;
	else if (walls[Wall_L] &&
		distance_395mm < ir_data[Sen_LF] && distance_395mm < ir_data[Sen_LB] &&
		ir_data[Sen_LF] < distance_405mm && ir_data[Sen_LB] < distance_405mm )
		// Good distance
		wall_relation[Wall_L] = good;
	else if (walls[Wall_L] && ir_data[Sen_LF] < distance_400mm && ir_data[Sen_LB] < distance_400mm )
		// Too close
		wall_relation[Wall_L] = close;
	else if (!walls[Wall_L])
		// No wall
		wall_relation[Wall_L] = invalid;
		
	// Right wall
	if (walls[Wall_R] && ir_data[Sen_RF] > distance_400mm && ir_data[Sen_RB] > distance_400mm )
		// Far away
		wall_relation[Wall_R] = far;
	else if (walls[Wall_R] &&
		distance_395mm < ir_data[Sen_RF] && distance_395mm < ir_data[Sen_RB] &&
		ir_data[Sen_RF] < distance_405mm && ir_data[Sen_RB] < distance_405mm )
		// Good distance
		wall_relation[Wall_R] = good;
	else if (walls[Wall_R] && ir_data[Sen_RF] < distance_400mm && ir_data[Sen_RB] < distance_400mm )
		// Too close
		wall_relation[Wall_R] = close;
	else if (!walls[Wall_R])
		// No wall
		wall_relation[Wall_R] = invalid;
}

// See end of file for ascii drawings for the different scenarios
// Detect the relative angle to left and right walls
void angle_to_wall()
{
	uint8_t distance_2mm = 2;
	// Left wall
	if (walls[Wall_L])
	{
		if ( (ir_data[Sen_LF] <= (ir_data[Sen_LB] + distance_2mm)) && (ir_data[Sen_LF] >= (ir_data[Sen_LB] - distance_2mm)) )
			// Parallel to the wall
			wall_angle[Wall_L] = parallel;
		if (ir_data[Sen_LF] >= ir_data[Sen_LB])
			// angling away from the wall.
			wall_angle[Wall_L] = away;
		if (ir_data[Sen_LF] < ir_data[Sen_LB])
			// angling into the wall.
			wall_angle[Wall_L] = into;
		if (ir_data_validation(ir_data[Sen_LF]) == false)
			// Detected a husknut to the left (we left the current wall)
			wall_angle[Wall_L] = invalid;
		if (ir_data_validation(ir_data[Sen_LB]) == false)
			// Detected a husknut to the left (we found a new wall)
			wall_angle[Wall_L] = invalid;
	}
	
	// Right wall
	if (walls[Wall_R])
	{		
		if ( (ir_data[Sen_RF] <= (ir_data[Sen_RB] + distance_2mm)) && (ir_data[Sen_RF] >= (ir_data[Sen_RB] - distance_2mm)) )
			// Parallel to the wall
			wall_angle[Wall_R] = parallel;
		if (ir_data[Sen_RF] >= ir_data[Sen_RB])
			// angling away from the wall.
			wall_angle[Wall_R] = away;
		if (ir_data[Sen_RF] < ir_data[Sen_RB])
			// angling into the wall.
			wall_angle[Wall_R] = into;
		if (ir_data_validation(ir_data[Sen_RF]) == false)
			// Detected a husknut to the right (we left the current wall)
			wall_angle[Wall_R] = invalid;
		if (ir_data_validation(ir_data[Sen_RB]) == false)
			// Detected a husknut to the right (we found a new wall)
			wall_angle[Wall_R] = invalid;
	}
}

// Calculate where we should go, based on: if there is a wall, our relation and angle to it.
void calculate_trajectory()
{
	if (walls[Wall_L])
	{
		if (wall_relation[Wall_L] == close)
		{
			if (wall_angle[Wall_L] == parallel)
			{
				dir = 'N';
				speed_left = 1;
				speed_right = 0;
			}
			else if (wall_angle[Wall_L] == into)
			{
				dir = 'E';
				speed_left = 1;
				speed_right = 1;
			}
			else if (wall_angle[Wall_L] == away)
			{
				dir = 'N';
				speed_left = 1;
				speed_right = 1;
			}
		}
		else if (wall_relation[Wall_L] == good)
		{
			if (wall_angle[Wall_L] == parallel)
			{
				dir = 'N';
				speed_left = 1;
				speed_right = 1;
			}
			else if (wall_angle[Wall_L] == into)
			{
				dir = 'N';
				speed_left = 1;
				speed_right = 0;
			}
			else if (wall_angle[Wall_L] == away)
			{
				dir = 'N';
				speed_left = 0;
				speed_right = 1;
			}
		}
		else if (wall_relation[Wall_L] == far)
		{
			if (wall_angle[Wall_L] == parallel)
			{
				dir = 'N';
				speed_left = 0;
				speed_right = 1;
			}
			else if (wall_angle[Wall_L] == into)
			{
				dir = 'N';
				speed_left = 1;
				speed_right = 1;
			}
			else if (wall_angle[Wall_L] == away)
			{
				dir = 'W';
				speed_left = 1;
				speed_right = 1;
			}
		}
	}
	
	if (walls[Wall_R])
	{
		if (wall_relation[Wall_R] == close)
		{
			if (wall_angle[Wall_R] == parallel)
			{
				dir = 'N';
				speed_left = 0;
				speed_right = 1;
			}
			else if (wall_angle[Wall_R] == into)
			{
				dir = 'W';
				speed_left = 1;
				speed_right = 1;
			}
			else if (wall_angle[Wall_R] == away)
			{
				dir = 'N';
				speed_left = 1;
				speed_right = 1;
			}
		}
		else if (wall_relation[Wall_R] == good)
		{
			if (wall_angle[Wall_R] == parallel)
			{
				dir = 'N';
				speed_left = 1;
				speed_right = 1;
			}
			else if (wall_angle[Wall_R] == into)
			{
				dir = 'N';
				speed_left = 0;
				speed_right = 1;
			}
			else if (wall_angle[Wall_R] == away)
			{
				dir = 'N';
				speed_left = 1;
				speed_right = 0;
			}
		}
		else if (wall_relation[Wall_L] == far)
		{
			if (wall_angle[Wall_L] == parallel)
			{
				dir = 'N';
				speed_left = 1;
				speed_right = 0;
			}
			else if (wall_angle[Wall_L] == into)
			{
				dir = 'N';
				speed_left = 1;
				speed_right = 1;
			}
			else if (wall_angle[Wall_L] == away)
			{
				dir = 'E';
				speed_left = 1;
				speed_right = 1;
			}
		}
	}
}

// unused
const uint16_t arctan_table[32] = {};
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
#define Scale_Fac 16

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
		if(ir_data[Sen_RF] > ir_data[Sen_RB])
			h = ir_data[Sen_RF] - ir_data[Sen_RB];
		else
			h = ir_data[Sen_RB] - ir_data[Sen_RF];
		
		ang = arctan(l / h);
	}
	if(side == 'L')
	{
		if(ir_data[Sen_LF] > ir_data[Sen_LB])
			h = ir_data[Sen_LF] - ir_data[Sen_LB];
		else
			h = ir_data[Sen_LB] - ir_data[Sen_LF];
		
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