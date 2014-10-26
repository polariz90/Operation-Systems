#include "keyboard.h"
#include "i8259.h"
/*
extern void kb_init()
{

}
*/
extern void kb_enable()
{
	enable_irq(KB_IRQ);
}

extern void kb_disable()
{
	disable_irq(KB_IRQ);
}