#include "cpu.h"

void cpu_call(cpu_state* state, uint16_t address, uint16_t next_instr) {
	DEBUG_OUT("CPU CALL 0x%02X\n", address);
	stack_push16(state, next_instr);
	state->registers.pc = address;
}

void cpu_call_nn(cpu_state* state) {
	cpu_call(state, mem_get16(&state->mem, state->registers.pc + 1), state->registers.pc + 3);
}

void cpu_ret(cpu_state* state) {
	DEBUG_OUT("CPU RET INSTR\n");
	state->registers.pc = stack_pop16(state);
}