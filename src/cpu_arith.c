#include "cpu.h"
#include <stdlib.h>

void cpu_inc_reg8(cpu_state* state, uint8_t* reg) {
	cpu_add_reg8(state, reg, 1);
}

void cpu_dec_reg8(cpu_state* state, uint8_t* reg) {
	cpu_sub_reg8(state, reg, 1);
}

bool bit_7_carry(uint8_t v1, uint8_t v2) {
	return (v1 + v2) < v1;
}

bool bit_3_carry(uint8_t v1, uint8_t v2) {
	return ((v1 << 4) + (v2 << 4)) < (v1 << 4);
}

void cpu_add_reg8(cpu_state* state, uint8_t* reg, uint8_t v) {
	uint16_t carried = *reg + v;
	uint8_t half_carry = (*reg & 0xF) + (v & 0xF);
	*reg = *reg + v;
	cpu_set_flags(state, *reg == 0, 0, half_carry > 15, carried > 255); //TODO: HC
	cpu_instr_m(state, 1);
}

void cpu_add_reg16(cpu_state* state, uint16_t* reg, uint16_t v) {
	uint32_t carried = *reg + v;
	uint16_t half_carry = (*reg & 0xF) + (v & 0xF);
	*reg = *reg + v;
	cpu_set_flags(state, *reg == 0, 0, half_carry > 256, carried > 65536); //TODO: HC
	cpu_instr_m(state, 4);
}

void cpu_adc_reg8(cpu_state* state, uint8_t* reg, uint8_t v) {
	cpu_add_reg8(state, reg, cpu_is_flag(state, CARRY_FLAG) ? v + 1 : v);
}

void cpu_sub_reg8(cpu_state* state, uint8_t* reg, uint8_t v) {
	int16_t subbed = *reg - v;
	*reg = *reg - v;
	cpu_set_flags(state, *reg == 0, 1, 0, subbed < 0); //TODO: HC
	cpu_instr_m(state, 1);
}

void cpu_and_reg8(cpu_state* state, uint8_t* reg, uint8_t v) {
	*reg = (*reg && v) ? 1 : 0;
	cpu_set_flags(state, *reg == 0, 0, 1, 0);
	cpu_instr_m(state, 1);
}

void cpu_or_reg8(cpu_state* state, uint8_t* reg, uint8_t v) {
	*reg = *reg || v;
	cpu_set_flags(state, *reg == 0, 0, 0, 0);
	cpu_instr_m(state, 1);
}

void cpu_xor_reg8(cpu_state* state, uint8_t* reg, uint8_t v) {
	*reg = *reg ^ v;
	cpu_set_flags(state, *reg == 0, 0, 0, 0);
	cpu_instr_m(state, 1);
}

void cpu_inc_reg16(cpu_state* state, uint16_t* reg) {
	*reg = *reg + 1;
	cpu_instr_m(state, 2);
	//No flags affected
}

void cpu_dec_reg16(cpu_state* state, uint16_t* reg) {
	*reg = *reg - 1;
	cpu_instr_m(state, 2);
	//No flags affected
}

void cpu_grid_0x00x3_0x40x5(cpu_state* state, uint8_t gnibble, uint8_t lnibble) {

	//Row of (HL) isntructions
	if (gnibble == 0x3) {
		uint8_t v;
		switch (lnibble) {
			case 4: //Inc
				v = mem_get(&state->mem, state->registers.hl);
				cpu_inc_reg8(state, &v);
				mem_set(&state->mem, state->registers.hl, v);
				break;
			case 5: //Dec
				v = mem_get(&state->mem, state->registers.hl);
				cpu_dec_reg8(state, &v);
				mem_set(&state->mem, state->registers.hl, v);
				break;
		}
	} else {
		uint8_t* reg = cpu_reg_8_bdh(state, gnibble);
		
		switch (lnibble) {
			case 4: //Inc
				cpu_inc_reg8(state, reg);
				break;
			case 5: //Dec
				cpu_dec_reg8(state, reg);
				break;
		}
	}
}

void cpu_grid_0x00x3_0xC0xD(cpu_state* state, uint8_t gnibble, uint8_t lnibble) {

	uint8_t* reg = cpu_reg_cela(state, gnibble);
		
	switch (lnibble) {
		case 0xC: //Inc
			cpu_inc_reg8(state, reg);
			break;
		case 0xD: //Dec
			cpu_dec_reg8(state, reg);
			break;
	}
}

void cpu_grid_dec_16(cpu_state* state, uint8_t gnibble) {
	cpu_dec_reg16(state, cpu_reg_16_bdhs(state, gnibble));
}

void cpu_grid_inc_16(cpu_state* state, uint8_t gnibble) {
	cpu_inc_reg16(state, cpu_reg_16_bdhs(state, gnibble));
}

void cpu_swap_reg8(cpu_state* state, uint8_t* reg) {
	printf("WARNING: SWAP UNTESTED\n");

	uint8_t v = *reg;
	uint8_t lhs = v & 0xF;
	uint8_t rhs = v & (0xF << 4);
	uint8_t new = (lhs << 4) + (rhs >> 4);

	*reg = new;
	cpu_set_flags(state, *reg == 0, 0, 0, 0);
	cpu_instr_m(state, 2);
}

void cpu_rl_reg8(cpu_state* state, uint8_t* reg) {
	uint8_t carry_bit = cpu_is_flag(state, CARRY_FLAG);

	//We rotate a into the carry so
	//its a left shift of 1 carrying
	//the 8th bit at the start into the carry
	uint8_t next_carry = *reg & 0x80;

	*reg = *reg << 1;

	if (carry_bit) {
		*reg = *reg | 1;
	}

	cpu_set_flags(state, *reg == 0, 0, 0, next_carry);
	cpu_instr_m(state, 2);
}

void cpu_rlc_reg8(cpu_state* state, uint8_t* reg) {

	//We rotate a into the carry so
	//its a left shift of 1 carrying
	//the 8th bit at the start into the carry
	uint8_t next_carry = *reg & 0x80;

	*reg = *reg << 1;

	if (next_carry) {
		*reg = *reg | 1;
	}

	cpu_set_flags(state, *reg == 0, 0, 0, next_carry);
	cpu_instr_m(state, 2);
}

void cpu_add_hl(cpu_state* state, uint8_t gnibble) {
	uint16_t* reg = cpu_reg_16_bdhs(state, gnibble);
	cpu_add_reg16(state, &state->registers.hl, *reg);
}

void cpu_grid_xor8(cpu_state* state, uint8_t lnibble) {
	cpu_xor_reg8(state, &state->registers.a, *cpu_reg_bcdehla(state, lnibble - 0x8));
}

void cpu_grid_adc(cpu_state* state, uint8_t lnibble) {
	cpu_adc_reg8(state, &state->registers.a, *cpu_reg_bcdehla(state, lnibble - 0x8));
}

void cpu_grid_arith_0x80xB_0x00x7(cpu_state* state, uint8_t gnibble, uint8_t lnibble) {

	if (lnibble == 0x6) {
		printf("GRID HL METHODS NOT IMPL YET\n");
		exit(1);
	}

	uint8_t* reg = cpu_reg_bcdehla(state, lnibble);

	switch (gnibble) {
		case 0x8: //Add
			cpu_add_reg8(state, &state->registers.a, *reg);
			break;
		case 0x9: //Sub
			cpu_sub_reg8(state, &state->registers.a, *reg);
			break;
		case 0xA: //And
			cpu_and_reg8(state, &state->registers.a, *reg);
			break;
		case 0xB: //Or
			cpu_or_reg8(state, &state->registers.a, *reg);
			break;
	}
}