#include "i8259.h" 
#include "rtc.h"
#include "lib.h"


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
