#include "cpu.h"

void cpu_inc_pc(cpu_state* state, uint16_t off) {
	state->registers.pc += off;
}

uint16_t* cpu_util_16_bit_reg(cpu_state* state, uint8_t off) {
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