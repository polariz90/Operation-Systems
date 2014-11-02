#include "terminal.h"
#include "lib.h"


/* Opens the terminal
  *
  *
  */ 
int terminal_open()
{
	curr_terminal_loc = 0;
	caps = 0; 
	ctrl = 0;
	return 0;
}

/* Reads count bytes from the terminal 
  *
  * Returns the number of bytes read
  */ 
int terminal_read(char *buf, uint32_t count )
{
	if(count == 0)
	{
		return 0;
	}

	int curr_index = 0;
	while(curr_index < count && terminal_buffer[curr_index] != '\n' && curr_index < curr_terminal_loc )
	{
		buf[curr_index] =  terminal_buffer[curr_index];
		curr_index++;
	}
	return curr_index;
}

/* Writes count bytes to the buffer 
  *
  *
  */ 
int terminal_write(char *buf, uint32_t count )
{
	curr_terminal_loc = 0;
	if(count == 0)
	{
		return 0;
	}

	int curr_index = 0;
	while(curr_index < count && buf[curr_index] != '\n' )
	{  
		terminal_buffer[curr_index] = buf[curr_index];
		curr_terminal_loc++;
		curr_index++;
	}
	return curr_index;

}



/* Clears the buffer and closes it
  *
  * Should return 0
  */ 
int terminal_close()
{
	return 0;
}

int write_buf_to_screen()
{
	
	clear_line();
	int i;
	for(i = 0; i<curr_terminal_loc; i++)
	{
		printf("%c",terminal_buffer[i]);
	}

	move_screen_xy(-1*curr_terminal_loc);
	return 0;
}


//vertical scrolling function moved to lib.h


int is_special_key(int key)
{

	/* 
	 * ltab press = 15
	 * ltab release = 143
	 *
	 * caps press = 58
	 * caps release = 186
	 *
	 * lshift press =  42 
	 * lshift release = 170 
	 *
	 * bs press = 14
	 * bs release = 142
	 *
	 * enter press = 28
	 * enter release = 156
	 *
	 * lcontrolp = 29
	 * lcontrolr =  157
	 *
	 * rshift press = 54
	 * rshift release = 182
	 *
	 * rcontrolp = 29 
	 * rcontrolr = 157
	 *
	 *
	 */


	if( key == LTABP   				||
		key == LTABR   				||
		key == CAPP    				||
		key == CAPR    				||
		key == LSHFTP  				||
	    key == LSHFTR  				||
		key == BSP     				||	
		key == BSR     				||	
		key == ENTP 			    ||	
		key == ENTR   				||	
		key == RSHFTP  				||	
		key == RSHFTR  				||
		key == CTLP 			    ||
		key == CTLR    				||
	    (key == Lp && ctrl == 1)			
	  )
	{
		return 1;
	}

	else
	{
		return 0; 
	}	
	
}



void exe_special_key(int key)
{
	switch(key)
	{

		case LTABP :
			break;
			
		case CAPP :
			toggle_caps();
			break;

		case LSHFTP :
			toggle_shift();
			break;

		case LSHFTR :
			toggle_shift();
			break;

		case BSP :
			if(curr_terminal_loc != 0 )
			{
				terminal_buffer[curr_terminal_loc] = ' ';
				curr_terminal_loc--;
				write_buf_to_screen();
			}
			break;

		case ENTP :
			putc('\n');
			curr_terminal_loc = 0;
			break;

		case RSHFTP :
			toggle_shift();
			break;

		case RSHFTR :
			toggle_shift();
			break;

		case CTLP :
			toggle_ctrl();
			break;

		case CTLR :
			toggle_ctrl();
			break;

		case Lp :
			clear();
			break;


	return;
	}	
}


void toggle_caps()
{
	if(caps == 1)
		caps = 0;
	else
		caps =1;
}

void toggle_shift()
{
	if(shift == 1)
		shift = 0;
	else
		shift =1;
}

void toggle_ctrl()
{
	if(ctrl == 1)
		ctrl = 0;
	else
		ctrl = 1;

}	
