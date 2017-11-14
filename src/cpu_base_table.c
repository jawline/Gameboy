#include "cpu.h"

bool cpu_base_table(cpu_state* state, uint8_t c_instr) {
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
			//Increment PC
			mem_set(&state->mem, state->registers.hl, state->registers.a);
			state->registers.hl++;
			cpu_instr_m(state, 2);
			break;

		case JP_NN:
			cpu_jump(state, cpu_instr_nw(state));
			break;

		case CP_n:
			cpu_set_flags(state, (state->registers.a - cpu_instr_nb(state)) == 0, 1, 0, 0); //TODO: Carry flags#
			cpu_instr_m(state, 2);
			break;

		case CPL_A:
		    state->registers.a = ~state->registers.a;
			cpu_set_flags(state, cpu_is_flag(state, ZERO_FLAG), 1, 1, cpu_is_flag(state, CARRY_FLAG));
			break;

		case ENABLE_INTERRUPTS:
			cpu_setinterrupts(state, 1);
			break;

		case DISABLE_INTERRUPTS:
			cpu_setinterrupts(state, 0);
			break;

		case POP_BC:
			state->registers.bc = stack_pop16(state);
			cpu_instr_m(state, 3);
			break;

		case PUSH_BC:
			stack_push16(state, state->registers.bc);
			cpu_instr_m(state, 4);
			break;
		
		case CALL_nn: {
			uint16_t call_loc = cpu_instr_nw(state);
			cpu_call(state, call_loc, state->registers.pc);
			cpu_instr_m(state, 3);
			break;
		}

		case CALL_Z_nn: {
			uint16_t call_loc = cpu_instr_nw(state);
			
			if (cpu_is_flag(state, ZERO_FLAG)) {
				cpu_call(state, call_loc, state->registers.pc);
			}

			break;
		}

		case RET:
			cpu_ret(state);
			cpu_instr_m(state, 2);
			break;

		//16 bit address loads
		case LD_A_REF_nn:
			state->registers.a = mem_get(&state->mem, cpu_instr_nw(state));
			break;

		case LD_REF_nn_A:
			mem_set(&state->mem, cpu_instr_nw(state), state->registers.a);
			break;

		case LD_REF_nn_SP:
			mem_set16(&state->mem, cpu_instr_nw(state), state->registers.sp);
			break;

		case RET_NZ:

			DEBUG_OUT("RET NZ\n");

			if (!cpu_is_flag(state, ZERO_FLAG)) {
				cpu_ret(state);
			}

			break;
		case RST_38:
			cpu_call(state, 0x0038, state->registers.pc);
			break;
		default:
			printf("OpCode %x Unknown\n", c_instr);
			return false;
	}

	return true;
}