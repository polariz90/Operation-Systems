.global	call_keyboard_handler
.align 4

call_keyboard_handler:
	pushal
	call keyboard_handler
	popal
	iret

.global call_general_handler
.align 4
call_general_handler:

	pushal
	call general_handler
	popal
	iret