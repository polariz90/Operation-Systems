/**
  * Page.c
  * Octo. 26, 2014
  * Group prOS 
  * 	Page header file, manage memory mapping and paging
  *		Include initializing page at booting, and creating
  * 	new pages with execution systemcall was called
  */

  #include "page.h"
  #include "x86_desc.h"
  #include "lib.h"




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

	int i; /* counter for loops */

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
		kernel_page_dir[i].PT_base_add = i;
	}
	for(i = 0; i < PAGE_TABLE_SIZE; i++){

		video_page_table[i].present = 0;
		video_page_table[i].read_write = 0;
		video_page_table[i].user_supervisor = 0;
		video_page_table[i].write_through = 0;
		video_page_table[i].cache_disabled = 0;
		video_page_table[i].accessed = 0;
		video_page_table[i].dirty = 0;
		video_page_table[i].PT_attribute_idx = 0;
		video_page_table[i].global_page = 0;
		video_page_table[i].avail = 0;
		video_page_table[i].page_base_add = i;
	}


	/* set up kernel page entries */
	kernel_page_dir[1].present = 1;
	kernel_page_dir[1].read_write = 1;
	kernel_page_dir[1].user_supervisor = 0;
	kernel_page_dir[1].write_through = 0;
	kernel_page_dir[1].cache_disabled = 0;
	kernel_page_dir[1].accessed = 0;
	kernel_page_dir[1].reserved = 0;
	kernel_page_dir[1].page_size = 1;
	kernel_page_dir[1].global_page = 0;
	kernel_page_dir[1].avail = 0;
	kernel_page_dir[1].PT_base_add = 1024;

	/* set up video page directory  entries */

	kernel_page_dir[0].present = 1;
	kernel_page_dir[0].read_write = 1;
	kernel_page_dir[0].user_supervisor = 0;
	kernel_page_dir[0].write_through = 0;
	kernel_page_dir[0].cache_disabled = 0;
	kernel_page_dir[0].accessed = 0;
	kernel_page_dir[0].reserved =0;
	kernel_page_dir[0].page_size =0;
	kernel_page_dir[0].global_page = 0;
	kernel_page_dir[0].avail = 0;
	kernel_page_dir[0].PT_base_add = (int)video_page_table;

	/* set up video page table entries*/
	video_page_table[VIDEO_TABLE_IDX].present = 1;
	video_page_table[VIDEO_TABLE_IDX].read_write = 1;
	video_page_table[VIDEO_TABLE_IDX].user_supervisor = 0;
	video_page_table[VIDEO_TABLE_IDX].write_through = 0;
	video_page_table[VIDEO_TABLE_IDX].cache_disabled = 0;
	video_page_table[VIDEO_TABLE_IDX].accessed = 0;
	video_page_table[VIDEO_TABLE_IDX].dirty = 0;
	video_page_table[VIDEO_TABLE_IDX].PT_attribute_idx = 0;
	video_page_table[VIDEO_TABLE_IDX].global_page = 0;
	video_page_table[VIDEO_TABLE_IDX].avail = 0;
	video_page_table[VIDEO_TABLE_IDX].page_base_add = VIDEO_TABLE_IDX;		

	/* copies the address of the page directory into the CR3 register and enable paging*/
/*
	
	asm volatile( "\n\
				  movl %%eax, %%cr3\n\
				  movl %%cr0, %%eax\n\
				  orl $0x80000000, %%eax\n\
				  movl %%eax, %%cr0\n\
				  " 
				  :
				  :"a" (kernel_page_dir)
				  :"eax", "memory"
				);
*/
	
asm volatile (	"mov (kernel_page_dir), %eax\n"
			"mov %eax, %cr3\n"
			"mov %cr0, %eax\n"
			"orl $0x80000000, %eax\n"
			"mov %eax, %cr0\n");

}