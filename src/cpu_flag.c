#include "cpu.h"

const uint8_t SIGN_FLAG = 0x1 << 7;
const uint8_t ZERO_FLAG = 0x1 << 6;
const uint8_t UDOC_FLAG = 0x1 << 5;
const uint8_t HALF_CARRY_FLAG = 0x1 << 4;
const uint8_t UDOC_2_FLAG = 0x1 << 3;
const uint8_t PO_FLAG = 0x1 << 2; //Parity or offset
const uint8_t SUBTRACT_FLAG = 0x1 << 2;
const uint8_t CARRY_FLAG = 0x1;

#define SET_FLAG(r, f) r |= f
#define UNSET_FLAG(r, f) r &= ~f
#define BUILD_FLAG(r, f, v) if (v) { SET_FLAG(r, f); } else { UNSET_FLAG(r, f); }

void flag(cpu_state* state, uint8_t flags) {
	state->registers.f = flags;
}

bool cpu_is_flag(cpu_state* state, uint8_t flags) {
	return (state->registers.f & flags) != 0;
}

void cpu_set_flags(cpu_state* state, bool zero_flag, bool negative_flag, bool half_carry, bool carry) {
	uint8_t flags = state->registers.f;

	BUILD_FLAG(flags, ZERO_FLAG, zero_flag);
	BUILD_FLAG(flags, SUBTRACT_FLAG, negative_flag);
	BUILD_FLAG(flags, HALF_CARRY_FLAG, half_carry);
	BUILD_FLAG(flags, CARRY_FLAG, carry);

	flag(state, flags);
}