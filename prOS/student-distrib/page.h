/**
  * Page.h
  * Octo. 26, 2014
  * Group prOS 
  * 	Page header file, manage memory mapping and paging
  */

#ifndef PROS_PAGING_H
#define PROS_PAGING_H

#define video_table_idx 0XB8

/* Function initializing paging */
  void init_paging();


/* structure tracking of the bytes */
typedef struct mem_track
{
	uint32_t mem_cap[1024];
}mem_track;

/* tracking number of bytes that in specific page tables */
extern mem_track process_memory_cap[NUM_PROCESSES];

/* function to map the virtual memory addess to physical memory address */
int change_process_page(uint32_t pid, uint32_t vir_add, uint32_t phy_add, uint32_t privilage);

/* function to map specific 4kb pages */
int map_4kb_page(uint32_t pid, uint32_t vir_add, uint32_t phy_add, uint32_t privilage, uint32_t pd_add, uint32_t pt_add, uint32_t read_write);
#endif
