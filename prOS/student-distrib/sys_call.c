/**
  * sys_call.c
  * Dec. 2nd 2014
  * project prOS
  *    primary system calls for OS
  */

#include "lib.h"
#include "sys_call.h"
#include "file.h"
#include "x86_desc.h"
#include "assembly_ops.h"
#include "page.h"
#include "terminal.h"
#include "rtc.h"
#include "pit.h"
#include "mouse.h"
#include "clock.h"

#define space_char 			32
#define vir_mem_add 		0x08000000
#define phy_mem_add 		0x800000
#define four_mb 			0x400000
#define eight_mb 			0x800000
#define eight_kb 			0x2000
#define buffer_size 		128


/* array to keep in check of process number */
//uint32_t occupied[7] = {0,0,0,0,0,0,0};
process_arr process_occupy;

uint8_t ELF[4]={0x7f, 0x45, 0x4c, 0x46};


/* Description:
 * system call halt.
 *	DESCIPTION: called in the end of a process, restoring parent process's
 * ESP/EBP in order to switch to parent's stack, and returning from parent's 
 * stack which is right after iret in execute function. 
 * 
 */
int32_t halt(uint8_t status){
	cli();
	/*restore parent's esp/ebp and anything else you need*/
	pcb* current_pcb = getting_to_know_yourself(); /* geeting current pcb*/
//	int i;
//	int pid;
//	for(i = 1; i < 7; i++){
//		if(terminals[scheduling_terminal].pros_pids[i] == 1){
//			if(process_occupy.top_process_flag[i] == 1){
//				pid = i;
//				break;
//			}
//		}
//	}

//	pcb* current_pcb = getting_the_ghost(pid);

	/*may want to prevent user to close the last shell*/
	if(current_pcb->pid==1||terminals[scheduling_terminal].pros_pids[current_pcb->parent_pid]==0){ /* kernel + last shell */
	//if(0){ /* kernel + last shell */
		uint8_t buf[buffer_size];
		uint32_t shell_entry_point;
		if(read_file_img((int8_t*)"shell",(uint8_t*) buf, buffer_size) == -1){
			printf("read file in halt failed \n");
			return -1;
		}

			if(current_pcb->pid==1){
	
				terminal_write(1,"Are you satisfied with your care (y or n)  ", 43);
				char temp_buf[1];
				terminal_read(1,temp_buf, 1);
				if(temp_buf[0] == 'y'){
	
				//stop scheduler
				pit_disable();
					/* set TSS back to point at parent's kernel stack */
				tss.esp0 = eight_mb - (eight_kb*current_pcb->parent_pid) - 4;
	
				int parent_page = (int)current_pcb->parent_page_dir_ptr;
	
				/* reset current processes mask for other process use */
				release_cur_pid(current_pcb->pid);
	
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
				terminals[curr_terminal].pros_pids[current_pcb->pid] = 0;
				process_occupy.occupied[current_pcb->pid] = N_USED;
				process_occupy.top_process_flag[current_pcb->pid]= 0;
				process_occupy.top_process_flag[current_pcb->parent_pid] = 1;
	
				return status;
			}
		}
		//printf("Are you Are you satisfied with your care? -- Big Hero 6 (y or n)\n");
		memcpy(&shell_entry_point, buf+24, 4);

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
				: "b"(shell_entry_point), "d"(eflag|0x4200), "c"(USER_CS), "a"(USER_DS) 
				: "memory", "cc" );


		asm ("iret");
		return status;

	}
	else{

	terminals[scheduling_terminal].pros_pids[current_pcb->pid] = 0;
	process_occupy.top_process_flag[current_pcb->pid]= 0;
	process_occupy.occupied[current_pcb->pid] = N_USED;
	process_occupy.top_process_flag[current_pcb->parent_pid] = 1;


	/* set TSS back to point at parent's kernel stack */
	tss.esp0 = eight_mb - (eight_kb*current_pcb->parent_pid) - 4;

	int parent_page = (int)current_pcb->parent_page_dir_ptr;

		/*restore parent's paging*/
	asm(
		"movl %%eax, %%cr3			;"
		: 
		: "a"(parent_page) 
		: "memory","cc"
		);

		/* reset current processes mask for other process use */
	release_cur_pid(current_pcb->pid);

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

	cli();
	mouse_click_flag = 0;
	int i,j; /* loop counter */
	int new_term_flag = 1; /* flag to see if this is the new temrinal: 1 means this is called by new open terminal*/
	uint32_t entry_point;

	/*Parse*/
	uint8_t com_arr[buffer_size];
	uint8_t arg_arr[buffer_size];
	/* special case check */

	if(command == NULL){
		return -1;
	}
	if(command[0] == space_char){
		return -1;
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

	/* loop to check if this is the new terminal*/
	for( i = 0; i < 7; i++){/* looping through entire terminal process */
		if(terminals[curr_terminal].pros_pids[i] == 1){
			new_term_flag = 0;
		}
	}

	/* getting new pid for processes */
	int pid = get_next_pid((int8_t*)com_arr);
	/* store the pid ino the terminal structure */
	terminals[curr_terminal].pros_pids[pid] = 1;
	if(pid == -1){

		terminal_write(1, "Warning!!! Reaching maximum process capacity!! Calm Down Pls!!!!\n", 65);
		terminals[curr_terminal].pros_pids[pid] = 0;
		return -1;
	}


	/* checking special commands */
	if(strncmp((int8_t*)com_arr, "clear", 5) == 0){ /* clear screen command */
		clear();
		release_cur_pid(pid);
		return 0;
	}

	/* special case for malloc */
	if(strncmp((int8_t*)com_arr, "malloc", 6) == 0){ /* case when malloc is calling */

		uint32_t ret = malloc(2034);
		if(ret == -1){ /* case bad malloc */
			printf("Bad malloc, running out of memory man~~~!!!\n");
		}
		else{
			printf("Memory address is : %x\n", ret);
			release_cur_pid(pid);
			return 0;
		}
	}

	if(strncmp((int8_t*)com_arr, "pid", 3) == 0){ /* clear screen command */
		pcb* current_pcb = getting_to_know_yourself();
		printf("********************%d\n", current_pcb->pid);
		release_cur_pid(pid);
		return 0;
	}

	/*Excutable check*/
	uint8_t buf[buffer_size];
	if(read_file_img((int8_t*)com_arr,(uint8_t*) buf, buffer_size) == -1){
		//printf("**********execute check 4-2\n");
		release_cur_pid(pid);
		//occupied[pid]=0;
		terminals[curr_terminal].pros_pids[pid] = 0;
		return -1;
	}

	if(strncmp((int8_t*)buf, (int8_t*)ELF, (uint32_t)4)){

		release_cur_pid(pid);
		//occupied[pid]=0;
		terminals[curr_terminal].pros_pids[pid] = 0;
		return -1;
	}
	else{
		/* it is executable, do nothing */
	}

	/*Paging*/
	uint32_t parent_cr3_add;
	asm volatile("movl %%cr3, %%eax "               \
			: "=a"(parent_cr3_add)
			: 
			: "memory", "cc" );
	change_process_page(pid, vir_mem_add, phy_mem_add+(pid-1)*four_mb, 1);
	/* paging test */


	/*File loader*/
	int file_size; 
	file_size = load_file_img((int8_t*)com_arr);
	if(file_size == -1){
		release_cur_pid(pid);
		//occupied[pid]=0;
		terminals[curr_terminal].pros_pids[pid] = 0;
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
	new_pcb->parent_page_dir_ptr= (void*)parent_cr3_add; /**/
	new_pcb->process_size = file_size;

	if(new_term_flag != 1){
		process_occupy.top_process_flag[current_pcb->pid] = 0; /* you are not top anymore */
	}
	process_occupy.top_process_flag[pid] = 1; /* your children is on the top*/


	/*context switch*/
	int temp = eight_kb*pid;
	tss.esp0= eight_mb - temp - 4;
	tss.ss0= KERNEL_DS;
	/*getting entry_point from file image*/
	memcpy(&entry_point, buf+24, 4);


	uint32_t eflag = 0;
	cli_and_save(eflag);
	sti();
	display_clock();

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

/* Description:
 * system call read.
 *	Read system call: passing in fd with read buffer and number of bytes need to 
 * be read. return number of bytes that read, or return 0 when reach the end of 
 * of the file
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
	if(fd < 0 || fd > 7){
		return -1;
	}
	if((getting_to_know_yourself())->file_descriptor[fd].flags == N_USED)
	{
		return -1;
	}

//	pcb* current_pcb = getting_to_know_yourself(); /* geeting current pcb*/

	return (*(getting_to_know_yourself())->file_descriptor[fd].file_opt_ptr->opt_read) (fd, buf, nbytes);

}


/* Description:
 * system call write.
 *  Write system call: passing in fd with buffer and number of bytes need to 
 * be write. Return number of bytes that write to the terminal. 
 */
int32_t write(int32_t fd, void* buf, int32_t nbytes){
	if (fd < 0 || fd > 7){
		return -1;
	}

	//testing if file has not been used yet
	if((getting_to_know_yourself())->file_descriptor[fd].flags == N_USED)
	{
		return -1;
	}

	return (*(getting_to_know_yourself())->file_descriptor[fd].file_opt_ptr->opt_write) (fd, buf, nbytes);

}


/* Description:
 * system call open.
 *  Open system call: passing in with a filename, and allocate fd location for the 
 * file, if fd is full, return -1. else return 0 with side effect of a functional 
 *  file descriptor 
 */
int32_t open(const uint8_t* filename){
	if(filename[0] == 0x72){
		if(filename[1] == 'a'){
			return 	open((uint8_t*)"frame0.txt");
		}
	}
	/*don't need this if user never call open(terminal)*/
	if(!strncmp((int8_t*)filename,(int8_t*) "terminal", 9)){

	}

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
				for(j=0;j< 6;j++){
					if(current_pcb->file_descriptor[j+2].flags==0){
						if(type==0){
							current_pcb->file_descriptor[j+2].file_opt_ptr=(opt*)rtc_opt;
						}	//set this pcb to rtc
						else if(type==1){
							current_pcb->file_descriptor[j+2].file_opt_ptr=(opt*)dir_opt;
						}	//set this pcb to directory
						else if(type==2){
							current_pcb->file_descriptor[j+2].file_opt_ptr=(opt*)file_opt;
							/*Excutable check*/
							uint8_t buf[buffer_size];
							if(read_file_img((int8_t*)s_block->file_entries[i].filename, (uint8_t*) buf, buffer_size) == -1){
								printf("Excutable check fail!\n");
								while(1);
							}
							
							if(strncmp((int8_t*)buf, (int8_t*)ELF, (uint32_t)4)){
							//		printf("not Excutable!!\n");
								current_pcb->file_descriptor[j+2].exe_flag=0;
							}
							else{
						//		printf("this is executable\n");
								current_pcb->file_descriptor[j+2].exe_flag=1;
							}
						}	//set this pcb to regular file
						current_pcb->file_descriptor[j+2].inode_ptr=(inode_struct*)(s_block+ (s_block->file_entries[i].inode_num+1)*four_kb);
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
				/* call open function */
				(*current_pcb->file_descriptor[j+2].file_opt_ptr->opt_open) ( );
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
	if (fd < 2 || fd > 7){
		return -1;
	}

	pcb* current_pcb = getting_to_know_yourself(); /* geeting current pcb*/

	if(current_pcb->file_descriptor[fd].flags == N_USED)
	{
		return -1;
	}

	/* calling close functions in fd*/
	(*current_pcb->file_descriptor[fd].file_opt_ptr->opt_close) ( );

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
	
	/* check valid buffer */
	if(buf == NULL){
		return -1; 
	}

	pcb* current_pcb = getting_to_know_yourself(); /* PCB at current process */
	
	uint32_t length = strlen((int8_t*) current_pcb->arg); /* getting length of the argument */

	if(length > nbytes || length < 0){
		return -1; /* case argument does not fit buffer */
	}	

	strncpy((int8_t*) buf, (int8_t*) current_pcb->arg, nbytes); /* copying argument */

	//will never get here, stops compiler warnings 
	asm("movl $0, %eax");  //comment this line after add the function
	asm("leave;ret");
	return 0;
}


/* Description:
 * system call vidmap.
 *		vidmap system call mapping a new new page table into video memory
 * 	which user are able to access video memory
 *	
 */
int32_t vidmap(uint8_t** screen_start){
	
	/* check screen_start memory location first */
	if(screen_start == NULL){
		return -1;
	}

	pcb * current_pcb = getting_to_know_yourself(); /* getting current pcb */

	int upper_bound = _132mb;
	int lower_bound = _128mb;
	/* check bound if in the user code space */
	if((int)screen_start > upper_bound|| (int)screen_start < lower_bound )
	{
		return -1;
	}


	uint32_t vir_add = 0x10000000; /* virtual address 256MB*/
	uint32_t phy_add = 0xB8000; /* physcial address video memory */
	uint32_t pid = current_pcb->pid; /* current pid */
	uint32_t pd_add = (uint32_t)(&processes_page_dir[pid]); /* page directory address */
	uint32_t pt_add = (uint32_t)(&vidmap_page_table[pid]); /* page table address */

	int ret = map_4kb_page(pid, vir_add, phy_add, 1, pd_add, pt_add, 1);

	if(ret == 0){
		/*not sure what to do here */
		* screen_start = (uint8_t*)vir_add;
		asm("movl $0, %eax");  //comment this line after add the function
		asm("leave;ret");
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
uint32_t get_next_pid(int8_t* buf){
	uint32_t i = 0; /* loop counter */
	while(i < NUM_PROCESSES){
		if(process_occupy.occupied[i] == N_USED){/* case avaliable*/
			process_occupy.occupied[i] = USED;
			process_occupy.num_process += 1;
			//sti();
			return i;
		}
		else{
			i++;
		}
	}
	return -1;
}

/**
  * release cur pid 
  *		release current pid mask in the process occupy
  *	control tracking structure 
  */
void release_cur_pid(uint32_t pid){
		process_occupy.occupied[pid] = N_USED;
		process_occupy.num_process -= 1;
}


/**
  * Malloc System call
  * 		allocating a new page for the user program
  *  return the address of that page. 
  */
uint32_t malloc(uint32_t size ){

	/* getting current pcb first */
	pcb* current_pcb = getting_to_know_yourself();
	uint32_t ret; /* return value */


	int i, j; /* counter */
	/* walking down the heap section for that process */
	for( i = 255; i < 512; i++){ /* searching through entire heap */

		/**** this will not work if free() function is added to the OS!!! ****/
		uint32_t left_over = size; /* tracking how many bytes left */
		if(process_memory_cap[current_pcb->pid].mem_cap[i] != four_kb){/* case this page is not full */
			//ret = eight_mb + ((current_pcb->pid - 1)*four_mb) + (i*four_kb) + process_memory_cap[current_pcb->pid].mem_cap[i]; /* return value */
			ret = _128mb + (i*four_kb) + process_memory_cap[current_pcb->pid].mem_cap[i];
			j = 0; /* number of pages tracking */
			while(left_over != 0){
				if(left_over <= four_kb){/* case this will be the last page */
					/* initializing this page */
					process_page_table[current_pcb->pid].dir_arr[i+j].present = 1;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].read_write = 1; 
			   		process_page_table[current_pcb->pid].dir_arr[i+j].user_supervisor = 1;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].write_through = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].cache_disabled = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].accessed = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].dirty = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].PT_attribute_idx = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].global_page = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].avail = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].page_base_add = ((1024*(current_pcb->pid + 1)) + i);
					
					process_memory_cap[current_pcb->pid].mem_cap[i+j] += left_over;
					left_over = 0;
					break;
				}
				else{
					/* initializing this page */
					process_page_table[current_pcb->pid].dir_arr[i+j].present = 1;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].read_write = 1; 
			   		process_page_table[current_pcb->pid].dir_arr[i+j].user_supervisor = 1;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].write_through = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].cache_disabled = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].accessed = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].dirty = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].PT_attribute_idx = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].global_page = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].avail = 0;
			   		process_page_table[current_pcb->pid].dir_arr[i+j].page_base_add = ((1024*(current_pcb->pid + 1)) + i);
					
					process_memory_cap[current_pcb->pid].mem_cap[i+j] = four_kb;
					left_over -= four_kb;
					j++;
				}
			}
		return ret;
		}
	}
	return -1;
} 

