#include "view.h"

const unsigned INTERNAL_WIDTH = 160;
const unsigned INTERNAL_HEIGHT = 144;

uint8_t view_init(view_t* view) {

	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("%s\n", SDL_GetError());
		return 0;
	}

	view->width = 640;
	view->height = 480;

	view->wscale = view->width / INTERNAL_WIDTH;
	view->hscale = view->height / INTERNAL_HEIGHT;

	view->window = SDL_CreateWindow("Gameboy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, view->width, view->height, SDL_WINDOW_SHOWN);

	if (!view->window) {
		printf("%s\n", SDL_GetError());
		SDL_Quit();
		return 0;
	}

	view->renderer = SDL_CreateRenderer(view->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	if (!view->renderer){
		printf("%s\n", SDL_GetError());
		SDL_DestroyWindow(view->window);
		SDL_Quit();
		return 0;
	}

	if (SDL_RenderSetScale(view->renderer, view->wscale, view->hscale)) {
		printf("%s\n", SDL_GetError());
		SDL_DestroyWindow(view->window);
		SDL_Quit();
		return 0;	
	}


	return 1;
}

uint8_t view_render_tiles(view_t* view, cpu_state* s, int offx, int offy) {

	for (int x = 0; x < 32; x++) {
		for (int y = 0; y < 32; y++) {

			SDL_Rect base;
			
			base.x = offx + (8 * x);
			base.y = offy + (8 * y);
			base.w = 8;
			base.h = 8;

			SDL_SetRenderDrawColor(view->renderer, base.x, 0, base.y, 0);
			SDL_RenderDrawRect(view->renderer, &base);
		}
	}

	return 1;
}

uint8_t view_render(view_t* view, cpu_state* s) {

	//First clear the renderer
	SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 0);
	SDL_RenderClear(view->renderer);	

	for (unsigned int round_x = 0; round_x < 3; round_x++) {
		for (unsigned int round_y = 0; round_y < 3; round_y++) {
			view_render_tiles(view, s, -256 + (256 * round_x) + s->mem.scx, -256 + (256 * round_y) + s->mem.scy);
		}
	}
	
	//Update the screen
	SDL_RenderPresent(view->renderer);
	SDL_UpdateWindowSurface(view->window);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
        	printf("Quit Event\n");
    		exit(1);
    	}
    }

	return 1;
}