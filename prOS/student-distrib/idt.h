/*H file containing the functions and relevent material for idt
 *
 *
 *
 *
 *
 */

#ifndef _IDT_H
#define _IDT_H

/*idt functions*/
void init_idt();
void general_handler(); 


/*interrupt specific handlers*/
void keyboard_handler();
void rtc_handler();




#endif /* _IDT_H  */
