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
//#define four_mb 0x200000
#define four_mb 0x400000
#define eight_mb 0x800000
#define eight_kb 0x8000


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

	asm(
		"movl %%eax, %%esp 			;"
		"movl %%ebx, %%ebp 			;"
		:
		: "a"(current_pcb->parent_esp), "b"(current_pcb->parent_ebp)
		: "memory", "cc"
		);


	/*restore parent's paging*/
	asm(
		"movl %%eax, %%cr3			;"
		: 
		: "a"(current_pcb->parent_page_dir_ptr) 
		: "memory","cc"
		);

	/* reset current processes mask for other process use */
	occupied[current_pcb->pid] = 0;

	asm volatile(
				"movl $0,  %%eax;"
				"leave			;"
				"ret 			;"
				: : :"eax","memory","cc"
				);

//	asm("movl $0, %eax");
//	asm("iret");
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
	com_arr[i] = '\0';
	j = 0; i++;
	while(command[i] != NULL){/* copying argument */
		arg_arr[j] = command[i];
		i++; j++;
	}
	arg_arr[j] = '\0';

	printf("filename: %s\n", com_arr);

	/*Excutable check*/
	uint8_t buf[four_kb];
	read_file_img((int8_t*)com_arr,(uint8_t*) buf);
	uint8_t ELF[4];
	ELF[0]=0x7f;
	ELF[1]=0x45;
	ELF[2]=0x4c;
	ELF[3]=0x46;
	if(strncmp((int8_t*)buf, (int8_t*)ELF, (uint32_t)4)){
		printf("not Excutable!!\n");
		return -1;
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
		return -1;
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
	//tss.prev_task_link=KERNEL_TSS;
	//tss.eflags = 0x00004000;
	//tss_desc_ptr.dpl=0x3;


	printf("esp0: %x\n", tss.esp0);
	printf("ss0: %x\n", tss.ss0);


	memcpy(&entry_point, buf+24, 4);
	printf("entry point: %x\n", entry_point);

//	test_out(entry_point);


	uint32_t eflag = 0;
	cli_and_save(eflag);
//	restore_flags(eflag|0x00004000);

	//sti();



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

	return 0;
}


void test_execute(){
	int i = 0;
	execute("shell abc");
}

/* Description:
 * system call read.
 *
 * 
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
	sti();
	int bytesread;
	int pid = get_next_pid();

	pcb * new_pcb = add_process_stack(pid);

	if( fd < 0 || fd > 7 || buf == NULL || new_pcb->file_descriptor[fd].flags == 0 )
	{
			return -1;
	}

/*	uint8_t* filename = new_pcb->filenames[fd];
	uint32_t fileposition = new_pcb->file_descriptor[fd].file_pos;

	asm volatile("pushl %0          ;"
				 "pushl %1              ;"
				 "pushl %2              ;"
				 "pushl %3              ;"
				 "call  *%4             ;"
				 :
				 : "g" (fileposition), "g" ((int32_t)filename), "g" (nbytes), "g" ((int32_t)buf),
				   "g" (new_pcb->file_descriptor[fd].file_opt_ptr[1]));
							 
	asm volatile("movl %%eax, %0":"=g"(bytesread));
	asm volatile("addl $16, %esp    ;");

	new_pcb->file_descriptor[fd].file_pos += bytesread;
*/	
	return bytesread;
}


/* Description:
 * system call write.
 *
 * 
 */
int32_t write(int32_t fd, void* buf, int32_t nbytes){
/*	int pid = get_next_pid();
	pcb * new_pcb = add_process_stack(pid);
	
	if( fd < 0 || fd > 7 || buf == NULL || new_pcb->filedescriptor[fd].flags == 0 )
	{
			return -1;
	}


	asm volatile("pushl %0          ;"
				 "pushl %1              ;"
				 "call  *%2             ;"
				 "leave					;"
				 "ret					;"
				 :
				 : "g" (nbytes), "g" ((int32_t )buf), "g" (new_pcb->filedescriptor[fd].file_opt_ptr[2]));
*/	
	kernel_pcb_ptr->file_descriptor[1].file_opt_ptr[2]=terminal_write;
	kernel_pcb_ptr->file_descriptor[1].flags=USED;
	uint32_t fun_addr=(uint32_t)kernel_pcb_ptr->file_descriptor[fd].file_opt_ptr[2];
	asm volatile("pushal \n \
		pushl %%ebx \n \
		pushl %%eax \n \
		call %%ecx"
		:
		: "a"(buf), "b"(nbytes), "c"(fun_addr)
		: "cc", "memory");
	

	return 0;
}


/* Description:
 * system call open.
 *
 * 
 */
int32_t open(const uint8_t* filename){
	//asm("pushal");
	if(!strncmp(filename, "terminal", 9)){
		printf("get terminal argument\n");
		//kernel_pcb_ptr->file_descriptor[1].file_opt_ptr=stdout_ops;
		kernel_pcb_ptr->file_descriptor[1].flags=USED;
	}	
	//asm("popal;leave;iret");
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
//	asm("pushal");
	printf("system call handle!!\n");
	int32_t temp;
	temp = execute("testprint arg");
	printf("execute finished, and returned into the wrong palce \n");
//	asm("popal;leave;iret");
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
