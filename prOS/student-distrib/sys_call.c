#include "lib.h"
#include "sys_call.h"
#include "file.h"
#include "x86_desc.h"


#define space_char 32
#define vir_mem_add 0x08000000
#define phy_mem_add 0x800000
//#define four_mb 0x200000
#define four_mb 0x400000
#define eight_mb 0x800000
#define eight_kb 0x8000

/* array to keep in check of process number */
uint32_t occupied[7] = {1,0,0,0,0,0,0};
uint32_t entry_point;


/* Description:
 * system call halt.
 *
 * 
 */
int32_t halt(uint8_t status){
	asm("movl $0, %eax");
	asm("iret");
}



/* Description:
 * system call execute.
 *
 * 
 */
int32_t execute(const uint8_t* command){
	int i,j; /* loop counter */
	/* getting new pid for processes */
	int pid = get_next_pid();
	if(pid == -1){
		return -1; /* fail execute */
	}

	/*Parse*/
	uint8_t com_arr[128];
	uint8_t arg_arr[128];
	/* special case check */
	if(command == NULL){
		/* case empty string */
		return -1;
	}
	if(command[0] == space_char){
		/* case single space string */
		return -1; 
	}
	i = 0;
	while(command[i] != space_char){/* copying command */
		com_arr[i] = command[i];
		i++;
	}
	com_arr[i+1] = '\0';
	j = 0; i++;
	while(command[i] != NULL){/* copying argument */
		arg_arr[j] = command[i];
		i++; j++;
	}
	arg_arr[j+1] = '\0';


	/*Excutable check*/
	uint8_t buf[four_kb];
	read_file_img(com_arr, buf);
	uint8_t ELF[4];
	ELF[0]=0x7f;
	ELF[1]=0x45;
	ELF[2]=0x4c;
	ELF[3]=0x46;
	if(strncmp((uint8_t*)buf, (uint8_t*)ELF, 4)){
		printf("not Excutable!!\n");
		return -1;
	}
	else{
		printf("this is executable\n");
	}

	/*Paging*/
	map_4KB_page(pid, vir_mem_add, phy_mem_add+(pid-1)*four_mb, 1);

	/*File loader*/
	if(load_file_img(com_arr) == -1){
		return -1;
	}

	/*new PCB*/
	pcb* new_pcb = add_process_stack(pid);

	/* filling PCB with stuff */
	new_pcb->pid = pid;
	strcpy((int8_t*)new_pcb->arg, (int8_t*)arg_arr);
	new_pcb->parent_eip=tss.eip;

	/*context switch*/
	//set up tss.esp0, ss0
	tss.esp0= eight_mb- eight_kb -4;
	tss.ss0= KERNEL_DS;

	printf("esp0: %x\n", tss.esp0);
	printf("ss0: %x\n", tss.ss0);

	//uint32_t entry_point;

	for(i=0;i<10;i++)
		printf("%d: 0x%x\n", i, buf[20+i]);
	memcpy(&entry_point, buf+24, 4);
	printf("entry point: %x\n", entry_point);
	//asm("pushal");

	//asm("movl %eip, %eax\n\t" "pushl %eax\n\t");
	//asm("pushl %%eax" : : : "eax");
	asm volatile("pushl %%eax        \n      \
			pushl $0x083FFFFC        \n      \
			pushl %%edx        \n      \
			pushl %%ecx        \n      \
			pushl %%ebx"                    \
			: 
			: "b"(entry_point), "c"(USER_CS), "d"(tss.eflags|0x00004000), "a"(USER_DS) 
			: "memory", "cc" );

	asm("iret");
/*	asm("pushl %%ebx	;
		pushl %%ebx"
		 : :  : "eax" );
*/
	//asm("popal");

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
	int i;
	i=0;

	printf("system call handle!!\n");
	int32_t temp;
	temp = execute("shell arghaha");
	printf("execute finished, and returned into the wrong palce \n");
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