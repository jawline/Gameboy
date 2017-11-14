#include "gpu.h"
#include <string.h>

void gpu_init(gpu_state* state) {
	memset(state, 0, sizeof(gpu_state));
}

void gpu_enter_mode(gpu_state* state, GPU_MODE mode) {
	state->cycles_in_mode = 0;
	state->mode = mode;
}

DRAW_MODE gpu_step(cpu_state* state, gpu_state* gstate) {
	gstate->cycles_in_mode += state->registers.lc.t;

	switch (gstate->mode) {
		case OAM:
			if (gstate->cycles_in_mode >= 80) {
				gpu_enter_mode(gstate, VRAM);
			}
			break;
		case VRAM:
			if (gstate->cycles_in_mode >= 172) {
				gpu_enter_mode(gstate, HBLANK);
				return RASTER_LINE;
			}
			break;
		case HBLANK:
			if (gstate->cycles_in_mode >= 204) {
				gstate->line++;
				printf("Line %u\n", gstate->line);
				if (gstate->line == 143) {
					gpu_enter_mode(gstate, VBLANK);
				} else {
					gpu_enter_mode(gstate, OAM);
				}
			}
			break;
		case VBLANK:
			if (gstate->cycles_in_mode >= 456) {
				printf("VBLANK\n");
				gstate->line = 0;
				gpu_enter_mode(gstate, HBLANK);
				return REDRAW_ALL;
			}
			break;
	}

	return NONE;
}