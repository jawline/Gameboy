#include "cpu.h"

void cpu_ld8_n(cpu_state* state, uint8_t* reg) {
	uint8_t lval = mem_get(&state->mem, state->registers.pc + 1);
	*reg = lval;
	cpu_inc_pc(state, 2);
}

void cpu_ld16_nn(cpu_state* state, uint16_t* reg) {
	*reg = mem_get16(&state->mem, state->registers.pc + 1);
}

void cpu_ld8(cpu_state* state, uint8_t* to, uint8_t val) {
	*to = val;
	cpu_inc_pc(state, 1);
}

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

uint8_t* ld_table_dst_lookup_second(cpu_state* state, uint8_t off) {
	switch (off) {
		case 0:
			return &state->registers.c;
		case 1:
			return &state->registers.e;
		case 2:
			return &state->registers.l;
		case 3:
			return &state->registers.a;
	}

	DEBUG_OUT("LD OFFSET SHOULD BE UNREACHABLE (DST LOOKUP 2)\n");
	return 0;
}

bool cpu_ld_16_imm_list(cpu_state* state, uint8_t gnibble) {
	DEBUG_OUT("CPU LD 16 IMM\n");

	uint16_t* reg = cpu_util_16_bit_reg(state, gnibble);

	cpu_ld16_nn(state, reg);
	cpu_inc_pc(state, 3);
}

bool cpu_ld_table_large(cpu_state* state, uint8_t c_instr) {
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
			cpu_ld8(state, dst, *src);
		}

		return true;
	}

	//Saving a register to HL
	if (c_instr_greater_nibble == 0x7 && c_instr_lesser_nibble < 0x8) {
		uint8_t v = *reg_ld_table_offset(state, c_instr_lesser_nibble);
		mem_set(&state->mem, state->registers.hl, v);
		cpu_inc_pc(state, 1);
		return true;
	}

	if (c_instr_lesser_nibble == 0xE) {
		printf("Unhandled LD (HL INSTR)\n");
		return false;
	}

	uint8_t* dst = ld_table_dst_lookup_second(state, c_instr_greater_nibble - 4);
	uint8_t* src = reg_ld_table_offset(state, c_instr_lesser_nibble - 0x8);

	cpu_ld8(state, dst, *src);

	return true;
}