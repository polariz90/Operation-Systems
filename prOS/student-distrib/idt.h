/*H file containing the functions and relevent material for idt
 *
 *
 *
 *
 *
 */

#ifndef _IDT_H
#define _IDT_H

extern volatile int flag;
/*idt functions*/
void init_idt(); void general_handler(); 


/*interrupt specific handlers*/
void divide_error_exception(void);
void debug_exception();
void nmi();
void breakpoint_exception();
void overflow_exception();
void bound_range_exception();
void invalid_opcode();
void device_not_availible();
void double_fault_exception(); 
void invalid_tss_exception();
void keyboard_handler();
void rtc_handler();




#endif /* _IDT_H  */
