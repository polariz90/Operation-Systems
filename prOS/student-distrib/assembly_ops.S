


.text

.globl sys_call_jmptable
.globl sys_call_linkage
.globl stdin_ops, stdout_ops



stdin_ops:
_stdin_ops:
	.long terminal_open
	.long 0x0
	.long 0x0
	.long 0x0

stdout_ops:
_stdout_ops:
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0

# need to add other operations here

sys_call_linkage:
_sys_call_linkage:
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
	jmp *sys_call_jumptable(,%eax,4)
sys_call_linkage_buttom:

sys_call_jumptable:
_sys_call_jumptable:
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
