#include "cpu.h"

void stack_push16(cpu_state* state, uint16_t v) {
	DEBUG_OUT("PUSH 16 %x %x\n", state->registers.sp, v);
	mem_set16(&state->mem, state->registers.sp, v);
	state->registers.sp -= 2;
}

uint16_t stack_pop16(cpu_state* state) {
	DEBUG_OUT("POP 16\n");
	state->registers.sp += 2;
	return mem_get16(&state->mem, state->registers.sp);
}