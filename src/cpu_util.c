#include "cpu.h"

void cpu_inc_pc(cpu_state* state, uint16_t off) {
	state->registers.pc += off;
}

uint16_t* cpu_reg_16_bdhs(cpu_state* state, uint8_t off) {
	switch (off) {
		case 0:
			return &state->registers.bc;
		case 1:
			return &state->registers.de;
		case 2:
			return &state->registers.hl;
		case 3:
			return &state->registers.sp;
	}

	DEBUG_OUT("ERR LOOKUP 16 BIT REG\n");
	return 0;
}

uint8_t* cpu_reg_bcdehla(cpu_state* state, uint8_t c_instr_lesser_nibble) {
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

	DEBUG_OUT("Should be unrachable hit BCDEHLA\n");
	return 0;
}

uint8_t* cpu_reg_cela(cpu_state* state, uint8_t off) {
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

	DEBUG_OUT("Should be unreachable hit CELA\n");
	return 0;
}