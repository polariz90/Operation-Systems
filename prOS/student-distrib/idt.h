/*H file containing the functions and relevent material for idt
 *
 *
 */

#ifndef _IDT_H
#define _IDT_H


extern volatile int rtc_flag;
/*idt functions*/
void init_idt();
void general_handler(); 


#define VID_MEM_IDX 184

/*interrupt specific handlers*/
void keyboard_handler();
void rtc_handler();
void pit_handler();

#endif /* _IDT_H  */





