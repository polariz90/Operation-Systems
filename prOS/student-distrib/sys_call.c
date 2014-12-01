#include "lib.h"
#include "sys_call.h"
#include "file.h"
#include "x86_desc.h"
#include "assembly_ops.h"
#include "page.h"
#include "terminal.h"
#include "rtc.h"


#define space_char 32
#define vir_mem_add 0x08000000
#define phy_mem_add 0x800000
#define four_mb 0x400000
#define eight_mb 0x800000
#define eight_kb 0x2000
#define size_of_occupied 7
#define buffer_size 128


/* array to keep in check of process number */
uint32_t occupied[7] = {0,0,0,0,0,0,0};
uint32_t entry_point;
uint8_t ELF[4]={0x7f, 0x45, 0x4c, 0x46};
//ELF[0]=0x7f; /* executable check for magic number ELF*/
//ELF[1]=0x45;
//ELF[2]=0x4c;
//ELF[3]=0x46;


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

	int parent_page = (int)current_pcb->parent_page_dir_ptr;

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

		sti();

	asm volatile(
				"movl $0,  %%eax;"
				"leave			;"
				"ret 			;"
				: : :"eax","memory","cc"
				);

	return status;

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

		printf("Warning!!! Reaching maximum process capacity!! Calm Down Pls!!!!\n");
		
		asm("movl $-1, %eax");
		asm("leave;ret"); /* fail execute */

	}

//	printf("command input : %s\n", command);

	/*Parse*/
	uint8_t com_arr[buffer_size];
	uint8_t arg_arr[buffer_size];
	/* special case check */

	if(command == NULL){
		/* case empty string */
		occupied[pid]=0;
		asm("movl $-1, %eax");
		asm("leave;ret");
	}
	if(command[0] == space_char){
		/* case single space string */
		occupied[pid]=0;
		asm("movl $-1, %eax");
		asm("leave;ret");
	}

	i = 0;
	while(!((command[i] == space_char)||(command[i] == '\0'))){/* copying command */
		com_arr[i] = command[i];
		i++;
	}
	com_arr[i] = '\0';
	j = 0; i++;

	while(command[i] != '\0'){/* copying argument */
		arg_arr[j] = command[i];
		i++; j++;
	}
	arg_arr[j] = '\0';

	/* checking special commands */
	if(strncmp(com_arr, "clear", 5) == 0){ /* clear screen command */
		clear();
		occupied[pid]=0;
		//asm("movl $-1, %eax");
		//asm("leave;ret");
		return 0;
	}

	/*Excutable check*/
	uint8_t buf[buffer_size];
	if(read_file_img((int8_t*)com_arr,(uint8_t*) buf, buffer_size) == -1){
		occupied[pid]=0;
		asm("movl $-1, %eax");
		asm("leave;ret");
	}

	if(strncmp((int8_t*)buf, (int8_t*)ELF, (uint32_t)4)){
//		printf("not Excutable!!\n");

		occupied[pid]=0;
		asm("movl $-1, %eax");
		asm("leave;ret");
	}
	else{
//		printf("this is executable\n");

	}

	/*Paging*/
	uint32_t parent_pcb;
	asm volatile("movl %%cr3, %%eax "               \
			: "=a"(parent_pcb)
			: 
			: "memory", "cc" );
	change_process_page(pid, vir_mem_add, phy_mem_add+(pid-1)*four_mb, 1);
	/* paging test */


	/*File loader*/
	if(load_file_img((int8_t*)com_arr) == -1){
		occupied[pid]=0;
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

	return -1;

}

/**
  * test function to call execute system call
  */
void test_execute(){
	execute((uint8_t*)"shell abc");
}

/* Description:
 * system call read.
 *	Read system call: passing in fd with read buffer and number of bytes need to 
 * be read. return number of bytes that read, or return 0 when reach the end of 
 * of the file
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
	if(fd < 0 || fd > 7){
		asm("movl $-1, %eax");
		asm("leave;ret");
	}
	sti();

	pcb* current_pcb = getting_to_know_yourself(); /* geeting current pcb*/

	uint32_t fun_addr=(uint32_t)current_pcb->file_descriptor[fd].file_opt_ptr[1];

	asm volatile("pushal \n \
		pushl %%ebx \n \
		pushl %%eax \n \
		pushl %%edx \n \
		call  %%ecx	\n \
		addl $12, %%esp"
		:
		: "a"(buf), "b"(nbytes), "c"(fun_addr), "d"(fd)
		: "cc", "memory");

	/*return 0*/
	asm("leave;ret");

	//will never get here, stops compiler warnings 
	return 0;
}


/* Description:
 * system call write.
 *  Write system call: passing in fd with buffer and number of bytes need to 
 * be write. Return number of bytes that write to the terminal. 
 */
int32_t write(int32_t fd, void* buf, int32_t nbytes){
	if (fd < 0 || fd > 7){
		asm("movl $-1, %eax");
		asm("leave;ret");
	}

	pcb* current_pcb = getting_to_know_yourself(); /* geeting current pcb*/

	uint32_t fun_addr=(uint32_t)current_pcb->file_descriptor[fd].file_opt_ptr[2];
	asm volatile("pushal \n \
		pushl %%ebx \n \
		pushl %%eax \n \
		pushl %%edx \n \
		call *%%ecx	\n \
		addl $12, %%esp"
		:
		: "a"(buf), "b"(nbytes), "c"(fun_addr),"d"(fd)
		: "cc", "memory");

	/*return 0*/
	asm("leave;ret");

	//will never get here, stops compiler warnings 
	return 0;
}


/* Description:
 * system call open.
 *  Open system call: passing in with a filename, and allocate fd location for the 
 * file, if fd is full, return -1. else return 0 with side effect of a functional 
 *  file descriptor 
 */
int32_t open(const uint8_t* filename){
	/*don't need this if user never call open(terminal)*/
	if(!strncmp((int8_t*)filename,(int8_t*) "terminal", 9)){
	//	printf("get terminal argument\n");
	}
	//printf("in the open sys call*************\n");

	pcb* current_pcb = getting_to_know_yourself(); /* geeting current pcb*/


	int i, j;	/* loop counter */
	uint32_t num_entries = s_block->dir_entries; /* variable hold # of entries */
	uint32_t length; /* variable to hold filename string length */
	length = strlen((int8_t*)filename);	

	for(i = 0; i < num_entries; i++){ /* looping through entire entries to find file*/
		if(strlen(s_block->file_entries[i].filename) == length){/* case 2 names doesnt have the same length*/
			if(strncmp((int8_t*)filename, s_block->file_entries[i].filename, length) == 0){ /* check if 2 are the same */
				/* using strncpy from lib to make deep copy*/
				int type;
				type= s_block->file_entries[i].file_type;
	//			printf("filename: %s\n", s_block->file_entries[i].filename);
	//			printf("file_type: %d\n", s_block->file_entries[i].file_type);
	//			printf("inode num: %d\n", s_block->file_entries[i].inode_num);
				for(j=0;j<6;j++){
					if(current_pcb->file_descriptor[j+2].flags==0){
						if(type==0){
							current_pcb->file_descriptor[j+2].file_opt_ptr=rtc_opt;
						}	//set this pcb to rtc
						else if(type==1){
							current_pcb->file_descriptor[j+2].file_opt_ptr=dir_opt;
						}	//set this pcb to directory
						else if(type==2){
							current_pcb->file_descriptor[j+2].file_opt_ptr=file_opt;
							/*Excutable check*/
							uint8_t buf[buffer_size];
							if(read_file_img((int8_t*)s_block->file_entries[i].filename, (uint8_t*) buf, buffer_size) == -1){
								printf("Excutable check fail!\n");
								while(1);
							}
							printf("************************start of the file************************%s\n", buf);
							
							if(strncmp((int8_t*)buf, (int8_t*)ELF, (uint32_t)4)){
							//		printf("not Excutable!!\n");
								current_pcb->file_descriptor[j+2].exe_flag=0;
							}
							else{
						//		printf("this is executable\n");
								current_pcb->file_descriptor[j+2].exe_flag=1;
							}
						}	//set this pcb to regular file
						current_pcb->file_descriptor[j+2].inode_ptr=(uint32_t)s_block+ (s_block->file_entries[i].inode_num+1)*four_kb;
						current_pcb->file_descriptor[j+2].inode_num=s_block->file_entries[i].inode_num;
						current_pcb->file_descriptor[j+2].flags=USED;
						break;	
					}
					else if(j==5&&current_pcb->file_descriptor[j+2].flags!=0){
						//return -1; 	//array is full
						asm("movl $-1, %eax");  //comment this line after add the function
						asm("leave;ret");
					}
				}

				//return fd number
				asm("movl %%ebx, %%eax" 
				:
				: "b"(j+2)
				: "cc", "memory"
				);
				asm("leave;ret");
				
				return 0; /* operation success*/
			}
		}

	}
	return -1; /* operation failed */

	asm("movl $-, %eax");  //comment this line after add the function
	asm("leave;ret");

	//will never get here 
	return 0;
}


/* Description:
 * system call close.
 *   Close system call: passing in with fd, close the corresponding fd for the file
 * let it free to be used by other process. return -1 for fail operation, and return 
 * 0 when success
 */
int32_t close(int32_t fd){
	
	pcb* current_pcb = getting_to_know_yourself(); /* geeting current pcb*/
	current_pcb->file_descriptor[fd].file_opt_ptr=NULL;
	current_pcb->file_descriptor[fd].file_pos = 0;
	current_pcb->file_descriptor[fd].flags = N_USED;

	asm("movl $0, %eax");  //comment this line after add the function
	asm("leave;ret");

	//will never get here, stops compiler warnings 
	return 0;
}


/**
  *Description
  * System Call: getarg 
  * 	reads the program's command line arguments into a user-level buffer
  *   Merely copied into user space. Return -1 if argument do not fit in the 
  *   buffer.
  */
int32_t getargs(uint8_t* buf, int32_t nbytes){
	
	pcb* current_pcb = getting_to_know_yourself(); /* PCB at current process */
	
	uint32_t length = strlen((int8_t*) current_pcb->arg); /* getting length of the argument */

	if(length > nbytes || length < 0){
		return -1; /* case argument does not fit buffer */
	}	

	strncpy((int8_t*) buf, (int8_t*) current_pcb->arg, nbytes); /* copying argument */

	//will never get here, stops compiler warnings 
	return 0;
}


/* Description:
 * system call vidmap.
 *
 * 
 */
int32_t vidmap(uint8_t** screen_start){
	
	
	/* check screen_start memory location first */


	pcb * current_pcb = getting_to_know_yourself(); /* getting current pcb */

	uint32_t vir_add = 0x10000000; /* virtual address */
	uint32_t phy_add = 0x8000; /* physcial address */
	uint32_t pid = current_pcb->pid; /* current pid */
	uint32_t pd_add = (uint32_t)(&processes_page_dir[pid]); /* page directory address */
	uint32_t pt_add = (uint32_t)(&vidmap_page_table[pid]); /* page table address */

	int ret = map_4kb_page(pid, vir_add, phy_add, 1, pd_add, pt_add, 1);

	if(ret == 0){
		/*not sure what to do here */
		* screen_start = vir_add;
		return 0;
	}
	else{
		return -1; 
	}

}


/* Description:
 * system call set_handler.
 *
 * 
 */
int32_t set_handler(int32_t signum, void* handler_address){
	
	asm("movl $0, %eax"); //comment this line after add the function
	asm("leave;ret");

	//will never get here, stops compiler warnings 
	return 0;
}



/* Description:
 * system call sigreturn.
 *
 * 
 */
int32_t sigreturn(void){
	

	asm("movl $0, %eax"); //comment this line after add the function
	asm("leave;ret");

	//will never get here, stops compiler warnings 
	return 0;
}


/**
  * get next pid
  * INPUT: none
  * OUTPUT: pid for the new process
  * SIDE EFFECT: none
  */
uint32_t get_next_pid(void){
	int i = 0; /* loop counter */
	while(i < size_of_occupied){
		if(occupied[i] == N_USED){/* case avaliable*/
			occupied[i] = USED;
			return i;
		}
		else{
			i++;
		}
	}
	return -1;
}