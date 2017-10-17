#include "cpu.h"

void cpu_setinterrupts(cpu_state* state, uint8_t on) {
	state->interrupts = on;
}