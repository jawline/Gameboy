#include "cpu.h"

void cpu_jnz_imm_8(cpu_state* state) {
	const unsigned INSTR_SIZE = 2;
	cpu_inc_pc(state, INSTR_SIZE);
	
	if (!cpu_is_flag(state, ZERO_FLAG)) {
		uint16_t dst = state->registers.pc + ((int8_t) mem_get(&state->mem, state->registers.pc + 1));
		DEBUG_OUT("JR NZ %x to %x\n", state->registers.pc, dst);
		cpu_jump(state, dst);
	}
}

bool cpu_base_table(cpu_state* state, uint8_t c_instr) {
	switch (c_instr) {
		case NOOP:
			cpu_inc_pc(state, 1);
			break;
		case RLC_A:
			cpu_rlc_reg8(state, &state->registers.a);
			cpu_inc_pc(state, 1);
			break;
		case RL_A:
			cpu_rl_reg8(state, &state->registers.a);
			cpu_inc_pc(state, 1);
			break;
		case LDD_REF_HL_A:
			mem_set(&state->mem, state->registers.hl, state->registers.a);
			cpu_dec_reg16(state, &state->registers.hl);
			cpu_inc_pc(state, 1);
			break;

		case JR_NZ_n:
			cpu_jnz_imm_8(state);
			break;

		case LDH_REF_C_A:
			mem_set(&state->mem, 0xFF00 + state->registers.c, state->registers.a);
			cpu_inc_pc(state, 1);
			break;

		case LDH_REF_n_A:
			mem_set(&state->mem, 0xFF00 + mem_get(&state->mem, state->registers.pc + 1), state->registers.a);
			cpu_inc_pc(state, 2);
			break;

		case LDH_REF_A_n:
			state->registers.a = mem_get(&state->mem, 0xFF00 + mem_get(&state->mem, state->registers.pc + 1));
			cpu_inc_pc(state, 2);
			break;

		case LD_A_REF_BC:
			state->registers.a = mem_get(&state->mem, state->registers.bc);
			cpu_inc_pc(state, 1);
			break;

		case LD_A_REF_DE:
			state->registers.a = mem_get(&state->mem, state->registers.de);
			cpu_inc_pc(state, 1);
			break;
		
		case LD_REF_HL_n:
			mem_set(&state->mem, state->registers.hl, mem_get(&state->mem, state->registers.pc + 1));
			cpu_inc_pc(state, 2);
			break;

		case LD_D_REF_HL:
			cpu_ld8(state, &state->registers.d, mem_get(&state->mem, state->registers.hl));
			break;

		case LDI_REF_HL_A:
			mem_set(&state->mem, state->registers.hl, state->registers.a);
			state->registers.hl++;
			cpu_inc_pc(state, 1);
			break;

		case JP_NN:
			cpu_jump(state, mem_get16(&state->mem, state->registers.pc + 1));
			break;

		case CP_n:
			cpu_set_flags(state, (state->registers.a - mem_get(&state->mem, state->registers.pc + 1)) == 0, 1, 0, 0); //TODO: Carry flags
			cpu_inc_pc(state, 2);
			break;

		case CPL_A:
		    state->registers.a = ~state->registers.a;
			cpu_set_flags(state, cpu_is_flag(state, ZERO_FLAG), 1, 1, cpu_is_flag(state, CARRY_FLAG));
			cpu_inc_pc(state, 1);
			break;

		case ENABLE_INTERRUPTS:
			cpu_setinterrupts(state, 1);
			cpu_inc_pc(state, 1);
			break;
		case DISABLE_INTERRUPTS:
			cpu_setinterrupts(state, 0);
			cpu_inc_pc(state, 1);
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

		//16 bit address loads
		case LD_A_REF_nn:
			state->registers.a = mem_get(&state->mem, mem_get16(&state->mem, state->registers.pc + 1));
			cpu_inc_pc(state, 3);
			break;

		case LD_REF_nn_A:
			mem_set(&state->mem, mem_get16(&state->mem, state->registers.pc + 1), state->registers.a);
			cpu_inc_pc(state, 3);
			break;

		case LD_REF_nn_SP:
			mem_set16(&state->mem, mem_get16(&state->mem, state->registers.pc + 1), state->registers.sp);
			cpu_inc_pc(state, 3);
			break;

		case RET_NZ:

			DEBUG_OUT("RET NZ\n");

			if (!cpu_is_flag(state, ZERO_FLAG)) {
				cpu_ret(state);
			} else {
				cpu_inc_pc(state, 1);
			}

			break;
		case RST_38:
			cpu_call(state, 0x0038, state->registers.pc + 1);
			break;
		default:
			printf("OpCode %x Unknown\n", c_instr);
			return false;
	}

	return true;
}