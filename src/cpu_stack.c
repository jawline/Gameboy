#include "cpu.h"

void stack_push16(cpu_state* state, uint16_t v) {
	DEBUG_OUT("PUSH 16 %x %x\n", state->registers.sp, v);
	state->registers.sp -= 2;
	mem_set16(&state->mem, state->registers.sp, v);
}

uint16_t stack_pop16(cpu_state* state) {
	DEBUG_OUT("POP 16\n");
	uint16_t r = mem_get16(&state->mem, state->registers.sp);
	state->registers.sp += 2;
	return r;
}