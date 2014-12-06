#include "lib.h"
#include "scheduler.h"
#include "sys_call.h"
#include "file.h"

int scheduler(void){
	int i;

	//check running process
	if(process_occupy.num_process==0){
		return -1;
	}
	else
	{
		pcb* current_pcb = getting_to_know_yourself(); /* geeting current pcb*/
		int current_pid= current_pcb->pid;
		for (i = 0; i < 7; ++i)
		{
			/* code */
			//get next process pid
			int pid= (current_pid+i+1) % 7; //exclude kernel process
			
		//	if(process_occupy.occupied[pid]==1&&process_occupy.top_process_flag[pid]==1){
				return pid;
		//	}
		}
	}

	//should nerver get here
	return -1;
}