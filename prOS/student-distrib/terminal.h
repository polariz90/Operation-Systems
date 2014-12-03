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
#define DOWNP			80
#define ALTP			56
#define Lc 				46
#define F1P				59
#define F2P				60
#define F3P				61
#define ALTR			184


#define CAPS_CONV		0x20
#define his_buff_size 	50
#define magic_fd        -2

#define term1_vid_buf   0xB9000
#define term2_vid_buf   0xBA000
#define term3_vid_buf   0xBB000



/*Decleared variables for the terminal*/
uint32_t curr_terminal;				/*current index into buffer to be written*/

typedef struct 
{
	uint8_t match; /* indicate wheather this node matches or not */
	uint32_t size; /* given the size of array stored in this node */
	char arr_[32];  
}history_node;

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

/* terminal buffer which store the status for the specific terminal */
typedef struct
{
	history_buffer terminal_history; /* store history of the terminal */
	char buf[BUF_SIZE];
	uint32_t xloc;
	uint32_t yloc;
	uint32_t caps;
	uint32_t size;
	uint32_t shift;						/*0 if not pressed, one if pressed*/
	uint32_t ctrl; 						/*0 if not pressed, one if pressed*/
	uint32_t alt;
	volatile uint8_t reading;
	uint32_t pros_pids[6];      /* bit map for the process in the terminal */

}terminal_buffer;

extern terminal_buffer terminals[3];
extern uint32_t terminal_vid_buf[3];

extern void * stdin_opt[4];
extern void * stdout_opt[4];


/** add_to_history
  * 	function which add current command into the 
  * terminal_history buffer, return nothing, always success
  */
void add_to_history(char* buffer, uint32_t terminal_idx);

 /* Opens the terminal
  * Initializes important variables
  *	returns 0
  */ 
int terminal_open();

/* Reads count bytes from the terminal 
 * returns number of bytes sucessfully read
 */ 

int terminal_read(int32_t fd, char *buf, int32_t count );

/* Writes count bytes to the buffer 
  * returns number of bytes written to the terminal
  */ 
int terminal_write(int32_t fd, char *buf, int32_t count );



/* Clears the buffer and closes it
  *  returns 0
  */ 
int terminal_close();


/* terminal print
 * takes a char prints to the current screen location
 * returns nothing
 */
void printt(char c);


/* terminal print hex
 * takes a char prints to the current screen location
 * returns nothing
 */
void printt_hex(char c);

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
void toggle_alt();
void toggle_ctrl(); 

/*stdin/stdout useless functions */
int stdin_write();

int stdout_read();

/* hard code node history for current file image */
void creating_node_history();

/* looking for a tap match */
void find_tap_match(const int8_t* buf);


/* functions to help switching terminals */
void terminal_switch(uint32_t terminal_id);





#endif /* _IDT_H  */
