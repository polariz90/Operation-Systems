/**
  * file.c
  * Oct. 30, 2014
  * project prOS
  * 	File system c file, create basic inode file system
  */

#include "multiboot.h"
#include "file.h"
//#include "kernel.c"
#include "lib.h"

#define four_kb 4096 /* 4KB = 4096 bytes */


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
	uint32_t num_entries = s_block->dir_entries; /* variable hold # of entries */
	uint32_t length; /* variable to hold fname string length */
	length = strlen((int8_t*)fname);

	int8_t * input_arr[32]; /* new input string */
	if (length >= 32){/* case too long */
		strncpy(input_arr, fname, 31);
		input_arr[32] = '\0';
	}
	else{
		strncpy(input_arr, fname, 32);
	}
	
	uint32_t input_length = strlen(input_arr); /* cutted array length */

	for(i = 0; i < num_entries; i++){ /* looping through entire entries to find file*/
		if(strlen(s_block->file_entries[i].filename) == input_length){/* case 2 names doesnt have the same length*/
			if(strncmp((int8_t*)fname, s_block->file_entries[i].filename, input_length) == 0){ /* check if 2 are the same */
				/* copy over to dentry_t*/
				/* using strncpy from lib to make deep copy*/
				strcpy((int8_t*)dentry->filename, (int8_t*)s_block->file_entries[i].filename);
				dentry->file_type = s_block->file_entries[i].file_type;
				dentry->inode_num = s_block->file_entries[i].inode_num;
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

	uint32_t num_entries = s_block->dir_entries; /* variable hold # of entries */

	/* check if index is in bound */
	if(index < 0 || index >= num_entries){
		return -1; /* operation failed */
	}

	/* length of copying file name */
	//uint32_t length = strlen((int8_t*)s_block->file_entries[index].filename);

	/* copy operation */
	//dentry_t.filename = s_block->file_entries[index].filename;
	strcpy(dentry->filename, s_block->file_entries[index].filename);
	dentry->file_type = s_block->file_entries[index].file_type;
	dentry->inode_num = s_block->file_entries[index].inode_num;
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

	/* need more case check for sure */
	uint32_t inodes_N = s_block->inodes; /* total number of dentries we have in file system */
	uint32_t dentry_add = (uint32_t)s_block + four_kb; /*first dentry block address */
	uint32_t data_add = (uint32_t)s_block + (inodes_N+1)*four_kb; /* address for first data block*/

	uint32_t num_block = offset / four_kb;	/* compute number of data block offset is in */
	uint32_t num_skip = offset % four_kb;	/* compute number of bytes need to skip in initial data block */ 
	inode_struct * curr_inode =(inode_struct*)(dentry_add + inode*four_kb); /* pointer points at current inode */
	//data_struct * curr_data = (data_struct*)(data_add + num_block*four_kb); /* pointer points at current data*/

	uint32_t file_length = curr_inode->length; /* length of this file */
	uint32_t bytes_left = file_length - offset; /* this gives number of bytes left to read */
	uint32_t ori_length = length; /* keep in track of original length*/
	uint32_t block_number; /* keep in track of which block is on */

	/* check boundary conditions */
	if(inode < 0 || inode > s_block->inodes){
		return -1; /* case invalid inode number */
	}
	if(offset < 0 || offset > file_length){
		return -1; /* case invalid offset */
	}
	if(file_length == 0){
		return 0; /* case empty file */
	}


	block_number = curr_inode->data_blocks[num_block];
	 /* variable to point at current data block */
	data_struct* curr_data = (data_struct*)((uint32_t)s_block + (inodes_N + block_number+1)*four_kb);

	for(; length > 0; length --){ /* loop to read length bytes of data into buffer */
		if(bytes_left == 0){ /* case finished the entire file */
			return 0; 
		}

		*buf = curr_data->data[num_skip];
		buf ++; num_skip ++; bytes_left --;

		if(num_skip == (four_kb-1)){ /* reach end of the block */
			num_block ++;
			num_skip = 0; /* goes to next block */
			block_number = curr_inode->data_blocks[num_block];
			curr_data = (data_struct*)((uint32_t)s_block + (inodes_N + block_number+1)*four_kb);
			/* points at the new data block*/
		}
	}

	return ori_length - length; 
}



/** open_dir
  * DESCRIPTION: 	read up to length bytes stating from position offset
  *					in the file with inode number inode
  * INPUT:			inode, offset, buffer and length of bytes need to read
  * OUTPUT:			# of bytes read and placed in the buffer
  *					0 when reach end of the file 
  * SIDE EFFECT:	fill up the buffer with file information
  */
int32_t open_f(const uint8_t * fname){

	int i;	/* loop counter */
	uint32_t num_entries = s_block->dir_entries; /* variable hold # of entries */
	uint32_t length; /* variable to hold fname string length */
	length = strlen((int8_t*)fname);	
	//dentry->filename[1] = 1;0

	for(i = 0; i < num_entries; i++){ /* looping through entire entries to find file*/
		if(strlen(s_block->file_entries[i].filename) == length){/* case 2 names doesnt have the same length*/
			if(strncmp((int8_t*)fname, s_block->file_entries[i].filename, length) == 0){ /* check if 2 are the same */
				/* using strncpy from lib to make deep copy*/
				printf("%s", s_block->file_entries[i].filename);
				printf("	file_type: %d", s_block->file_entries[i].file_type);
				printf("	inode num: %d\n", s_block->file_entries[i].inode_num);

				return 0; /* operation success*/
			}
		}

	}

	return -1; /* operation failed */
}
