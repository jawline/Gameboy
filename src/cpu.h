#ifndef _CPU_DEF_H_
#define _CPU_DEF_H_
#include "memory.h"
#include "cpu_ops.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define DEBUG 1

#ifdef DEBUG 
#define DEBUG_OUT(...) printf(__VA_ARGS__)
#else
#define DEBUG_OUT(...)
#endif

typedef struct cpu_clock {
	uint32_t m;
	uint32_t t;
} cpu_clock;

typedef struct cpu_registers {
	uint16_t pc, sp;
	cpu_clock lc;
	
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
void cpu_step(cpu_state* state);

void cpu_check_interrupts(cpu_state* state);

/**
 * Extended CPU Table Methods
 */

void ext_cpu_step(cpu_state* state);

/**
 * Interrupt calls
 */
void cpu_setinterrupts(cpu_state* state, uint8_t on);

/**
 * CPU Calls
 */

void cpu_call(cpu_state* state, uint16_t address, uint16_t next_instr);
void cpu_ret(cpu_state* state);
void cpu_rst_table_offset(cpu_state* state, uint8_t gnibble, uint16_t offset);

/**
 * Arithmetic operations
 */

void cpu_grid_0x00x3_0x40x5(cpu_state* state, uint8_t gnibble, uint8_t lnibble);
void cpu_grid_0x00x3_0xC0xD(cpu_state* state, uint8_t gnibble, uint8_t lnibble);
void cpu_grid_arith_0x80xB_0x00x7(cpu_state* state, uint8_t gnibble, uint8_t lnibble);

void cpu_grid_dec_16(cpu_state* state, uint8_t gnibble);
void cpu_grid_inc_16(cpu_state* state, uint8_t gnibble);

void cpu_grid_xor8(cpu_state* state, uint8_t lnibble);
void cpu_grid_adc(cpu_state* state, uint8_t lnibble);

void cpu_swap_reg8(cpu_state* state, uint8_t* reg);
void cpu_add_reg8(cpu_state* state, uint8_t* reg, uint8_t v);
void cpu_sub_reg8(cpu_state* state, uint8_t* reg, uint8_t v);
void cpu_and_reg8(cpu_state* state, uint8_t* reg, uint8_t v);
void cpu_or_reg8(cpu_state* state, uint8_t* reg, uint8_t v);
void cpu_dec_reg16(cpu_state* state, uint16_t* reg);
void cpu_inc_reg16(cpu_state* state, uint16_t* reg);

void cpu_rl_reg8(cpu_state* state, uint8_t* reg);
void cpu_rlc_reg8(cpu_state* state, uint8_t* reg);

/**
 * Interrupts logic
 */
void cpu_setinterrupts(cpu_state* state, uint8_t on);

/**
 * Util Methods
 */

void stack_push16(cpu_state* state, uint16_t v);
uint16_t stack_pop16(cpu_state* state);
void cpu_inc_pc(cpu_state* state, uint16_t off);
uint8_t* cpu_reg_8_bdh(cpu_state* state, uint8_t off);
uint16_t* cpu_reg_16_bdha(cpu_state* state, uint8_t off);
uint16_t* cpu_reg_16_bdhs(cpu_state* state, uint8_t off);
uint8_t* cpu_reg_bcdehla(cpu_state* state, uint8_t c_instr_lesser_nibble);
uint8_t* cpu_reg_cela(cpu_state* state, uint8_t off);
void cpu_instr_m(cpu_state* state, uint8_t m);

//Get next byte or word from memory pointed by PC then inc PC
uint8_t cpu_instr_nb(cpu_state* state);
uint16_t cpu_instr_nw(cpu_state* state);

/**
 * Flag Methods
 */

bool cpu_is_flag(cpu_state* state, uint8_t flags);
void cpu_set_flags(cpu_state* state, bool zero_flag, bool negative_flag, bool half_carry, bool carry);

/**
 * LD Table Calls
 */

void cpu_ld_table_large(cpu_state* state, uint8_t c_instr);
void cpu_ld8_n(cpu_state* state, uint8_t* reg);
void cpu_ld16(cpu_state* state, uint16_t* reg);
void cpu_ld_list_0x6(cpu_state* state, uint8_t gnibble);
void cpu_ld8(cpu_state* state, uint8_t* to, uint8_t val);
void cpu_ld_16_imm_list(cpu_state* state, uint8_t gnibble);
void cpu_ld_8_n_list_E(cpu_state* state, uint8_t gnibble);

void cpu_push_16_list(cpu_state* state, uint8_t gnibble);
void cpu_pop_16_list(cpu_state* state, uint8_t gnibble);

/**
 * CPU Jumps
 */

void cpu_jump(cpu_state* state, uint16_t address);
void cpu_jr_flag(cpu_state* state, unsigned flag);

/**
 * Base Table
 */

void cpu_base_table(cpu_state* state, uint8_t c_instr);

#endif //_CPU_DEF_H_
