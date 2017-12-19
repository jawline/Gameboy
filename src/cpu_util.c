#include "cpu.h"
#include <stdlib.h>

uint8_t* cpu_reg_8_bdh(cpu_state* state, uint8_t off) {

	switch (off) {
		case 0:
			return &state->registers.b;
		case 1:
			return &state->registers.d;
		case 2:
			return &state->registers.h;
	}

	DEBUG_OUT("ERR LOOKUP 8 BIT REG (BDH) given %i\n", off);
	exit(1);
}

uint16_t* cpu_reg_16_bdha(cpu_state* state, uint8_t off) {

	switch (off) {
		case 0:
			return &state->registers.bc;
		case 1:
			return &state->registers.de;
		case 2:
			return &state->registers.hl;
		case 3:
			return &state->registers.af;
	}

	DEBUG_OUT("ERR LOOKUP 16 BIT REG\n");
	exit(1);
}

uint16_t* cpu_reg_16_bdhs(cpu_state* state, uint8_t off) {

	switch (off) {
		case 0:
			return &state->registers.bc;
		case 1:
			return &state->registers.de;
		case 2:
			return &state->registers.hl;
		case 3:
			return &state->registers.sp;
	}

	DEBUG_OUT("ERR LOOKUP 16 BIT REG\n");
	exit(1);
}

uint8_t* cpu_reg_bcdehla(cpu_state* state, uint8_t c_instr_lesser_nibble) {
	
	switch (c_instr_lesser_nibble) {
		case 0:
			return &state->registers.b;
		case 1:
			return &state->registers.c;
		case 2:
			return &state->registers.d;
		case 3:
			return &state->registers.e;
		case 4:
			return &state->registers.h;
		case 5:
			return &state->registers.l;
		case 7:
			return &state->registers.a;
	}

	DEBUG_OUT("Should be unrachable hit BCDEHLA\n");
	exit(1);
}

uint8_t* cpu_reg_cela(cpu_state* state, uint8_t off) {

	switch (off) {
		case 0:
			return &state->registers.c;
		case 1:
			return &state->registers.e;
		case 2:
			return &state->registers.l;
		case 3:
			return &state->registers.a;
	}

	DEBUG_OUT("Should be unreachable hit CELA\n");
	exit(1);
}

uint8_t cpu_instr_nb(cpu_state* state) {
	return mem_get(&state->mem, state->registers.pc++);
}

uint16_t cpu_instr_nw(cpu_state* state) {
	uint16_t t = mem_get16(&state->mem, state->registers.pc);
	state->registers.pc += 2;
	return t;
}

void cpu_instr_m(cpu_state* state, uint8_t m) {
	state->registers.lc.m = m;
	state->registers.lc.t = m * 4;
}