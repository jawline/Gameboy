#include "cpu.h"

void cpu_setinterrupts(cpu_state* state, char on) {
	state->interrupts = on;
	cpu_inc_pc(state, 1);
}