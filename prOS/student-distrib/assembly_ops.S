#define ASM_SYS 1
#include "assembly_ops.h"
#include "sys_call.h"

.text

.globl stdin_ops,stdout_ops
.globl sys_call_linkage 

sys_call_linkage:
_sys_call_linkage:
	pushl %eax
	pushl %eax
	pushl %eax
	pushl %eax
	jmp *sys_call_jumptable(,%eax,4)
sys_call_linkage_bot:

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

stdin_ops:
_stdin_ops:
	.long terminal_open
	.long 0x0
	.long 0x0
	.long 0x0
stdin_ops_bot:


stdout_ops:
_stdout_ops:
	.long 0x0
	.long 0x0
	.long 0x0
	.long 0x0
stdout_ops_bot:

