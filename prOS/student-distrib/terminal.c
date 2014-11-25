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


terminal_buffer terminals[3];


history_buffer terminal_history;

 /* Opens the terminal
  * Initializes important variables
  *	returns 0
  */ 
int terminal_open()
{
	//opens the next terminal
	//for now setting the current terminal to open is the 0th one
	curr_terminal = 0; 
	
	//initilizes element	
	terminals[curr_terminal].size = 0;
	terminals[curr_terminal].yloc = 0;
	terminals[curr_terminal].xloc = 0;
	terminals[curr_terminal].caps = 0;
	terminals[curr_terminal].shift = 0;
	terminals[curr_terminal].ctrl = 0;
	terminals[curr_terminal].reading  = 0;
	return 0;
}

/* Reads count bytes from the terminal 
 * returns number of bytes sucessfully read
 */ 
int terminal_read(char *buf, int32_t count )
{
	//passed in a bad buffer
	if(buf == NULL)
		return -1;
		
	//if they want to read 0 bytes
	if(count == 0)
		return 0;

	terminals[curr_terminal].reading =1;
	//need to wait until the buffer has been terminated with a \n or the buffer fills up
	while( terminals[curr_terminal].reading  != 0 )
	{
		//do the dew and wait for reading to finish
	}

	int curr_index = 0;
	//codes for the cases 
	//1.reading count bytes
	//2.until null terminaled
	//3.current index is larger than the size of the buffer
	while(curr_index < count && 
		  terminals[curr_terminal].buf[curr_index] != '\n' &&
		  curr_index < terminals[curr_terminal].size )
	{
		buf[curr_index] =  terminals[curr_terminal].buf[curr_index];
		curr_index++;
	}



	//finished reading
	
	//the line that is return should include the line terminating character	
	buf[curr_index] = '\n';

	//stores the current buffer into the history an clears the buffer
	 terminals[curr_terminal].size = 0;
	return curr_index;
}

/* Writes count bytes to the buffer 
  * returns number of bytes written to the terminal
  */ 
int terminal_write(char *buf, int32_t count )
{
	//want to write zero bytes
	if(count == 0)
		return 0;

	// bad buffer
	if(buf == NULL)
		return -1;

	
	uint32_t curr_index =0 ;
	//1.writing count bytes
	while(curr_index < count)
	{
		printt(buf[curr_index]); 
		curr_index++;
	}

	//returns the number of bytes written
	return curr_index;
}



/* Clears the buffer and closes it
  *  returns 0
  */ 
int terminal_close()
{
	//curr_terminal_loc = 0;
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
	    (key == Lp && terminals[curr_terminal].ctrl == 1)			
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
			if(terminals[curr_terminal].size != 0)
			{
				//setting the x position to 
				set_screen_x(terminals[curr_terminal].xloc -1);
				putc(' ');

				//setting the current screen location back
				terminals[curr_terminal].xloc--;
				set_screen_x(terminals[curr_terminal].xloc -1);
				terminals[curr_terminal].size--;
			}

			break;

		case ENTP :

			//currenly executing terminal read
			if( terminals[curr_terminal].reading == 1)
			{
				//getting a new line and writing the terminal character into the buffer
				new_line();
				terminals[curr_terminal].buf[terminals[curr_terminal].size] = '\n';

				//finished reading
				terminals[curr_terminal].reading =0;
			}

			//now im just printing
			else
			{
				//checking the case if at the bottom of the screen
				new_line();
			}

			/*store entire line into the history */
			add_to_history((char*)terminals[curr_terminal].buf );
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

		//l is pressed as well as the ctrl
		case Lp :
			terminals[curr_terminal].size =  0;
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
//			//	printf("case 2\n");
//				strcpy((int8_t*)terminal_buffer, (int8_t*)terminal_history.command[terminal_history.current].cmd);
//
//				for( i = 0; i < 128; i++){ /* clear current terminal buffer*/
//					terminals[curr_terminal].buf[i] = ' ';
//				}
//			//	printf("case 2\n");
//				strcpy((int8_t*)terminals[curr_terminal].buf, (int8_t*)terminal_history.command[terminal_history.end-1].cmd);
//
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
	if(terminals[curr_terminal].caps == 1)
		terminals[curr_terminal].caps = 0;
	else
		terminals[curr_terminal].caps =1;
}

void toggle_shift()
{
	if(terminals[curr_terminal].shift == 1)
		terminals[curr_terminal].shift = 0;
	else
		terminals[curr_terminal].shift =1;
}

void toggle_ctrl()
{
	if(terminals[curr_terminal].ctrl == 1)
		terminals[curr_terminal].ctrl = 0;
	else
		terminals[curr_terminal].ctrl = 1;

}	


void new_line()
{
		if( get_screen_y() == NUM_ROWS -1)
		{	
			//scrolling the screen		
			vert_scroll(1);

			//moving the current location
			set_screen_y( NUM_ROWS -1);
			set_screen_x( 0);
			terminals[curr_terminal].yloc = get_screen_y();
			terminals[curr_terminal].xloc = get_screen_x();
		}
		else
		{
			//printing the screen down
			putc('\n');

			//moving the x screen location
			set_screen_y( terminals[curr_terminal].yloc + 1);
			set_screen_x( 0 );
			terminals[curr_terminal].yloc = get_screen_y();
			terminals[curr_terminal].xloc = get_screen_x();
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

/* terminal print
 * takes a char and a current screen location and prints the char there
 * returns nothing
 *
 * should also set the current x and y correct
 */
void printt(char c)
{
	//here is where I should code for special cases

	//sets the screen loc in video mem
	set_screen_y(terminals[curr_terminal].yloc);	
	set_screen_x(terminals[curr_terminal].xloc);
	
	//last line
	if(get_screen_y() == NUM_ROWS -1)
	{
		//character is an enter
		if(c == '\n')
		{
			new_line();
		}
		//at the end 
		else if(get_screen_x() == NUM_COLS -1)
		{
			putc(c);
			new_line();
		}
		//somewhere in the middle of the screen
		else 
		{
			//prints the new character]
			putc(c);
		}
	}
	//not on last line
	else	
	{
			if(get_screen_x() ==  NUM_COLS -1)
			{
				putc(c);
				new_line();
			}
			else
			{
			putc(c);
			}
	}

	//gets the new screen loc from video mem
	terminals[curr_terminal].yloc = get_screen_y();
	terminals[curr_terminal].xloc = get_screen_x();


}
