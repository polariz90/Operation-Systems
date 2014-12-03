#include "terminal.h"
#include "file.h"
#include "lib.h"
#include "sys_call.h"

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


terminal_buffer terminals[3];/* 3 terminal structs for 3 terminals opening */
uint32_t terminal_vid_buf[3] = {term1_vid_buf, term2_vid_buf, term3_vid_buf};
int8_t* tap_file_names[15] = {	"frame1.txt",
								"verylargetxtwithverylongname.tx",
								"ls",
								"grep",
								"hello",
								"rtc",
								"testprint",
								"sigtest",
								"shell",
								"syserr",
								"fish",
								"cat",
								"frame0.txt",
								"pingpong",
								"counter"
};

 /* Opens the terminal
  * Initializes important variables
  *	returns 0
  */ 
int terminal_open()
{
	int i;
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
	terminals[curr_terminal].alt = 0;
	terminals[curr_terminal].reading  = 0;
	for(i = 0; i < 6; i++){
		terminals[curr_terminal].pros_pids[i] = 0;
	}
	/* initial terminal history structure */
	terminals[curr_terminal].terminal_history.begin = 0;
	terminals[curr_terminal].terminal_history.end = 0;
	terminals[curr_terminal].terminal_history.pre_pos = 0;
	terminals[curr_terminal].terminal_history.current = 0;

	return 0;
}

/* Reads count bytes from the terminal 
 * returns number of bytes sucessfully read
 */ 

int terminal_read(int32_t fd, char *buf, int32_t count )
{
	sti();
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
int terminal_write(int32_t fd, char *buf, int32_t count )
{
	pcb* current_pcb = getting_to_know_yourself(); /* geeting current pcb*/
    
	//want to write zero bytes
	if(count == 0)
		return 0;

	// bad buffer
	if(buf == NULL)
		return -1;

	
	uint32_t curr_index =0 ;
	//writing count bytes

	if(fd == magic_fd){
		while(curr_index < count)
		{
			printt(buf[curr_index]); 
			curr_index++;
		}
	}

	//executable file
	if(current_pcb->file_descriptor[fd].exe_flag == 0)
	{	
		while(curr_index < count)
		{
			printt(buf[curr_index]); 
			curr_index++;
		}
	}
	else
	{
		while(curr_index < count)
		{
			printt_hex(buf[curr_index]); 
			curr_index++;
		}

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
	 * altp   56
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
		key == ALTR					||
		key == DOWNP 				||
		key == ALTP 				||
		(key == F1P && terminals[curr_terminal].alt == 1) ||
		(key == F2P && terminals[curr_terminal].alt == 1) ||
		(key == F3P && terminals[curr_terminal].alt == 1) ||
		(key == Lc && terminals[curr_terminal].ctrl == 1) ||
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
	int i; /* loop counter */
	int8_t tap_buffer[32]; /* buffer to hold thing from tap */
	switch(key)
	{

		case LTABP :
			/* clear tap buffer first */
			for (i = 0; i < 32; i++){
				tap_buffer[i] = '\0';
			}
			cli();
			getting_tap_buffer(tap_buffer);
			find_tap_match(tap_buffer);
			sti();
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
			add_to_history((char*)terminals[curr_terminal].buf, curr_terminal);

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

		case ALTP :
			toggle_alt();
			break;

		case ALTR :
			toggle_alt();
			break;

		//l is pressed as well as the ctrl
		case Lp :
			terminals[curr_terminal].size =  0;
			clear();
			break;

		case Lc :
			halt(4);
			break;

		case UPP: /* case where up arrow key is pressed */
			//move the screen location back to the beginning of the buffer
			set_screen_x( terminals[curr_terminal].xloc - terminals[curr_terminal].size);
			
			//printing spaces
			for(i =0; i< terminals[curr_terminal].size; i++)
			{
				putc(' ');
			}

			//moves the xlocation back
			terminals[curr_terminal].xloc -= terminals[curr_terminal].size;
   

			/* clear current terminal buffer */
			for( i = 0; i < BUF_SIZE; i++){
				terminals[curr_terminal].buf[i] = '\0';
			}

			/* moving position pointer */
			if(terminals[curr_terminal].terminal_history.end == terminals[curr_terminal].terminal_history.begin){ /* case history is empty */
				break;
			}
			else if(terminals[curr_terminal].terminal_history.pre_pos == terminals[curr_terminal].terminal_history.begin){
				/* case reach the last history*/
				/*do nothing */
			}
			else if(terminals[curr_terminal].terminal_history.pre_pos == terminals[curr_terminal].terminal_history.end){
				terminals[curr_terminal].terminal_history.pre_pos --; 
				if(terminals[curr_terminal].terminal_history.pre_pos < 0){/* case reach the beginning */
					terminals[curr_terminal].terminal_history.pre_pos = his_buff_size-1;
				}
				/* put old command into terminal buffer */
				strcpy(terminals[curr_terminal].buf, terminals[curr_terminal].terminal_history.command[terminals[curr_terminal].terminal_history.pre_pos].cmd);
				/* updating the size of the buffer */
				terminals[curr_terminal].size = strlen(terminals[curr_terminal].terminal_history.command[terminals[curr_terminal].terminal_history.pre_pos].cmd);

				/* output the new command to terminal */
				terminal_write(1, terminals[curr_terminal].buf, strlen(terminals[curr_terminal].buf));
				break;
			}
			else{
				terminals[curr_terminal].terminal_history.pre_pos --; 
				if(terminals[curr_terminal].terminal_history.pre_pos < 0){/* case reach the beginning */
					terminals[curr_terminal].terminal_history.pre_pos = his_buff_size-1;
				}
			}

			/* put old command into terminal buffer */
			strcpy(terminals[curr_terminal].buf, terminals[curr_terminal].terminal_history.command[terminals[curr_terminal].terminal_history.pre_pos].cmd);
			/* updating the size of the buffer */
			terminals[curr_terminal].size = strlen(terminals[curr_terminal].terminal_history.command[terminals[curr_terminal].terminal_history.pre_pos].cmd);

			/* output the new command to terminal */
			terminal_write(1, terminals[curr_terminal].buf, strlen(terminals[curr_terminal].buf));
			break;

		case DOWNP:
			//move the screen location back to the beginning of the buffer
			set_screen_x( terminals[curr_terminal].xloc - terminals[curr_terminal].size);
			
			//printing spaces
			for(i =0; i< terminals[curr_terminal].size; i++)
			{
				putc(' ');
			}
			//moves the xlocation back
			terminals[curr_terminal].xloc -= terminals[curr_terminal].size;
		 
			/* clear current terminal buffer */
			for( i = 0; i < BUF_SIZE; i++){
				terminals[curr_terminal].buf[i] = '\0';
			}
			if(terminals[curr_terminal].terminal_history.end == terminals[curr_terminal].terminal_history.begin){ /* case history is empty */
				break;
			}
			else if(terminals[curr_terminal].terminal_history.pre_pos == terminals[curr_terminal].terminal_history.end){
				/* case reach the last history*/
				terminals[curr_terminal].terminal_history.pre_pos --; 
				if(terminals[curr_terminal].terminal_history.pre_pos < 0){/* case reach the beginning */
					terminals[curr_terminal].terminal_history.pre_pos = his_buff_size-1;
				}
				/* put old command into terminal buffer */
				strcpy(terminals[curr_terminal].buf, terminals[curr_terminal].terminal_history.command[terminals[curr_terminal].terminal_history.pre_pos].cmd);
				/* updating the size of the buffer */
				terminals[curr_terminal].size = strlen(terminals[curr_terminal].terminal_history.command[terminals[curr_terminal].terminal_history.pre_pos].cmd);
				/* output the new command to terminal */
				terminal_write(1, terminals[curr_terminal].buf, strlen(terminals[curr_terminal].buf));
				terminals[curr_terminal].terminal_history.pre_pos ++;
				if(terminals[curr_terminal].terminal_history.pre_pos > his_buff_size){/* case reach the beginning */
					terminals[curr_terminal].terminal_history.pre_pos = 0;
				}
				break;
			}
			else{
				terminals[curr_terminal].terminal_history.pre_pos ++;
				if(terminals[curr_terminal].terminal_history.pre_pos > his_buff_size){/* case reach the beginning */
					terminals[curr_terminal].terminal_history.pre_pos = 0;
				}
				if(terminals[curr_terminal].terminal_history.pre_pos == terminals[curr_terminal].terminal_history.end){
					/* case reach the last history*/
					terminals[curr_terminal].terminal_history.pre_pos --; 
					if(terminals[curr_terminal].terminal_history.pre_pos < 0){/* case reach the beginning */
						terminals[curr_terminal].terminal_history.pre_pos = his_buff_size-1;
					}
					/* put old command into terminal buffer */
					strcpy(terminals[curr_terminal].buf, terminals[curr_terminal].terminal_history.command[terminals[curr_terminal].terminal_history.pre_pos].cmd);
					/* updating the size of the buffer */
					terminals[curr_terminal].size = strlen(terminals[curr_terminal].terminal_history.command[terminals[curr_terminal].terminal_history.pre_pos].cmd);
					/* output the new command to terminal */
					terminal_write(1, terminals[curr_terminal].buf, strlen(terminals[curr_terminal].buf));
					terminals[curr_terminal].terminal_history.pre_pos ++;
					if(terminals[curr_terminal].terminal_history.pre_pos > his_buff_size){/* case reach the beginning */
						terminals[curr_terminal].terminal_history.pre_pos = 0;
					}
					break;
				}
			}
			/* put old command into terminal buffer */
			strcpy(terminals[curr_terminal].buf, terminals[curr_terminal].terminal_history.command[terminals[curr_terminal].terminal_history.pre_pos].cmd);
			/* updating the size of the buffer */
			terminals[curr_terminal].size = strlen(terminals[curr_terminal].terminal_history.command[terminals[curr_terminal].terminal_history.pre_pos].cmd);
			/* output the new command to terminal */
			terminal_write(1, terminals[curr_terminal].buf, strlen(terminals[curr_terminal].buf));
			break;



		case F1P:
			if (curr_terminal == 0){
				break;
			}
			terminal_switch(0);
			break;

		case F2P:
			if(curr_terminal == 1){
				break;
			}
			terminal_switch(1);
			break;

		case F3P:
			if(curr_terminal == 2){
				break;
			}
			terminal_switch(2);
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

void toggle_alt()
{
	if(terminals[curr_terminal].alt == 1)
		terminals[curr_terminal].alt = 0;
	else
		terminals[curr_terminal].alt = 1;

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
	return -1;
}

/**
  *stdout_read
  * place holder function in file descriptor for stdout 
  * which should never be called, and when it is called 
  * return 0
  */
int stdout_read(){
	return -1;
}

/**
  * add_to_history
  *   function which add entire command line into 
  *  terminal history buffer for future use
  */
void add_to_history(char* buffer, uint32_t terminal_idx){
	//terminal_write(1,"add to history reached ",24);
	cli();
	int i = 0;
	/* copying terminal buffer into history buffer */
	while(buffer[i] != '\n'){
		terminals[terminal_idx].terminal_history.command[terminals[terminal_idx].terminal_history.end].cmd[i] = buffer[i];
		i++;
	}
	terminals[terminal_idx].terminal_history.command[terminals[terminal_idx].terminal_history.end].cmd[i] = '\0' ;

	/* increment the end pointer */
	terminals[terminal_idx].terminal_history.end ++;
	terminals[terminal_idx].terminal_history.pre_pos = terminals[terminal_idx].terminal_history.end;
	if(terminals[terminal_idx].terminal_history.end >= his_buff_size){ /* case reach out range of buffer  */
		terminals[terminal_idx].terminal_history.end = 0; /* loop it around to 0*/
	}
	if(terminals[terminal_idx].terminal_history.end == terminals[terminal_idx].terminal_history.begin){ /* case buffer is full */
		terminals[terminal_idx].terminal_history.begin ++; /* move begin over 1*/
		if(terminals[terminal_idx].terminal_history.begin >= his_buff_size){ /*case begin reach the end*/
			terminals[terminal_idx].terminal_history.begin = 0; /* loop it around */
		}
	}
	
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


/* terminal print hex
 * takes a char and a current screen location and prints the char there
 * returns nothing
 *
 * should also set the current x and y correct
 */
void printt_hex(char c)
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
			printf("%x",c);
			new_line();
		}
		//somewhere in the middle of the screen
		else 
		{
			//prints the new character]
			printf("%x",c);
		}
	}
	//not on last line
	else	
	{
			if(get_screen_x() ==  NUM_COLS -1)
			{
				printf("%x",c);
				new_line();
			}
			else
			{
			printf("%x",c);
			}
	}

	//gets the new screen loc from video mem
	terminals[curr_terminal].yloc = get_screen_y();
	terminals[curr_terminal].xloc = get_screen_x();


}

/**
  * find tap match 
  *		going through the history node array to find the 
  * most match one that contains the buffer inside 
  */

void find_tap_match(const int8_t* buf){
	int i, j; /* loop counter */
	int length = strlen(buf) - 1; /* getting input buffer length */
	int match_mask[15]; /* match mask */
	int match_num = 0; /* match number */
	for(i = 0; i < 15; i++){/* loop to find potential match */
		if(strncmp(buf, tap_file_names[i], length) == 0){/* case potential match */
			match_mask[i] = 1;
			match_num ++;
		}
		else{
			match_mask[i] = 0;
		}
	}
	if(match_num == 1){ /*only 1 match */
		i = 0; /* find position of match */
		while(match_mask[i] != 1){
			i++;
		}

		int position = length + 1; /* keep in trach of the name position */
		int8_t temp_buffer[32]; /* buffer to hold rest of the name */
		int8_t compare_buffer[32]; /*buffer to hold the name to compare */
		strcpy(compare_buffer, tap_file_names[i]);
		j = 0;
		while(compare_buffer[position] != '\0'){
			temp_buffer[j] = compare_buffer[position];
			j++; position++;
		}
		temp_buffer[j] = '\0';
		/*writing rest of them into terminal */
		terminal_write(1, temp_buffer, (strlen(tap_file_names[i])-length));
		/* store into the terminal buffer */
		j = 0; position = terminals[curr_terminal].size;
		while(temp_buffer[j] != '\0'){
			terminals[curr_terminal].buf[position] = temp_buffer[j];
			j++; position++; terminals[curr_terminal].size++;
		}
		terminals[curr_terminal].buf[position] = '\0';
		terminals[curr_terminal].size++;
	}
	else if (match_num == 2){
		print_tap_match(match_mask, length);
	}
}

/**
  *  print tap match
  * 		function to print out the tap match
  */
void print_tap_match(const int* mask, int length){
	int8_t buf[32]; /* buffer to hold the printing character */
	//length ++;  /* starting with the next unread character */
	int i, j; /* loop counter */
	int position; 
	int arr1_idx = -1;
	int arr2_idx = -1;
	int8_t buf_1[32], buf_2[32];

	for(i = 0; i < 15; i++){/* loop though mask to find match */
		if (mask[i] == 1){
			if(arr1_idx == -1){
				arr1_idx = i;
				strcpy(buf_1, tap_file_names[i]);
			}
			else{
				arr2_idx = i;
				strcpy(buf_2, tap_file_names[i]);
				break;
			}
		}
	}
	j = 0;
	while(strncmp(buf_1, buf_2, length+1) == 0){ /* comparing up to length*/
		buf[j] = buf_1[length+1];
		j++; length ++;
	}

	/* write rest of the word into terminal */
	terminal_write(1, buf, strlen(buf));
	/* store into terminal buffer */
	j = 0; position = terminals[curr_terminal].size;
	while(buf[j] != '\0'){
		terminals[curr_terminal].buf[position] = buf[j];
		j++; position++; terminals[curr_terminal].size++;
	}
}

/**
  * getting tap buffer
  *		which looping forward into terminal buffer and 
  *  get the current word of tap, store the word into the buffer
  */
void getting_tap_buffer(int8_t* buf){
	int curr_position = terminals[curr_terminal].size - 1; /* store current position */
	int start = curr_position; /* pointer going forward search the starting of the world */
	int i = 0; /* loop counter */

	/* searching beginning */
	while(start != 0 && terminals[curr_terminal].buf[start] != 32){
		start --; 
	}

	if(terminals[curr_terminal].buf[start] == 32){
		start ++;
	}

	while(start <= curr_position){
		buf[i] = terminals[curr_terminal].buf[start];
		i++; start ++;
	}
}


/**
  * terminal_switch
  * 	  helper functions to switch terminals, steps that includes:
  * 	1. copying current video memory into corresponding terminal bffer 
  * 	2. map current terminal video memory page into the buffer 
  * 	3. copying new terminal buffer into video memory
  *		4. map new terminal video memory page into the video memory
  */

void terminal_switch(uint32_t terminal_id){
	cli();
	int i;
	uint32_t vir_add = 0x10000000; /* virtual address 256MB*/
	uint32_t vid_add = 0xB8000; /* physcial address video memory */
	/* getting the new terminal structure */
	//curr_terminal = terminal_id; 

	/* step 1: copying current video memory into corresponding terminal buffer */
	memcpy((void*)terminal_vid_buf[curr_terminal], (void*)0xB8000, four_kb);
	/* step 2: switching out all old terminal processes to terminal buffer */
	for (i = 0; i < 6; i++){
		if (terminals[curr_terminal].pros_pids[i] == 1){ /* case the ith process is in this terminal*/
			uint32_t pd_add = (uint32_t)(&processes_page_dir[i]); /* page directory address */
			uint32_t pt_add = (uint32_t)(&vidmap_page_table[i]); /* page table address */
			map_4kb_page(i, vir_add, terminal_vid_buf[terminal_id], 1, pd_add, pt_add, 1); /* mapping to the buffer */
		}
	}

	/* step 3: copying new terminal buffer into video memory */
	memcpy((void*)0xB8000, (void*)terminal_vid_buf[terminal_id], four_kb);
	/* step 4: copying future video memory into the video memory address */
	for(i = 0; i < 6; i ++){
		if (terminals[terminal_id].pros_pids[i] == 1){ /* case the ith process is in the future terminal */
			uint32_t pd_add = (uint32_t)(&processes_page_dir[i]); /* page directory address */
			uint32_t pt_add = (uint32_t)(&vidmap_page_table[i]); /* page table address */
			map_4kb_page(i, vir_add, vid_add, 1, pd_add, pt_add, 1); /* mapping to the buffer */
		}
	}

	curr_terminal = terminal_id; 


	sti();
}


