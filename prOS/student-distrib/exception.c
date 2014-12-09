#include "exception.h"
#include "lib.h"
#include "file.h"
#include "x86_desc.h"

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
void div_err_excpn_0()
{
	printf("Invalid divisor operand(#0)");
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
void debug_excpn_1()
{
	printf("One or more debug-exception conditions detected(#1)");
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
void NMI_excpn_2()
{
	printf("Nonmaskable interrupt generated(#2)");
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
void brkpnt_excpn_3()
{
	printf("breakpoint instruction executed(#3)");
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
void overflow_excpn_4()
{
	printf("Overflow trap occured(#4)");
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
void bound_range_exceed_excpn_5()
{
	printf("Bound-range-exceeded fault occurred(#5)");
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
void invalid_opcode_excpn_6()
{
	printf("Invalide opcode(#6)");
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
void device_unvailable_excpn_7()
{
	printf("Devide is not available(#7)");
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
void dbl_fault_excpn_8()
{
	printf("Second exception detected(#8)");
	asm("leave;iret");
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
void coproc_sgmt_excpn_9()
{
	printf("Page or segment violation(#9)");
	asm("leave;iret");
	return;
}


void invalid_TSS_excpn_10()
{
	printf("TSS error(#10)");
	asm("leave;iret");
	return;
}
void sgmt_not_present_excpn_11()
{
	printf("Present flag of a segment or gate descriptor i1)");
	asm("leave;iret");
	return;
}
void stack_fault_excpn_12()
{
	printf("Stack fault condition detected(#12)");
	asm("leave;iret");
	return;
}
void gen_protection_excpn_13()
{
	printf("General protection violation detected(#13)");

	asm("leave;iret");
	return;
}
void page_fault_excpn_14()
{
	pcb* current_pcb = getting_to_know_yourself(); // geeting current pcb
	video_page_table[current_pcb->pid].dir_arr[184].page_base_add = 184;
	flush_tlb();
	printf("Page fault detected(#14)");
	blue_screen();
	while(1){}
	asm("leave;iret");
	return;
}
void FPU_F_P_excpn_16()
{
	printf("Floating point error detected(#16)");
	asm("leave;iret");
	return;
}
void algnmnt_chk_excpn_17()
{
	printf("Unaligned memory operand detected(#17)");
	asm("leave;iret");
	return;
}
void machine_chk_excpn_18()
{
	printf("Internal machine or bus error detected(#18)");
	asm("leave;iret");
	return;
}
void SIMD_F_P_excpn_19()
{
	printf("SSE/SSE2/SSE3 SIMD floating-point exception detected(#19)");
	asm("leave;iret");
	return;
}
