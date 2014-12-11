
#define ASM_SYS 1
#include "assembly_ops.h"
#define ASM     1
#include "x86_desc.h"

.text

# /*global variable declaration*/ 
.globl sys_call_linkage, _sys_call_linkage
.globl test_out

	.align  4
sys_call_linkage:
_sys_call_linkage:
    cli
# /*callee setup */
	pushl	%ebp
	movl	%esp,	%ebp
# /* save all registers */
	pushl   %ecx
	pushl   %edx
	pushl	%ebx
	pushl	%esi
	pushl	%edi
# /* pushing on parameters */
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
# /* check eax input valid*/
	cmpl $1, %eax		# Check that eax is greater than 1
	jl FAIL_EAX
	cmpl $10, %eax		# Check that eax is less than 10
	jg FAIL_EAX

	pushl	%edx
	pushl	%ecx
	pushl	%ebx

continue_sys_call:	
	call *sys_call_jumptable(,%eax,4)
	addl $12, %esp
	jmp LINK_RET

FAIL_EAX:
	movl $-1, 	%eax
	addl $12, %esp
	jmp LINK_RET

LINK_RET:
# /* pop off parameters */
	popl	%ebx 
	popl 	%ecx 
	popl	%edx 
#	/*pop all registers*/
	popl	%edi
	popl	%esi
	popl	%ebx
	popl   %edx
	popl   %ecx
 
 	sti 
 	
	leave;
	iret;

sys_call_linkage_buttom:



# A jump table to C functions that implement the system calls themselves.
sys_call_jumptable:
	.long FAIL_EAX
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



