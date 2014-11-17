#ifndef _SYS_CALL_H
#define _SYS_CALL_H

#include "types.h"
#include "file.h"
#ifndef ASM_SYS


extern uint32_t occupied[7];
extern pcb* kernel_pcb_ptr;


extern void sys_call_handler();

extern int32_t halt(uint8_t status);
extern int32_t execute(const uint8_t* command);
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t write(int32_t fd, void* buf, int32_t nbytes);
extern int32_t close(int32_t fd);
extern int32_t getargs(uint8_t* buf, int32_t nbytes);
extern int32_t vidmap(uint8_t** screen_start);
extern int32_t set_handler(int32_t signum, void* handler_address);
extern int32_t sigreturn(void);
	   uint32_t get_next_pid(void); 

#endif /* ASM_SYS*/
#endif






