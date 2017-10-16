#ifndef _CPU_DEF_H_
#define _CPU_DEF_H_
#include "memory.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define DEBUG 1

#ifdef DEBUG 
#define DEBUG_OUT(...) printf(__VA_ARGS__)
#else
#define DEBUG_OUT(...)
#endif

typedef enum {
	NOOP = 0,
	RLC_A = 0x6,

	HALT = 0x76,

	LD_adr_bc_nn = 0x2,

	LD_A_REF_BC = 0x0A,
	LD_A_REF_DE = 0x1A,

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

	POP_BC = 0xC1,
	PUSH_BC = 0xC5,


	RL_A = 0x17,

	CALL_nn = 0xCD,
	CALL_Z_nn = 0xCC,

	RET = 0xC9,
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

extern const uint8_t SIGN_FLAG;
extern const uint8_t ZERO_FLAG;
extern const uint8_t UDOC_FLAG;
extern const uint8_t HALF_CARRY_FLAG;
extern const uint8_t UDOC_2_FLAG;
extern const uint8_t PO_FLAG;
extern const uint8_t SUBTRACT_FLAG;
extern const uint8_t CARRY_FLAG;

void cpu_init(cpu_state* state);
bool cpu_step(cpu_state* state);

/**
 * Extended CPU Table Methods
 */

bool ext_cpu_step(cpu_state* state);

/**
 * CPU Calls
 */

void cpu_call(cpu_state* state, uint16_t address, uint16_t next_instr);
void cpu_call_nn(cpu_state* state);
void cpu_ret(cpu_state* state);

/**
 * Arithmetic operations
 */
bool cpu_inc_16_bit_0x3(cpu_state* state, uint8_t gnibble);

/**
 * Interrupts logic
 */
void cpu_setinterrupts(cpu_state* state, char on);

/**
 * Util Methods
 */

void stack_push16(cpu_state* state, uint16_t v);
uint16_t stack_pop16(cpu_state* state);
void cpu_inc_pc(cpu_state* state, uint16_t off);
uint16_t* cpu_reg_16_bdhs(cpu_state* state, uint8_t off);
uint8_t* cpu_reg_bcdehla(cpu_state* state, uint8_t c_instr_lesser_nibble);
uint8_t* cpu_reg_cela(cpu_state* state, uint8_t off);

/**
 * Flag Methods
 */

bool cpu_is_flag(cpu_state* state, uint8_t flags);
void cpu_set_flags(cpu_state* state, bool zero_flag, bool negative_flag, bool half_carry, bool carry);

/**
 * LD Table Calls
 */

bool cpu_ld_table(cpu_state* state, uint8_t c_instr);
void cpu_ld8_n(cpu_state* state, uint8_t* reg);
void cpu_ld16(cpu_state* state, uint16_t* reg);
void cpu_ld8(cpu_state* state, uint8_t* to, uint8_t val);

#endif //_CPU_DEF_H_
