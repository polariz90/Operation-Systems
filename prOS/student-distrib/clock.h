/**
  * clock.h
  * Nov. 11, 2014
  * Project prOS
  *    Clock system that using RTC to keep in track of clock
  */
#ifndef PROS_CLOCK_H
#define PROS_CLOCK_H


#include "lib.h"


extern volatile uint32_t real_time; 
extern uint32_t time_tracker; 
extern uint32_t min_h;
extern uint32_t min_l;
extern uint32_t sec_h;
extern uint32_t sec_l;

void update_time ();
char in_to_char (uint32_t input);
void delay(const uint32_t delay);


#endif

