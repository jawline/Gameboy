#include "cpu.h"

void cpu_jump(cpu_state* state, uint16_t address) {
	state->registers.pc = address;
	DEBUG_OUT("Jump Immediate: %x\n", address);
}

void cpu_jr_flag(cpu_state* state, unsigned flag) {
	int8_t rloc = cpu_instr_nb(state);
	
	if (flag) {
		uint16_t dst = state->registers.pc + rloc;
		DEBUG_OUT("JR NZ %x to %x\n", state->registers.pc, dst);
		cpu_jump(state, dst);
	}
}