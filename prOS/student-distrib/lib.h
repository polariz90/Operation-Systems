/* lib.h - Defines for useful library functions
 * vim:ts=4 noexpandtab
 */

#ifndef _LIB_H
#define _LIB_H

#include "types.h"

#define _8MB 		0x800000
#define _8KB		0x2000
#define four_GB     0x100000000
#define four_MB     0x400000
#define four_KB     0x1000
#define four_kb     4096 /* 4KB = 4096 bytes */
#define one_kb      1024 /* 1KB = 1024 bytes */
#define eight_mb 	0x800000
#define eight_kb 	0x2000
#define _132mb 		0x8400000
#define _128mb 		0x8000000

/*Varible declarations*/
//int screen_x;
//int screen_y ;
//char* video_mem = (char *)VIDEO;



int32_t printf(int8_t *format, ...);
void putc(uint8_t c);
int32_t puts(int8_t *s);
int8_t *itoa(uint32_t value, int8_t* buf, int32_t radix);
int8_t *strrev(int8_t* s);
uint32_t strlen(const int8_t* s);
void clear(void);
void blue(void);
void normal_screen(void);
void blue_screen(void);
void block_screen(int clk_pos, int cur_pos);

void* memset(void* s, int32_t c, uint32_t n);
void* memset_word(void* s, int32_t c, uint32_t n);
void* memset_dword(void* s, int32_t c, uint32_t n);
void* memcpy(void* dest, const void* src, uint32_t n);
void* memmove(void* dest, const void* src, uint32_t n);
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n);
int8_t* strcpy(int8_t* dest, const int8_t*src);
int8_t* strncpy(int8_t* dest, const int8_t*src, uint32_t n);
int atoin(int8_t* buf);
int power(int base, int power);

//
void test_interrupts();

/*more functions added from terminal*/

/*scrolls the screen count times*/
void vert_scroll(uint32_t count);

/*moves the current screen position of  loc*/
void move_screen_x(int loc); 
/*moves the current screen position of  loc*/
void move_screen_y(int loc); 

/*clears the current line based on the current screen y position*/
void clear_line();

/*returns the screen_y()*/
int get_screen_y();
/*returns the screen_x()*/
int get_screen_x();

/*sets the screen_y()*/
void set_screen_y(uint32_t yloc);
/*setd the screen_x()*/
void set_screen_x(uint32_t xloc);

/* random number generator */
int rand();


/* Userspace address-check functions */
int32_t bad_userspace_addr(const void* addr, int32_t len);
int32_t safe_strncpy(int8_t* dest, const int8_t* src, int32_t n);

/*used to test rtc interrupt*/
void test_interrupts(void);

/* Port read functions */
/* Inb reads a byte and returns its value as a zero-extended 32-bit
 * unsigned int */
static inline uint32_t inb(port)
{
	uint32_t val;
	asm volatile("xorl %0, %0\n \
			inb   (%w1), %b0" 
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
} 

/* Reads two bytes from two consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them zero-extended
 * */
static inline uint32_t inw(port)
{
	uint32_t val;
	asm volatile("xorl %0, %0\n   \
			inw   (%w1), %w0"
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
}

/* Reads four bytes from four consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them */
static inline uint32_t inl(port)
{
	uint32_t val;
	asm volatile("inl   (%w1), %0"
			: "=a"(val)
			: "d"(port)
			: "memory" );
	return val;
}

/* Writes a byte to a port */
#define outb(data, port)                \
do {                                    \
	asm volatile("outb  %b1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Writes two bytes to two consecutive ports */
#define outw(data, port)                \
do {                                    \
	asm volatile("outw  %w1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Writes four bytes to four consecutive ports */
#define outl(data, port)                \
do {                                    \
	asm volatile("outl  %l1, (%w0)"     \
			:                           \
			: "d" (port), "a" (data)    \
			: "memory", "cc" );         \
} while(0)

/* Clear interrupt flag - disables interrupts on this processor */
#define cli()                           \
do {                                    \
	asm volatile("cli"                  \
			:                       \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Save flags and then clear interrupt flag
 * Saves the EFLAGS register into the variable "flags", and then
 * disables interrupts on this processor */
#define cli_and_save(flags)             \
do {                                    \
	asm volatile("pushfl        \n      \
			popl %0         \n      \
			cli"                    \
			: "=r"(flags)           \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Set interrupt flag - enable interrupts on this processor */
#define sti()                           \
do {                                    \
	asm volatile("sti"                  \
			:                       \
			:                       \
			: "memory", "cc"        \
			);                      \
} while(0)

/* Restore flags
 * Puts the value in "flags" into the EFLAGS register.  Most often used
 * after a cli_and_save_flags(flags) */
#define restore_flags(flags)            \
do {                                    \
	asm volatile("pushl %0      \n      \
			popfl"                  \
			:                       \
			: "r"(flags)            \
			: "memory", "cc"        \
			);                      \
} while(0)

#endif /* _LIB_H */
