#include "cpu.h"
#include <stdio.h>

const uint16_t START_PC = 0x0;
const uint16_t START_STACK = 0xFFFE;

const uint8_t SIGN_FLAG = 0x1 << 7;
const uint8_t ZERO_FLAG = 0x1 << 6;
const uint8_t UDOC_FLAG = 0x1 << 5;
const uint8_t HALF_CARRY_FLAG = 0x1 << 4;
const uint8_t UDOC_2_FLAG = 0x1 << 3;
const uint8_t PO_FLAG = 0x1 << 2; //Parity or offset
const uint8_t SUBTRACT_FLAG = 0x1 << 2;
const uint8_t CARRY_FLAG = 0x0;

void cpu_init(cpu_state* state) {
	state->registers.pc = START_PC;
	state->registers.sp = START_STACK;
}

void inc_pc(cpu_state* state, uint16_t off) {
	state->registers.pc += off;
}

void stack_push16(cpu_state* state, uint16_t v) {
	printf("PUSH 16 %x %x\n", state->registers.sp, v);
	mem_set16(&state->mem, state->registers.sp, v);
	state->registers.sp -= 2;
}

uint16_t stack_pop16(cpu_state* state) {
	printf("POP 16\n");
	state->registers.sp += 2;
	return mem_get16(&state->mem, state->registers.sp);
}

void cpu_call(cpu_state* state, uint16_t val) {
	stack_push16(state, state->registers.pc + 3);
	state->registers.pc = val;
}

void cpu_call_nn(cpu_state* state) {
	cpu_call(state, mem_get16(&state->mem, state->registers.pc + 1));
}

uint8_t rotl8(const uint8_t value, uint32_t shift) {
    if ((shift &= sizeof(value) * 8 - 1) == 0)
      return value;
    return (value << shift) | (value >> (sizeof(value) * 8 - shift));
}

void flag(cpu_state* state, uint8_t flags) {
	state->registers.f = flags;
}

char isflag(cpu_state* state, uint8_t flags) {
	return state->registers.f & flags;
}

void do_flags(cpu_state* state, bool zero_flag, bool negative_flag, bool half_carry, bool carry) {
	uint8_t flags = state->registers.f;

	if (zero_flag) {
		flags &= ZERO_FLAG;
	} else {
		flags |= ZERO_FLAG;
	}

	if (negative_flag) {
		flags &= SUBTRACT_FLAG;
	} else {
		flags |= SUBTRACT_FLAG;
	}

	if (half_carry) {
		flags &= HALF_CARRY_FLAG;
	} else {
		flags |= HALF_CARRY_FLAG;
	}

	if (carry) {
		flags &= CARRY_FLAG;
	} else {
		flags |= CARRY_FLAG;
	}

	flag(state, flags);
}

void cpu_addfix8(cpu_state* state, int8_t v, uint8_t* reg) {
	*reg += v;
	do_flags(state, !(*reg), 0, 0, 0);
	inc_pc(state, 1);
}

void cpu_addfix16(cpu_state* state, int16_t v, uint16_t* reg) {
	*reg += v;
	inc_pc(state, 1);
}

void cpu_ld16(cpu_state* state, uint16_t* reg) {
	*reg = mem_get16(&state->mem, state->registers.pc + 1);
	inc_pc(state, 3);
}

void cpu_mov8(cpu_state* state, uint8_t* to, uint8_t val) {
	*to = val;
	inc_pc(state, 1);
}

void cpu_sub8(cpu_state* state, uint8_t* lhs, uint8_t rhs) {
	*lhs -= rhs;
	do_flags(state, !(*lhs), 1, 0, 0);
	inc_pc(state, 1);
}

void cpu_load_8(cpu_state* state, uint8_t* reg) {
	uint8_t lval = mem_get(&state->mem, state->registers.pc + 1);
	*reg = lval;
	inc_pc(state, 2);
}

void cpu_setinterrupts(cpu_state* state, char on) {
	state->interrupts = on;
	inc_pc(state, 1);
}

void cpu_mov_ref_hl8(cpu_state* state, uint8_t* reg) {
	mem_set(&state->mem, state->registers.hl, *reg);
	inc_pc(state, 1);
}

//Save register to FF00h + n 
void cpu_load_flags_register(cpu_state* state, uint8_t* reg) {
	const uint16_t ADDR_OFFSET = 0xFF00;
	uint16_t addr = ADDR_OFFSET + mem_get(&state->mem, state->registers.pc + 1);
	mem_set(&state->mem, addr, *reg);
	inc_pc(state, 2);
}

void cpu_save_flags_register(cpu_state* state, uint8_t* reg) {
	const uint16_t ADDR_OFFSET = 0xFF00;
	uint16_t addr = ADDR_OFFSET + mem_get(&state->mem, state->registers.pc + 1);
	*reg = mem_get(&state->mem, addr);
	inc_pc(state, 2);
}

void cpu_load_reg_16_reg_then_dec(cpu_state* state, uint16_t* reg_addr, uint8_t* reg) {
	mem_set(&state->mem, *reg_addr, *reg);
	*reg_addr -= 1;
	inc_pc(state, 1);
}

void cpu_load_addr_16_reg(cpu_state* state, uint8_t* reg) {
	uint16_t addr = mem_get16(&state->mem, state->registers.pc + 1);
	mem_set(&state->mem, addr, *reg);
	inc_pc(state, 3);
}

void cpu_load_addr_16_reg16(cpu_state* state, uint16_t* reg) {
	uint16_t addr = mem_get16(&state->mem, state->registers.pc + 1);
	mem_set16(&state->mem, addr, *reg);
	inc_pc(state, 3);
}

void cpu_load_ref_reg_16_imm_8(cpu_state* state) {
	uint8_t val = mem_get(&state->mem, state->registers.pc + 1);
	mem_set(&state->mem, state->registers.hl, val);
	inc_pc(state, 2);
}

void cpu_jnz_imm_8(cpu_state* state) {
	if (!isflag(state, ZERO_FLAG)) {
		int8_t rjump = (int8_t) mem_get(&state->mem, state->registers.pc);
		state->registers.pc += rjump;
		printf("Signed Jump %x from %x\n", rjump, mem_get(&state->mem, state->registers.pc));
	} else {
		inc_pc(state, 2);
	}
}

void cpu_jump_imm_16(cpu_state* state) {
	uint16_t addr = mem_get16(&state->mem, state->registers.pc + 1);
	state->registers.pc = addr;
	printf("Jump Immediate: %x\n", addr);
}

void cpu_cp_a(cpu_state* state, uint8_t val) {
	uint8_t res = state->registers.a - val;
	do_flags(state, !val, 1, 0, 0);
}

void cpu_cmp_a_imm_8(cpu_state* state) {
	cpu_cp_a(state, mem_get(&state->mem, state->registers.pc + 1));
	inc_pc(state, 2);
}

void cpu_cpl(cpu_state* state, uint8_t* reg) {
    *reg = ~(*reg);
	do_flags(state, isflag(state, ZERO_FLAG), 1, 1, isflag(state, CARRY_FLAG));
	inc_pc(state, 1);
}

void cpu_xor_reg(cpu_state* state, uint8_t* reg1, uint8_t* reg2) {
	*reg1 = *reg1 ^ *reg2;
	do_flags(state, !(*reg1), 0, 0, 0);
	inc_pc(state, 1);
}

void ext_cpu_step_bit_test_8bit_reg(cpu_state* state, uint8_t* reg, uint8_t bit) {
	uint8_t tested = *reg & (0x1 << bit);
	printf("Tested bit R %x\n", tested);

	uint8_t current_flags = state->registers.f;

	if (tested == 0) {
		current_flags &= ZERO_FLAG;
	} else {
		current_flags |= ZERO_FLAG;
	}

	current_flags &= HALF_CARRY_FLAG;
	current_flags |= SUBTRACT_FLAG;

	flag(state, current_flags);
}

bool ext_cpu_step_bit(uint8_t c_instr, cpu_state* state) {

	uint8_t c_instr_greater_nibble = c_instr >> 4;
	uint8_t c_instr_lesser_nibble = c_instr & 0x0F;

	printf("EXT CPU Step Bit instr %x %x\n", c_instr_greater_nibble, c_instr_lesser_nibble);

	uint8_t start_offset = 0;

	if (c_instr_lesser_nibble >= 8) {
		start_offset = 1;
		c_instr_lesser_nibble -= 8;
	}

	uint8_t selected_bit = (c_instr_greater_nibble - 4) * 2 + start_offset;

	printf("Selected Bit %x\n", selected_bit);

	//Custom logic for the HL
	if (c_instr_lesser_nibble == 6) {
		printf("HL BIT NOT IMPL\n");
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
		state->registers.pc += 1;
		return true;
	}
}

bool rl_8bit_reg(cpu_state* state, uint8_t* reg) {

	uint8_t bit_7 = *reg & (1 << 7);

	printf("Pre %x v %x\n", *reg, (*reg << 1) | (*reg >> 7));
	*reg = (*reg << 1) | (*reg >> 7);

	uint8_t current_flags = state->registers.f;

	if (*reg) {
		current_flags |= ZERO_FLAG;
	} else {
		current_flags &= ZERO_FLAG;
	}

	current_flags |= SUBTRACT_FLAG;
	current_flags |= HALF_CARRY_FLAG;

	if (bit_7 == 0) {
		current_flags |= CARRY_FLAG;
	} else {
		current_flags &= CARRY_FLAG;
	}

	flag(state, current_flags);

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

	state->registers.pc += 1;
	return rl_8bit_reg(state, reg);
}

bool ext_cpu_step(cpu_state* state) {
	state->registers.pc += 1;

	printf("EXT PC=%x\n", state->registers.pc);
	uint8_t c_instr = mem_get(&state->mem, state->registers.pc);
	printf("EXTInstr %x\n", c_instr);

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

bool cpu_step(cpu_state* state) {
	printf("Step PC=%x\n", state->registers.pc);
	uint8_t c_instr = mem_get(&state->mem, state->registers.pc);

	printf("Instr %x\n", c_instr);

	switch (c_instr) {
		case NOOP:
			inc_pc(state, 1);
			break;
		case RLC_A:
			state->registers.a = rotl8(state->registers.a, 1);
			inc_pc(state, 1);
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
			cpu_addfix8(state, 1, &state->registers.b);
			break;
		case INC_L:
			cpu_addfix8(state, 1, &state->registers.l);
			break;
		case INC_D:
			cpu_addfix8(state, 1, &state->registers.d);
			break;
		case INC_H:
			cpu_addfix8(state, 1, &state->registers.h);
			break;
		case DEC_BC:
			cpu_addfix16(state, -1, &state->registers.bc);
			break;
		case DEC_H:
			cpu_addfix8(state, -1, &state->registers.h);
			break;
		case DEC_E:
			cpu_addfix8(state, -1, &state->registers.e);
			break;
		case DEC_B:
			cpu_addfix8(state, -1, &state->registers.b);
			break;
		case DEC_C:
			cpu_addfix8(state, -1, &state->registers.c);
			break;
		case DEC_D:
			cpu_addfix8(state, -1, &state->registers.d);
			break;
		case RL_A:
			rl_8bit_reg(state, &state->registers.a);
			state->registers.pc += 1;
			break;
		case LDD_REF_HL_A:
			cpu_load_reg_16_reg_then_dec(state, &state->registers.hl, &state->registers.a);
			break;
		case JR_NZ_n:
			cpu_jnz_imm_8(state);
			break;
		case LDH_REF_n_A:
			cpu_load_flags_register(state, &state->registers.a);
			break;
		case LDH_REF_A_n:
			cpu_save_flags_register(state, &state->registers.a);
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
			break;
		case LD_B_B:
			cpu_mov8(state, &state->registers.b, state->registers.b);
			break;
		case LD_D_B:
			cpu_mov8(state, &state->registers.d, state->registers.b);
			break;
		case LD_D_C:
			cpu_mov8(state, &state->registers.d, state->registers.c);
			break;
		case LD_D_D:
			cpu_mov8(state, &state->registers.d, state->registers.d);
			break;
		case LD_D_E:
			cpu_mov8(state, &state->registers.d, state->registers.e);
			break;
		case LD_D_H:
			cpu_mov8(state, &state->registers.d, state->registers.h);
			break;
		case LD_D_L:
			cpu_mov8(state, &state->registers.d, state->registers.l);
			break;
		case LD_D_A:
			cpu_mov8(state, &state->registers.d, state->registers.a);
			break;
		case LD_E_H:
			cpu_mov8(state, &state->registers.e, state->registers.h);
			break;
		case LD_E_B:
			cpu_mov8(state, &state->registers.e, state->registers.b);
			break;
		case LD_L_H:
			cpu_mov8(state, &state->registers.l, state->registers.h);
			break;
		case LD_E_C:
			cpu_mov8(state, &state->registers.e, state->registers.c);
			break;
		case LD_E_D:
			cpu_mov8(state, &state->registers.e, state->registers.d);
			break;
		case LD_C_B:
			cpu_mov8(state, &state->registers.c, state->registers.b);
			break;
		case LD_C_C:
			cpu_mov8(state, &state->registers.c, state->registers.c);
			break;
		case LD_C_D:
			cpu_mov8(state, &state->registers.c, state->registers.d);
			break;
		case LD_E_E:
			cpu_mov8(state, &state->registers.e, state->registers.e);
			break;
		case LD_L_A:
			cpu_mov8(state, &state->registers.l, state->registers.a);
			break;
		case LD_REF_HL_n:
			cpu_load_ref_reg_16_imm_8(state);
			break;
		case LD_REF_HL_L:
			cpu_mov_ref_hl8(state, &state->registers.l);
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
		case LD_L_REF_HL:
			state->registers.l = mem_get(&state->mem, state->registers.hl);
			inc_pc(state, 1);
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
		case LD_C_E:
			state->registers.c = state->registers.e;
			inc_pc(state, 1);
			break;
		case LDI_REF_HL_A:
			state->registers.a = mem_get(&state->mem, state->registers.hl);
			state->registers.hl++;
			inc_pc(state, 1);
			break;
		case ADD_HL_DE:
			state->registers.hl += state->registers.de;
			inc_pc(state, 1);
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
			cpu_xor_reg(state, &state->registers.a, &state->registers.a);
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

		case PUSH_BC:
			stack_push16(state, state->registers.bc);
			state->registers.pc += 1;
			break;
		
		case CALL_nn: 
			cpu_call_nn(state);
			break;

		case CALL_Z_nn:
			
			if (isflag(state, ZERO_FLAG)) {
				cpu_call_nn(state);
			} else {
				inc_pc(state, 3);
			}

			break;

		case RET_NZ:

			if (!isflag(state, ZERO_FLAG)) {
				state->registers.pc = stack_pop16(state);
			} else {
				state->registers.pc += 1;
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

	printf("Done INSTR=%x PC=%x\n", c_instr, state->registers.pc);

	return true;
}