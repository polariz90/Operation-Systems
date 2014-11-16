/**
  * file.h
  * Octo. 28, 2014
  * project prOS
  * 	File system header file, create basic inode file system
  */

#ifndef PROS_FILE_H
#define PROS_FILE_H


#define BOT_KERNEL_MEM 	0x800000	/*third page in memory.8MB*/
#define STACK_OFF	 	0x2000		/*size of a process block 8KB*/
#define SIZE_128MB  0x8000000 /*size of 128mb*/

#define four_kb 4096 /* 4KB = 4096 bytes */
#define name_length 32 /* length of the name string */

#include "x86_desc.h"
#include "assembly_ops.h"

/**
  * Basic structure for file system:
  * data_block for file system
  * contain 4kb char array
  */
typedef struct 
{
  char data[4096]; /*4kb --> 4098 bytes --> 4096 chars */
}data_struct;

/**
  * Basic structure for file system:
  * file struct
  * total 64B structure
  */
typedef struct 
{
	int8_t filename[32]; /* 32 bytes of file name */
	uint32_t file_type; /* 4 bytes file type: (0) RTC, (1) Directory, (2) regular file */
	uint32_t inode_num;	/* 4 bytes inode number index */
	uint8_t reserved[24];	/* 24 bytes reserved */
}dentry_t;

/**
  * Basic structure for file system:
  * inode struct
  * total 4kb structure
  */
typedef struct 
{
	uint32_t length; /*length of file? in Bytes */
	uint32_t data_blocks[1023]; /* pointers to data blocks */
	/* each file can totally holds 1023 4kb-data blocks */
	/* each file max 4MB? */
}inode_struct;

/**
  * Basic structures for file system:
  * boot block for file system
  * total 4kb structure
  */

typedef struct 
{
  uint32_t dir_entries; /* 4 bytes directory entries*/
  uint32_t inodes;  /* 4 bytes inodes (N) */
  uint32_t data_blocks; /* 4 bytes data blocks */
  uint8_t reserved[52]; /* 52 bytes reserved */
  /*4031 bytes left for entries, total of 62 entries, need round*/
  /* not sure if this is right, or need to alian, need ask */
  dentry_t file_entries[63];
}super_block;


/**
  * Basic entry for file system:
  * will form a file descriptor which is an array of these size 8 
  * (the element of this array)
  */
typedef struct 
{
  void * file_opt_ptr;  /*4 bytes file operation table pointer*/
  inode_struct* inode_ptr;  /* 4 bytes inodes ptr */
  uint32_t file_pos; /* 4 bytes file position */
  uint32_t flags; /* 4 bytes flags */
}file_entry;

/* Process control block
 * contains:
 * filedescriptors,
 * pointers to page table and page dir
 * parent_process , unsure what eactly what it is pointing to
 * debug info, what ever we want
 */

typedef struct
{
	file_entry file_descriptor[8];
	void* page_dir_ptr;
	void* page_table_ptr;
	void* parent_process;
  uint32_t parent_eip;
	uint32_t debug_info;
  uint32_t pid;
  uint8_t arg[128];
  tss_t tss;
}pcb;

//extern pcb file_desc[8];

extern super_block* s_block;

int32_t read_dentry_by_name (const uint8_t * fname, dentry_t * dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t * dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length);
int32_t open_file_sys(const uint8_t * fname);
int32_t read_sys (int32_t fd, void * buf, int32_t nbytes);
int32_t read_file(const int8_t* fname, void * buf, uint32_t nbytes);
int32_t read_dir(int8_t* fname, uint8_t * buf, uint32_t nbytes);
int32_t write_sys(int32_t fd, const void * buf, int32_t nbytes);
int32_t write_dir();
int32_t write_file();
int32_t open_file(const uint8_t *filename);
int32_t open_dir(const uint8_t *filename);
int32_t close_file(const uint8_t *filename);
int32_t close_dir(const uint8_t *filename);

/*added for pcb */
void init_pcb(pcb* curr_pcb);
pcb* add_process_stack(uint8_t num );

int32_t read_file_img(const int8_t * fname, uint8_t* buffer);
void load_file_img(int8_t* fname);

#endif
