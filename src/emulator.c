#include "emulator.h"
#include "romloader.h"
#include <stdint.h>

char emu_init(cpu_state* state, char const* bios, char const* rom) {
	uint8_t* romdat = rom_load(bios, rom);

	if (!romdat) {
		return 0;
	}
	
	cpu_init(state);
	memory_init(&state->mem, romdat);

	return 1;
}