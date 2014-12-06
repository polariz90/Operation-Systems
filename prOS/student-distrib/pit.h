#ifndef _PIT_H
#define _PIT_H

#define PIT_CH1_PORT 0x40
#define PIT_REG_PORT 0x43    //     Mode/Command register (write only, a read is ignored)

#define PIT_PORT 0x70
#define PIT_CMOS_PORT 0x71
#define PIT_IRQ	0
#define PIT_A 0x8A
#define PIT_B 0x8B
#define PIT_C 0x8C

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

extern void PIT_enable();
extern void PIT_disable();
extern void * PIT_opt[4];


extern int32_t PIT_read();
extern int32_t PIT_write(int fd, const int32_t* buf, int32_t nbytes);
extern int32_t PIT_open();
extern int32_t PIT_close();

extern void test_read();
//extern void test_write(int ret_val, int32_t nbytes);

#endif
