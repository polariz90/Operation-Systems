#include "lib.h"
#include "sys_call.h"
#include "file.h"
#include "x86_desc.h"
#include "assembly_ops.h"
#include "page.h"
#include "terminal.h"


#define space_char 32
#define vir_mem_add 0x08000000
#define phy_mem_add 0x800000
#define four_mb 0x400000
#define eight_mb 0x800000
#define eight_kb 0x2000


/* array to keep in check of process number */
uint32_t occupied[7] = {0,0,0,0,0,0,0};
uint32_t entry_point;
pcb* kernel_pcb_ptr;

/* Description:
 * system call halt.
 *	DESCIPTION: called in the end of a process, restoring parent process's
 * ESP/EBP in order to switch to parent's stack, and returning from parent's 
 * stack which is right after iret in execute function. 
 * 
 */
int32_t halt(uint8_t status){
	/*restore parent's esp/ebp and anything else you need*/
	pcb* current_pcb = getting_to_know_yourself(); /* geeting current pcb*/

	/*may want to prevent user to close the last shell*/

	/* set TSS back to point at parent's kernel stack */
	tss.esp0 = eight_mb - (eight_kb*current_pcb->parent_pid) - 4;

	int parent_page = current_pcb->parent_page_dir_ptr;

	/* reset current processes mask for other process use */
	occupied[current_pcb->pid] = 0;

		/*restore parent's paging*/
	asm(
		"movl %%eax, %%cr3			;"
		: 
		: "a"(parent_page) 
		: "memory","cc"
		);

	/* transfer back to parent stack */
	asm(
		"movl %%eax, %%esp 			;"
		"movl %%ebx, %%ebp 			;"
		"pushl %%ecx 				;"
		:
		: "a"(current_pcb->parent_esp), "b"(current_pcb->parent_ebp), "c"(status)
		: "memory", "cc"
		);

		stil();
	asm volatile(
				"movl $0,  %%eax;"
				"leave			;"
				"ret 			;"
				: : :"eax","memory","cc"
				);

	return 0;

}




/* Description:
 * system call execute.
 *	execute system called when a new user process is created 
 * 	it going through steps as : parse --> executable --> paging
 *				--> new PCB --> context switch
 * in the end of execute function, we will switch to user code program
 * which are operating in user level with new set of page directories. 
 */
int32_t execute(const uint8_t* command){
	int i,j; /* loop counter */
	/* getting new pid for processes */
	int pid = get_next_pid();
	if(pid == -1){
		
		asm("movl $-1, %eax");
		asm("leave;ret"); /* fail execute */
	}

	printf("command input : %s\n", command);

	/*Parse*/
	uint8_t com_arr[128];
	uint8_t arg_arr[128];
	/* special case check */
	if(command == NULL){
		/* case empty string */
		
		asm("movl $-1, %eax");
		asm("leave;ret");
	}
	if(command[0] == space_char){
		/* case single space string */
		
		asm("movl $-1, %eax");
		asm("leave;ret");
	}
	i = 0;
	while(!((command[i] == space_char)||(command[i] == NULL))){/* copying command */
		com_arr[i] = command[i];
		i++;
	}
	com_arr[i] = '\0';
	j = 0; i++;
	while(command[i] != NULL){/* copying argument */
		arg_arr[j] = command[i];
		i++; j++;
	}
	arg_arr[j] = '\0';

	printf("filename: %s\n", com_arr);

	/*Excutable check*/
	uint8_t buf[100];
	read_file_img((int8_t*)com_arr,(uint8_t*) buf, 100);
	uint8_t ELF[4];
	ELF[0]=0x7f;
	ELF[1]=0x45;
	ELF[2]=0x4c;
	ELF[3]=0x46;
	if(strncmp((int8_t*)buf, (int8_t*)ELF, (uint32_t)4)){
		printf("not Excutable!!\n");
		
		asm("movl $-1, %eax");
		asm("leave;ret");
	}
	else{
		printf("this is executable\n");
	}

	/*Paging*/
	uint32_t parent_pcb;
	asm volatile("movl %%cr3, %%eax "              \
			: "=a"(parent_pcb)
			: 
			: "memory", "cc" );
	map_4KB_page(pid, vir_mem_add, phy_mem_add+(pid-1)*four_mb, 1);
	/* paging test */


	/*File loader*/
	if(load_file_img((int8_t*)com_arr) == -1){
		
		asm("movl $-1, %eax");
		asm("leave;ret");
	}


	/*new PCB*/
	pcb* new_pcb = add_process_stack(pid); /* new PCB for new process */

	pcb* current_pcb = getting_to_know_yourself(); /* PCB at current process */


	/* filling PCB with stuff */
	new_pcb->pid = pid; /* saving new process pid */
	strcpy((int8_t*)new_pcb->arg, (int8_t*)arg_arr); /* saving new process arg */
	uint32_t parent_esp, parent_ebp;
	asm volatile("movl %%esp, %0" : "=g"(parent_esp));
	new_pcb->parent_esp = parent_esp; /* save parent esp */
	asm volatile("movl %%ebp, %0" : "=g"(parent_ebp));
	new_pcb->parent_ebp = parent_ebp; /* save parent ebp */
	new_pcb->parent_pid = current_pcb->pid; /* loading parent pid */

	//new_pcb->parent_eip=(uint32_t)tss.eip;
	new_pcb->parent_page_dir_ptr= (void*)parent_pcb; /**/

	/*context switch*/

	int temp = eight_kb*pid;
	tss.esp0= eight_mb - temp - 4;
	tss.ss0= KERNEL_DS;

	/*getting entry_point from file image*/
	memcpy(&entry_point, buf+24, 4);



	uint32_t eflag = 0;
	cli_and_save(eflag);
	sti();

	asm volatile(
			"pushl %%eax     		;"
			"pushl $0x083FFFF0   	;"  
			"pushl %%edx      		;"		
			"pushl %%ecx      		;"
			"pushl %%ebx 			;"		
			"movw %%ax, %%ds  		;"
			"movw %%ax, %%gs  		;"
			"movw %%ax, %%fs  		;"
			"movw %%ax, %%es  		;"
			: 
			: "b"(entry_point), "d"(eflag|0x4200), "c"(USER_CS), "a"(USER_DS) 
			: "memory", "cc" );


	asm ("iret");

	asm("movl $0, %eax");
	asm("leave;ret");
}


void test_execute(){
	int i = 0;
	execute("shell abc");
}

/* Description:
 * system call read.
 *	Read system call: passing in fd with read buffer and number of bytes need to 
 * be read. return number of bytes that read, or return 0 when reach the end of 
 * of the file
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
	sti();

	uint32_t fun_addr=(uint32_t)kernel_pcb_ptr->file_descriptor[fd].file_opt_ptr[1];
	asm volatile("pushal \n \
		pushl %%ebx \n \
		pushl %%eax \n \
		call %%ecx	\n \
		addl $8, %%esp"
		:
		: "a"(buf), "b"(nbytes), "c"(fun_addr)
		: "cc", "memory");

	/*return 0*/
	asm("leave;ret");
}


/* Description:
 * system call write.
 *  Write system call: passing in fd with buffer and number of bytes need to 
 * be write. Return number of bytes that write to the terminal. 
 */
int32_t write(int32_t fd, void* buf, int32_t nbytes){


	uint32_t fun_addr=(uint32_t)kernel_pcb_ptr->file_descriptor[fd].file_opt_ptr[2];
	asm volatile("pushal \n \
		pushl %%ebx \n \
		pushl %%eax \n \
		call %%ecx	\n \
		addl $8, %%esp"
		:
		: "a"(buf), "b"(nbytes), "c"(fun_addr)
		: "cc", "memory");

	/*return 0*/
	asm("leave;ret");
}


/* Description:
 * system call open.
 *  Open system call: passing in with a filename, and allocate fd location for the 
 * file, if fd is full, return -1. else return 0 with side effect of a functional 
 *  file descriptor 
 */
int32_t open(const uint8_t* filename){
	if(!strncmp(filename, "terminal", 9)){
	}
	
	asm("movl $0, %eax");  //comment this line after add the function
	asm("leave;ret");
}


/* Description:
 * system call close.
 *   Close system call: passing in with fd, close the corresponding fd for the file
 * let it free to be used by other process. return -1 for fail operation, and return 
 * 0 when success
 */
int32_t close(int32_t fd){
	
	asm("movl $0, %eax");  //comment this line after add the function
	asm("leave;ret");
}


/* Description:
 * system call getargs.
 *  Getarg system call: 
 * 
 */
int32_t getargs(uint8_t* buf, int32_t nbytes){
	
	asm("movl $0, %eax");  //comment this line after add the function
	asm("leave;ret");
}


/* Description:
 * system call vidmap.
 *
 * 
 */
int32_t vidmap(uint8_t** screen_start){
	
	asm("movl $0, %eax");  //comment this line after add the function
	asm("leave;ret");
}


/* Description:
 * system call set_handler.
 *
 * 
 */
int32_t set_handler(int32_t signum, void* handler_address){
	
	asm("movl $0, %eax"); //comment this line after add the function
	asm("leave;ret");
}


/* Description:
 * system call sigreturn.
 *
 * 
 */
int32_t sigreturn(void){
	

	asm("movl $0, %eax"); //comment this line after add the function
	asm("leave;ret");
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
//	asm("pushal");
	printf("system call handle!!\n");
	int32_t temp;
	temp = execute("testprint arg");
	printf("execute finished, and returned into the wrong palce \n");
//	asm("leave;iret");
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
