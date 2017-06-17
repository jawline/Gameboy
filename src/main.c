#include "cpu.h"
#include "emulator.h"
#include <stdio.h>
#include "types.h"

int main(int argc, char const* const* argv) {
	cpu_state s;

	if (!emu_init(&s, argv[1])) {
		printf("EMU INIT FAIL\n");
		return 1;
	}
	
	while (cpu_step(&s)) {
	}

	return 0;
}
