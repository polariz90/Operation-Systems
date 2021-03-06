/* This file will contain the implementation of
 * initialization of keyboard usage.
 *
 * The general procedure of keyboard initialization is to call PIC function 
 * which enables and disabes certain irq. In this case, irq1.
 */

#include "mouse.h"
#include "i8259.h"
#include "lib.h"
#include "file.h"
#include "terminal.h"
#include "idt.h"
#include "sys_call.h"

uint8_t  mouse_cycle;   //cycle through 0 to 2, 3 signals
int8_t mouse_byte[3];   //3 sized buffer
int8_t mouse_x;         // x movement signal of mouse
int8_t mouse_y;         // y movement signal of mouse
int8_t mouse_data;		// data signal of mouse
int8_t prevdata;		//flag used for check duplicate signal

double pos_x;	//current position of cursor for x coordinate
double pos_y;	//current position of cursor for x coordinate

unsigned char prevpos_x;	//previous position of cursor for x coordinate
unsigned char prevpos_y;	//previous position of cursor for y coordinate

int8_t x_right;
int8_t y_up;
int8_t x_left;
int8_t y_down;

uint8_t curr_char, curr_attrib;
uint8_t mouse_buf[four_kb];
uint8_t bufcpy[four_kb];
uint8_t bufrclk[four_kb];
uint8_t Bazinga[128];

uint8_t mouse_click_flag;

int block_flag;	//flag used for blocking
int temp;
int t_toggle;
int bufpos;		//current buffer position 
int prevbufpos;	//previous buffer position for block
int prevbufpos_c; //previous buffer position for click
int counter;	//counter for copying characters to the right
int counter2;	//counter for copying characters to the left

int i; //Loop constant

extern void mouse_()
{
  switch(mouse_cycle)
  {
    case 0:
      mouse_byte[0]=inb(0x60);
      mouse_y = mouse_byte[0];
      mouse_cycle++;
      break;
    case 1:
      mouse_byte[1]=inb(0x60);
      mouse_data = mouse_byte[1];
      mouse_cycle++;
      break;
    case 2:
      mouse_byte[2]=inb(0x60);
      mouse_x = mouse_byte[2];
      mouse_cycle=0;
      
      set_pointer();
      if((mouse_byte[1] & 1) == 1){/* case left mouse buttom pressed*/
      //	printf("mouse clicked \n");
      	mouse_l_click();
      }
      if((mouse_byte[1] & 2) == 1){/* case right mouse buttom pressed*/
      	printf("You activated the ultimate weapon! Self Destruction in 5.\n");
      	mouse_r_click();
      }
      break;
  }

}

/*
 * Enables the irq1 for keyboard
 */
extern void mouse_enable()
{
	enable_irq(M_IRQ);
}

/*
 * Disables the irq1 for keyboard
 */
extern void mouse_disable()
{
	disable_irq(M_IRQ);
}


extern void mouse_write(unsigned char port) 
{
	//Wait to be able to send a command
	mouse_wait(1);
	//Tell the mouse we are sending a command
	outb(0xD4, 0x64);
	//Wait for the final part
	mouse_wait(1);
	//Finally write
	outb(port, 0x60);
}

extern unsigned char mouse_read() 
{
	//Get's response from mouse
	mouse_wait(0);

	return inb(0x60);
}

extern unsigned char mouse_res(unsigned char signal)
{
	mouse_wait(0);
	return inb(signal);
}

extern void mouse_wait(unsigned char signal)
{
	int _time_out = 100000; //unsigned int
	if (signal == 0) {
		while (_time_out--) //Data
		{
			if ((inb(KEYBOARD_STATUS_PORT) & 1) == 1) {
				return;
			}
		}
		return;
	} else {
		while (_time_out--) //Signal
		{
			if ((inb(KEYBOARD_STATUS_PORT) & 2) == 0) {
				return;
			}
		}
		return;
	}
}


extern void mouse_install() {
	unsigned char _status;  //unsigned char

	x_right = 0;
    y_up = 0;
    x_left = 0;
    y_down = 0;

    mouse_cycle = 0;
    mouse_x = 0;
    mouse_y = 0;

    mouse_data = 0;

    prevpos_x=40;
	prevpos_y=12;

	pos_x=40;
	pos_y=12;

	mouse_click_flag = 0;

	counter2 = 0;
	counter = 0;
	temp = 0;
	//t_toggle = 0;
	block_flag = 0; //for mouse blocking words

	//Enable the auxiliary mouse device
	mouse_wait(1);
	outb(0xA8, 0x64);

	//Enable the interrupts
	mouse_wait(1);
	outb(0x20, 0x64);
	mouse_wait(0);
	_status = (inb(0x60) | 2);
	mouse_wait(1);
	outb(0x60, 0x64);
	mouse_wait(1);
	outb(_status, 0x60);

	//Tell the mouse to use default settings
	mouse_write(0xF6);
	mouse_read();  //Acknowledge

	// 0xF3 (Set Sample Rate)
	mouse_write(0xF3);
	mouse_read();  //Acknowledge

	// 10 samples/sec
	mouse_write(10);
	mouse_read();  //Acknowledge

	//Enable the mouse
	mouse_write(0xF4);
	mouse_read();  //Acknowledge

	//mouse_write(0xE6);
	//mouse_read();

	mouse_write(0xE8);
	mouse_read();
	mouse_write(0x00);




	//Setup the mouse handler
	mouse_enable();



}
/*
 * Decides the movement of the mouse cursor
 * 
 */
extern void set_pointer()
{
	{
	
		if ( mouse_y > 0){/* when the bit is not set, y is going down*/
			if( y_up += mouse_y > 5 ){
				temp = mouse_y/20;
				pos_y = pos_y - (1+temp);
				y_up = 0;
			}
		}
		else 
		{	
			if( y_down += mouse_y < -5){
				temp = mouse_y/20;
				pos_y = pos_y + (1-temp);
				//pos_y = pos_y + 1;
				y_down = 0;
			}
		}
	}
	{
		if (mouse_x > 0){
			if( x_right += mouse_x > 2){
				temp = mouse_x/5;
				pos_x = pos_x + 1 + temp;
				x_right = 0;
			}
		}
		else
		{
			if(x_left += mouse_x < -2){
				temp = mouse_x/5;
				pos_x = pos_x - 1 + temp;
				x_left = 0;
			}
		}
	}

	if(pos_x >= 80 || pos_x < 0) //boundary check
	{
		pos_x = prevpos_x;
	}
	
	if(pos_y >= 25 || pos_y < 0)
	{
		pos_y = prevpos_y;
	}

	//Save the current terminal status 
	cli();
	pcb* curr_pcb = getting_to_know_yourself();
	uint32_t curr_base_add = video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add;
	video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = VID_MEM_IDX;
	flush_tlb();

	memcpy((void*) mouse_buf, (void*) 0xB8000, four_kb);

	video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = curr_base_add;
	flush_tlb();
	sti();
	

	set_screen_x(prevpos_x);
	set_screen_y(prevpos_y);

	/* 
	 * previous mouse pointer positions
	 * Erases pointer
	 */

	bufpos = 2*prevpos_x + 160*prevpos_y;	//calculating previous buf position
	
	if(mouse_buf[bufpos] == 32 || mouse_buf[bufpos] == 0 || mouse_buf[bufpos] == 219)		//when there were blank, null, and cursor previous position
	{
		if(bufcpy[bufpos+1] != NULL){
			prevbufpos = bufpos;	//save the previous buffer position
		}
		else{
			cli();
			pcb* curr_pcb = getting_to_know_yourself();
			uint32_t curr_base_add = video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add;
			video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = VID_MEM_IDX;
			flush_tlb();

			mouse_buf[bufpos] = 32;	//set the buffer as blank, erases
			memcpy( (void*) 0xB8000,(void*) mouse_buf, four_kb);

			video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = curr_base_add;
			flush_tlb();
			sti();
		}
	}
	else
	{
		if(bufcpy[bufpos+1] != NULL){
			cli();
			pcb* curr_pcb = getting_to_know_yourself();
			uint32_t curr_base_add = video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add;
			video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = VID_MEM_IDX;
			flush_tlb();

			prevbufpos = bufpos+1;	//saves current character color
			mouse_buf[bufpos+1] = 112;	//reverse the color
			memcpy( (void*) 0xB8000,(void*) mouse_buf, four_kb);

			video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = curr_base_add;
			flush_tlb();
			sti();
		}
		else{
			cli();
			pcb* curr_pcb = getting_to_know_yourself();
			uint32_t curr_base_add = video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add;
			video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = VID_MEM_IDX;
			flush_tlb();

			mouse_buf[bufpos+1] = 7;	//ordinary color
			memcpy( (void*) 0xB8000,(void*) mouse_buf, four_kb);

			video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = curr_base_add;
			flush_tlb();
			sti();
		}
	}

	set_screen_x(pos_x);
	set_screen_y(pos_y);

	/* 
	 * current mouse pointer positions
	 * Draws pointer
	 */
	bufpos = 2*pos_x + 160*pos_y;	//calculate current mouse position

	/* save the position char in global variable */
	curr_char = mouse_buf[bufpos];  curr_attrib = mouse_buf[bufpos+1];

	//when there are blank, null, cursor at the current position
	if(mouse_buf[bufpos] == 32 || mouse_buf[bufpos] == 0 || mouse_buf[bufpos] == 219)	
	{
		cli();
		pcb* curr_pcb = getting_to_know_yourself();
		uint32_t curr_base_add = video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add;
		video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = VID_MEM_IDX;
		flush_tlb();

		mouse_buf[bufpos] = 219;	//draw cursor
		putc(mouse_buf[bufpos]); 	//draw

		video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = curr_base_add;
		flush_tlb();
		sti();
	}
	else // if cursor is above characters, block the characters
	{

		cli();
		pcb* curr_pcb = getting_to_know_yourself();
		uint32_t curr_base_add = video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add;
		video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = VID_MEM_IDX;
		flush_tlb();

		mouse_buf[bufpos+1] = 112;	//reverse color
		memcpy( (void*) 0xB8000,(void*) mouse_buf, four_kb);

		video_page_table[curr_pcb->pid].dir_arr[VID_MEM_IDX].page_base_add = curr_base_add;
		flush_tlb();
		sti();
	}


	prevpos_y = pos_y;	//save current position for future use
	prevpos_x = pos_x;	//save current position for future use

}

/*
 *	
 */
extern void rclk_block(){
	if(mouse_buf[prevbufpos] == 219){  //erase block created at empty place
		mouse_buf[prevbufpos] = 32;
		memcpy( (void*) 0xB8000,(void*) mouse_buf, four_kb);
	}

	for(i=0; i<4000; i++){		//buffer clear
		bufcpy[i] = NULL;
	}
	normal_screen();
	bufcpy[bufpos+1] = 112;		//save buffer
}

/*
 *	
 */
extern void lclk_copy()
{
	prevbufpos_c = bufpos;
	while((mouse_buf[prevbufpos_c] != 32)){

		bufrclk[prevbufpos_c] = mouse_buf[prevbufpos_c];
		prevbufpos_c+=2;
		counter2++;
		
	}
	prevbufpos_c -= 2;
	while((mouse_buf[prevbufpos_c] != 32)){
		bufrclk[prevbufpos_c] = mouse_buf[prevbufpos_c];
		prevbufpos_c-=2;
		counter++;
		if(prevbufpos_c == 0) {
			bufrclk[0] = mouse_buf[0];
			break;
		}
			
	}
	

	prevbufpos_c = bufpos;
}

extern void mouse_r_click()
{

} 


extern void mouse_l_click()
{

	/* clearing buffer first */
	for(i = 0; i < 128; i++){
		Bazinga[i] = '\0';
	}

	int temp = bufpos;
	/* getting to the starting position see */
	while( !((mouse_buf[temp] == 32) || (temp%160 == 0))){/* case havent reach starting  of buffer yet */
		temp -= 2;
	}

	/* than looking for the end */
	if(mouse_buf[temp] == 32){
		temp += 2;
	}

	int i = 0;
	while( !((mouse_buf[temp] == 32) || ((temp+1)%160 == 0))){ /* case havent reach end of buffer yet */
		Bazinga[i] = mouse_buf[temp];
		i ++;
		temp += 2;
	}
	send_eoi(M_IRQ);
	if(pos_x==79&&pos_y==0){
		halt(0);
	}
	execute((uint8_t*)Bazinga);
}



