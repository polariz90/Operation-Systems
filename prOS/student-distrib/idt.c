/* This file will contain the implementation of
 * all of the interrupt handlers and the init
 * of the idt
 *
 * The general procedure for handling an interrupt is to push all general purpose registers 
 * then do handler function, then pop all registers, then leave and iret.
 * If this is too slow optimizations will be made
 *
 *
 *
 */


#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"
#include "i8259.h"
#include "rtc.h"
#include "exception.h"
#include "terminal.h"
#include "clock.h"
#include "assembly_ops.h"
#include "pit.h"
#include "scheduler.h"
#include "file.h"

#define NUM_COLS 80
#define NUM_ROWS 25


#define eight_mb 			0x800000
#define eight_kb 			0x2000

volatile int flag;
 unsigned char code_set[0x59];
 unsigned char code_set_shift[0x59];


/*
 * This function initializes every interrupt descriptor table to enter 
 * general handler. If there are special exceptions, it will jump to the 
 * corresponding table.	
 *
 */
void init_idt()
{
	//setting elements that are constant among the exceptions
	int i;
	for(i =0; i< NUM_VEC; i++ )
	{
		idt[i].seg_selector = KERNEL_CS;
		idt[i].dpl = 0;
		idt[i].size = 1;
		idt[i].present = 1;
		idt[i].reserved4 = 0; 
		idt[i].reserved3 = 0;
		idt[i].reserved2 = 1;
		idt[i].reserved1 = 1;
		idt[i].reserved0 = 0 ;
	   	SET_IDT_ENTRY(idt[i],general_handler);
	}
	idt[128].dpl = 0x3;

	//initializing the specific vector values
	SET_IDT_ENTRY(idt[0], div_err_excpn_0); 			//divide by 0
	SET_IDT_ENTRY(idt[1], debug_excpn_1);
	SET_IDT_ENTRY(idt[2],NMI_excpn_2);
	SET_IDT_ENTRY(idt[3],brkpnt_excpn_3);
	SET_IDT_ENTRY(idt[4],overflow_excpn_4);
	SET_IDT_ENTRY(idt[5],bound_range_exceed_excpn_5);
	SET_IDT_ENTRY(idt[6],invalid_opcode_excpn_6);
	SET_IDT_ENTRY(idt[7],device_unvailable_excpn_7);
	SET_IDT_ENTRY(idt[8],dbl_fault_excpn_8);
	SET_IDT_ENTRY(idt[9],coproc_sgmt_excpn_9);
	SET_IDT_ENTRY(idt[10],invalid_TSS_excpn_10);
	SET_IDT_ENTRY(idt[11],sgmt_not_present_excpn_11);
	SET_IDT_ENTRY(idt[12],stack_fault_excpn_12);
	SET_IDT_ENTRY(idt[13],gen_protection_excpn_13);
	SET_IDT_ENTRY(idt[14],page_fault_excpn_14);
	SET_IDT_ENTRY(idt[16],FPU_F_P_excpn_16);
	SET_IDT_ENTRY(idt[17],algnmnt_chk_excpn_17);
	SET_IDT_ENTRY(idt[18],machine_chk_excpn_18);
	SET_IDT_ENTRY(idt[19],SIMD_F_P_excpn_19);

	//initilizing the other idt 

	SET_IDT_ENTRY(idt[32], pit_handler);
	SET_IDT_ENTRY(idt[33], keyboard_handler);     			//keyboard 
	SET_IDT_ENTRY(idt[40], rtc_handler);     				//rtc 

//	SET_IDT_ENTRY(idt[128], sys_call_handler);			//sys call jumptable
	SET_IDT_ENTRY(idt[128], sys_call_linkage);
}

/*
 *	
 *	handler that handles general inturrupt
 *
 */
void general_handler()
{
		printf("Has not been intilized to handle this interrupt\n");
		asm("leave;iret");

}


/* Description:
 * Handler for the rtc interruption. While those other interrupts are being handled
 * (until your OS sends an EOI and STI), your OS will not receive any clock ticks.
 * What is important is that if register C is not read after an IRQ 8, then the interrupt will not happen again.
 *
 * Exception Class:
 *
 *
 * Exception Error Code:
 * 
 *
 * Saved Instruction Pointer:
 * 
 */
void rtc_handler()
{
	asm("pushal");
	outb(0x0C, RTC_PORT);	// select register C
	inb(RTC_CMOS_PORT);	
	send_eoi(RTC_IRQ);
	flag = 0;
	/* timer implementation */
	update_time();

	sti();
	asm("popal;leave;iret");
}




/* Description:
 * Handler for the keyboard interruption. The function receives keyboard signals and converts 
 * the signal into keys to be written.
 *
 * Exception Class:
 *
 *
 * Exception Error Code:
 * 
 *
 * Saved Instruction Pointer:
 * 
 */
void pit_handler()
{
	cli();
	int i=0;
	while(i<1000000){i++;}

	asm("pushal");
	send_eoi(PIT_IRQ);
	
	//call scheduler
	int next_pid= scheduler();

	//store TSS
	int temp = eight_kb*next_pid;
	tss.esp0= eight_mb - temp - 4;

	//store current esp, ebp
	pcb* current_pcb = getting_to_know_yourself(); /* geeting current pcb*/
	asm("movl %%esp, %%eax  \n  \
		movl %%ebp, %%ebx"
		: "=a"(current_pcb->current_esp), "=b"(current_pcb->current_ebp)
		:
		: "cc", "memory");

	//jump to next kernel stack
	pcb* next_pcb = getting_the_ghost(next_pid);
	asm("movl %%eax, %%esp  \n  \
		movl %%ebx, %%ebp"
		: 
		: "a"(next_pcb->current_esp), "b"(next_pcb->current_ebp)
		: "cc", "memory");
	sti();
	//asm("popal;leave;iret");
	asm("popal");
	asm("leave");
	asm("iret");
}

/* Description:
 * Handler for the keyboard interruption. The function receives keyboard signals and converts 
 * the signal into keys to be written.
 *
 * Exception Class:
 *
 *
 * Exception Error Code:
 * 
 *
 * Saved Instruction Pointer:
 * 
 */
void keyboard_handler()
{
	asm("pushal");

	//reading from the keyboard port and sending the end of interrut signal	
	unsigned char temp = inb(KEYBOARD_PORT); 				//get signal from the keyboard
	send_eoi(KB_IRQ);

	//status of the keys
	int shift = terminals[curr_terminal].shift;
	int caps  = terminals[curr_terminal].caps;

	/*checking for the sepecial cases*/
	if(is_special_key((int)temp) == 1)
	{
		exe_special_key((int)temp);
	}

	/*Writing to the buffer if it is a valid character*/
	else if(terminals[curr_terminal].size < BUF_SIZE && (int) temp <= 58)
	{

		//prints the shifted key
		if (shift ==1)
		{
			//writes into the buffer
			terminals[curr_terminal].buf[terminals[curr_terminal].size] = code_set_shift[(int)temp];
			terminals[curr_terminal].size++;
		  	printt(code_set_shift[(int)temp]);

		}

		//non shifted version keys 
		else
		{
			//writes the buffer	
			terminals[curr_terminal].buf[terminals[curr_terminal].size] =  code_set[(int)temp] - ((caps+shift)%2)*(CAPS_CONV);
			terminals[curr_terminal].size++;
		  	printt(code_set[(int)temp] - ((caps+shift)%2)*(CAPS_CONV));

		}

	}	
	//printf("%d",(int) temp);

 	asm("popal;leave;iret");
}


/* Description:
 * Code set table of keyboard keys.
 * Signals recieved from keyboard will be converted via the table below.
 */
unsigned char code_set[0x59] = {
	'\0','\e','1','2','3','4','5','6','7','8','9','0','-','=','\b',
	'\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
	'\0','a','s','d','f','g','h','j','k','l',';','\'','`',
	'\0','\\','z','x','c','v','b','n','m',',','.','/','\0',	
	'*','\0',' ','\0',
	'\0',		// F1
	'\0',		// F2
	'\0',		// F3
	'\0',		// F4
	'\0',		// F5
	'\0',		// F6
	'\0',		// F7
	'\0',		// F8
	'\0',		// F9
	'\0',		// F10
	'\0',		// num lock
	'\0',		// scroll lock
	'7','8','9','-','4','5','6','+','1','2','3','0','.',//keypad
	'\0',
	'\0',
	'\0',
	'\0',		// F11
	'\0',		// F12
};


/* Description:
 * Code set table of keyboard keys when shift is held down.
 * Signals recieved from keyboard will be converted via the table below.
 */
unsigned char code_set_shift[0x59] = {
	'\0','\e','!','@','#','$','%','^','&','*','(',')','_','+','\b',
	'\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
	'\0','A','S','D','F','G','H','J','K','L',':','\'','`',
	'\0','\\','Z','X','C','V','B','N','M','<','>','?','\0',	
	'*','\0',' ','\0',
	'\0',		// F1
	'\0',		// F2
	'\0',		// F3
	'\0',		// F4
	'\0',		// F5
	'\0',		// F6
	'\0',		// F7
	'\0',		// F8
	'\0',		// F9
	'\0',		// F10
	'\0',		// num lock
	'\0',		// scroll lock
	'7','8','9','-','4','5','6','+','1','2','3','0','.',//keypad
	'\0',
	'\0',
	'\0',
	'\0',		// F11
	'\0',		// F12
};





