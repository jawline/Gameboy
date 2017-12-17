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

	SDL_Texture* texture;
} view_t;

uint8_t view_init(view_t* view, char const* title);
uint8_t view_render(view_t* view, cpu_state* s, gpu_state* gs);

#endif