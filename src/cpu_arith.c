#include "cpu.h"

void cpu_addfix16(cpu_state* state, int16_t v, uint16_t* reg) {
	*reg = *reg + v;
	cpu_inc_pc(state, 1);
}

void cpu_inc_reg8(cpu_state* state, uint8_t* reg) {
	*reg = *reg + 1;
	cpu_set_flags(state, *reg == 0, 0, 0, 0);
}

void cpu_dec_reg8(cpu_state* state, uint8_t* reg) {
	*reg = *reg - 1;
	cpu_set_flags(state, *reg == 0, 1, 0, 0); //TODO: Carry flags
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