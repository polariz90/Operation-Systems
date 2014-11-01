/**
  * file.c
  * Oct. 30, 2014
  * project prOS
  * 	File system c file, create basic inode file system
  */

#include "multiboot.h"
#include <string.h>
#include "file.h"
#include "kernel.c"

#define FOUR_KB 4096 /* 4KB = 4096 bytes */

/** pointer to the super block is in kernel.c 
  * variable name s_block (global)
  */

uint32_t NUM_ENTRIES = s_block->dir_entries; /* variable hold # of entries */
uint32_t NUM_INODES = s_block->inode;	/* variable hold # of inodes */

/**	read_dentry_by_name 
  *	DESCRIPTION:	read the dentry from the file system according to the name
  * 				of the file
  * INPUT:			filename, dentry_t blcok
  * OUTPUT:			-1 (failure)
  *					0  (success)
  *	SIDE EFFECT:	fill in the dentry_t block passed in as parameter with file
  *					name, file type and inode when success
  */
int32_t read_dentry_by_name(const uint8_t * fname, dentry_t * dentry){
	int i;	/* loop counter */

	uint32_t length; /* variable to hold fname string length */
	length = strlen(fname);	

	for(i = 0; i < NUM_ENTRIES; i++){ /* looping through entire entries to find file*/
		if(strlen(s_block->file_entries->filename) == length){/* case 2 names doesnt have the same length*/
			if(strncmp(fname, s_block->file_entries->filename, length) == 0){ /* check if 2 are the same */
				/* copy over to dentry_t*/
				dentry_t->filename = s_block->file_entries.filename;
				dentry_t->file_type = s_block->file_entries.file_type;
				dentry_t->inode_num = s_block->file_entries.inode_num;
				return 0; /* operation success*/
			}
		}

	}

	return -1; /* operation failed */
}

/** read_dentry_by_index
  * DESCRIPTION: 	read the dentry from the file system according to the index 
  *					of the file
  * INPUT:			index, dentry_t block 
  * OUTPUT:			-1 (failure)
  *					0 (success)
  *	SIDE EFFECT:	fill in the dentry_t block passed in as parameter with file name
  *					file type, and inode when success
  */
int32_t read_dentry_by_index(uint32_t index, dentry_t * dentry){
	/* check if index is in bound */
	if(index < 0 || index >= NUM_ENTRIES){
		return -1; /* operation failed */
	}

	/* copy operation */
	dentry_t->filename = s_block->file_entries[index].filename;
	dentry_t->file_type = s_block->file_entries[index].file_type;
	dentry_t->inode_num = s_block->file_entries[index].inode_num;
	return 0; /* operation success*/
}

/** read_data
  * DESCRIPTION: 	read up to length bytes stating from position offset
  *					in the file with inode number inode
  * INPUT:			inode, offset, buffer and length of bytes need to read
  * OUTPUT:			# of bytes read and placed in the buffer
  *					0 when reach end of the file 
  * SIDE EFFECT:	fill up the buffer with file information
  */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length){
	/* check boundary conditions */
	if(inode < 0 || inode > s_block.inodes){
		retrun -1; /* case invalid inode number */
	}
	if(offset < 0 || offset > s_block[inode+1].length){
		retrun -1; /* case invalid offset */
	}
	/* need more case check for sure */

	/* compute number of data block offset is in */
	uint32_t num_block = offset / FOUR_KB;
	/* compute number of bytes need to skip in initial data block */ 
	uint32_t num_left = offset % FOUR_KB;
	uint32_t inodes_N = s_block.inodes;

	for(; length > 0; length --){ /* loop to read length bytes of data into buffer */
		buf = s_block[inode_N + num_block + 1].data[num_left]; /* copying data */
		buf ++; num_left ++; 
		if(num_left == FOUR_KB){ /* reach end of the block */
			num_block ++;
			num_left = 0; /* goes to next block */
		}
	}
}