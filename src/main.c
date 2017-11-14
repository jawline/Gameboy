#include "cpu.h"
#include "gpu.h"
#include "emulator.h"
#include <stdio.h>
#include "types.h"
#include "view/view.h"

#define VIEW_ENABLED

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

	unsigned long total_count = 0;

	while (cpu_step(&s)) {
		gpu_step(&s);
		//fgetc(stdin);
#ifdef VIEW_ENABLED
		if (ticks % 200 == 0 && !view_render(&view, &s)) {
			break;
		}
		ticks++;
#endif
		total_count++;
	}

#ifdef VIEW_ENABLED
	while (true) {
		view_render(&view, &s);
		usleep(100);
	}
#endif

	return 0;
}
