#include "terminal.h"
#include "lib.h"

#define NUM_COLS 80
#define NUM_ROWS 25

void * stdin_opt[4]={
  terminal_open,
  terminal_read,
  stdin_write,
  terminal_close
};

void * stdout_opt[4]={
  terminal_open,
  stdout_read,
  terminal_write,
  terminal_close
};


history_buffer terminal_history;

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
int terminal_read(char *buf, int32_t count )
{
	if(buf == NULL)
		return -1;
		
	if(count == 0)
		return 0;

	reading = 1;
	//need to wait until the buffer has been terminated with a \n or the buffer fills up
	while( reading != 0 )
	{
		//do the dew
	}

	int curr_index = 0;
	//codes for the cases 
	//1.reading count bytes
	//2.until null terminaled
	//3.or until buffer is done
	while(curr_index < count && terminal_buffer[curr_index] != '\n' && curr_index < curr_terminal_loc )
	{
		buf[curr_index] =  terminal_buffer[curr_index];
		curr_index++;
	}
	
	buf[curr_index] = '\n';
	curr_terminal_loc = 0;
	return curr_index;
}


/* Writes count bytes to the buffer 
  * returns number of bytes written to the terminal
  */ 
int terminal_write(char *buf, int32_t count )
{

	int buf_size =  strlen(buf);
	int temp = count;
	//curr_terminal_loc = 0;

	if(buf_size < count )
		count = buf_size;
		
	if(buf == NULL)
		return -1;

	
	if(count == 0)
	{
		return 0;
	}

	if(count < 0)
	{
		count *= -1;
	}


	int i;
	if(temp <0)
	{
		int j =0;
		//write_buf_to_screen_hex();
		for(i = 0; i < count ; i++)
		{
			if( j == NUM_COLS  )
			{
				if(get_screen_y() ==  NUM_ROWS-1)
				{
					vert_scroll(1);
					move_screen_x(0);

				}
				else
				{
					putc('\n');
				}

				j=0;
			}
			j++;	
			printf("%x",buf[i]);
		}
	}
	else
	{
		int j =0;
		for(i =0; i < count ; i++)
		{
			if( j == NUM_COLS )
			{
				if(get_screen_y() ==  NUM_ROWS-1)
				{
					vert_scroll(1);
					move_screen_x(0);

				}
				else
				{
					putc('\n');
				}
				j=0;
			}	  
			else if(buf[i] == '\n')
			{
				if(get_screen_y() ==  NUM_ROWS-1)
				{
					vert_scroll(1);
					move_screen_x(0);
				}
				else
				{
					putc('\n');
				}
				j=0;
			}

			if(buf[i] != '\n')
			{
				j++;
				printf("%c",buf[i]);
			}
		}
	
	}

	//return curr_index;
	return count;
}



/* Clears the buffer and closes it
  *  returns 0
  */ 
int terminal_close()
{
	curr_terminal_loc = 0;
	return 0;
}



//vertical scroll gone to lib.c

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
		key == UPP 					||
		key == UPR 					||
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
	int i; /* loop counters */
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
				//terminal_buffer[curr_terminal_loc] = ' ';
				curr_terminal_loc--;
				move_screen_x(curr_terminal_loc);
				putc(' ');
				move_screen_x(curr_terminal_loc);
			}

			break;

		case ENTP :

			//currenly executing terminal read
			if( reading == 1)
			{
				new_line();
				terminal_buffer[curr_terminal_loc] = '\n';
				reading =0;
			}

			//now im just printing
			else
			{
				//checking the case if at the bottom of the screen
				new_line();
				curr_terminal_loc = 0;	
			}

			/*store entire line into the history */
			add_to_history((char*)terminal_buffer);
			terminal_history.current = terminal_history.end; /* reset current position */



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
			curr_terminal_loc =  0;
			clear();
			break;

		case UPP: /* case where up arrow key is pressed */
			//printf("up key pressed\n");
			/* load and print out prvious command */

			/* copying over the new string */
			if((terminal_history.current) == terminal_history.begin){ /* case reach beginning of buffer*/
				/* do nothing */
			//	printf("case 1\n");
			}
			else{/* case not at beginning of the buffer yet*/
				terminal_history.current --; /* move one position backwards */
	//			for( i = 0; i < 128; i++){ /* clear current terminal buffer*/
	//				terminal_buffer[i] = 0;
	//			}
				int count = 0;
				i = 0;
				while(!((terminal_buffer[i] == '\n') || (terminal_buffer[i] == '\0'))){
					terminal_buffer[i] = 0;
					count ++; i++; 
				}
			//	printf("case 2\n");
				strcpy((int8_t*)terminal_buffer, (int8_t*)terminal_history.command[terminal_history.current].cmd);
				/*print out the buffer */
			//	printf("%s\n", terminal_buffer);
				//printf("%s\n", terminal_history.command[terminal_history.end-1].cmd);
			}
			curr_terminal_loc = 0; //terminal_history.pre_pos;
			printf("%s", terminal_history.command[terminal_history.current].cmd);
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




void new_line()
{
		if( get_screen_y() == NUM_ROWS -1)
		{
			move_screen_x(0);
			vert_scroll(1);
		}
		else
		{
			putc('\n');
		}
}

/**
  * stdin_write
  * place holder function in file descriptor for stdin
  * which should never be called, and when it is called 
  * return 0
  */
int stdin_write(){
	return 0;
}

/**
  *stdout_read
  * place holder function in file descriptor for stdout 
  * which should never be called, and when it is called 
  * return 0
  */
int stdout_read(){
	return 0;
}

/**
  * add_to_history
  *   function which add entire command line into 
  *  terminal history buffer for future use
  */
void add_to_history(char* buffer){
	cli();
	int i = 0;
	terminal_history.end ++;
	if(terminal_history.end >= his_buff_size){ /* case reach out range of buffer  */
		terminal_history.end = 0; /* loop it around to 0*/
	}
	if(terminal_history.end == terminal_history.begin){ /* case buffer is full */
		terminal_history.begin ++; /* move begin over 1*/
		if(terminal_history.begin >= his_buff_size){ /*case begin reach the end*/
			terminal_history.begin = 0; /* loop it around */
		}
	}
	/* copying terminal buffer into history buffer */
	while(buffer[i] != '\n'){
		terminal_history.command[terminal_history.end].cmd[i] = buffer[i];
		i++;
	}
	terminal_history.command[terminal_history.end].cmd[i] = '\0' ;
	//strcpy((int8_t*)terminal_history.command[terminal_history.end].cmd, (int8_t*)buffer);
	sti();
}

