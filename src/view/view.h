#ifndef _VIEW_DEF_H_
#define _VIEW_DEF_H_
#include "../emulator.h"
#include <SDL2/SDL.h>

typedef struct emulator_view {
	SDL_Window* window;
	SDL_Renderer* renderer;

	unsigned setup:1;

	unsigned width;
	unsigned height;

	unsigned wscale;
	unsigned hscale;
} view_t;

uint8_t view_init(view_t* view);
uint8_t view_render(view_t* view, cpu_state* s);

#endif