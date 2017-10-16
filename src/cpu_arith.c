#include "cpu.h"

void cpu_addfix16(cpu_state* state, int16_t v, uint16_t* reg) {
	*reg += v;
	cpu_inc_pc(state, 1);
}

bool cpu_inc_16_bit_0x3(cpu_state* state, uint8_t gnibble) {
	uint16_t* reg = cpu_reg_16_bdhs(state, gnibble);
	*reg++;
	cpu_set_flags(state, *reg == 0, 0, 0, 0);
	cpu_inc_pc(state, 1);
	return true;	
}