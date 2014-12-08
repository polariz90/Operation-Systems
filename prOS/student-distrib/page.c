/**
  * Page.c
  * Octo. 26, 2014
  * Group prOS 
  * 	Page header file, manage memory mapping and paging
  *		Include initializing page at booting, and creating
  * 	new pages with execution systemcall was called
  */

  #include "x86_desc.h"
  #include "lib.h"
  #include "page.h"


  #define max_process_num 6
  #define size_of_pagedir 4096
  #define four_GB         0x100000000
  #define four_MB		  0x400000
  #define four_KB		  0x1000

  /* variable to hold the process page directory address for inline assembly */
  uint32_t new_page_dir_add;

  /*Function to initializing paging*/
  /**
    * init_paging
    *	INPUT: none
    *	OUTPUT: none
    *	RETURN: none
    *	SIDE EFFECTS: initializing paing, called when booting 
    *				up, set up Control register to enable paging,
    				page kernel to virtual memory 4MB-8MB
    */
void init_paging( ){


	int i, j; /* counter for loops */

	/* initialize kernel_page_dir and video_page table */
	for( i = 0; i < PAGE_DIRECTORY_SIZE; i++){
		kernel_page_dir[i].present = 0;
		kernel_page_dir[i].read_write = 0;
		kernel_page_dir[i].user_supervisor = 0;
		kernel_page_dir[i].write_through = 0;
		kernel_page_dir[i].cache_disabled = 0;
		kernel_page_dir[i].accessed = 0;
		kernel_page_dir[i].reserved = 0;
		kernel_page_dir[i].page_size = 0;
		kernel_page_dir[i].global_page = 0;
		kernel_page_dir[i].avail = 0;
		kernel_page_dir[i].PT_base_add = i*1024;
	}
	for(j = 0; j < 7; j++){
		for(i = 0; i < PAGE_TABLE_SIZE; i++){
			video_page_table[j].dir_arr[i].present = 1;
			video_page_table[j].dir_arr[i].read_write = 0;
			video_page_table[j].dir_arr[i].user_supervisor = 0;
			video_page_table[j].dir_arr[i].write_through = 0;
			video_page_table[j].dir_arr[i].cache_disabled = 0;
			video_page_table[j].dir_arr[i].accessed = 0;
			video_page_table[j].dir_arr[i].dirty = 0;
			video_page_table[j].dir_arr[i].PT_attribute_idx = 0;
			video_page_table[j].dir_arr[i].global_page = 0;
			video_page_table[j].dir_arr[i].avail = 0;
			video_page_table[j].dir_arr[i].page_base_add = i;
		}
	}

	/* set up kernel page entries -- the 4MB mapping to kernel code */
	kernel_page_dir[1].present = 1; /* enable page entry */
	kernel_page_dir[1].read_write = 1; /* read and write enable*/
	kernel_page_dir[1].user_supervisor = 0; /* 0 for supervisor privilege lvl */
	kernel_page_dir[1].write_through = 0; /* set to one, pass control to CR0 */
	kernel_page_dir[1].cache_disabled = 0; /* set to one, pass control to CR0*/
	kernel_page_dir[1].accessed = 0; /* set to one to access it */
	kernel_page_dir[1].reserved = 0; /* set to 0 */
	kernel_page_dir[1].page_size = 1; /* 1 indicate 4 MB pages */
	kernel_page_dir[1].global_page = 1; /* set to global*/
	kernel_page_dir[1].avail = 0; /* set to 0 */
	kernel_page_dir[1].PT_base_add = 1024;


	/* set up video page directory  entries -- the page directory which video memory in */
	kernel_page_dir[0].present = 1;
	kernel_page_dir[0].read_write = 1;
	kernel_page_dir[0].user_supervisor = 0;
	kernel_page_dir[0].write_through = 0;
	kernel_page_dir[0].cache_disabled = 0;
	kernel_page_dir[0].accessed = 0;
	kernel_page_dir[0].reserved =0;
	kernel_page_dir[0].page_size =0;
	kernel_page_dir[0].global_page = 1;
	kernel_page_dir[0].avail = 0;
	kernel_page_dir[0].PT_base_add = ((int)(&video_page_table[0]) >> 12);

	/* set up video page table entries -- the 4KB video memory in */
	for(j = 0; j < 7; j++){
		video_page_table[j].dir_arr[video_table_idx].present = 1;
		video_page_table[j].dir_arr[video_table_idx].read_write = 1;
		video_page_table[j].dir_arr[video_table_idx].user_supervisor = 0;
		video_page_table[j].dir_arr[video_table_idx].write_through = 0;
		video_page_table[j].dir_arr[video_table_idx].cache_disabled = 0;
		video_page_table[j].dir_arr[video_table_idx].accessed = 0;
		video_page_table[j].dir_arr[video_table_idx].dirty = 0;
		video_page_table[j].dir_arr[video_table_idx].PT_attribute_idx = 0;
		video_page_table[j].dir_arr[video_table_idx].global_page = 1;
		video_page_table[j].dir_arr[video_table_idx].avail = 0;
//		video_page_table[j].dir_arr[video_table_idx].page_base_add = 0XB8;	
	}

	for(j = 0; j < 7; j++){
		for(i = 1; i < 4; i++){
			video_page_table[j].dir_arr[video_table_idx + i].present = 1;
			video_page_table[j].dir_arr[video_table_idx + i].read_write = 1;
			video_page_table[j].dir_arr[video_table_idx + i].user_supervisor = 1;
			video_page_table[j].dir_arr[video_table_idx + i].write_through = 0;
			video_page_table[j].dir_arr[video_table_idx + i].cache_disabled = 0;
			video_page_table[j].dir_arr[video_table_idx + i].accessed = 0;
			video_page_table[j].dir_arr[video_table_idx + i].dirty = 0;
			video_page_table[j].dir_arr[video_table_idx + i].PT_attribute_idx = 0;
			video_page_table[j].dir_arr[video_table_idx + i].global_page = 1;
			video_page_table[j].dir_arr[video_table_idx + i].avail = 0;
		}
	}

	/* copies the address of the page directory into the CR3 register and enable paging*/

asm (
	"movl $kernel_page_dir, %%eax    ;"
	"movl %%eax, %%cr3                ;"
	"movl %%cr4, %%eax				  ;"
	"andl $0xFFFFFFDF, %%eax          ;"
	"orl $0x00000010, %%eax			  ;"
	"movl %%eax, %%cr4 				  ;"
	"movl %%cr0, %%eax                ;"
	"orl $0x80000000, %%eax 	      ;"
	"movl %%eax, %%cr0                ;"
	: : : "eax", "memory" ,"cc" );

}

 /*Function to set up new pages for processes */
  /**
    * map_4KB_page
    *	INPUT: process id, virtual address, physical address, user_superviser
    *	OUTPUT: none
    *	RETURN: none
    *	SIDE EFFECTS: initilizing new page directories for process, allocate 4MB
    *					memory locations depending on the process id.
    */
int change_process_page(uint32_t pid, uint32_t vir_add, uint32_t phy_add, uint32_t privilage){

		int i; /* loop counter for initialize page directory */
		int vir_address = 32;

		/* check for valid pid number */
		if( pid < 0 || pid > max_process_num){
			return -1; /* case pid out of bound */
		}

		/* page directory pointer which point at the new page directory */
		page_directory * new_page_dir;
		new_page_dir = (page_directory*) &processes_page_dir[pid]; 

		new_page_dir_add = (uint32_t)(&processes_page_dir[pid]);


		/* initialize entire page directory   */
		for( i = 0; i < PAGE_DIRECTORY_SIZE; i++){
			new_page_dir->dir_arr[i].present = 0;
			new_page_dir->dir_arr[i].read_write = 0;
			new_page_dir->dir_arr[i].user_supervisor = 0;
			new_page_dir->dir_arr[i].write_through = 0;
			new_page_dir->dir_arr[i].cache_disabled = 0;
			new_page_dir->dir_arr[i].accessed = 0;
			new_page_dir->dir_arr[i].reserved = 0;
			new_page_dir->dir_arr[i].page_size = 0;
			new_page_dir->dir_arr[i].global_page = 0;
			new_page_dir->dir_arr[i].avail = 0;
			new_page_dir->dir_arr[i].PT_base_add = i*1024;
		}
		

		/*initialize the 4MB memory, at Virtual 128MB, physical 4MB+pid*4MB */
		/* this is the new page memory for process */
			new_page_dir->dir_arr[vir_address].present = 1; /* enable page entry */
			new_page_dir->dir_arr[vir_address].read_write = 1; /* read and write enable */
			new_page_dir->dir_arr[vir_address].user_supervisor = privilage; /* set to user privilage */
			new_page_dir->dir_arr[vir_address].write_through = 0; /* disable write through*/
			new_page_dir->dir_arr[vir_address].cache_disabled = 0; /* disable cache */
			new_page_dir->dir_arr[vir_address].accessed = 0; /* set to one to access it */
			new_page_dir->dir_arr[vir_address].reserved = 0; /* reserved set to 0 */
			new_page_dir->dir_arr[vir_address].page_size = 1; /* 1 indicate to 4MB pages */
			new_page_dir->dir_arr[vir_address].global_page = 0; /* process not global page */
			new_page_dir->dir_arr[vir_address].avail = 0; /* set to 0*/
			new_page_dir->dir_arr[vir_address].PT_base_add = 1024*(pid+1); /*set to physcial address */

			/* set up video page directory  entries */
			new_page_dir->dir_arr[0].present = 1;
			new_page_dir->dir_arr[0].read_write = 1;
			new_page_dir->dir_arr[0].user_supervisor = 0;
			new_page_dir->dir_arr[0].write_through = 0;
			new_page_dir->dir_arr[0].cache_disabled = 0;
			new_page_dir->dir_arr[0].accessed = 0;
			new_page_dir->dir_arr[0].reserved =0;
			new_page_dir->dir_arr[0].page_size =0;
			new_page_dir->dir_arr[0].global_page = 1;
			new_page_dir->dir_arr[0].avail = 0;
			new_page_dir->dir_arr[0].PT_base_add = ((int)(&video_page_table[pid]) >> 12);
			//printf("******************************change pt to pid: %d\n", pid);

			/* set up kernel page entries */
			new_page_dir->dir_arr[1].present = 1; /* enable page entry */
			new_page_dir->dir_arr[1].read_write = 1; /* read and write enable*/
			new_page_dir->dir_arr[1].user_supervisor = 0; /* 0 for supervisor privilege lvl */
			new_page_dir->dir_arr[1].write_through = 0; /* set to one, pass control to CR0 */
			new_page_dir->dir_arr[1].cache_disabled = 0; /* set to one, pass control to CR0*/
			new_page_dir->dir_arr[1].accessed = 0; /* set to one to access it */
			new_page_dir->dir_arr[1].reserved = 0; /* set to 0 */
			new_page_dir->dir_arr[1].page_size = 1; /* 1 indicate 4 MB pages */
			new_page_dir->dir_arr[1].global_page = 1; /* set to global*/
			new_page_dir->dir_arr[1].avail = 0; /* set to 0 */
			new_page_dir->dir_arr[1].PT_base_add = 1024;


		asm (
			"movl new_page_dir_add, %%eax     ;"
			"movl %%eax, %%cr3                ;"
			"movl %%cr4, %%eax				  ;"
			"andl $0xFFFFFFDF, %%eax          ;"
			"orl $0x00000010, %%eax			  ;"
			"movl %%eax, %%cr4 				  ;"
			"movl %%cr0, %%eax                ;"
			"orl $0x80000000, %%eax 	      ;"
			"movl %%eax, %%cr0                ;"
			: : : "eax" ,"cc" );	



		return 0;

}

/**
  * map_4KB_page
  *		mapping a 4kb page table into privilage	level space
  * INPUT: pid 			process id 
  *		   vir_add 		which we are mapping into
  * 	   phy_add 		which physical address we are mapping into
  * 	   privilage 	privilage level for this page table
  *	 	   pd_add 		page directory address
  * 	   pt_add 		page table address
  * OUTPUT:
  *			return 0 when success, reutn -1 whe failed 
  */
int map_4kb_page(uint32_t pid, uint32_t vir_add, uint32_t phy_add, uint32_t privilage, uint32_t pd_add, uint32_t pt_add, uint32_t read_write){

	/* initializing pointer points at page table and page directory */
	page_directory* cur_page_directory;
	page_table* cur_page_table;

	cur_page_directory = (page_directory*)pd_add;
	cur_page_table = (page_table*)pt_add;

	/* check invalid physical address */
	if(phy_add < 0 ){
		return -1; 
	}
	/* check invalid virtual address */
	if(vir_add < 0 ){
		return -1;
	}
	/* check if the virtual address is already signed */
	uint32_t pd_dir_entries = (vir_add/four_MB); //should be the 64 entries at 256MB
//	if(cur_page_directory->dir_arr[pd_dir_entries].present == 1){
//		printf("page table at this vir_add is already present \n");
//		return -1;
//	}

	/* mapping the page directory first */
	cur_page_directory->dir_arr[pd_dir_entries].present = 1; /* emable page entry*/
	cur_page_directory->dir_arr[pd_dir_entries].read_write = read_write; /*read and write enable */
	cur_page_directory->dir_arr[pd_dir_entries].user_supervisor = privilage; /* set privilage level */
	cur_page_directory->dir_arr[pd_dir_entries].write_through = 0; /* disable write through */
	cur_page_directory->dir_arr[pd_dir_entries].cache_disabled = 0; /* disable cache */
	cur_page_directory->dir_arr[pd_dir_entries].accessed = 0; /* set to one to access it */
	cur_page_directory->dir_arr[pd_dir_entries].reserved = 0; /* reserved set to 0*/
	cur_page_directory->dir_arr[pd_dir_entries].page_size = 0; /* set map to 4kb page */
	cur_page_directory->dir_arr[pd_dir_entries].global_page = 1; /* set to global page */
	cur_page_directory->dir_arr[pd_dir_entries].avail = 0; /*set to 0*/
	cur_page_directory->dir_arr[pd_dir_entries].PT_base_add = ((int)pt_add >> 12); /* page table address shifted */

	uint32_t base_add = (phy_add & 0xFFFFF000) >> 12; /* get last 20 bits of physical address */
	uint32_t temp = vir_add % four_MB; /* getting page table entries, should be 0 at this point */
	uint32_t pt_entries_index = temp/ four_KB; /* get the page_table entry index */
	/* mapping the page table */
	cur_page_table->dir_arr[pt_entries_index].present = 1;
	cur_page_table->dir_arr[pt_entries_index].read_write = read_write;
	cur_page_table->dir_arr[pt_entries_index].user_supervisor = privilage;
	cur_page_table->dir_arr[pt_entries_index].write_through = 0;
	cur_page_table->dir_arr[pt_entries_index].cache_disabled = 0;
	cur_page_table->dir_arr[pt_entries_index].accessed = 0;
	cur_page_table->dir_arr[pt_entries_index].dirty = 0;
	cur_page_table->dir_arr[pt_entries_index].PT_attribute_idx = 0;
	cur_page_table->dir_arr[pt_entries_index].global_page = 1;
	cur_page_table->dir_arr[pt_entries_index].avail = 0;
	cur_page_table->dir_arr[pt_entries_index].page_base_add = base_add;

	return 0;

}

