#include "cpu.h"
#include <stdio.h>

const uint16_t START_PC = 0x0;

void cpu_init(cpu_state* state) {
	memset(&state->registers, 0, sizeof(state->registers));
	state->registers.pc = START_PC;
	state->registers.f = 0;
}

bool cpu_step(cpu_state* state) {

	memset(&state->registers.lc, 0, sizeof(state->registers.lc));
	
	uint16_t start_pc = state->registers.pc;
	uint8_t c_instr = mem_get(&state->mem, state->registers.pc++);
	//DEBUG_OUT("Instr 0x%02X PC(idx):%i mem_get:%02X (%i)\n", c_instr, state->registers.pc, mem_get(&state->mem, state->registers.pc), mem_get(&state->mem, state->registers.pc));

	uint8_t c_instr_greater_nibble = c_instr >> 4;
	uint8_t c_instr_lesser_nibble = c_instr & 0x0F;
 
	if (c_instr == EXT_OP) {
		return ext_cpu_step(state);
	} if (c_instr >= 0x40 && c_instr < 0x80 && c_instr != HALT) {
		if (!cpu_ld_table_large(state, c_instr)) {
			return false;
		}
	} else if (c_instr_greater_nibble < 4 && c_instr_lesser_nibble == 0x1) {
		if (!cpu_ld_16_imm_list(state, c_instr_greater_nibble)) {
			return false;
		}
	} else if (c_instr_greater_nibble < 4 && c_instr_lesser_nibble == 0xE) {
		if (!cpu_ld_8_n_list_E(state, c_instr_greater_nibble)) {
			return false;
		}
	} else if (c_instr_greater_nibble < 4 && (c_instr_lesser_nibble == 0x4 || c_instr_lesser_nibble == 0x5)) {
		if (!cpu_grid_0x00x3_0x40x5(state, c_instr_greater_nibble, c_instr_lesser_nibble)) {
			return false;
		}
	} else if (c_instr_greater_nibble < 4 && c_instr_lesser_nibble == 0x6) {
		cpu_ld_list_0x6(state, c_instr_greater_nibble);
	} else if (c_instr_greater_nibble < 4 && (c_instr_lesser_nibble == 0xC || c_instr_lesser_nibble == 0xD)) {
		if (!cpu_grid_0x00x3_0xC0xD(state, c_instr_greater_nibble, c_instr_lesser_nibble)) {
			return false;
		}
	} else if (c_instr_greater_nibble < 4 && c_instr_lesser_nibble == 0xB) {
		cpu_grid_dec_16(state, c_instr_greater_nibble);
	} else if (c_instr_greater_nibble < 4 && c_instr_lesser_nibble == 0x3) {
		cpu_grid_inc_16(state, c_instr_greater_nibble);
	} else if (c_instr_greater_nibble == 0xA && c_instr_lesser_nibble >= 0x8) {
		cpu_grid_xor8(state, c_instr_lesser_nibble);
	} else if (c_instr_greater_nibble == 0x8 && c_instr_lesser_nibble >= 0x8) {
		cpu_grid_adc(state, c_instr_lesser_nibble);
	} else if (c_instr_greater_nibble >= 0x8 && c_instr_greater_nibble <= 0xB && c_instr_lesser_nibble < 8) {
		if (!cpu_grid_arith_0x80xB_0x00x7(state, c_instr_greater_nibble, c_instr_lesser_nibble)) {
			return false;
		}
	} else {
		if(!cpu_base_table(state, c_instr)) {
			return false;
		}
	}

	state->clock.m += state->registers.lc.m;
	state->clock.t += state->registers.lc.t;

	DEBUG_OUT("SPC=0x%02X INSTR=0x%02X (%i) PC=0x%02X SIZE=%i BC=0x%04X AF=0x%04X DE=0x%04X HL=0x%04X FLAGS=%01i%01i%01i%01i CLOCK=%i\n", start_pc, c_instr, c_instr, state->registers.pc, state->registers.pc - start_pc, state->registers.bc, state->registers.af, state->registers.de, state->registers.hl, cpu_is_flag(state, ZERO_FLAG), cpu_is_flag(state, SUBTRACT_FLAG), cpu_is_flag(state, HALF_CARRY_FLAG), cpu_is_flag(state, CARRY_FLAG), state->clock.t);

	if (!state->registers.lc.m) {
		printf("BAD INSTR CLK\n");
		exit(1);
	}

	return true;
}