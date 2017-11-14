#ifndef _GPU_DEF_H_
#define _GPU_DEF_H_
#include "cpu.h"

typedef enum {
	HBLANK,
	VBLANK,
	OAM,
	VRAM
} GPU_MODE;

typedef enum {
	NONE,
	RASTER_LINE,
	REDRAW_ALL
} DRAW_MODE;

typedef struct {
	GPU_MODE mode;
	unsigned int cycles_in_mode;
	unsigned int line;
} gpu_state;

void gpu_init(gpu_state* state);
DRAW_MODE gpu_step(cpu_state* state, gpu_state* gstate);

#endif