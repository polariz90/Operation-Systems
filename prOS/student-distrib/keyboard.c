/* This file will contain the implementation of
 * initialization of keyboard usage.
 *
 * The general procedure of keyboard initialization is to call PIC function 
 * which enables and disabes certain irq. In this case, irq1.
 */

#include "keyboard.h"
#include "i8259.h"


/*
 * Enables the irq1 for keyboard
 */
extern void kb_enable()
{
	enable_irq(KB_IRQ);
}

/*
 * Disables the irq1 for keyboard
 */
extern void kb_disable()
{
	disable_irq(KB_IRQ);
}
