#include "cpu.h"
#include "gpu.h"
#include "emulator.h"
#include <stdio.h>
#include "types.h"
#include "view/view.h"

#define VIEW_ENABLED

int main(int argc, char const* const* argv) {
	cpu_state s;
	gpu_state gs;

	if (!emu_init(&s, &gs, argv[1], argv[2])) {
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
#endif

	while (cpu_step(&s)) {
		DRAW_MODE redraw = gpu_step(&s, &gs);
		//fgetc(stdin);
		#ifdef VIEW_ENABLED
			if (redraw == REDRAW_ALL) view_render(&view, &s, &gs);
		#endif
	}

#ifdef VIEW_ENABLED
	while (true) {
		view_render(&view, &s, &gs);
		usleep(100);
	}
#endif

	return 0;
}
