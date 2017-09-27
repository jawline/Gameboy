#ifndef _VIEW_DEF_H_
#define _VIEW_DEF_H_
#include "../emulator.h"
#include <SDL2/SDL.h>

typedef struct emulator_view {
	unsigned setup:1;
	SDL_Window* window;
	SDL_Renderer* renderer;
} view_t;

uint8_t view_init(view_t* view);
uint8_t view_render(view_t* view, cpu_state* s);

#endif