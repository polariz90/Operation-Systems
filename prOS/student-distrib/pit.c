#include "lib.h"
#include "pit.h"
#include "i8259.h"

extern void pit_enable()
{
/*	outb(PIT_B, PIT_PORT);		// select register B, and disable NMI
	char prev=inb(PIT_CMOS_PORT);	// read the current value of register B
	outb(PIT_B, PIT_PORT);		// set the index again (a read will reset the index to register D)
	outb(prev | 0x40, PIT_CMOS_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
*/	enable_irq(PIT_IRQ);
}

extern void pit_disable()
{
	disable_irq(PIT_IRQ);
}


/*
 * PIT_read()
 *
 * Always return 0 only after an interrupt has occurred. 
 * (set a flag and wait until the interrupt handler clears it, then 
 * return 0).
 *
 * Inputs: none
 * Retvals: none
 */
extern int32_t PIT_read()
{	
	sti();
	//flag = 1;
	//while(flag);
	//printf("i am read\n");
	return 0;
}

/*
 * PIT_write()
 *
 * Should always accept only a 4-byte integer specifying the interrupt 
 * rate in Hz, and should set the rate of periodic interrupts accordingly.
 *
 * Inputs: 
 * buf: hz to be set
 * nbytes: number of bytes to set
 * Retvals
 * -1: failure
 * n: number of bytes written
 */
extern int32_t PIT_write(int fd, const int32_t* buf, int32_t nbytes)
{
	unsigned char prev_a; // temporary 
	int8_t freq;
	int32_t new_buf;

	//condition which write fails
	if(nbytes != 4 || buf == NULL)
	{
		return -1;
	}

	new_buf = *buf;

	//save old values
	outb(PIT_A, PIT_PORT);
	prev_a = inb(PIT_CMOS_PORT);

	//PIT frequency limited up to 1024
	if(new_buf > 1024) {return -1;}
	else
	{
		if(new_buf == 1024) {freq = F1024HZ;}
		if(new_buf == 512) {freq = F512HZ;}
		if(new_buf == 256) {freq = F256HZ;}
		if(new_buf == 128) {freq = F128HZ;}
		if(new_buf == 64) {freq = F64HZ;}
		if(new_buf == 32) {freq = F32HZ;}
		if(new_buf == 16) {freq = F16HZ;}
		if(new_buf == 8) {freq = F8HZ;}
		if(new_buf == 4) {freq = F4HZ;}
		if(new_buf == 2) {freq = F2HZ;}
		if(new_buf == 0) {freq = NONE;}
	}

	outb(PIT_A, PIT_PORT);
	outb((0xF0 & prev_a) | freq, PIT_CMOS_PORT);

	//write success!! (always 0) */
	return 0;
	
}

/*
 * PIT_open()
 *
 * Initialize PIT by setting the frequency to 2hz
 *
 * Inputs: 
 * Outputs:
 */
extern int32_t PIT_open()
{
	outb(PIT_A, PIT_PORT);
	char prev_b = inb(PIT_CMOS_PORT);
	outb(PIT_A, PIT_PORT);
	outb((0xF0 & prev_b) | F2HZ, PIT_CMOS_PORT);
	return 0;
}

/*
 * PIT_close()
 *
 * returns 0
 *
 * Inputs: 
 * Outputs:
 */
/*extern int32_t PIT_close()
{
	PIT_disable();
	return 0;
}
*/