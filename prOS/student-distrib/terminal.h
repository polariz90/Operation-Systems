#ifndef _TERMINAL_H
#define _TERMINAL_H

/* Header file for the terminal driver functions
  * 
  */ 
#include "lib.h"



/*Defined constants*/
#define	BUF_SIZE 	128
#define LTABP		15 
#define LTABR 		143
#define CAPP		58 
#define CAPR		186 
#define LSHFTP		42
#define LSHFTR		170 
#define BSP	    	14 
#define BSR 		142
#define ENTP 		28
#define ENTR 		156
#define CTLP		29
#define CTLR		157
#define RSHFTP 		54
#define RSHFTR		182
#define Lp			38 

#define CAPS_CONV	0x20



/*Decleared variables for the terminal*/
int curr_terminal_loc;
char terminal_buffer[BUF_SIZE];
uint8_t caps;
uint8_t shift;
uint8_t ctrl; 








/* Opens the terminal
  *
  *
  */ 
int terminal_open( );

/* Reads count bytes from teh terminal 
  *
  *
  */ 
int terminal_read(char *buf, uint32_t count );

/* Writes count bytes to the buffer 
  *
  *
  */ 
int terminal_write(char *buf, uint32_t count );



/* Clears the buffer and closes it
  *
  * Should return 0
  */ 
int terminal_close();

int write_buf_to_screen();

int is_special_key(int key);

void exe_special_key(int key);

void toggle_caps();
void toggle_shift();
void toggle_ctrl(); 










#endif /* _IDT_H  */
