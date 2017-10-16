#include "cpu.h"

uint8_t* reg_ld_table_offset(cpu_state* state, uint8_t c_instr_lesser_nibble) {
		switch (c_instr_lesser_nibble) {
			case 0:
				return &state->registers.b;
			case 1:
				return &state->registers.c;
			case 2:
				return &state->registers.d;
			case 3:
				return &state->registers.e;
			case 4:
				return &state->registers.h;
			case 5:
				return &state->registers.l;
			case 7:
				return &state->registers.a;
		}
}

bool cpu_ld_table(cpu_state* state, uint8_t c_instr) {
	printf("LD Instruction 0x%02X\n", c_instr);

	uint8_t c_instr_greater_nibble = c_instr >> 4;
	uint8_t c_instr_lesser_nibble = c_instr & 0x0F;

	//Either single byte loads between registers or from (HL)
	if (c_instr_greater_nibble < 0x7 && c_instr_lesser_nibble < 0x8) {
		uint8_t* dst;

		switch (c_instr_greater_nibble) {
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
				printf("Unhandled LD table %x\n", c_instr_greater_nibble);
				break;
		}

		if (c_instr_lesser_nibble == 0x6) {
			*dst = mem_get(&state->mem, state->registers.hl);
			cpu_inc_pc(state, 1);
		} else {
			uint8_t* src = reg_ld_table_offset(state, c_instr_lesser_nibble);
			cpu_mov8(state, dst, *src);
		}

		return true;
	}

	//Saving a register to HL
	if (c_instr_greater_nibble == 0x7 && c_instr_lesser_nibble < 0x8) {
		uint8_t v = *reg_ld_table_offset(state, c_instr_lesser_nibble);
		cpu_save_reg_at(state, 0, state->registers.hl, v);
		cpu_inc_pc(state, 1);
		return true;
	}

	printf("Unhandled LD\n");
	return false;
}