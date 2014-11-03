#include "terminal.h"
#include "lib.h"

#define NUM_COLS 80
#define NUM_ROWS 25

 /* Opens the terminal
  * Initializes important variables
  *	returns 0
  */ 
int terminal_open()
{
	curr_terminal_loc = 0;
	caps = 0; 
	shift = 0;
	ctrl = 0;
	written = 0;
	return 0;
}

/* Reads count bytes from the terminal 
 * returns number of bytes sucessfully read
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
  * returns number of bytes written to the terminal
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

	write_buf_to_screen();
	return curr_index;

}



/* Clears the buffer and closes it
  *  returns 0
  */ 
int terminal_close()
{
	curr_terminal_loc = 0;
	return 0;
}


/* Clears the current line and writes the buffer to the screen
 * actions change if the buffer is longer than the screen width	
 *
 */
int write_buf_to_screen()
{
	//may need to change 
	int i;
	
	//tests vertical position
	if(get_screen_y() == NUM_ROWS-1)
	{
			vert_scroll(1);
			move_screen_x();
			clear_line();
	}
	else if(written == 0 )
	{
	}
	else
	{
		putc('\n');
	}

	written = 1;

	/*still on the first line*/
	if(curr_terminal_loc < NUM_COLS)
	{
		//clear_line();
		for(i = 0; i<curr_terminal_loc; i++)
		{
			printf("%c",terminal_buffer[i]);
		}

	}
	
	/*The case where we are just going into the second line*/
	else if( curr_terminal_loc > NUM_COLS)
	{
		//clear_line();
		//printing on the first line
		for(i = 0; i< NUM_COLS; i++)
		{
			printf("%c",terminal_buffer[i]);
		}
		if(get_screen_y() == NUM_ROWS-1)
		{
			vert_scroll(1);
			move_screen_x();
		}
		else
		{
			putc('\n');
		}

		//printing the rest of ther characters 
		for(i=NUM_COLS; i < curr_terminal_loc; i++)
		{
			printf("%c",terminal_buffer[i]);
		}
	}


	return 0;
}


//vertical scrolling function moved to lib.h



/* 	Simple function that has a logic to check if the key pressed is a special case that should not be printed to the screen
 *
 *	returns 1 of it is a special key 0 otherwise
 */
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


/*	This function executes the function that the special key denotes when called
 * 	returns nothing
 */
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
			if(get_screen_y() == NUM_ROWS-1)
			{
				vert_scroll(1);
				clear_line();
				curr_terminal_loc = 0;
			}
			else 
			{
				putc('\n');
				curr_terminal_loc = 0;
			}	
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
			written = 0;
			clear();
			break;


	return;
	}	
}

/* these function simply toggle the vaule of the keys when called*/
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
