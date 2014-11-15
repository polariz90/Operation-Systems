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
		kernel_page_dir[i].PT_base_add = i*1024;
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


	/* set up video page directory  entries */

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
	kernel_page_dir[0].PT_base_add = ((int)video_page_table >> 12);

	/* set up video page table entries*/
	video_page_table[VIDEO_TABLE_IDX].present = 1;
	video_page_table[VIDEO_TABLE_IDX].read_write = 1;
	video_page_table[VIDEO_TABLE_IDX].user_supervisor = 0;
	video_page_table[VIDEO_TABLE_IDX].write_through = 0;
	video_page_table[VIDEO_TABLE_IDX].cache_disabled = 0;
	video_page_table[VIDEO_TABLE_IDX].accessed = 0;
	video_page_table[VIDEO_TABLE_IDX].dirty = 0;
	video_page_table[VIDEO_TABLE_IDX].PT_attribute_idx = 0;
	video_page_table[VIDEO_TABLE_IDX].global_page = 1;
	video_page_table[VIDEO_TABLE_IDX].avail = 0;
//	video_page_table[VIDEO_TABLE_IDX].page_base_add = 0X8000;		

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

/*wr=0 -> read_only*/
int set_same_virtual_addr(int physical_addr, int mem_size, int wr)
{
	int pd_offset=physical_addr>>22;
	//int pt_offset=(physical_addr>>12)&0x000003FF;

	if(mem_size<=4096){
		/*****map to 4KB not complete yet, don't know how to create a new page table...******/
		kernel_page_dir[pd_offset].present = 1; /* enable page entry */
		kernel_page_dir[pd_offset].read_write = wr; /* read and write enable*/
		kernel_page_dir[pd_offset].user_supervisor = 0; /* 0 for supervisor privilege lvl */
		kernel_page_dir[pd_offset].write_through = 0; /* set to one, pass control to CR0 */
		kernel_page_dir[pd_offset].cache_disabled = 0; /* set to one, pass control to CR0*/
		kernel_page_dir[pd_offset].accessed = 0; /* set to one to access it */
		kernel_page_dir[pd_offset].reserved = 0; /* set to 0 */
		kernel_page_dir[pd_offset].page_size = 0; /* 1 indicate 4 MB pages */
		kernel_page_dir[pd_offset].global_page = 1; /* set to global*/
		kernel_page_dir[pd_offset].avail = 0; /* set to 0 */
		kernel_page_dir[pd_offset].PT_base_add = physical_addr>>22;
		printf("set 4KB page at virtual memory address 0x%x\n", physical_addr);
		printf("memory size %d\n", mem_size);	
		}
	else if(mem_size<=4096*1024){
		kernel_page_dir[pd_offset].present = 1; /* enable page entry */
		kernel_page_dir[pd_offset].read_write = wr; /* read and write enable*/
		kernel_page_dir[pd_offset].user_supervisor = 0; /* 0 for supervisor privilege lvl */
		kernel_page_dir[pd_offset].write_through = 0; /* set to one, pass control to CR0 */
		kernel_page_dir[pd_offset].cache_disabled = 0; /* set to one, pass control to CR0*/
		kernel_page_dir[pd_offset].accessed = 0; /* set to one to access it */
		kernel_page_dir[pd_offset].reserved = 0; /* set to 0 */
		kernel_page_dir[pd_offset].page_size = 1; /* 1 indicate 4 MB pages */
		kernel_page_dir[pd_offset].global_page = 1; /* set to global*/
		kernel_page_dir[pd_offset].avail = 0; /* set to 0 */
		kernel_page_dir[pd_offset].PT_base_add = physical_addr>>22;
		printf("set 4MB page at virtual memory address 0x%x\n", kernel_page_dir[pd_offset].PT_base_add<<22);
		printf("memory size %d\n", mem_size);
	}
	else{
		return -1;
	}
	return 0;
}
