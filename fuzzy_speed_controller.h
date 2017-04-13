//
//  fuzzy_speed_controller.h
//  homeWork
//
//  Created by Mathias Dalshagen on 2017-04-11.
//  Copyright © 2017 Mathias Dalshagen. All rights reserved.
//

#ifndef fuzzy_speed_controller_h
#define fuzzy_speed_controller_h

#include <stdio.h>


extern struct io_type distance;    // input 1
extern struct io_type speed;       // input 2
extern struct io_type pwm;         // output
extern struct rule_type rule1;

void FLC_road(void);
void get_measurements(int v, int d);
void doFuzzySpeed();

#endif /* fuzzy_speed_controller_h */
