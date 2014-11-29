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

/*function to map the vertual memery address to the same physical memery address*/
//int set_same_virtual_addr(int physical_addr, int mem_size, int wr);

/* function to map the virtual memory addess to physical memory address */
int change_process_page(uint32_t pid, uint32_t vir_add, uint32_t phy_add, uint32_t privilage);

/* function to map specific 4kb pages */
int map_4kb_page(uint32_t pid, uint32_t vir_add, uint32_t phy_add, uint32_t privilage, uint32_t pd_add, uint32_t pt_add, uint32_t read_write);
#endif
