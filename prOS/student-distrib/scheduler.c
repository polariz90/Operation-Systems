#include "lib.h"
#include "scheduler.h"
#include "sys_call.h"

int current_pid;

void scheduler(void){
	int i;	

	//check running process
	if(process_occupy.num_process==0){
		return;
	}
	else
	{
		for (i = 0; i < 6; ++i)
		{
			/* code */
			int pid=i+1; //exclude kernel process
			if(process_occupy.occupied[pid]==1){

			}
		}
	}


	return;
}