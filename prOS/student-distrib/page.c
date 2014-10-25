#include "page.h"

void enable_paging()
{
	asm volatile ("movl	%cr0,%eax":);/* get control word */
	asm volatile ("orl	$0x80000000,%eax":);/* enable paging */
	asm volatile ("movl	%eax,%cr0":"eax");/* and let's page NOW! */
}

