#include "cpu.h"

void cpu_inc_pc(cpu_state* state, uint16_t off) {
	state->registers.pc += off;
}