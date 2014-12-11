#ifndef _SYS_CALL_H
#define _SYS_CALL_H

#include "types.h"
#include "file.h"
#ifndef ASM_SYS

typedef struct 
{
	uint8_t 	occupied[NUM_PROCESSES];
	uint32_t 	num_process;
	uint8_t 	top_process_flag[NUM_PROCESSES];
}process_arr;


//extern uint32_t occupied[7];
extern process_arr process_occupy;

extern int32_t halt(uint8_t status);
extern int32_t execute(const uint8_t* command);
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t write(int32_t fd, void* buf, int32_t nbytes);
extern int32_t close(int32_t fd);
extern int32_t getargs(uint8_t* buf, int32_t nbytes);
extern int32_t vidmap(uint8_t** screen_start);
extern int32_t set_handler(int32_t signum, void* handler_address);
extern int32_t sigreturn(void);
extern uint32_t malloc(uint32_t size);
	   uint32_t get_next_pid(int8_t* buf); 
	   void release_cur_pid(uint32_t pid);


#endif /* ASM_SYS*/
#endif






