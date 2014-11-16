
#ifndef ASSEMBLY_OPS_H
#define ASSEMBLY_OPS_H

/*PCB declarations*/
//extern uint32_t stdin_ops;
//extern uint32_t stdout_ops;

/*System call stuff*/


#ifndef ASM_SYS
extern uint32_t stdin_ops;
extern uint32_t stdout_ops;
extern uint32_t sys_call_linkage(void);
#endif /*ASM_SYS*/



#endif /*ASSEMBLY OPS*/




