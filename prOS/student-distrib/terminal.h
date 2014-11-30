#ifndef _TERMINAL_H
#define _TERMINAL_H

/* Header file for the terminal driver functions
  * 
  */ 
#include "lib.h"

/*Defined constants*/
#define	BUF_SIZE   		128
#define LTABP	    	15 
#define LTABR 			143
#define CAPP	     	58 
#define CAPR	     	186 
#define LSHFTP	  	 	42
#define LSHFTR	   		170 
#define BSP	       		14 
#define BSR 	     	142
#define ENTP 	    	28
#define ENTR 		    156
#define CTLP	     	29
#define CTLR	     	157
#define RSHFTP 	   		54
#define RSHFTR	   		182
#define Lp			    38 
#define UPP 	        72
#define UPR     	    200

#define CAPS_CONV		0x20
#define his_buff_size 	100



/*Decleared variables for the terminal*/
uint32_t curr_terminal;				/*current index into buffer to be written*/


typedef struct
{
	char buf[BUF_SIZE];
	uint32_t xloc;
	uint32_t yloc;
	uint32_t caps;
	uint32_t size;
	uint32_t shift;						/*0 if not pressed, one if pressed*/
	uint32_t ctrl; 						/*0 if not pressed, one if pressed*/
	volatile uint8_t reading;
}terminal_buffer;

extern terminal_buffer terminals[3];


extern void * stdin_opt[4];
extern void * stdout_opt[4];

/*structure to hold specific command */
typedef	struct 
{
	char cmd[128]; 
}cmd_line;

/*history buffer, max 20 buffers */
typedef struct 
{
	uint32_t begin; /* point at oldest command */
	uint32_t end;  /* point at newest command */
  uint32_t current; /* keep in track of the current history position*/
	uint32_t pre_pos; /* keep in track of previous position */
	cmd_line command[his_buff_size]; /* array to store all commands */
}history_buffer;

extern history_buffer terminal_history;


/** add_to_history
  * 	function which add current command into the 
  * terminal_history buffer, return nothing, always success
  */
void add_to_history(char* buffer);

 /* Opens the terminal
  * Initializes important variables
  *	returns 0
  */ 
int terminal_open( );

/* Reads count bytes from the terminal 
 * returns number of bytes sucessfully read
 */ 
int terminal_read(int8_t* filename, char *buf, int32_t count );

/* Writes count bytes to the buffer 
  * returns number of bytes written to the terminal
  */ 
int terminal_write(char *buf, int32_t count );



/* Clears the buffer and closes it
  *  returns 0
  */ 
int terminal_close();


/* terminal print
 * takes a char prints to the current screen location
 * returns nothing
 */
void printt(char c);

/* 	Simple function that has a logic to check if the key pressed is a special case that should not be printed to the screen
 *
 *	returns 1 of it is a special key 0 otherwise
 */
int is_special_key(int key);


/*	This function executes the function that the special key denotes when called
 * 	returns nothing
 */
void exe_special_key(int key);


void new_line();


/* these function simply toggle the vaule of the keys when called*/
void toggle_caps();
void toggle_shift();
void toggle_ctrl(); 

/*stdin/stdout useless functions */
int stdin_write();

int stdout_read();







#endif /* _IDT_H  */
