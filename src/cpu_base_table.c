#include "cpu.h"

bool cpu_base_table(cpu_state* state, uint8_t c_instr) {
	switch (c_instr) {
		case NOOP:
			cpu_inc_pc(state, 1);
			break;
		case RLC_A:
			state->registers.a = rotl8(state->registers.a, 1);
			cpu_inc_pc(state, 1);
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
			mem_set(&state->mem, 0xFF00 + mem_get(&state->mem, state->registers.pc + 1), state->registers.a);
			cpu_inc_pc(state, 2);
			break;
		case LDH_REF_C_A:
			mem_set(&state->mem, 0xFF00 + state->registers.c, state->registers.a);
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

		case LD_D_REF_HL:
			cpu_ld8(state, &state->registers.d, mem_get(&state->mem, state->registers.hl));
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

		case JP_NN:
			cpu_jump_imm_16(state);
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
			cpu_call(state, 0x0038, state->registers.pc + 1);
			break;
		default:
			printf("OpCode %x Unknown\n", c_instr);
			return false;
	}

	return true;
}