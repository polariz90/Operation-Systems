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

 unsigned char code_set[0x59];

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

	SET_IDT_ENTRY(idt[33], keyboard_handler);     			//keyboard 
	SET_IDT_ENTRY(idt[40], rtc_handler);     				//rtc 

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
	test_interrupts();
	outb(0x0C, RTC_PORT);	// select register C
	inb(RTC_CMOS_PORT);	
	send_eoi(RTC_IRQ);
	sti();
}

/* Description:
 * Divisor operand is zero
 *
 * Exception Class:
 * Fault
 *
 * Exception Error Code:
 * None
 *
 * Saved Instruction Pointer:
 * Saved contents fo CS and EIP registers point to the instruction that generated the exeption
 *
 * Program State Change:
 * No state change
 */
void divide_error_exception()
{
	//asm("pushad");
	asm("leave;iret");
}



/* Description:
 * Debug exception has occurred
 *
 * Exception Class:
 * Trap
 *
 * Exception Error Code:
 * None 
 *
 * Saved Instruction Pointer:
 * Fault- Saved contents of the CS and EIP registers point to function that generated the exception
 * Trap- Saved contents of the CS and EIP registers point to following function that generated the exception
 *
 * Program State Change:
 * Fault- No program change
 * Trap- No program change 
 */
void debug_exception()
{
	printf("Debug exception\n");
	asm("leave;iret");
	return;
}



/* Description:
 * A nonmaskable interrupt has been generated
 *
 * Exception Class:
 * N/a
 *
 * Exception Error Code:
 * None
 *
 * Saved Instruction Pointer:
 * 
 *
 * Program State Change:
 * 
 */
void nmi()
{
	printf("Nonmaskable interrupt\n");
	asm("leave;iret");
	return;	
}



/* Description:
 * Break point exception has been generated. INT 3 was called
 *
 * Exception Class:
 * Trap
 *
 * Exception Error Code:
 * None
 *
 * Saved Instruction Pointer:
 * Saved contents of CS and EIP point to the instruction after INT 3
 *
 * Program State Change:
 * Essentially unchanged
 */
void breakpoint_exception()
{
	printf("Breakpoint Exception\n");
	asm("leave;iret");
	return;	
}



/* Description:
 * Overflow trap occurred when an INTO instruction was executed
 *
 * Exception Class:
 * Trap
 *
 * Exception Error Code:
 * None
 *
 * Saved Instruction Pointer:
 * Saved contents fo the CS and EIP point to the instruction following hte INTO
 *
 * Program State Change:
 * Essentially unchanged
 */
void overflow_exception()
{
	printf("Overflow exception\n");
	asm("leave;iret");
	return;	
}




/* Description:
 * Bound range fault occurred when a BOUND instruction was executed
 *
 * Exception Class:
 * Fault
 *
 * Exception Error Code:
 * None
 *
 * Saved Instruction Pointer:
 * Saved contents fo the CS and EIP point to the BOUND instruction that generated teh exception
 *
 * Program State Change:
 * None
 */
void bound_range_exception()
{
	printf("Bound range exception\n");
	asm("leave;iret");
	return;	
}




/* Description:
 * Attempted to execute invalid or reserved opcode
 * Attempted to execute an instruction with an operand type that is invalid
 * A variety of other reasons(see p.171 manual)
 *
 * Exception Class:
 * Fault
 *
 * Exception Error Code:
 * None 
 *
 * Saved Instruction Pointer:
 * Saved contents fo the CS and EIP point to the instruction that generated the exception 
 *
 * Program State Change:
 * None
 */
void invalid_opcode()
{
	printf("Invalid opcode\n");
	asm("leave;iret");
	return;	
}


/* Description:
 * Device not availble 
 *
 * Exception Class:
 * Fault
 *
 * Exception Error Code:
 * None 
 *
 * Saved Instruction Pointer:
 * Saved contents fo the CS and EIP point to the instruction or the WAIT/FWAIT that generated the exception
 *
 * Program State Change:
 * None
 */
void device_not_availible()
{
	printf("Device not availible\n");
	asm("leave;iret");
	return;	
}



/* Description:
 * A second exception was generated during the handling of the first
 *
 * Exception Class:
 * Abort
 *
 * Exception Error Code:
 * Zero is pushed onto the stack
 *
 * Saved Instruction Pointer:
 * Saved contents fo the CS and EIP are undefined 
 *
 * Program State Change:
 * Undefined 
 */
void double_fault_exception()
{
	printf("Double fault exception\n");
	asm("leave;iret");
	//should push zero on the stack here
	
	return;	
}



/* Description:
 * 
 *
 * Exception Class:
 *
 *
 * Exception Error Code:
 * 
 *
 * Saved Instruction Pointer:
 * Saved contents fo the CS and EIP point to the instruction
 *
 * Program State Change:
 * 
 */
void invalid_tss_exception()
{
	printf("\n");
	asm("leave;iret");
	return;	
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
	
	unsigned char temp = inb(KEYBOARD_PORT); 				//get signal from the keyboard
	if((int)temp<=58)	printf("%c", code_set[(int)temp]);	//print a key thay corresponds to the signal
	send_eoi(KB_IRQ);										//send PIC end of interrupt
	
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
 * 
 *
 * Exception Class:
 *
 *
 * Exception Error Code:
 * 
 *
 * Saved Instruction Pointer:
 * Saved contents fo the CS and EIP point to the instruction
 *
 * Program State Change:
 * 
 */

