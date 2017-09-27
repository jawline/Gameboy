#include "cpu.h"
#include "emulator.h"
#include <stdio.h>
#include "types.h"
#include "view/view.h"

int main(int argc, char const* const* argv) {
	cpu_state s;

	if (!emu_init(&s, argv[1])) {
		printf("EMU INIT FAIL\n");
		return 1;
	}

	uint16_builder big;

	big.low = 0x50;
	big.high = 0x01;

	printf("Big %x low %x high %x\n", big.full, big.low, big.high);

	view_t view;

	if (!view_init(&view)) {
		return 1;
	}

	while (cpu_step(&s)) {
		if (!view_render(&view, &s)) {
			return;
		}
	}

	return 0;
}
