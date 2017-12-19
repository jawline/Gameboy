#include "cpu.h"
#include <stdlib.h>

void cpu_base_table(cpu_state* state, uint8_t c_instr) {
	switch (c_instr) {

		case NOOP:
			cpu_instr_m(state, 1);
			break;
		
		case RLC_A:
			cpu_rlc_reg8(state, &state->registers.a);
			break;
		
		case RL_A:
			cpu_rl_reg8(state, &state->registers.a);
			break;

		case LDD_REF_HL_A:
			mem_set(&state->mem, state->registers.hl, state->registers.a);
			cpu_dec_reg16(state, &state->registers.hl);
			break;

		/**
		 * Relative Jumps
		 */

		case JR_n:
			cpu_jr_flag(state, 1);
			cpu_instr_m(state, 1);
			break;
		case JR_Z_n:
			cpu_jr_flag(state, cpu_is_flag(state, ZERO_FLAG));
			cpu_instr_m(state, 2);
			break;
		case JR_NZ_n:
			cpu_jr_flag(state, !cpu_is_flag(state, ZERO_FLAG));
			cpu_instr_m(state, 2);
			break;

		/**
		 * End of relative jumps
		 */

		case LDH_REF_C_A:
			mem_set(&state->mem, 0xFF00 + state->registers.c, state->registers.a);
			cpu_instr_m(state, 2);
			break;

		case LDH_REF_n_A:
			mem_set(&state->mem, 0xFF00 + cpu_instr_nb(state), state->registers.a);
			cpu_instr_m(state, 4);
			break;

		case LDH_REF_A_n:
			state->registers.a = mem_get(&state->mem, 0xFF00 + cpu_instr_nb(state));
			cpu_instr_m(state, 4);
			break;

		case LD_A_REF_BC:
			state->registers.a = mem_get(&state->mem, state->registers.bc);
			cpu_instr_m(state, 2);
			break;

		case LD_A_REF_DE:
			state->registers.a = mem_get(&state->mem, state->registers.de);
			cpu_instr_m(state, 2);
			break;

		case LD_D_REF_HL:
			cpu_ld8(state, &state->registers.d, mem_get(&state->mem, state->registers.hl));
			break;

		case LDI_REF_HL_A:
			//Set (HL) to a
			//Increment HL
			mem_set(&state->mem, state->registers.hl, state->registers.a);
			state->registers.hl++;
			cpu_instr_m(state, 2);
			break;

		case LDI_A_REF_HL:
			//Load A from address HL
			//Increment HL
			state->registers.a = mem_get(&state->mem, state->registers.hl++);
			cpu_instr_m(state, 2);
			break;

		case JP_NN:
			cpu_jump(state, cpu_instr_nw(state));
			cpu_instr_m(state, 3);
			break;

		case CP_n: {
			uint8_t nb = cpu_instr_nb(state);
			cpu_set_flags(state, (state->registers.a - nb) == 0,
				1, 0, state->registers.a < nb); //TODO: HC
			DEBUG_OUT("Compare %x %x\n", state->registers.a, nb);
			cpu_instr_m(state, 2);
			break;
		}

		case ADD_n: {
			cpu_add_reg8(state, &state->registers.a, cpu_instr_nb(state));
			cpu_instr_m(state, 4);
			break;
		}

		case SUB_n: {
			cpu_sub_reg8(state, &state->registers.a, cpu_instr_nb(state));
			cpu_instr_m(state, 4);
			break;
		}

		case AND_n: {
			cpu_and_reg8(state, &state->registers.a, cpu_instr_nb(state));
			cpu_instr_m(state, 4);
			break;
		}
		case OR_n: {
			cpu_or_reg8(state, &state->registers.a, cpu_instr_nb(state));
			cpu_instr_m(state, 4);
			break;
		}

		case CPL_A:
			state->registers.a = ~state->registers.a;
			cpu_set_flags(state, cpu_is_flag(state, ZERO_FLAG), 1, 1, cpu_is_flag(state, CARRY_FLAG));
			cpu_instr_m(state, 4);
			break;

		case ENABLE_INTERRUPTS:
			DEBUG_OUT("Interrupts Enabled\n");
			cpu_setinterrupts(state, 1);
			cpu_instr_m(state, 1);
			break;

		case DISABLE_INTERRUPTS:
			DEBUG_OUT("Interrupts Disabled\n");
			cpu_setinterrupts(state, 0);
			cpu_instr_m(state, 1);
			break;

		case POP_BC:
			state->registers.bc = stack_pop16(state);
			cpu_instr_m(state, 3);
			break;
		
		case CALL_nn:
			cpu_call(state, cpu_instr_nw(state), state->registers.pc);
			cpu_instr_m(state, 3);
			break;

		case CALL_Z_nn: {
			uint16_t call_loc = cpu_instr_nw(state);
			
			if (cpu_is_flag(state, ZERO_FLAG)) {
				cpu_call(state, call_loc, state->registers.pc);
			}

			cpu_instr_m(state, 3);

			break;
		}

		case RET:
			cpu_ret(state);
			cpu_instr_m(state, 2);
			break;

		//16 bit address loads
		case LD_A_REF_nn:
			state->registers.a = mem_get(&state->mem, cpu_instr_nw(state));
			cpu_instr_m(state, 4); //TODO: This is a random number
			break;

		case LD_REF_nn_A:
			mem_set(&state->mem, cpu_instr_nw(state), state->registers.a);
			cpu_instr_m(state, 4);
			break;

		case LD_REF_nn_SP:
			mem_set16(&state->mem, cpu_instr_nw(state), state->registers.sp);
			break;

		case RET_Z: {
			if (cpu_is_flag(state, ZERO_FLAG)) {
				cpu_ret(state);
			}

			cpu_instr_m(state, 8);
			break;
		}

		//Enable interrupts and return
		case RET_I: {
			cpu_setinterrupts(state, 1);
			cpu_ret(state);
			cpu_instr_m(state, 8);
			break;
		}

		case RET_NZ:

			if (!cpu_is_flag(state, ZERO_FLAG)) {
				cpu_ret(state);
			}

			cpu_instr_m(state, 8);
			break;

		default:
			printf("OpCode %x Unknown\n", c_instr);
			exit(1);
	}
}