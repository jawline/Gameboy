#include "emulator.h"
#include "romloader.h"
#include <stdint.h>

char emu_init(cpu_state* state, char const* rom) {
	uint8_t* romdat = rom_load(rom);

	if (!romdat) {
		return 0;
	}
	
	memory_init(&state->mem, romdat);
	cpu_init(state);

	return 1;
}