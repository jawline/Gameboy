#include "cpu.h"
#include <stdlib.h>

void ext_cpu_step_bit_test_8bit_reg(cpu_state* state, uint8_t reg, uint8_t bit) {
	cpu_set_flags(state, (reg & (0x1 << bit)) == 0, 0, 1, cpu_is_flag(state, CARRY_FLAG));
}

void ext_cpu_step_bit(uint8_t c_instr, cpu_state* state) {

	uint8_t c_instr_greater_nibble = c_instr >> 4;
	uint8_t c_instr_lesser_nibble = c_instr & 0x0F;

	//DEBUG_OUT("EXT CPU Step Bit instr %x %x\n", c_instr_greater_nibble, c_instr_lesser_nibble);
	
	uint8_t start_offset = 0;

	if (c_instr_lesser_nibble >= 8) {
		start_offset = 1;
		c_instr_lesser_nibble -= 8;
	}

	uint8_t selected_bit = (c_instr_greater_nibble - 4) * 2 + start_offset;

	DEBUG_OUT("Selected Bit %x %x\n", c_instr, selected_bit);

	//Custom logic for the HL
	if (c_instr_lesser_nibble == 6) {
		DEBUG_OUT("HL BIT NOT IMPL\n");
		exit(1);
	} else {
		//It's an 8 bit reg instr
		uint8_t* reg = cpu_reg_bcdehla(state, c_instr_lesser_nibble);
		ext_cpu_step_bit_test_8bit_reg(state, *reg, selected_bit);
	}
}

void ext_cpu_rl_8bit(cpu_state* state, uint8_t c_instr) {
	uint8_t c_instr_lesser_nibble = c_instr & 0x0F;
	uint8_t* reg = cpu_reg_bcdehla(state, c_instr_lesser_nibble);
	cpu_rl_reg8(state, reg);
}

void ext_cpu_swap_8bit(cpu_state* state, uint8_t c_instr) {
	uint8_t c_instr_lesser_nibble = c_instr & 0x0F;
	uint8_t* reg = cpu_reg_bcdehla(state, c_instr_lesser_nibble);
	cpu_swap_reg8(state, reg);
}


void ext_cpu_step(cpu_state* state) {

	DEBUG_OUT("EXT PC=%x\n", state->registers.pc);
	uint8_t c_instr = mem_get(&state->mem, state->registers.pc++);
	DEBUG_OUT("EXTInstr %x\n", c_instr);

	if (c_instr >= 0x40 && c_instr < 0x80) {
		ext_cpu_step_bit(c_instr, state);
	} else if (c_instr >= 0x10 && c_instr < 0x16) {
		ext_cpu_rl_8bit(state, c_instr);
	} else if (c_instr >= 0x30 && c_instr < 0x36) {
		ext_cpu_swap_8bit(state, c_instr);
	} else if (c_instr == 0x37) {
		uint8_t v = mem_get(&state->mem, state->registers.hl);
		cpu_swap_reg8(state, &v);
		mem_set(&state->mem, state->registers.hl, v);
	} else {	
		printf("Fatal: Unknown EXT opcode %x\n", c_instr);
		exit(1);
	}
}