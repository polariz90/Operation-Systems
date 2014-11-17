
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

	cmpl 	$1,%eax
	jne 	check_2
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	jmp		continue_sys_call
 
check_2:
	cmpl 	$2,%eax
	jne		check_3
	pushl	%edx
	pushl	%ecx
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
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	jmp		continue_sys_call

check_6:
	cmpl 	$6,%eax
	jne		check_7
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	jmp		continue_sys_call

check_7:
	cmpl 	$7,%eax
	jne		check_8
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	jmp		continue_sys_call

check_8:
	cmpl 	$8,%eax
	jne		check_9
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	jmp		continue_sys_call

check_9:
	cmpl 	$9,%eax
	jne		check_10
	pushl	%edx
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



