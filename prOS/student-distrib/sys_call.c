#include "lib.h"
#include "sys_call.h"

/* Description:
 * system call halt.
 *
 * 
 */
int32_t halt(uint8_t status){
	return 0;
}



/* Description:
 * system call execute.
 *
 * 
 */
int32_t execute(const uint8_t* command){
	return 0;
}

/* Description:
 * system call read.
 *
 * 
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
	return 0;
}


/* Description:
 * system call write.
 *
 * 
 */
int32_t write(int32_t fd, void* buf, int32_t nbytes){
	return 0;
}


/* Description:
 * system call open.
 *
 * 
 */
int32_t open(const uint8_t* filename){
	return 0;
}


/* Description:
 * system call close.
 *
 * 
 */
int32_t close(int32_t fd){
	return 0;
}


/* Description:
 * system call getargs.
 *
 * 
 */
int32_t getargs(uint8_t* buf, int32_t nbytes){
	return 0;
}


/* Description:
 * system call vidmap.
 *
 * 
 */
int32_t vidmap(uint8_t** screen_start){
	return 0;
}


/* Description:
 * system call set_handler.
 *
 * 
 */
int32_t set_handler(int32_t signum, void* handler_address){
	return 0;
}


/* Description:
 * system call sigreturn.
 *
 * 
 */
int32_t sigreturn(void){
	return 0;
}

/* Description:
 * Handler for the system call. This should be a jump table
 *
 * Exception Class:
 *
 *
 * Exception Error Code:
 * 
 *
 * Saved Instruction Pointer:
 * 
 */
void sys_call_handler(){
	asm("pushal");
	printf("system call handle!!\n");
	int temp;
	temp=execute("ls");
	asm("popal;leave;iret");
}
