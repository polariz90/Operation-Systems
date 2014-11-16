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
	//send ICW1 bytes to first master and slave port to initialization
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);

	//send ICW2 bytes to second master and slave port to initialization
	outb(ICW2_MASTER, MASTER_8259_PORT+1);
	outb(ICW2_SLAVE  , SLAVE_8259_PORT+1);

	//send ICW3 bytes to master and slave port to initialization
	outb(ICW3_MASTER, MASTER_8259_PORT+1);
	outb(ICW3_SLAVE, SLAVE_8259_PORT+1);

	//send ICW4 bytes to master and slave port to initialization
	outb(ICW4, MASTER_8259_PORT+1);
	outb(ICW4, SLAVE_8259_PORT+1);

	//masks all the interrupts
	outb(master_mask, MASTER_8259_PORT+1);
	outb(slave_mask, SLAVE_8259_PORT+1);

	enable_irq(2);	//enable slave pic
}

/* Enable (unmask) the specified IRQ 
 * Specified IRQ will be enabled.
 */
void
enable_irq(uint32_t irq_num)
{

	if(irq_num < 8)	//if irq_num is 0 ~ 7, bit unmask the specified IRQ and send to master PIC
	{
		outb(inb(MASTER_8259_PORT+1) & ~(1 << irq_num), MASTER_8259_PORT+1);
	}
	else			//if irq_num is 8 ~ 15, bit unmask the specified IRQ and send to slave PIC
	{
		irq_num -= 8;
		outb(inb(SLAVE_8259_PORT+1) & ~(1 << irq_num), SLAVE_8259_PORT+1);
	}

}

/* Disable (mask) the specified IRQ 
 * Specified IRQ will be disabled.
 */
void
disable_irq(uint32_t irq_num)
{
	if(irq_num < 8)	//if irq_num is 0 ~ 7, bit mask the specified IRQ and send to master PIC
	{
		outb(inb(MASTER_8259_PORT+1) | ~(1 << irq_num), MASTER_8259_PORT+1);
	}
	else			//if irq_num is 0 ~ 7, bit mask the specified IRQ and send to slave PIC
	{
		irq_num -= 8;
		outb(inb(SLAVE_8259_PORT+1) | ~(1 << irq_num), SLAVE_8259_PORT+1);
	}
}

/* Send end-of-interrupt signal for the specified IRQ 
 * interrupt will be enabled for lower priorit IRQs
 */
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

