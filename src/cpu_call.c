#include "cpu.h"

void cpu_call(cpu_state* state, uint16_t address, uint16_t next_instr) {
	DEBUG_OUT("CPU CALL 0x%02X\n", address);
	stack_push16(state, next_instr);
	state->registers.pc = address;
}

void cpu_ret(cpu_state* state) {
	DEBUG_OUT("CPU RET INSTR\n");
	state->registers.pc = stack_pop16(state);
}

void cpu_rst_table_offset(cpu_state* state, uint8_t gnibble, uint16_t offset) {
	uint16_t callsite = offset + (0x10 * (gnibble - 0xC));
	DEBUG_OUT("RST %x\n", callsite);
	cpu_call(state, callsite, state->registers.pc);
	cpu_instr_m(state, 8);
}