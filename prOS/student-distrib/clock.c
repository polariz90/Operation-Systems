/**
  * clock.c
  * Nov. 11, 2014
  * Project prOS
  *    Clock system that using RTC to keep in track of clock
  */

#include "clock.h"
#include "terminal.h"
#include "types.h"
#include "lib.h"
#include "idt.h"
#include "rtc.h"
#include "file.h"


#define lowerbound 48
#define upperbound 58

volatile uint32_t real_time;
//volatile uint32_t delay_start, delay_end;

uint32_t time_tracker = 0;
char time_buffer[5];

/*initializing time variables */
uint32_t min_h = 0; uint32_t min_l = 0; uint32_t sec_h = 0; uint32_t sec_l = 0; 


/**
  * initialization volatile variables in timer 
  */
void initial_timer(){
	real_time = 0;
//	delay_start = 0; 
//	delay_end = 0; 
	time_tracker = 0;
}


/**
  *update_time
  * INPUT: none
  * OUTPUT: none
  * RETURN: none
  * SIDE EFFECTS: updating the clock in the OS, and keep printing into the system 
  */
void update_time(){
//	char time_buffer[5]; /* time buffer, showing as _ _ : _ _*/
	/* filling the buffer with time */
	time_buffer[0] = in_to_char(min_h);
	time_buffer[1] = in_to_char(min_l);
	time_buffer[2] = 58;
	time_buffer[3] = in_to_char(sec_h);
	time_buffer[4] = in_to_char(sec_l);	



	time_tracker ++;
	if(time_tracker%rtc_freq == 0){ /* updating time in every second*/
	int temp_x, temp_y;

	cli();
	/* changing screen buffers in order to print onto screen every time */
	pcb* curr_pcb = getting_to_know_yourself();
	uint32_t curr_base_add = video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add;
	video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = VID_MEM_IDX;
	flush_tlb();

	temp_x = get_screen_x(); temp_y = get_screen_y();
	set_screen_x(75);
	set_screen_y(24);
	printf("%d%d%c%d%d", min_h,min_l,time_buffer[2],sec_h,sec_l);

	video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = curr_base_add;
	flush_tlb();

	set_screen_x(temp_x); set_screen_y(temp_y);

	//terminal_write(time_buffer, 5);
		time_tracker = 0;
		sec_l ++;
		if(sec_l == 10){
			sec_l = 0;
			sec_h ++;
			if(sec_h == 6){
				sec_h = 0;
				min_l ++;
				if(min_l == 10){
					min_l = 0;
					min_h ++;
					if(min_h == 6){
						min_h = 0; min_l = 0; sec_h = 0; sec_l = 0;
					}
				}
			}
		}

		real_time ++; 
		if (real_time == 0xFFFFFFFF){
			real_time = 0;
		}
	}


}

/**
  * in_to_char
  * INPUT: interger input to translate into char
  * OUTPUT: character corresponsing to the input 
  * RETURN: character being translated from interger to character
  * Side effects: none 
  */
char in_to_char (uint32_t input){
	char ret;
	if(input < lowerbound || input > upperbound){
		return -1; /* invalid input */
	}
	ret = input + 48;
	return ret;
}

/**
  *delay
  * INPUT: interger time for delay
  * OUTPUT: none
  * RETURN: none
  * SIDE EFFECTS: delay program for a certain time 
  */
void delay(const uint32_t delay){
	sti();
	volatile uint32_t delay_end = real_time + delay;
	volatile uint32_t delay_start = real_time;
	while(delay_start < delay_end){
		delay_start = real_time;
		/*expensive while loop*/
	}
	return;
}


/**
  * display clock
  * 	physically display current timer onto screen 
  */
void display_clock(){
	int temp_x, temp_y;
	temp_x = get_screen_x(); temp_y = get_screen_y();
	set_screen_x(75);set_screen_y(24);
	printf("%d%d%c%d%d", min_h,min_l,time_buffer[2],sec_h,sec_l);
	set_screen_x(temp_x); set_screen_y(temp_y);
}

