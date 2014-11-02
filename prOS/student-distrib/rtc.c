#include "i8259.h" 
#include "rtc.h"
#include "lib.h"
#include "idt.h"


extern void rtc_enable()
{
	outb(0x8B, RTC_PORT);		// select register B, and disable NMI
	char prev=inb(RTC_CMOS_PORT);	// read the current value of register B
	outb(0x8B, RTC_PORT);		// set the index again (a read will reset the index to register D)
	outb(prev | 0x40, RTC_CMOS_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
	enable_irq(RTC_IRQ);
}

extern void rtc_disable()
{
	disable_irq(RTC_IRQ);
}


/*
 * rtc_read()
 *
 * Always return 0 only after an interrupt has occurred. 
 * (set a flag and wait until the interrupt handler clears it, then 
 * return 0).
 *
 * Inputs: none
 * Retvals: none
 */
extern int32_t rtc_read()
{	
	flag = 1;
	volatile while(flag);

	return 0;
}

/*
 * rtc_write()
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
extern int32_t rtc_write(const int32_t* buf, int32_t nbytes)
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
	outb(0x8B, RTC_PORT);
	prev_a = inb(RTC_CMOS_PORT);

	//rtc frequency limited up to 1024
	if(new_buf > 1024)
	{
		return -1;
	}
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
	}

	outb(0x8B, RTC_PORT);
	outb((0xF0 & prev_a) | freq, RTC_CMOS_PORT);

	printf("freq/n");

	//write success!! (always 0) */
	return 0;
	
}

/*
 * rtc_open()
 *
 * Initialize RTC by setting the frequency to 2hz
 *
 * Inputs: 
 * Outputs:
 */
extern int32_t rtc_open(const uint8_t* filesname)
{
	outb(0x8B, RTC_PORT);
	char prev_b = inb(RTC_CMOS_PORT);
	outb(0x8B, RTC_PORT);
	outb((0xF0 & prev_b) | F2HZ, RTC_CMOS_PORT);
	return 0;
}


extern int32_t rtc_close(int32_t fd)
{
	return 0;
}

extern void test_read()
{
	// set RTC speed
int counter = 0;
int temp_counter = 0;
int i;
int ret_val = 32;
rtc_write(&ret_val, 4);
	while(1) {
		// read the rtc
		printf("HAHAHAHAHAHAHA3\n");
		rtc_read();
		printf("HAHAHAHAHAHAHA4\n");
		counter++;
		if (counter % 5 == 0) {
			// do something visible on the screen
			printf("HAHAHAHAHAHAHA5\n");
			temp_counter = counter / 10;
			clear();
			for(i = 0 ; i<temp_counter;i++)
			{
				printf("<3");
			}	
		}
	}
}

/*
extern void test_write(int ret_val, int32_t nbytes)
{
	rtc_write(&ret_val, nbytes);
}
*/
