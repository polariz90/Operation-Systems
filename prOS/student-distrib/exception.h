#ifndef _EXCEPTION_H
#define _EXCEPTION_H

extern void div_err_excpn_0();
extern void debug_excpn_1();
extern void NMI_excpn_2();
extern void brkpnt_excpn_3();
extern void overflow_excpn_4();
extern void bound_range_exceed_excpn_5();
extern void invalid_opcode_excpn_6();
extern void device_unvailable_excpn_7();
extern void dbl_fault_excpn_8();
extern void coproc_sgmt_excpn_9();
extern void invalid_TSS_excpn_10();
extern void sgmt_not_present_excpn_11();
extern void stack_fault_excpn_12();
extern void gen_protection_excpn_13();
extern void page_fault_excpn_14();
extern void FPU_F_P_excpn_16();
extern void algnmnt_chk_excpn_17();
extern void machine_chk_excpn_18();
extern void SIMD_F_P_excpn_19();

#endif
