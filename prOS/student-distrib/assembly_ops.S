# assembly_ops.S - contains the operation tables for the file descriptor tables, also contains the jumptable for system calls
# vim:ts=4 noexpandtab


.text

.globl sys_call_jmptable
.globl stdin_ops, stdout_ops
# unsure if I need this, included it due to its location in x86_desc.S


#	Operation jumptables are constructred in this format
#	0:	open
#	1:	read
#	2:	write
#	3:	close


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
	pushal
	jmp *sys_call_jmptable(,%eax,4)

sys_call_jmptable:
_sys_call_jmptable:
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


