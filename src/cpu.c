#include "cpu.h"
#include <stdio.h>

const uint16_t START_PC = 0x0;

void cpu_init(cpu_state* state) {
	state->registers.pc = START_PC;
	state->registers.f = 0;
}

uint8_t rotl8(const uint8_t value, uint32_t shift) {
    if ((shift &= sizeof(value) * 8 - 1) == 0)
      return value;
    return (value << shift) | (value >> (sizeof(value) * 8 - shift));
}

void cpu_inc_reg8(cpu_state* state, int8_t v, uint8_t* reg) {
	*reg += v;
	cpu_set_flags(state, !(*reg), 0, 0, 0);
	cpu_inc_pc(state, 1);
}

void cpu_addfix16(cpu_state* state, int16_t v, uint16_t* reg) {
	*reg += v;
	cpu_inc_pc(state, 1);
}

void cpu_sub8(cpu_state* state, uint8_t* lhs, uint8_t rhs) {
	*lhs -= rhs;
	cpu_set_flags(state, !(*lhs), 1, 0, 0); //TODO: Carry flags
	cpu_inc_pc(state, 1);
}

void cpu_dec16(cpu_state* state, uint16_t* reg) {
	*reg -= 1;
	cpu_set_flags(state, *reg == 0, 1, 0, 0); //TODO: Carry flags
	cpu_inc_pc(state, 1);	
}

void cpu_dec8(cpu_state* state, uint8_t* reg) {
	*reg -= 1;
	cpu_set_flags(state, !(*reg), 1, 0, 0); //TODO: Carry flags
	cpu_inc_pc(state, 1);
}

void cpu_setinterrupts(cpu_state* state, char on) {
	state->interrupts = on;
	cpu_inc_pc(state, 1);
}

void cpu_mov_ref_hl8(cpu_state* state, uint8_t* reg) {
	mem_set(&state->mem, state->registers.hl, *reg);
	cpu_inc_pc(state, 1);
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
	cpu_set_flags(state, !res, 1, 0, 0);
}

void cpu_cmp_a_imm_8(cpu_state* state) {
	cpu_cp_a(state, mem_get(&state->mem, state->registers.pc + 1));
	cpu_inc_pc(state, 2);
}

void cpu_cpl(cpu_state* state, uint8_t* reg) {
    *reg = ~(*reg);
	cpu_set_flags(state, cpu_is_flag(state, ZERO_FLAG), 1, 1, cpu_is_flag(state, CARRY_FLAG));
	cpu_inc_pc(state, 1);
}

void cpu_xor_reg(cpu_state* state, uint8_t* reg, uint8_t v) {
	DEBUG_OUT("XOR %x\n", *reg);
	*reg = *reg ^ v;
	cpu_set_flags(state, *reg == 0, 0, 0, 0);
	cpu_inc_pc(state, 1);
}

void cpu_add_reg_to_a(cpu_state* state, uint8_t reg) {
	state->registers.a += reg;
	cpu_set_flags(state, state->registers.a == 0, 0, 0, 0); //TODO: Carry flags
	cpu_inc_pc(state, 1);
}

void cpu_load_a_from_address(cpu_state* state, uint16_t addr) {
	state->registers.a = mem_get(&state->mem, addr);
	cpu_inc_pc(state, 1);
}

bool cpu_step(cpu_state* state) {
	
	uint16_t start_pc = state->registers.pc;
	DEBUG_OUT("Step PC=0x%02X\n", state->registers.pc);
	uint8_t c_instr = mem_get(&state->mem, state->registers.pc);
	DEBUG_OUT("Instr 0x%02X PC(idx):%i mem_get:%02X (%i)\n", c_instr, state->registers.pc, mem_get(&state->mem, state->registers.pc), mem_get(&state->mem, state->registers.pc));

	uint8_t c_instr_greater_nibble = c_instr >> 4;
	uint8_t c_instr_lesser_nibble = c_instr & 0x0F;

	if (c_instr >= 0x40 && c_instr < 0x80 && c_instr != HALT) {
		if (!cpu_ld_table_large(state, c_instr)) {
			return false;
		}
	} else if (c_instr_greater_nibble < 4 && c_instr_lesser_nibble == 0x1) {
		if (!cpu_ld_16_imm_list(state, c_instr_greater_nibble)) {
			return false;
		}
	} else if (c_instr_greater_nibble < 4 && c_instr_lesser_nibble == 0xE) {
		if (!cpu_ld_8_n_list_E(state, c_instr_greater_nibble)) {
			return false;
		}
	} else {
		if(!cpu_base_table(state, c_instr)) {
			return false;
		}
	}

	printf("Done INSTR=0x%02X (%i) SPC=0x%02X PC=0x%02X SIZE=%i HL=0x%04X FLAGS=%01i%01i%01i%01i\n", c_instr, c_instr, start_pc, state->registers.pc, state->registers.pc - start_pc, state->registers.hl, cpu_is_flag(state, ZERO_FLAG), cpu_is_flag(state, SUBTRACT_FLAG), cpu_is_flag(state, HALF_CARRY_FLAG), cpu_is_flag(state, CARRY_FLAG));

	return true;
}