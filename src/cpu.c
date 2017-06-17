#include "cpu.h"
#include <stdio.h>

const uint16_t START_PC = 0x100;
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

void cpu_call(cpu_state* state, uint16_t val) {
	stack_push16(state, state->registers.pc + 3);
	printf("Pushed stack\n");
	state->registers.pc = val;
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

void do_flags8(cpu_state* state, uint8_t v) {
	uint8_t flags = 0;

	if (!v) {
		flags = flags & ZERO_FLAG;
	}

	flag(state, ZERO_FLAG);
}

void do_flags16(cpu_state* state, uint8_t v) {
	uint8_t flags = 0;

	if (!v) {
		flags = flags & ZERO_FLAG;
	}

	flag(state, ZERO_FLAG);
}

void cpu_addfix8(cpu_state* state, int8_t v, uint8_t* reg) {
	*reg += v;
	do_flags8(state, *reg);
	inc_pc(state, 1);
}

void cpu_addfix16(cpu_state* state, int16_t v, uint16_t* reg) {
	*reg += v;
	do_flags16(state, *reg);
	inc_pc(state, 1);
}

void cpu_ld16(cpu_state* state, uint16_t* reg) {
	*reg = mem_get16(&state->mem, state->registers.pc + 1);
	inc_pc(state, 3);
}

void cpu_mov8(cpu_state* state, uint8_t* to, uint8_t* from) {
	*to = *from;
	inc_pc(state, 1);
}

void cpu_sub8(cpu_state* state, uint8_t* lhs, uint8_t* rhs) {
	*lhs -= *rhs;
	do_flags8(state, *lhs);
	inc_pc(state, 1);
}

bool cpu_step(cpu_state* state) {
	printf("Step PC=%x\n", state->registers.pc);
	uint8_t c_instr = mem_get(&state->mem, state->registers.pc);

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
		case DEC_D:
			cpu_addfix8(state, -1, &state->registers.d);
			break;
		case LDD_REF_HL_A:
			mem_set(&state->mem, state->registers.hl, state->registers.a);
			state->registers.hl--;
			inc_pc(state, 1);
			break;
		case JR_NZ_n:
			
			if (!isflag(state, ZERO_FLAG)) {
				int8_t rjump = (int8_t) mem_get(&state->mem, state->registers.pc);
				printf("Signed Jump %x from %x\n", rjump, mem_get(&state->mem, state->registers.pc));
				state->registers.pc += rjump;
			} else {
				inc_pc(state, 1);
			}

			break;
		case LD_D_B:
			cpu_mov8(state, &state->registers.d, &state->registers.b);
			break;
		case LD_D_C:
			cpu_mov8(state, &state->registers.d, &state->registers.c);
			break;
		case LD_D_D:
			cpu_mov8(state, &state->registers.d, &state->registers.d);
			break;
		case LD_D_E:
			cpu_mov8(state, &state->registers.d, &state->registers.e);
			break;
		case LD_D_H:
			cpu_mov8(state, &state->registers.d, &state->registers.h);
			break;
		case LD_D_L:
			cpu_mov8(state, &state->registers.d, &state->registers.l);
			break;
		case LD_D_A:
			cpu_mov8(state, &state->registers.d, &state->registers.a);
			break;
		case LD_E_B:
			cpu_mov8(state, &state->registers.e, &state->registers.b);
			break;
		case LD_L_H:
			cpu_mov8(state, &state->registers.l, &state->registers.h);
			break;
		case LD_E_C:
			cpu_mov8(state, &state->registers.e, &state->registers.c);
			break;
		case LD_E_D:
			cpu_mov8(state, &state->registers.e, &state->registers.d);
			break;
		case LD_C_B:
			cpu_mov8(state, &state->registers.c, &state->registers.b);
			break;
		case LD_C_C:
			cpu_mov8(state, &state->registers.c, &state->registers.c);
			break;
		case LD_C_D:
			cpu_mov8(state, &state->registers.c, &state->registers.d);
			break;
		case LD_E_E:
			cpu_mov8(state, &state->registers.e, &state->registers.e);
			break;
		case LD_L_A:
			cpu_mov8(state, &state->registers.l, &state->registers.a);
			break;
		case LD_REF_HL_L:
			mem_set(&state->mem, state->registers.hl, state->registers.l);
			inc_pc(state, 1);
			break;
		case LD_L_REF_HL:
			state->registers.l = mem_get(&state->mem, state->registers.hl);
			inc_pc(state, 1);
			break;
		case SUB_A_B:
			state->registers.a -= state->registers.b;
			inc_pc(state, 1);
			break;
		case SUB_A_C:
			state->registers.a -= state->registers.c;
			inc_pc(state, 1);
			break;
		case SUB_A_D:
			state->registers.a -= state->registers.d;
			inc_pc(state, 1);
			break;
		case SUB_A_E:
			state->registers.a -= state->registers.e;
			inc_pc(state, 1);
			break;
		case SUB_A_H:
			state->registers.a -= state->registers.h;
			inc_pc(state, 1);
			break;
		case SUB_A_L:
			state->registers.a -= state->registers.h;
			inc_pc(state, 1);
			break;
		case LD_D_REF_HL:
			state->registers.d = mem_get(&state->mem, state->registers.hl);
			inc_pc(state, 1);
			break;
		case LD_C_n:
			state->registers.c = mem_get(&state->mem, state->registers.pc + 1);
			inc_pc(state, 2);
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
			state->registers.pc = mem_get16(&state->mem, state->registers.pc + 1);
			printf("JP %x\n", state->registers.pc);
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
		case XOR_A:
			state->registers.a = state->registers.a ^ state->registers.a;
			inc_pc(state, 1);
			printf("XOR %x\n", state->registers.a);
			break;
		case LOGICAL_NOT_A:
			state->registers.a = !state->registers.a;
			inc_pc(state, 1);
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