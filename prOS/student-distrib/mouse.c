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

uint8_t buf[four_kb];
uint8_t bufcpy[four_kb];
uint8_t bufrclk[four_kb];
uint8_t Bazinga[128];

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
      break;
  }

mouse_r_click();
mouse_l_click();

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
	memcpy((void*) buf, (void*) 0xB8000, four_kb);
	

	set_screen_x(prevpos_x);
	set_screen_y(prevpos_y);

	/* 
	 * previous mouse pointer positions
	 * Erases pointer
	 */

	bufpos = 2*prevpos_x + 160*prevpos_y;	//calculating previous buf position
	
	if(buf[bufpos] == 32 || buf[bufpos] == 0 || buf[bufpos] == 219)		//when there were blank, null, and cursor previous position
	{
		if(bufcpy[bufpos+1] != NULL){
			prevbufpos = bufpos;	//save the previous buffer position
		}
		else{
			buf[bufpos] = 32;	//set the buffer as blank, erases
			memcpy( (void*) 0xB8000,(void*) buf, four_kb);
		}
	}
	else
	{
		if(bufcpy[bufpos+1] != NULL){
			prevbufpos = bufpos+1;	//saves current character color
			buf[bufpos+1] = 112;	//reverse the color
			memcpy( (void*) 0xB8000,(void*) buf, four_kb);
		}
		else{
			buf[bufpos+1] = 7;	//ordinary color
			memcpy( (void*) 0xB8000,(void*) buf, four_kb);
		}
	}

	set_screen_x(pos_x);
	set_screen_y(pos_y);

	/* 
	 * current mouse pointer positions
	 * Draws pointer
	 */
	bufpos = 2*pos_x + 160*pos_y;	//calculate current mouse position

	//when there are blank, null, cursor at the current position
	if(buf[bufpos] == 32 || buf[bufpos] == 0 || buf[bufpos] == 219)	
	{
		buf[bufpos] = 219;	//draw cursor
		putc(buf[bufpos]); 	//draw
	}
	else // if cursor is above characters, block the characters
	{
		buf[bufpos+1] = 112;	//reverse color
		memcpy( (void*) 0xB8000,(void*) buf, four_kb);
	}


	prevpos_y = pos_y;	//save current position for future use
	prevpos_x = pos_x;	//save current position for future use

}

/*
 *	
 */
extern void rclk_block(){
	if(buf[prevbufpos] == 219){  //erase block created at empty place
		buf[prevbufpos] = 32;
		memcpy( (void*) 0xB8000,(void*) buf, four_kb);
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
	while((buf[prevbufpos_c] != 32)){

		bufrclk[prevbufpos_c] = buf[prevbufpos_c];
		prevbufpos_c+=2;
		counter2++;
		
	}
	prevbufpos_c -= 2;
	while((buf[prevbufpos_c] != 32)){
		bufrclk[prevbufpos_c] = buf[prevbufpos_c];
		prevbufpos_c-=2;
		counter++;
		if(prevbufpos_c == 0) {
			bufrclk[0] = buf[0];
			break;
		}
			
	}
	

	prevbufpos_c = bufpos;
}

/*
extern void mouse_rclick()
{
	if (mouse_byte[1] == 9){
   		//t_toggle = curr_terminal;

   		if(curr_terminal == 0){
   			if(prevdata == 1){
   				terminal_switch(1);
   			}
   		}
   		else if(curr_terminal == 1){
   			if(prevdata == 1){
   				terminal_switch(2);
   			}
   		}
   		else if(curr_terminal == 2){
   			if(prevdata == 1){
   				terminal_switch(0);
   			}
   		}
   		prevdata = 0;
   	}
   	if(mouse_byte[1] == 8)
   	{
   		prevdata = 1;
   	}
}

extern void mouse_lclick()
{
	if (mouse_byte[1] == 10){
   		//t_toggle = curr_terminal;

   		if(curr_terminal == 0){
   			if(prevdata == 1){
   				terminal_switch(2);
   			}
   		}
   		else if(curr_terminal == 1){
   			if(prevdata == 1){
   				terminal_switch(0);
   			}
   		}
   		else if(curr_terminal == 2){
   			if(prevdata == 1){
   				terminal_switch(1);
   			}
   		}
   		prevdata = 0;
   	}
   	if(mouse_byte[1] == 8)
   	{
   		prevdata = 1;
   	}
}
*/

extern void mouse_r_click()
{

	if (mouse_byte[1] == 10){
		if(prevdata == 1){
			//lclk_block();
			i = 0;
			counter2 = counter - counter2;
			prevbufpos_c = prevbufpos_c - counter2*2;
			while(counter != 0)
			{
			//	printf("%c", bufrclk[prevbufpos_c+i]);
				Bazinga[i/2]= bufrclk[prevbufpos_c+i];
				i = i+2;
				counter--;
			}
			counter = 0;
			counter2 = 0;
			execute((int8_t*)Bazinga);
		} 
		prevdata = 0;
	}
	if(mouse_byte[1] == 8)
	{
		prevdata = 1;
	}

} 


extern void mouse_l_click()
{
	if (mouse_byte[1] == 9){
		if(prevdata ==1){
			lclk_copy();
			execute((int8_t*)bufrclk);

		}
		prevdata = 0;
		
	}
	if(mouse_byte[1] == 8)
   	{
   		prevdata = 1;
   	}
}

