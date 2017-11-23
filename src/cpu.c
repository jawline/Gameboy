#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const uint16_t START_PC = 0x100;

void cpu_init(cpu_state* state) {
	memset(&state->registers, 0, sizeof(state->registers));
	state->registers.pc = START_PC;
	state->registers.f = 0;
	state->interrupts = 1;
	state->mem.interrupts_enabled = 0xFF;
}

void cpu_check_interrupts(cpu_state* state) {
	if (state->interrupts & state->mem.interrupts_enabled & state->mem.interrupts) {
		
		uint8_t mask_with_enabled = state->mem.interrupts_enabled & state->mem.interrupts;

		if (mask_with_enabled & 0x1) {
			state->mem.interrupts &= (0xFF - 0x1);
			cpu_call(state, 0x0040, state->registers.pc);
		} else {

		}
	}
}

void cpu_step(cpu_state* state) {

	memset(&state->registers.lc, 0, sizeof(state->registers.lc));
	
	uint16_t start_pc = state->registers.pc;
	uint8_t c_instr = mem_get(&state->mem, state->registers.pc++);
	DEBUG_OUT("Instr 0x%02X PC(idx):%x\n", c_instr, state->registers.pc);

	uint8_t c_instr_greater_nibble = c_instr >> 4;
	uint8_t c_instr_lesser_nibble = c_instr & 0x0F;
 
	if (c_instr == EXT_OP) {
		return ext_cpu_step(state);
	} if (c_instr >= 0x40 && c_instr < 0x80 && c_instr != HALT) {
		cpu_ld_table_large(state, c_instr);
	} else if (c_instr_greater_nibble < 4 && c_instr_lesser_nibble == 0x1) {
		cpu_ld_16_imm_list(state, c_instr_greater_nibble);
	} else if (c_instr_greater_nibble < 4 && c_instr_lesser_nibble == 0xE) {
		cpu_ld_8_n_list_E(state, c_instr_greater_nibble);
	} else if (c_instr_greater_nibble < 4 && (c_instr_lesser_nibble == 0x4 || c_instr_lesser_nibble == 0x5)) {
		cpu_grid_0x00x3_0x40x5(state, c_instr_greater_nibble, c_instr_lesser_nibble);
	} else if (c_instr_greater_nibble < 4 && c_instr_lesser_nibble == 0x6) {
		cpu_ld_list_0x6(state, c_instr_greater_nibble);
	} else if (c_instr_greater_nibble < 4 && (c_instr_lesser_nibble == 0xC || c_instr_lesser_nibble == 0xD)) {
		cpu_grid_0x00x3_0xC0xD(state, c_instr_greater_nibble, c_instr_lesser_nibble);
	} else if (c_instr_greater_nibble < 4 && c_instr_lesser_nibble == 0xB) {
		cpu_grid_dec_16(state, c_instr_greater_nibble);
	} else if (c_instr_greater_nibble < 4 && c_instr_lesser_nibble == 0x3) {
		cpu_grid_inc_16(state, c_instr_greater_nibble);
	} else if (c_instr_greater_nibble == 0xA && c_instr_lesser_nibble >= 0x8) {
		cpu_grid_xor8(state, c_instr_lesser_nibble);
	} else if (c_instr_greater_nibble == 0x8 && c_instr_lesser_nibble >= 0x8) {
		cpu_grid_adc(state, c_instr_lesser_nibble);
	} else if (c_instr_greater_nibble >= 0x8 && c_instr_greater_nibble <= 0xB && c_instr_lesser_nibble < 8) {
		cpu_grid_arith_0x80xB_0x00x7(state, c_instr_greater_nibble, c_instr_lesser_nibble);
	} else if (c_instr_greater_nibble >= 0xC && c_instr_lesser_nibble == 0x7) {
		cpu_rst_table_offset(state, c_instr_greater_nibble, 0);
	} else if (c_instr_greater_nibble >= 0xC && c_instr_lesser_nibble == 0xF) {
		cpu_rst_table_offset(state, c_instr_greater_nibble, 8);
	} else {
		cpu_base_table(state, c_instr);
	}

	state->clock.m += state->registers.lc.m;
	state->clock.t += state->registers.lc.t;

	DEBUG_OUT("SPC=0x%02X INSTR=0x%02X (%i) PC=0x%02X SIZE=%i SP=0x%04X BC=0x%04X AF=0x%04X DE=0x%04X HL=0x%04X FLAGS=%01i%01i%01i%01i CLOCK=%i\n", start_pc, c_instr, c_instr, state->registers.pc, state->registers.pc - start_pc, state->registers.sp, state->registers.bc, state->registers.af, state->registers.de, state->registers.hl, cpu_is_flag(state, ZERO_FLAG), cpu_is_flag(state, SUBTRACT_FLAG), cpu_is_flag(state, HALF_CARRY_FLAG), cpu_is_flag(state, CARRY_FLAG), state->clock.t);

	if (!state->registers.lc.m) {
		printf("BAD INSTR CLK\n");
		exit(1);
	}
}