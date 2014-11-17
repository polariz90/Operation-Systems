/**
  * file.c
  * Oct. 30, 2014
  * project prOS
  * 	File system c file, create basic inode file system
  */

#include "multiboot.h"
#include "file.h"
#include "lib.h"


/*extern var: file descriptor*/
file_entry file_desc[8];

/*set the default value of file descriptor*/

/* NOT SURE IF WE NEED THIS ANYMORE
void init_file_desc(void){
	int i;
	for(i=0;i<8;i++){
		file_desc[i].flags=0; //mark as unused
		file_desc[i].file_pos=0; 
	}
}
  */


/* init_pcb
 * Description:		Initializes a process control block with initial values
 * 					sets up the stdin and stdout
 *					May need to be expaned later to add more initilization features
 *	Input:			Pointer to pcb
 *	Output:			None
 *
 */
void init_pcb(pcb* curr_pcb)
{
	/* intilizing the stdin and stdout*/
	int i;
	for( i = 0; i < 8; i++){
		curr_pcb->file_descriptor[i].file_opt_ptr = NULL;
		curr_pcb->file_descriptor[i].inode_ptr = NULL;
		curr_pcb->file_descriptor[i].file_pos = 0;
		curr_pcb->file_descriptor[i].flags = 0;
	}
//	curr_pcb->file_descriptor[0].file_opt_ptr = stdin_ops;
//	curr_pcb->file_descriptor[1].file_opt_ptr = stdout_ops;

	return;	
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

	int8_t * input_arr[name_length]; /* new input string */
	if (length >= name_length){/* case too long */
		strncpy((int8_t*)input_arr, (int8_t*)fname, name_length-1);
		input_arr[name_length] = '\0';
	}
	else{
		strncpy((int8_t*)input_arr, (int8_t*)fname, name_length);
	}
	
	uint32_t input_length = strlen((int8_t*)input_arr); /* cutted array length */

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
	if(buf == NULL){
		return -1; /* case passed in invalid buffer */
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



/** open_sys
  * DESCRIPTION: 	open system call
  					for file system read the file system by given file name
  *					open the process control block in file descriptor
  * INPUT:			fname---file name
  * OUTPUT:			0 when seccess
  *					-1 when fail: the file descriptor is full or cannot find the file
  * SIDE EFFECT:	open a new pcb in file_desc
  */
int32_t open_sys(const uint8_t * fname){

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
						if(type==0){}	//set this pcb to rtc
						else if(type==1){}	//set this pcb to directory
						else if(type==2){}	//set this pcb to regular file


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

/** read_file
  * DESCRIPTION: 	read a file from file system
  * INPUT:			filename, buffer, number of bytes need to read 
  * OUTPUT:			 0 when success 
  * 				-1 when failed 
  * SIDE EFFECT: 	filling the buffer with file chars
  */
int32_t read_file( const int8_t* fname, void * buf, uint32_t nbytes){
	/* case invalid fname and buffer */
	if( fname == NULL || buf == NULL){
		printf("failed to read file \n");
		return -1; 
	}

	dentry_t file_dentry; /* dentry to hold inofrmation of this file */

	read_dentry_by_name((uint8_t*)fname, &file_dentry); /* read dentry by name */

	int ret = read_data(file_dentry.inode_num, 0, buf, nbytes); /* read file data into buffer */

	return ret;
}

/** write_sys
  * DESCRIPTION: 	write system call
  * INPUT:
  * OUTPUT:			0 when success
  *					-1 when fail: cannot write
  * SIDE EFFECT:	
  */
int32_t write_sys(int32_t fd, const void * buf, int32_t nbytes){
	//go to the corresponding write function and jump back return;
	return -1;
}

/** read_dir
  * DESCRIPTION: 	read the directory from file system
  * INPUT:			filename, buffer, number of bytes need to read 
  * OUTPUT:			 0 when success 
  * 				-1 when failed 
  * SIDE EFFECT: 	filling the buffer with file chars
  */
int32_t read_dir(int8_t* fname, uint8_t * buf, uint32_t nbytes){

	if(*fname!='.') {
		printf("fail to read directory\n");
		return -1;
	}

	int fd;
	int pos;
	/*only for this check point, set directory file descriptor fd=2*/
	fd=2;

	/*pos=0 is the dir entry*/
	file_desc[fd].file_pos++;
	pos=file_desc[fd].file_pos;
	
	if(nbytes>=strlen(s_block->file_entries[pos].filename)&&pos<=s_block->dir_entries){
		strcpy((int8_t*)buf, s_block->file_entries[pos].filename);
		return strlen(s_block->file_entries[pos].filename);
	}
	else if(pos>s_block->dir_entries){
		file_desc[fd].file_pos=0;
		return 0;
	}
	else{
		printf("fail to read directory\n");
		return -1;
	}
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

/** close_sys
  * DESCRIPTION: 	close this pcb on file descriptor
  * INPUT:			fd---file descriptor number
  * OUTPUT:			return 0 
  * SIDE EFFECT:
  */
int32_t close_sys(int32_t fd){
	//read only return -1
	file_desc[fd].flags=0;
	return 0;
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

/** open file
  * DESCRIPTION: 	open a file from file system
  * INPUT: 			file name
  * OUTPUT:         none
  * SIDE EFFECT:    none
  */
int32_t open_file( const uint8_t * filename){
	return 0;
}

/** open directory
  * DESCRIPTION:   open a directory in file system
  * INPUT: 		   file name
  * OUTPUT:		   none
  * SIDE EFFECT:   none
  */
int32_t open_dir( const uint8_t * filename){
	return 0;
}

/** close file
  * DESCRIPTION: 	close a file from file system
  * INPUT: 			file name
  * OUTPUT:         none
  * SIDE EFFECT:    none
  */
int32_t close_file(const uint8_t * filename){
	return 0;
}

/** close directory
  * DESCRIPTION:   close a directory in file system
  * INPUT: 		   file name
  * OUTPUT:		   none
  * SIDE EFFECT:   none
  */
int32_t close_dir(const uint8_t * filename){
	return 0;
}


/* add process stack
 * description:		creates space on the kernel stack for the pcb and the individual process, stack
 * input:			process number
 * 					may need to add details(pt and pd tables, parent process information)
 * output:			none
 *
 *
 *
 */
pcb* add_process_stack(uint8_t num )
{
	
	//Finds location of current pcb in kernel memory
	pcb* curr_pcb = (pcb*)(BOT_KERNEL_MEM - (num+1)*STACK_OFF);

	//initilizes current_pcb
	init_pcb(curr_pcb);
	
	return curr_pcb;
}

/*
 * read_file_img()
 *
 * Description:
 * Reads an executable file into buffer.
 *
 * Inputs:
 * fname: name of file
 * buffer: buffer
 *
 * Retvals:
 * -1: failure
 * 0: success
 */
int32_t read_file_img(const int8_t * fname, uint8_t* buffer)
{
	dentry_t file_dentry;

	if( (fname == NULL) ||
		(read_dentry_by_name((uint8_t *) fname, &file_dentry) == -1 ) ||
		( read_data(file_dentry.inode_num, 0, (uint8_t*) buffer, four_kb) ))
	{
		return -1;
	}
	
	return 0;
}

/*
 * load_file_img()
 *
 * Description:
 * Loads an executable file into memory and prepares to begin the new process.
 *
 * Inputs:
 * fname: name of file
 *
 * Retvals:
 */
int load_file_img(int8_t* fname)
{

	dentry_t file_dentry; /* file_dentry to hold file */
	uint32_t offset = 0; /* offset of read file */
	uint32_t last_chunk = 0; /* last chunk of space to copy*/
	uint8_t buff[20] ; /* buffer to hold copy data */
	void* load_ptr; /* memory address pointer */
	int output; /* hold output value */
	int i; /* loop counter */

	load_ptr = file_vir_addr;
	read_dentry_by_name((uint8_t *) fname, &file_dentry);


	uint32_t dentry_add = (uint32_t)s_block + four_kb; /*first dentry block address */
	inode_struct * curr_inode =(inode_struct*)(dentry_add + file_dentry.inode_num*four_kb);

	

	do{
		output = read_data(file_dentry.inode_num, offset, (uint8_t*) buff, 20);

		if(output == 0){/* case hit the end of the file */
			last_chunk = curr_inode->length - offset;
			memcpy(load_ptr, buff, last_chunk); 
		}
		else if (output == -1){
			printf("File load failed \n");
			return -1;
		}
		else{ /* else case, load 20 */
			for(i = 0; i < 20; i++){
				printf("%x ", buff[i]);
			}
			printf("\n");
			memcpy(load_ptr, buff, 20);

			offset += 20;
			load_ptr += 20;
		}
	}while(output != 0);

	return 0;
	
}

