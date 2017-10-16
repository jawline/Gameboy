#include "cpu.h"

void cpu_addfix16(cpu_state* state, int16_t v, uint16_t* reg) {
	*reg += v;
	cpu_inc_pc(state, 1);
}

bool cpu_inc_16_bit_0x3(cpu_state* state, uint8_t gnibble) {
	uint16_t* reg = cpu_reg_16_bdhs(state, gnibble);
	cpu_addfix16(state, 1, cpu_reg_16_bdhs(state, gnibble));
	return true;	
}