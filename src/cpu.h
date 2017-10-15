#ifndef _CPU_DEF_H_
#define _CPU_DEF_H_
#include "memory.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
	NOOP = 0,
	RLC_A = 0x6,

	LD_BC_nn = 0x1,
	LD_DE_nn = 0x11,
	LD_HL_nn = 0x21,
	LD_SP_nn = 0x31,
	LD_adr_bc_nn = 0x2,

	LD_C_n = 0x0E,
	LD_E_n = 0x1E,
	LD_L_n = 0x2E,
	LD_A_n = 0x3E,

	LD_B_B = 0x40,
	LD_C_E = 0x4B,
	LD_D_B = 0x50,
	LD_D_C = 0x51,
	LD_D_D = 0x52,
	LD_D_E = 0x53,
	LD_D_H = 0x54,
	LD_D_L = 0x55,
	LD_D_A = 0x57,
	LD_E_B = 0x58,
	LD_E_C = 0x59,
	LD_E_D = 0x5A,
	LD_L_H = 0x6C,
	LD_E_E = 0x5B,
	LD_L_A = 0x6F,
	LD_E_H = 0x5c,
	LD_C_B = 0x48,
	LD_C_C = 0x49,
	LD_C_D = 0x4A,

	LD_D_REF_HL = 0x56,

	LDH_REF_n_A = 0xE0,
	LDH_REF_C_A = 0xE2,
	LDH_REF_A_n = 0xF0,

	LD_REF_HL_n = 0x36,

	LD_REF_HL_B = 0x70,
	LD_REF_HL_C = 0x71,
	LD_REF_HL_D = 0x72,
	LD_REF_HL_E = 0x73,
	LD_REF_HL_H = 0x74,
	LD_REF_HL_L = 0x75,
	LD_REF_HL_A = 0x77,

	ADD_A_B = 0x80,
	ADD_A_C = 0x81,
	ADD_A_D = 0x82,
	ADD_A_E = 0x83,
	ADD_A_H = 0x84,
	ADD_A_L = 0x85,
	ADD_A_REF_HL = 0x86,
	ADD_A_A = 0x87,

	LD_L_REF_HL = 0x6E,
	LDD_REF_HL_A = 0x32,
	
	SUB_A_B = 0x90,
	SUB_A_C = 0x91,
	SUB_A_D = 0x92,
	SUB_A_E = 0x93,
	SUB_A_H = 0x94,
	SUB_A_L = 0x95,
	SUB_A_A = 0x97,
	SUB_A_REF_HL = 0x96,

	INC_C = 0x0C,
	INC_E = 0x1C,
	INC_L = 0x2C,
	INC_A = 0x3C,

	INC_B = 0x4,
	INC_D = 0x14,
	INC_H = 0x24,

	INC_BC = 0x3,
	INC_DE = 0x13,
	INC_HL = 0x23,
	INC_SP = 0x33,
	
	DEC_BC = 0x0B,
	DEC_H = 0x25,
	DEC_D = 0x15,
	DEC_B = 0x05,

	DEC_C = 0xD,
	DEC_E = 0x1D,
	DEC_L = 0x2D,
	DEC_A = 0x3D,

	PUSH_BC = 0xC5,

	RL_A = 0x17,

	CALL_nn = 0xCD,
	CALL_Z_nn = 0xCC,
	RET_NZ = 0xC0,

	EXT_OP = 0xCB,

	LD_REF_nn_A = 0xEA,
	LD_REF_nn_SP = 0x08,
	
	LDI_REF_HL_A = 0x22,
	ADD_HL_DE = 0x19,
	JP_NN = 0xC3,
	JR_NZ_n = 0x20,
	XOR_A = 0xAF,

	CPL_A = 0x2F,
	CP_n = 0xFE,
	
	DISABLE_INTERRUPTS = 0xF3,
	ENABLE_INTERRUPTS = 0xFB,
	RST_38 = 0xFF
} cpu_ops;

typedef enum {
	TEST_7_H = 0x7C,
} cpu_ops_ext;

typedef struct cpu_registers {
	uint16_t pc, sp;
	
	union {
		struct { uint8_t b; uint8_t c; };
		uint16_t bc;
	};

	union {
		struct { uint8_t a; uint8_t f; };
		uint16_t af;		
	};

	union {
		struct { uint8_t d; uint8_t e; };
		uint16_t de;		
	};

	union {
		struct { uint8_t h; uint8_t l; };
		uint16_t hl;		
	};
} cpu_registers;

typedef struct cpu_clock {
	uint32_t m;
	uint32_t t;
} cpu_clock;

typedef struct cpu_state {
	char interrupts;
	cpu_registers registers;
	cpu_clock clock;
	memory mem;
} cpu_state;


void cpu_init(cpu_state* state);
bool cpu_step(cpu_state* state);

#endif //_CPU_DEF_H_
