#include "lib.h"
#include "sys_call.h"
#include "file.h"
#include "x86_desc.h"


#define space_char 32
#define vir_mem_add 0x08000000
#define phy_mem_add 0x800000
#define four_mb 0x200000

/* array to keep in check of process number */
uint32_t occupied[7] = {0,0,0,0,0,0,0};

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

	/* getting new pid for processes */
	int pid = get_next_pid();
	if(pid == -1){
		return -1; /* fail execute */
	}

	/*Parse*/
	uint8_t com_arr[128];
	uint8_t arg_arr[128];
	int i; /* loop counter */
	/* special case check */
	if(*command == NULL){
		/* case empty string */
		return -1;
	}
	if(*command == space_char){
		/* case single space string */
		return -1; 
	}
	i = 0;
	while(*command != space_char){/* copying command */
		com_arr[i] = *command;
		command ++;
	}
	i = 0;
	while(*command != '\n'){/* copying argument */
		arg_arr[i] = *command;
		command ++;
	}

	/*Excutable check*/
	uint8_t buf[four_kb];
	read_file_img(com_arr, buf);
	uint8_t ELF[4];
	ELF[0]=0x7f;
	ELF[1]=0x45;
	ELF[2]=0x4c;
	ELF[3]=0x46;
	if(strncmp((uint8_t*)buf, (uint8_t*)ELF, 4)) {printf("not Excutable!!\n");}

	/*Paging*/
	map_4KB_page(pid, vir_mem_add, phy_mem_add+(pid-1)*four_mb, 1);

	/*File loader*/
	load_file_img(com_arr);

	/*new PCB*/
	pcb* new_pcb = add_process_stack(pid);

	/* filling PCB with stuff */
	new_pcb->pid = pid;
	strcpy((int8_t*)new_pcb->arg, (int8_t*)arg_arr);


	/*context switch*/
	uint32_t entry_point;
	memcpy(&entry_point, buf+23, 4);
	//printf("%s\n", entry_point);
	asm("pushal");

	//asm("movl %eip, %eax\n\t" "pushl %eax\n\t");
	//asm("pushl %%eax" : : : "eax");
	asm volatile("pushl %%ebx        \n      \
			pushl %%ecx        \n      \
			pushl %%edx        \n      \
			pushl $0x00c00000        \n      \
			pushl %%ecx"                    \
			: 
			: "b"(entry_point), "c"(USER_CS), "d"(tss.eflags)
			: "eax" );

	asm("iret");
/*	asm("pushl %%ebx	;
		pushl %%ebx"
		 : :  : "eax" );
*/
	asm("popal");

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
	int32_t temp;
	temp = execute("ls");
	asm("popal;leave;iret");
}

/**
  * get next pid
  * INPUT: none
  * OUTPUT: pid for the new process
  * SIDE EFFECT: none
  */
uint32_t get_next_pid(void){
	int i = 0; /* loop counter */
	while(i < 7){
		if(occupied[i] == 0){/* case avaliable*/
			occupied[i] = 1;
			return i;
		}
		else{
			i++;
		}
	}
	return -1;
}