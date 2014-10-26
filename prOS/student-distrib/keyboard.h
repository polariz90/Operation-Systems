#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KB_IRQ	1

//extern void kb_init();
extern void kb_enable();
extern void kb_disable();



#endif