#ifndef _RTC_H
#define _RTC_H

#define RTC_PORT 0x70
#define RTC_CMOS_PORT 0x71
#define RTC_IRQ	8
#define RTC_A 0x8A
#define RTC_B 0x8B
#define RTC_C 0x8C

#define F2HZ    15
#define F4HZ    14
#define F8HZ    13
#define F16HZ   12
#define F32HZ   11
#define F64HZ   10
#define F128HZ  9
#define F256HZ  8
#define F512HZ  7
#define F1024HZ 6
#define NONE	0

extern void rtc_enable();
extern void rtc_disable();
extern void * rtc_opt[4];

extern int32_t rtc_read();
extern int32_t rtc_write(const int32_t* buf, int32_t nbytes);
extern int32_t rtc_open();
extern int32_t rtc_close(int32_t fd);

extern void test_read();
extern void test_write(int ret_val, int32_t nbytes);

#endif
