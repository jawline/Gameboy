#include "cpu.h"

void cpu_jump(cpu_state* state, uint16_t address) {
	state->registers.pc = address;
	DEBUG_OUT("Jump Immediate: %x\n", address);
}