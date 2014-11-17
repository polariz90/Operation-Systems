
#define ASM_SYS 1
#include "assembly_ops.h"
#define ASM     1
#include "x86_desc.h"

.text

# /*global variable declaration*/ 
.globl sys_call_linkage, _sys_call_linkage
.globl test_out
.globl stdout_ops, stdin_ops

	.align  4
sys_call_linkage:
_sys_call_linkage:
# /*callee setup */
	pushl	%ebp;
	movl	%esp,	%ebp;
# /* save all registers */
	pushl   %eax;
	pushl   %ecx;
	pushl   %edx;
	pushl	%ebx;
	pushl	%esi;
	pushl	%edi;
# /* check eax input valid*/
	cmpl $1, %eax		# Check that eax is greater than 1
	jl FAIL_EAX
	cmpl $10, %eax		# Check that eax is less than 10
	jg FAIL_EAX

	cmpl 	$1,%eax
	jne 	check_2
	pushl	%ebx
	jmp		continue_sys_call
 
check_2:
	cmpl 	$2,%eax
	jne		check_3
	pushl	%ebx
	jmp		continue_sys_call

check_3:
	cmpl 	$3,%eax
	jne		check_4
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	jmp		continue_sys_call

check_4:
	cmpl 	$4,%eax
	jne		check_5
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	jmp		continue_sys_call

check_5:
	cmpl 	$5,%eax
	jne		check_6
	pushl	%ebx
	jmp		continue_sys_call

check_6:
	cmpl 	$6,%eax
	jne		check_7
	pushl	%ebx
	jmp		continue_sys_call

check_7:
	cmpl 	$7,%eax
	jne		check_8
	pushl	%ecx
	pushl	%ebx
	jmp		continue_sys_call

check_8:
	cmpl 	$8,%eax
	jne		check_9
	pushl	%ebx
	jmp		continue_sys_call

check_9:
	cmpl 	$9,%eax
	jne		check_10
	pushl	%ecx
	pushl	%ebx
	jmp		continue_sys_call

check_10:
	cmpl 	$10,%eax
	je		continue_sys_call		
	movl 	$0,%eax
	jmp		continue_sys_call

continue_sys_call:	
	call *sys_call_jumptable(,%eax,4)

FAIL_EAX:
	movl $-1, 	%eax; 
	jmp LINK_RET;

LINK_RET:
	leave;
	iret;

sys_call_linkage_buttom:



# A jump table to C functions that implement the system calls themselves.
sys_call_jumptable:
	.long 0x0
	.long halt
	.long execute
	.long read
	.long write
	.long open
	.long close
	.long getargs
	.long vidmap
	.long set_handler
	.long sigreturn
sys_call_jumptable_bot:



stdin_ops:
_stdin_ops:
	.long terminal_open
	.long terminal_read
	.long stdin_write
	.long terminal_close

stdout_ops:
_stdout_ops:
	.long terminal_open
	.long stdout_read
	.long terminal_write
	.long terminal_close



# to_the_user_space()
# Description: In order to jump back to the user space we must return from
# the user level syscall interruption and rebuild the stack as shown below
# in  order to fake the return from interruption
#----TOP OF STACK----#
		#EIP
		#USER_CS
		#EFLAGS
		#USER_DS
# Inputs   :none
# Outputs  :none
# Registers: %EBX, %EAX
test_out:

	cli
	
	movl 4(%esp),%ebx 		# Extended Instruction Pointer -> ebx

	movl $USER_DS, %eax 	# Load segement selectors
	movw %ax, %gs 
	movw %ax, %fs  
	movw %ax, %ds 
	movw %ax, %es

	pushl %eax              # push the USER_DS to the top of the stack

	pushl $0x83FFFF0 		# push the User Space Extended Stack Pointer to the top of the stack

	pushf 					# push the flags to the stack
	popl %eax 				# pull them back down to...
	orl $0x4200, %eax 		# be altered appropriately
	pushl %eax 				# push the altered flags back onto the stack
	pushl $USER_CS 			# push the USER_CS to the top of the stack
	pushl %ebx 				# push the Extended Instruction Pointer to the top of the stack

	iret					#Return from interruption