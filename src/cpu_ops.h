#ifndef _CPU_OPS_DEF_H
#define _CPU_OPS_DEF_H

typedef enum {
	NOOP = 0,
	RLC_A = 0x7,

	HALT = 0x76,

	LD_A_REF_BC = 0x0A,
	LD_A_REF_DE = 0x1A,

	LD_D_REF_HL = 0x56,

	LDH_REF_n_A = 0xE0,
	LDH_REF_C_A = 0xE2,
	LDH_REF_A_n = 0xF0,

	LDD_REF_HL_A = 0x32,

	POP_BC = 0xC1,

	RL_A = 0x17,

	CALL_nn = 0xCD,
	CALL_Z_nn = 0xCC,

	RET = 0xC9,
	RET_Z = 0xC8,
	RET_NZ = 0xC0,
	RET_I = 0xD9,

	EXT_OP = 0xCB,

	LD_REF_nn_SP = 0x08,
	
	LDI_REF_HL_A = 0x22,
	LDI_A_REF_HL = 0x2A,

	JP_NN = 0xC3,
	JP_REF_HL = 0xE9,

	JR_n = 0x18,
	JR_NZ_n = 0x20,
	JR_NC_n = 0x30,
	JR_Z_n = 0x28,

	CPL_A = 0x2F,
	CP_n = 0xFE,

	ADD_n = 0xC6,
	SUB_n = 0xD6,
	AND_n = 0xE6,
	OR_n = 0xF6,

	LD_REF_nn_A = 0xEA,
	LD_A_REF_nn = 0xFA,
	
	DISABLE_INTERRUPTS = 0xF3,
	ENABLE_INTERRUPTS = 0xFB,
	RST_30 = 0xF7,
	RST_38 = 0xFF
} cpu_ops;

typedef enum {
	TEST_7_H = 0x7C,
} cpu_ops_ext;

#endif