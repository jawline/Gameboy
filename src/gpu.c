#include "gpu.h"
#include <string.h>

void gpu_init(gpu_state* state) {
	memset(state, 0, sizeof(gpu_state));
	state->canvas = malloc(INTERNAL_WIDTH * INTERNAL_HEIGHT * BYTES_PER_PIXEL);
	memset(state->canvas, 0, INTERNAL_WIDTH * INTERNAL_HEIGHT * BYTES_PER_PIXEL);
}

void gpu_enter_mode(gpu_state* state, GPU_MODE mode) {
	state->cycles_in_mode = 0;
	state->mode = mode;
}

uint8_t tile_value(cpu_state* cstate, gpu_state* gstate, uint8_t tileset, uint16_t tile_id, uint8_t x, uint8_t y) {
    const unsigned TILE_SIZE = 16;
    uint16_t addr = tileset == 1 ? 0x8000 : 0x8800;
    uint16_t tile_addr = addr + (TILE_SIZE * tile_id);
    uint16_t y_addr = tile_addr + (y * 2);
    uint16_t sx = 1 << (7-x);
    return (mem_get(&cstate->mem, y_addr) & sx) + (mem_get(&cstate->mem, y_addr + 1) & sx);
}

void gpu_render_line(cpu_state* state, gpu_state* gstate) {

	uint16_t map_offset = 0x8000 + 0x1C00;
	map_offset += (state->mem.scy + gstate->line) >> 3;

	printf("Map Offset %x\n", map_offset);

	uint16_t line_offset = state->mem.scx >> 3;

	printf("Line Offset %x\n", line_offset);

	uint16_t y = (gstate->line + state->mem.scy) & 7;
	uint16_t x = (state->mem.scx) & 7;
	uint8_t tile = mem_get(&state->mem, map_offset + line_offset);

	printf("Pixel X, Y Tile %x, %x, %x\n", x, y, tile);

	unsigned int canvas_scanline_size = INTERNAL_WIDTH * BYTES_PER_PIXEL;
	unsigned int canvas_offset = canvas_scanline_size * (gstate->line - 1);
	
	for (unsigned i = 0; i < 160; i++) {
		uint8_t val = tile_value(state, gstate, 1, tile, x, y);
		printf("%x\n", val);
		if (val != 0) {
			gstate->canvas[canvas_offset + (i * BYTES_PER_PIXEL)] = 255;
		}
		if(++x == 8) {
			x = 0;
			line_offset = (line_offset + 1) & 31;
			tile = mem_get(&state->mem, map_offset + line_offset);
			//if(GPU._bgtile == 1 && tile < 128) tile += 256;
	    }
	}
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
				gpu_render_line(state, gstate);
				gpu_enter_mode(gstate, HBLANK);
				return RASTER_LINE;
			}
			break;
		case HBLANK:
			if (gstate->cycles_in_mode >= 204) {
				gstate->line++;
				if (gstate->line == 145) {
					gpu_enter_mode(gstate, VBLANK);
				} else {
					gpu_enter_mode(gstate, OAM);
				}
			}
			break;
		case VBLANK:
			if (gstate->cycles_in_mode >= 4560) {
				gstate->line = 0;
				gpu_enter_mode(gstate, HBLANK);
				return REDRAW_ALL;
			}
			break;
	}

	return NONE;
}