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

#define FOUR_KB 4096 /* 4KB = 4096 bytes */

/*extern var: file descriptor*/
pcb file_desc[8];

void init_file_desc(void){
	int i;
	for(i=0;i<8;i++)
		file_desc[i].flags=0; /*mark as unused*/
}

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
	//dentry->filename[1] = 1;

	for(i = 0; i < num_entries; i++){ /* looping through entire entries to find file*/
		if(strlen(s_block->file_entries[i].filename) == length){/* case 2 names doesnt have the same length*/
			if(strncmp((int8_t*)fname, s_block->file_entries[i].filename, length) == 0){ /* check if 2 are the same */
				/* copy over to dentry_t*/
				//dentry_t.filename = s_block->file_entries.filename;
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
	uint32_t dentry_add = (uint32_t)s_block + FOUR_KB; /*first dentry block address */
	uint32_t data_block_add = (uint32_t)s_block + FOUR_KB + (s_block->inodes)*FOUR_KB;/* first data block address */

	uint32_t num_block = offset / FOUR_KB;	/* compute number of data block offset is in */
	uint32_t num_skip = offset % FOUR_KB;	/* compute number of bytes need to skip in initial data block */ 
	//uint32_t inodes_N = s_block.inodes; /* total number of dentries we have in file system */
	inode_struct * curr_inode =(inode_struct*)(dentry_add + inode*FOUR_KB); /* pointer points at current inode */
	data_struct * curr_data = (data_struct*)(data_block_add + num_block*FOUR_KB); /* pointer points at current data block */

	uint32_t file_length = curr_inode->length; /* length of this file */
	uint32_t bytes_left = file_length - offset; /* this gives number of bytes left to read */
	uint32_t ori_length = length; /* keep in track of original length*/


	/* check boundary conditions */
	if(inode < 0 || inode > s_block->inodes){
		return -1; /* case invalid inode number */
	}
	if(offset < 0 || offset > file_length){
		return -1; /* case invalid offset */
	}

	for(; length > 0; length --){ /* loop to read length bytes of data into buffer */
		if(bytes_left == 0){ /* case finished the entire file */
			return 0; 
		}

		//*buf = s_block[inodes_N + num_block + 1].data[num_skip]; /* copying data */
		*buf = curr_data->data[num_skip];
		buf ++; num_skip ++; bytes_left--; 
		if(num_skip == (FOUR_KB-1)){ /* reach end of the block */
			num_block ++;
			num_skip = 0; /* goes to next block */
		}
	}

	return ori_length - length; 
}



/** open_file_sys
  * DESCRIPTION: 	open system call for file system read the file system by given file name
  *					open the process control block in file descriptor
  * INPUT:			fname---file name
  * OUTPUT:			0 when seccess
  *					-1 when fail: the file descriptor is full or cannot find the file
  * SIDE EFFECT:	open a new pcb in file_desc
  */
int32_t open_file_sys(const uint8_t * fname){

	int i, j;	/* loop counter */
	uint32_t num_entries = s_block->dir_entries; /* variable hold # of entries */
	uint32_t length; /* variable to hold fname string length */
	length = strlen((int8_t*)fname);	

	for(i = 0; i < num_entries; i++){ /* looping through entire entries to find file*/
		if(strlen(s_block->file_entries[i].filename) == length){/* case 2 names doesnt have the same length*/
			if(strncmp((int8_t*)fname, s_block->file_entries[i].filename, length) == 0){ /* check if 2 are the same */
				/* using strncpy from lib to make deep copy*/
				int type;
				type= s_block->file_entries[i].file_type;
				printf("%s", s_block->file_entries[i].filename);
				printf("	file_type: %d", s_block->file_entries[i].file_type);
				printf("	inode num: %d\n", s_block->file_entries[i].inode_num);
				for(j=0;j<6;j++){
					if(file_desc[j+2].flags==0){
						if(type==0)	//set this pcb to rtc
						else if(type==1)	//set this pcb to directory
						else if(type==2)	//set this pcb to regular file
						file_desc[j+2].flags=1;
						break;	
					}
					else if(j==5&&file_desc[j+2].flags!=0){
						return -1; 	//array is full
					}
				}
				
				return 0; /* operation success*/
			}
		}

	}
	return -1; /* operation failed */
}

/** read_sys
  * DESCRIPTION: 	write system call
  * INPUT:
  * OUTPUT:			file, dir: 	return the number of bytes read
								return 0 if is the end of the file
					rtc:		return 0

  *					-1 when fail: cannot read
  * SIDE EFFECT:	
  */
int32_t read_sys(int32_t fd, void * buf, int32_t nbytes){
	//go to the corresponding read function and jump back return;
	return -1;
}

/** write_sys
  * DESCRIPTION: 	write system call
  * INPUT:
  * OUTPUT:			0 when seccess
  *					-1 when fail: cannot write
  * SIDE EFFECT:	
  */
int32_t write_sys(int32_t fd, const void * buf, int32_t nbytes){
	//go to the corresponding write function and jump back return;
	return -1;
}


/** write_dir
  * DESCRIPTION: 	write operation for directory
  * INPUT:
  * OUTPUT:			-1 because file sys is read only 
  * SIDE EFFECT:
  */
int32_t write_dir(){
	//read only return -1
	return -1;
}

/** write_file
  * DESCRIPTION: 	write operation for file
  * INPUT:
  * OUTPUT:			-1 because file sys is read only 
  * SIDE EFFECT:
  */
int32_t write_file(){
	//read only return -1
	return -1;
}

