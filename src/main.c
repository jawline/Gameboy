#include "cpu.h"
#include "emulator.h"
#include <stdio.h>
#include "types.h"
#include "view/view.h"

//#define VIEW_ENABLED

int main(int argc, char const* const* argv) {
	cpu_state s;

	if (!emu_init(&s, argv[1], argv[2])) {
		printf("EMU INIT FAIL\n");
		return 1;
	}

	uint16_builder big;

	big.low = 0x50;
	big.high = 0x01;

	printf("Big %x low %x high %x\n", big.full, big.low, big.high);

#ifdef VIEW_ENABLED
	view_t view;

	if (!view_init(&view)) {
		return 1;
	}

	uint8_t ticks = 0;
#endif

	while (cpu_step(&s)) {
#ifdef VIEW_ENABLED
		if (ticks % 30 == 0 && !view_render(&view, &s)) {
			break;
		}
		ticks++;
#endif
	}

#ifdef VIEW_ENABLED
	view_render(&view, &s);
#endif

	return 0;
}
