#ifndef _RTC_H
#define _RTC_H

#define RTC_PORT 0x70
#define RTC_CMOS_PORT 0x71
#define RTC_IRQ	8

extern void rtc_enable();
extern void rtc_disable();

#endif
