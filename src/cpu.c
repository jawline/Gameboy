#include "cpu.h"
#include <stdio.h>

const uint16_t START_PC = 0x0;

void cpu_init(cpu_state* state) {
	state->registers.pc = START_PC;
	state->registers.f = 0;
}

void cpu_call(cpu_state* state, uint16_t address) {
	DEBUG_OUT("CPU CALL 0x%02X\n", address);
	stack_push16(state, state->registers.pc + 3);
	state->registers.pc = address;
}

void cpu_call_nn(cpu_state* state) {
	cpu_call(state, mem_get16(&state->mem, state->registers.pc + 1));
}

uint8_t rotl8(const uint8_t value, uint32_t shift) {
    if ((shift &= sizeof(value) * 8 - 1) == 0)
      return value;
    return (value << shift) | (value >> (sizeof(value) * 8 - shift));
}

void cpu_inc_reg8(cpu_state* state, int8_t v, uint8_t* reg) {
	*reg += v;
	do_flags(state, !(*reg), 0, 0, 0);
	cpu_inc_pc(state, 1);
}

void cpu_addfix16(cpu_state* state, int16_t v, uint16_t* reg) {
	*reg += v;
	cpu_inc_pc(state, 1);
}

void cpu_ld16(cpu_state* state, uint16_t* reg) {
	*reg = mem_get16(&state->mem, state->registers.pc + 1);
	cpu_inc_pc(state, 3);
}

void cpu_mov8(cpu_state* state, uint8_t* to, uint8_t val) {
	*to = val;
	cpu_inc_pc(state, 1);
}

void cpu_sub8(cpu_state* state, uint8_t* lhs, uint8_t rhs) {
	*lhs -= rhs;
	do_flags(state, !(*lhs), 1, 0, 0); //TODO: Carry flags
	cpu_inc_pc(state, 1);
}

void cpu_dec16(cpu_state* state, uint16_t* reg) {
	*reg -= 1;
	do_flags(state, *reg == 0, 1, 0, 0); //TODO: Carry flags
	cpu_inc_pc(state, 1);	
}

void cpu_dec8(cpu_state* state, uint8_t* reg) {
	*reg -= 1;
	do_flags(state, !(*reg), 1, 0, 0); //TODO: Carry flags
	cpu_inc_pc(state, 1);
}

void cpu_load_8(cpu_state* state, uint8_t* reg) {
	uint8_t lval = mem_get(&state->mem, state->registers.pc + 1);
	*reg = lval;
	cpu_inc_pc(state, 2);
}

void cpu_setinterrupts(cpu_state* state, char on) {
	state->interrupts = on;
	cpu_inc_pc(state, 1);
}

void cpu_mov_ref_hl8(cpu_state* state, uint8_t* reg) {
	mem_set(&state->mem, state->registers.hl, *reg);
	cpu_inc_pc(state, 1);
}

void cpu_save_reg_at(cpu_state* state, uint16_t offset, uint8_t addr, uint8_t regval) {
	uint16_t final_addr = offset + addr;
	mem_set(&state->mem, final_addr, regval);
}

void cpu_save_flags_register(cpu_state* state, uint8_t* reg) {
	const uint16_t ADDR_OFFSET = 0xFF00;
	uint16_t addr = ADDR_OFFSET + mem_get(&state->mem, state->registers.pc + 1);
	*reg = mem_get(&state->mem, addr);
	cpu_inc_pc(state, 2);
}

void cpu_save_reg_to_addr_then_dec_addr(cpu_state* state, uint16_t* reg_addr, uint8_t* reg) {
	mem_set(&state->mem, *reg_addr, *reg);
	cpu_dec16(state, reg_addr);
}

void cpu_load_addr_16_reg(cpu_state* state, uint8_t* reg) {
	uint16_t addr = mem_get16(&state->mem, state->registers.pc + 1);
	mem_set(&state->mem, addr, *reg);
	cpu_inc_pc(state, 3);
}

void cpu_load_addr_16_reg16(cpu_state* state, uint16_t* reg) {
	uint16_t addr = mem_get16(&state->mem, state->registers.pc + 1);
	mem_set16(&state->mem, addr, *reg);
	cpu_inc_pc(state, 3);
}

void cpu_load_ref_reg_16_imm_8(cpu_state* state) {
	uint8_t val = mem_get(&state->mem, state->registers.pc + 1);
	mem_set(&state->mem, state->registers.hl, val);
	cpu_inc_pc(state, 2);
}

void cpu_jnz_imm_8(cpu_state* state) {
	const unsigned INSTR_SIZE = 2;
	DEBUG_OUT("Flags %x ZFLAG %x\n", state->registers.f, state->registers.f & ZERO_FLAG);
	if (!cpu_is_flag(state, ZERO_FLAG)) {
		DEBUG_OUT("JR NZ %i from %x\n", ((int8_t) mem_get(&state->mem, state->registers.pc + 1)), state->registers.pc);
		state->registers.pc += ((int8_t) mem_get(&state->mem, state->registers.pc + 1)) + INSTR_SIZE;
	} else {
		cpu_inc_pc(state, INSTR_SIZE);
	}
}

void cpu_jump_imm_16(cpu_state* state) {
	uint16_t addr = mem_get16(&state->mem, state->registers.pc + 1);
	state->registers.pc = addr;
	DEBUG_OUT("Jump Immediate: %x\n", addr);
}

void cpu_cp_a(cpu_state* state, uint8_t val) {
	uint8_t res = state->registers.a - val;
	do_flags(state, !res, 1, 0, 0);
}

void cpu_cmp_a_imm_8(cpu_state* state) {
	cpu_cp_a(state, mem_get(&state->mem, state->registers.pc + 1));
	cpu_inc_pc(state, 2);
}

void cpu_cpl(cpu_state* state, uint8_t* reg) {
    *reg = ~(*reg);
	do_flags(state, cpu_is_flag(state, ZERO_FLAG), 1, 1, cpu_is_flag(state, CARRY_FLAG));
	cpu_inc_pc(state, 1);
}

void cpu_xor_reg(cpu_state* state, uint8_t* reg, uint8_t v) {
	DEBUG_OUT("XOR %x\n", *reg);
	*reg = *reg ^ v;
	do_flags(state, *reg == 0, 0, 0, 0);
	cpu_inc_pc(state, 1);
}

void ext_cpu_step_bit_test_8bit_reg(cpu_state* state, uint8_t* reg, uint8_t bit) {
	DEBUG_OUT("Testing reg %x\n", *reg);
	uint8_t tested = *reg & (0x1 << bit);
	DEBUG_OUT("Tested bit R %x\n", tested);


	do_flags(state, tested == 0, 0, 1, cpu_is_flag(state, CARRY_FLAG));
}

bool ext_cpu_step_bit(uint8_t c_instr, cpu_state* state) {

	uint8_t c_instr_greater_nibble = c_instr >> 4;
	uint8_t c_instr_lesser_nibble = c_instr & 0x0F;

	DEBUG_OUT("EXT CPU Step Bit instr %x %x\n", c_instr_greater_nibble, c_instr_lesser_nibble);

	uint8_t start_offset = 0;

	if (c_instr_lesser_nibble >= 8) {
		start_offset = 1;
		c_instr_lesser_nibble -= 8;
	}

	uint8_t selected_bit = (c_instr_greater_nibble - 4) * 2 + start_offset;

	DEBUG_OUT("Selected Bit %x\n", selected_bit);

	//Custom logic for the HL
	if (c_instr_lesser_nibble == 6) {
		DEBUG_OUT("HL BIT NOT IMPL\n");
		return false;
	} else {
		//It's an 8 bit reg instr
		uint8_t* reg;

		switch (c_instr_lesser_nibble) {
			case 0:
				reg = &state->registers.b;
				break;
			case 1:
				reg = &state->registers.c;
				break;
			case 2:
				reg = &state->registers.d;
				break;
			case 3:
				reg = &state->registers.e;
				break;
			case 4:
				reg = &state->registers.h;
				break;
			case 5:
				reg = &state->registers.l;
				break;
			case 7:
				reg = &state->registers.a;
				break;
			default:
				printf("Error in reg table");
				return false;
		}

		ext_cpu_step_bit_test_8bit_reg(state, reg, selected_bit);
		cpu_inc_pc(state, 1);
		return true;
	}
}

bool rl_8bit_reg(cpu_state* state, uint8_t* reg) {

	uint8_t bit_7 = *reg & (1 << 7);

	DEBUG_OUT("Pre %x v %x\n", *reg, (*reg << 1) | (*reg >> 7));
	*reg = (*reg << 1) | (*reg >> 7);

	do_flags(state, *reg, 0, 0, bit_7);

	return true;
}

bool ext_cpu_rl_8bit(cpu_state* state, uint8_t c_instr) {
	uint8_t c_instr_lesser_nibble = c_instr & 0x0F;
	uint8_t* reg;

	switch (c_instr_lesser_nibble) {
		case 0:
			reg = &state->registers.b;
			break;
		case 1:
			reg = &state->registers.c;
			break;
		case 2:
			reg = &state->registers.d;
			break;
		case 3:
			reg = &state->registers.e;
			break;
		case 4:
			reg = &state->registers.h;
			break;
		case 5:
			reg = &state->registers.l;
			break;
		default:
			printf("Bad Table EXT_CPU_RL_8BIT\n");
			return false;
	}

	cpu_inc_pc(state, 1);
	return rl_8bit_reg(state, reg);
}

bool ext_cpu_step(cpu_state* state) {
	cpu_inc_pc(state, 1);

	DEBUG_OUT("EXT PC=%x\n", state->registers.pc);
	uint8_t c_instr = mem_get(&state->mem, state->registers.pc);
	DEBUG_OUT("EXTInstr %x\n", c_instr);

	if (c_instr >= 0x40 && c_instr < 0x80) {
		return ext_cpu_step_bit(c_instr, state);
	} else if (c_instr >= 0x10 && c_instr < 0x16) {
		return ext_cpu_rl_8bit(state, c_instr);
	} else {
		switch (c_instr) {
			default:
				printf("Unknown EXT instruction %x\n", c_instr);
				return false;
		}
	}

	return true;
}

void cpu_add_reg_to_a(cpu_state* state, uint8_t reg) {
	state->registers.a += reg;
	do_flags(state, state->registers.a == 0, 0, 0, 0); //TODO: Carry flags
	cpu_inc_pc(state, 1);
}

void cpu_load_a_from_address(cpu_state* state, uint16_t addr) {
	state->registers.a = mem_get(&state->mem, addr);
	cpu_inc_pc(state, 1);
}

void cpu_ret(cpu_state* state) {
	state->registers.pc = stack_pop16(state);
}

bool cpu_base_table(cpu_state* state, uint8_t c_instr) {
	switch (c_instr) {
		case NOOP:
			cpu_inc_pc(state, 1);
			break;
		case RLC_A:
			state->registers.a = rotl8(state->registers.a, 1);
			cpu_inc_pc(state, 1);
			break;
		case INC_BC:
			cpu_addfix16(state, 1, &state->registers.bc);
			break;
		case INC_DE:
			cpu_addfix16(state, 1, &state->registers.de);
			break;
		case INC_HL:
			cpu_addfix16(state, 1, &state->registers.hl);
			break;
		case INC_SP:
			cpu_addfix16(state, 1, &state->registers.sp);
			break;
		case INC_B:
			cpu_inc_reg8(state, 1, &state->registers.b);
			break;
		case INC_C:
			cpu_inc_reg8(state, 1, &state->registers.c);
			break;
		case INC_E:
			cpu_inc_reg8(state, 1, &state->registers.e);
			break;
		case INC_L:
			cpu_inc_reg8(state, 1, &state->registers.l);
			break;
		case INC_A:
			cpu_inc_reg8(state, 1, &state->registers.a);
			break;
		case INC_D:
			cpu_inc_reg8(state, 1, &state->registers.d);
			break;
		case INC_H:
			cpu_inc_reg8(state, 1, &state->registers.h);
			break;
		case DEC_BC:
			cpu_addfix16(state, -1, &state->registers.bc);
			break;

		case DEC_H:
			cpu_dec8(state, &state->registers.h);
			break;
		case DEC_E:
			cpu_dec8(state, &state->registers.e);
			break;
		case DEC_B:
			cpu_dec8(state, &state->registers.b);
			break;
		
		case DEC_C:
			cpu_dec8(state, &state->registers.c);
			break;
		case DEC_D:
			cpu_dec8(state, &state->registers.d);
			break;
		case DEC_L:
			cpu_dec8(state, &state->registers.l);
			break;
		case DEC_A:
			cpu_dec8(state, &state->registers.a);
			break;

		case RL_A:
			rl_8bit_reg(state, &state->registers.a);
			cpu_inc_pc(state, 1);
			break;
		case LDD_REF_HL_A:
			cpu_save_reg_to_addr_then_dec_addr(state, &state->registers.hl, &state->registers.a);
			break;
		case JR_NZ_n:
			cpu_jnz_imm_8(state);
			break;
		case LDH_REF_n_A:
			cpu_save_reg_at(state, 0xFF00, mem_get(&state->mem, state->registers.pc + 1), state->registers.a);
			cpu_inc_pc(state, 2);
			break;
		case LDH_REF_C_A:
			cpu_save_reg_at(state, 0xFF00, state->registers.c, state->registers.a);
			cpu_inc_pc(state, 1);
			break;
		case LDH_REF_A_n:
			cpu_save_flags_register(state, &state->registers.a);
			break;


		case LD_A_REF_BC:
			cpu_load_a_from_address(state, state->registers.bc);
			break;
		case LD_A_REF_DE:
			cpu_load_a_from_address(state, state->registers.de);
			break;

		case LD_C_n:
			cpu_load_8(state, &state->registers.c);
			break;
		case LD_E_n:
			cpu_load_8(state, &state->registers.e);
			break;
		case LD_L_n:
			cpu_load_8(state, &state->registers.l);
			break;
		case LD_A_n:
			cpu_load_8(state, &state->registers.a);
			printf("Set A %x\n", state->registers.a);
			break;
		
		case LD_REF_HL_n:
			cpu_load_ref_reg_16_imm_8(state);
			break;
		case LD_REF_HL_B:
			cpu_mov_ref_hl8(state, &state->registers.b);
			break;
		case LD_REF_HL_C:
			cpu_mov_ref_hl8(state, &state->registers.c);
			break;
		case LD_REF_HL_D:
			cpu_mov_ref_hl8(state, &state->registers.d);
			break;
		case LD_REF_HL_E:
			cpu_mov_ref_hl8(state, &state->registers.d);
			break;
		case LD_REF_HL_H:
			cpu_mov_ref_hl8(state, &state->registers.h);
			break;
		case LD_REF_HL_L:
			cpu_mov_ref_hl8(state, &state->registers.l);
			break;
		case LD_REF_HL_A:
			cpu_mov_ref_hl8(state, &state->registers.a);
			break;

		case LD_L_REF_HL:
			state->registers.l = mem_get(&state->mem, state->registers.hl);
			cpu_inc_pc(state, 1);
			break;
		case SUB_A_B:
			cpu_sub8(state, &state->registers.a, state->registers.b);
			break;
		case SUB_A_C:
			cpu_sub8(state, &state->registers.a, state->registers.c);
			break;
		case SUB_A_D:
			cpu_sub8(state, &state->registers.a, state->registers.d);
			break;
		case SUB_A_E:
			cpu_sub8(state, &state->registers.a, state->registers.e);
			break;
		case SUB_A_H:
			cpu_sub8(state, &state->registers.a, state->registers.h);
			break;
		case SUB_A_L:
			cpu_sub8(state, &state->registers.a, state->registers.l);
			break;
		case SUB_A_A:
			cpu_sub8(state, &state->registers.a, state->registers.a);
			break;
		case SUB_A_REF_HL:
			cpu_sub8(state, &state->registers.a, mem_get(&state->mem, state->registers.hl));
			break;
		case LD_D_REF_HL:
			cpu_mov8(state, &state->registers.d, mem_get(&state->mem, state->registers.hl));
			break;

		case LDI_REF_HL_A:
			state->registers.a = mem_get(&state->mem, state->registers.hl);
			state->registers.hl++;
			cpu_inc_pc(state, 1);
			break;
		case ADD_HL_DE:
			state->registers.hl += state->registers.de;
			cpu_inc_pc(state, 1);
			break;

		case ADD_A_B:
			cpu_add_reg_to_a(state, state->registers.b);
			break;
		case ADD_A_C:
			cpu_add_reg_to_a(state, state->registers.c);
			break;
		case ADD_A_D:
			cpu_add_reg_to_a(state, state->registers.d);
			break;
		case ADD_A_E:
			cpu_add_reg_to_a(state, state->registers.e);
			break;
		case ADD_A_H:
			cpu_add_reg_to_a(state, state->registers.h);
			break;
		case ADD_A_L:
			cpu_add_reg_to_a(state, state->registers.l);
			break;
		case ADD_A_A:
			cpu_add_reg_to_a(state, state->registers.a);
			break;

		case JP_NN:
			cpu_jump_imm_16(state);
			break;

		case LD_HL_nn:
			cpu_ld16(state, &state->registers.hl);
			break;
		case LD_SP_nn:
			cpu_ld16(state, &state->registers.sp);
			break;
		case LD_DE_nn:
			cpu_ld16(state, &state->registers.de);
			break;
		case LD_REF_nn_A:
			cpu_load_addr_16_reg(state, &state->registers.a);
			break;
		case LD_REF_nn_SP:
			cpu_load_addr_16_reg16(state, &state->registers.sp);
			break;
		case XOR_A:
			cpu_xor_reg(state, &state->registers.a, state->registers.a);
			break;
		case CP_n:
			cpu_cmp_a_imm_8(state);
			break;

		case CPL_A:
			cpu_cpl(state, &state->registers.a);
			break;

		case ENABLE_INTERRUPTS:
			cpu_setinterrupts(state, 1);
			break;
		case DISABLE_INTERRUPTS:
			cpu_setinterrupts(state, 0);
			break;

		case POP_BC:
			state->registers.bc = stack_pop16(state);
			cpu_inc_pc(state, 1);
			break;

		case PUSH_BC:
			stack_push16(state, state->registers.bc);
			cpu_inc_pc(state, 1);
			break;
		
		case CALL_nn: 
			cpu_call_nn(state);
			break;

		case CALL_Z_nn:
			
			if (cpu_is_flag(state, ZERO_FLAG)) {
				cpu_call_nn(state);
			} else {
				cpu_inc_pc(state, 3);
			}

			break;

		case RET:
			cpu_ret(state);
			break;

		case RET_NZ:

			DEBUG_OUT("RET NZ\n");

			if (!cpu_is_flag(state, ZERO_FLAG)) {
				cpu_ret(state);
			} else {
				cpu_inc_pc(state, 1);
			}

			break;

		case EXT_OP:
			
			if (!ext_cpu_step(state)) {
				return false;
			}

			break;
		case RST_38:
			cpu_call(state, 0x0038);
			break;
		default:
			printf("OpCode %x Unknown\n", c_instr);
			return false;
	}

	return true;
}

bool cpu_step(cpu_state* state) {
	
	uint16_t start_pc = state->registers.pc;
	DEBUG_OUT("Step PC=0x%02X\n", state->registers.pc);
	uint8_t c_instr = mem_get(&state->mem, state->registers.pc);
	DEBUG_OUT("Instr 0x%02X PC(idx):%i mem_get:%02X (%i)\n", c_instr, state->registers.pc, mem_get(&state->mem, state->registers.pc), mem_get(&state->mem, state->registers.pc));

	if (c_instr >= 0x40 && c_instr < 0x80 && c_instr != HALT) {
		if (!cpu_ld_table(state, c_instr)) {
			return false;
		}
	} else {
		if(!cpu_base_table(state, c_instr)) {
			return false;
		}
	}

	printf("Done INSTR=0x%02X (%i) SPC=0x%02X PC=0x%02X SIZE=%i FLAGS=%01i%01i%01i%01i\n", c_instr, c_instr, start_pc, state->registers.pc, state->registers.pc - start_pc, cpu_is_flag(state, ZERO_FLAG), cpu_is_flag(state, SUBTRACT_FLAG), cpu_is_flag(state, HALF_CARRY_FLAG), cpu_is_flag(state, CARRY_FLAG));

	return true;
}