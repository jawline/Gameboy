#include "emulator.h"
#include "romloader.h"
#include <stdint.h>

#define BIOS_ENABLED 1

char emu_init(cpu_state* state, gpu_state* gstate, char* title, char const* bios, char const* rom) {
	uint8_t* romdat = rom_load(rom, ROM_SIZE);
	uint8_t* bootdat = rom_load(bios, BIOS_SIZE);

	if (!bootdat || !romdat) {
		return 0;
	}

	extract_title(title, romdat);
	
	cpu_init(state);
	gpu_init(gstate);
	memory_init(&state->mem, romdat, bootdat);

	if (BIOS_ENABLED) {
		printf("Starting at PC0\n");
		state->registers.pc = 0x0;
		state->mem.bootrom_enabled = 1;
	} else {
		state->registers.pc = 0x100;
		state->mem.bootrom_enabled = 0;
	}

	return 1;
}
