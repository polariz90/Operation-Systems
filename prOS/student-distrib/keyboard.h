#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KB_IRQ	1

//extern void kb_init();
extern void kb_enable();
extern void kb_disable();
/*
unsigned char code_set[0x58] = {
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
};*/

#endif
