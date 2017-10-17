#include "cpu.h"

void cpu_inc_reg8(cpu_state* state, uint8_t* reg) {
	*reg = *reg + 1;
	cpu_set_flags(state, *reg == 0, 0, 0, 0);
}

void cpu_dec_reg8(cpu_state* state, uint8_t* reg) {
	*reg = *reg - 1;
	cpu_set_flags(state, *reg == 0, 1, 0, 0); //TODO: Carry flags
}

void cpu_dec_reg16(cpu_state* state, uint16_t* reg) {
	*reg = *reg - 1;
}

bool cpu_inc_16_bit_0x3(cpu_state* state, uint8_t gnibble) {
	uint16_t* reg = cpu_reg_16_bdhs(state, gnibble);
	*reg = *reg + 1;
	cpu_set_flags(state, *reg == 0, 0, 0, 0);
	cpu_inc_pc(state, 1);
	return true;	
}

bool cpu_grid_0x00x3_0x40x5(cpu_state* state, uint8_t gnibble, uint8_t lnibble) {

	//Row of (HL) isntructions
	if (gnibble == 0x3) {
		printf("HL NOT DONE\n");
		return false;
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

		cpu_inc_pc(state, 1);
	}

	return true;
}

bool cpu_grid_0x00x3_0xC0xD(cpu_state* state, uint8_t gnibble, uint8_t lnibble) {

	uint8_t* reg = cpu_reg_cela(state, gnibble);
		
	switch (lnibble) {
		case 0xC: //Inc
			cpu_inc_reg8(state, reg);
			break;
		case 0xD: //Dec
			cpu_dec_reg8(state, reg);
			break;
	}

	cpu_inc_pc(state, 1);

	return true;
}

void cpu_add_reg8(cpu_state* state, uint8_t* reg, uint8_t v) {
	*reg = *reg + v;
	cpu_set_flags(state, *reg == 0, 0, 0, 0); //TODO: Carry flags
}

void cpu_sub_reg8(cpu_state* state, uint8_t* reg, uint8_t v) {
	*reg = *reg - v;
	cpu_set_flags(state, *reg == 0, 1, 0, 0); //TODO: Carry flags
}

void cpu_and_reg8(cpu_state* state, uint8_t* reg, uint8_t v) {
	*reg = *reg && v;
	cpu_set_flags(state, *reg == 0, 0, 1, 0);
}

void cpu_or_reg8(cpu_state* state, uint8_t* reg, uint8_t v) {
	*reg = *reg || v;
	cpu_set_flags(state, *reg == 0, 0, 0, 0);
}

void cpu_grid_dec_16(cpu_state* state, uint8_t gnibble) {
	uint8_t* reg = cpu_reg_16_bdhs(state, gnibble);
	cpu_dec_reg16(state, reg);
	cpu_inc_pc(state, 1);
}

void cpu_rl_reg8(cpu_state* state, uint8_t* reg) {
	uint8_t carry_bit = cpu_is_flag(state, CARRY_FLAG);

	//We rotate a into the carry so
	//its a left shift of 1 carrying
	//the 8th bit at the start into the carry
	uint8_t next_carry = *reg & (1 << 7);

	*reg = *reg << 1;

	if (carry_bit) {
		*reg = *reg | 1;
	}

	cpu_set_flags(state, *reg == 0, 0, 0, next_carry);
}

void cpu_rlc_reg8(cpu_state* state, uint8_t* reg) {

	//We rotate a into the carry so
	//its a left shift of 1 carrying
	//the 8th bit at the start into the carry
	uint8_t next_carry = *reg & (1 << 7);

	*reg = *reg << 1;

	if (next_carry) {
		*reg = *reg | 1;
	}

	cpu_set_flags(state, *reg == 0, 0, 0, next_carry);
}

bool cpu_grid_arith_0x80xB_0x00x7(cpu_state* state, uint8_t gnibble, uint8_t lnibble) {
/*			case LD_L_REF_HL:
			state->registers.l = mem_get(&state->mem, state->registers.hl);
			cpu_inc_pc(state, 1);
			break; */
	if (lnibble == 0x6) {
		printf("GRID HL METHODS NOT IMPL YET\n");
		return false;
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

	cpu_inc_pc(state, 1);
	return true;
}