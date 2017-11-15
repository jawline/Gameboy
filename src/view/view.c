#include "view.h"

uint8_t view_init(view_t* view) {

	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("%s\n", SDL_GetError());
		return 0;
	}

	view->width = 512;
	view->height = 512;

	view->window = SDL_CreateWindow("Gameboy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, view->width, view->height, SDL_WINDOW_SHOWN);

	if (!view->window) {
		printf("%s\n", SDL_GetError());
		SDL_Quit();
		return 0;
	}

	view->renderer = SDL_CreateRenderer(view->window, -1, NULL);
	
	if (!view->renderer){
		printf("%s\n", SDL_GetError());
		SDL_DestroyWindow(view->window);
		SDL_Quit();
		return 0;
	}

	view->texture = SDL_CreateTexture(view->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, INTERNAL_WIDTH, INTERNAL_HEIGHT);

	if (!view->texture) {
		printf("%s\n", SDL_GetError());
		SDL_DestroyWindow(view->window);
		SDL_Quit();
		return 0;
	}

	SDL_RendererInfo info;
    SDL_GetRendererInfo(view->renderer, &info );
    printf("Renderer name: %s\n", info.name);
    
    for(uint32_t i = 0; i < info.num_texture_formats; i++) {
        printf("%s\n", SDL_GetPixelFormatName(info.texture_formats[i]));
    }

	return 1;
}

void copy_canvas(uint8_t* pixels, int pitch, gpu_state* gs) {
	
	const CANVAS_BPP = 4;

	#define GS_PIXEL(x, y, o) gs->canvas[(y * (INTERNAL_WIDTH * BYTES_PER_PIXEL)) + (x * BYTES_PER_PIXEL) + o];
	
	for (unsigned int y = 0; y < INTERNAL_HEIGHT; y++) {
		uint8_t* line_start = pixels + (pitch * y);
		for (unsigned int x = 0; x < INTERNAL_WIDTH; x++) {
			uint8_t* start_pixel = line_start + (x * CANVAS_BPP);
			start_pixel[0] = GS_PIXEL(x, y, 0);
			start_pixel[1] = GS_PIXEL(x, y, 1);
			start_pixel[2] = GS_PIXEL(x, y, 2);
		}
	}
	#undef GS_PIXEL
}

uint8_t view_render(view_t* view, cpu_state* s, gpu_state* gs) {

	void* pixels; int pitch;

	//First clear the renderer
	SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 0);
	SDL_RenderClear(view->renderer);

	//Lock and update the GB canvas
	SDL_LockTexture(view->texture, NULL, &pixels, &pitch);
	copy_canvas((uint8_t*) pixels, pitch, gs);
	SDL_UnlockTexture(view->texture);

	//Blit to the renderer
	SDL_RenderCopy(view->renderer, view->texture, NULL, NULL);
	SDL_RenderPresent(view->renderer);

	//Consume all the events TODO: Split
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
        	printf("Quit Event\n");
    		exit(1);
    	}
    }

	return 1;
}