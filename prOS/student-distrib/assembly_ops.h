
#ifndef ASSEMBLY_OPS_H
#define ASSEMBLY_OPS_H


/*System call stuff*/

#ifndef ASM_SYS
//extern uint32_t stdin_ops;
//extern uint32_t stdout_ops;
void sys_call_linkage(void);
//extern void * _sys_call_linkage;
extern void test_out(uint32_t newEIP);
extern uint32_t stdout_ops;
extern uint32_t stdin_ops;

#endif /*ASM_SYS*/



#endif /*ASSEMBLY OPS*/
