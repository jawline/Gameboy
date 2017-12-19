#include "cpu.h"
#include <stdlib.h>

void cpu_ld8(cpu_state* state, uint8_t* to, uint8_t val) {
	*to = val;
	cpu_instr_m(state, 2);
}

void cpu_ld8_n(cpu_state* state, uint8_t* reg) {
	cpu_ld8(state, reg, cpu_instr_nb(state));
}

void cpu_ld16_nn(cpu_state* state, uint16_t* reg) {
	*reg = cpu_instr_nw(state);
	cpu_instr_m(state, 4);
}

void cpu_ld_16_imm_list(cpu_state* state, uint8_t gnibble) {
	cpu_ld16_nn(state, cpu_reg_16_bdhs(state, gnibble));
}

void cpu_ld_8_n_list_E(cpu_state* state, uint8_t gnibble) {
	cpu_ld8_n(state, cpu_reg_cela(state, gnibble));
}

void cpu_ld_list_0x6(cpu_state* state, uint8_t gnibble) {
	if (gnibble == 3) {
		mem_set(&state->mem, state->registers.hl, cpu_instr_nb(state));
		cpu_instr_m(state, 4);
	} else {
		uint8_t* reg = cpu_reg_8_bdh(state, gnibble);
		cpu_ld8_n(state, reg);
	}
}

void cpu_pop_16_list(cpu_state* state, uint8_t gnibble) {
	uint16_t* reg = cpu_reg_16_bdha(state, gnibble - 0xC);
	*reg = stack_pop16(state);
	cpu_instr_m(state, 4);
}

void cpu_push_16_list(cpu_state* state, uint8_t gnibble) {
	uint16_t* reg = cpu_reg_16_bdha(state, gnibble - 0xC);
	stack_push16(state, *reg);
	cpu_instr_m(state, 4);
}

void cpu_ld_table_large(cpu_state* state, uint8_t c_instr) {

	uint8_t c_instr_greater_nibble = c_instr >> 4;
	uint8_t c_instr_lesser_nibble = c_instr & 0x0F;

	//Either single byte loads between registers or from (HL)
	if (c_instr_greater_nibble < 0x7 && c_instr_lesser_nibble < 0x8) {

		uint8_t* dst;

		switch (c_instr_greater_nibble - 0x4) {
			case 0:
				dst = &state->registers.b;
				break;
			case 1:
				dst = &state->registers.d;
				break;
			case 2:
				dst = &state->registers.h;
				break;
			default:
				printf("Unhandled LD table %x (%x)\n", c_instr, c_instr_greater_nibble);
				exit(1);
				break;
		}

		if (c_instr_lesser_nibble == 0x6) {
			*dst = mem_get(&state->mem, state->registers.hl);
		} else {
			uint8_t* src = cpu_reg_bcdehla(state, c_instr_lesser_nibble);
			cpu_ld8(state, dst, *src);
		}

	} else if (c_instr_greater_nibble == 0x7 && c_instr_lesser_nibble < 0x8) { //Saving a register to HL
		mem_set(&state->mem, state->registers.hl, *cpu_reg_bcdehla(state, c_instr_lesser_nibble));
		cpu_instr_m(state, 2);
	} else {

		if (c_instr_lesser_nibble == 0xE) {
			printf("Unhandled LD (HL INSTR)\n");
			exit(1);
		}

		uint8_t* dst = cpu_reg_cela(state, c_instr_greater_nibble - 0x4);
		uint8_t* src = cpu_reg_bcdehla(state, c_instr_lesser_nibble - 0x8);
		cpu_ld8(state, dst, *src);
	}
}