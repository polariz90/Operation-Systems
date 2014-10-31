/**
  * file.h
  * Octo. 28, 2014
  * project prOS
  * 	File system header file, create basic inode file system
  */

#ifndef PROS_FILE_H
#define PROS_FILE_H


/**
  * Basic structure for file system:
  * file struct
  * total 64B structure
  */
typedef struct 
{
	char filename[32]; /* 32 bytes of file name */
	uint32_t file_type; /* 4 bytes file type: (0) RTC, (1) Directory, (2) regular file */
	uint32_t inode_num;	/* 4 bytes inode number index */
	uint8_t reserved[24];	/* 24 bytes reserved */
}inode;

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
}data_struct;

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
  inode file_entries[63];
}super_block;


#endif
