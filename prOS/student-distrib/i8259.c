/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask=0xff; /* IRQs 0-7 */
uint8_t slave_mask=0xff; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void)
{

	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);

	outb(ICW2_MASTER, MASTER_8259_PORT+1);
	outb(ICW2_SLAVE  , SLAVE_8259_PORT+1);

	outb(ICW3_MASTER, MASTER_8259_PORT+1);
	outb(ICW3_SLAVE, SLAVE_8259_PORT+1);

	outb(ICW4, MASTER_8259_PORT+1);
	outb(ICW4, SLAVE_8259_PORT+1);

	outb(master_mask, MASTER_8259_PORT+1);
	outb(slave_mask, SLAVE_8259_PORT+1);

	enable_irq(2);	//enable slave pic
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{

	if(irq_num < 8)
	{
		outb(inb(MASTER_8259_PORT+1) & ~(1 << irq_num), MASTER_8259_PORT+1);
	}
	else
	{
		irq_num -= 8;
		outb(inb(SLAVE_8259_PORT+1) & ~(1 << irq_num), SLAVE_8259_PORT+1);
	}

}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
	if(irq_num < 8)
	{
		outb(inb(MASTER_8259_PORT+1) | ~(1 << irq_num), MASTER_8259_PORT+1);
	}
	else
	{
		irq_num -= 8;
		outb(inb(SLAVE_8259_PORT+1) | ~(1 << irq_num), SLAVE_8259_PORT+1);
	}
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{

	if(irq_num < 8)
	{
		outb(EOI | irq_num, MASTER_8259_PORT);
	}
	else
	{
		outb( EOI | (irq_num - 8), SLAVE_8259_PORT );
		outb( EOI + 2, MASTER_8259_PORT);
	}

}

