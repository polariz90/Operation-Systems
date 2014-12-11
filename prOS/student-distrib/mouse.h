#ifndef _MOUSE_H
#define _MOUSE_H

#include "lib.h"
#include "x86_desc.h"

#define KEYBOARD_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define M_IRQ	12

extern uint8_t curr_char;
extern uint8_t curr_attrib;
extern double  pos_x;
extern double  pos_y;
extern uint8_t mouse_buf[four_kb];
extern uint8_t mouse_click_flag;
extern uint8_t Bazinga[128];


extern void mouse_();
//extern void kb_init();
extern void mouse_enable();
extern void mouse_disable();
extern void mouse_write(unsigned char port);
extern unsigned char mouse_read();
extern void mouse_wait(unsigned char signal);
extern void mouse_install();
extern void set_pointer();
extern unsigned char mouse_read();
extern void mouse_rclick();
extern void mouse_lclick();

extern void mouse_r_click();
extern void mouse_l_click();

#endif
