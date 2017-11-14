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

	view->renderer = SDL_CreateRenderer(view->window, -1, SDL_RENDERER_ACCELERATED);
	
	if (!view->renderer){
		printf("%s\n", SDL_GetError());
		SDL_DestroyWindow(view->window);
		SDL_Quit();
		return 0;
	}

	view->texture = SDL_CreateTexture(view->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, INTERNAL_WIDTH, INTERNAL_HEIGHT);

	if (!view->texture) {
		printf("%s\n", SDL_GetError());
		SDL_DestroyWindow(view->window);
		SDL_Quit();
		return 0;
	}

	SDL_RendererInfo info;
    SDL_GetRendererInfo(view->renderer, &info );
    printf("Renderer name: %s\n", info.name);
    for( Uint32 i = 0; i < info.num_texture_formats; i++ )
    {
        printf("%s\n", SDL_GetPixelFormatName(info.texture_formats[i]));
    }

	return 1;
}

uint8_t view_render(view_t* view, cpu_state* s, gpu_state* gs) {

	//First clear the renderer
	SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 0);
	SDL_RenderClear(view->renderer);

	if (SDL_UpdateTexture(view->texture, NULL, &gs->canvas, 512 * BYTES_PER_PIXEL)) {
		printf("%s\n", SDL_GetError());
	}

	SDL_RenderCopy(view->renderer, view->texture, NULL, NULL);

	//Update the screen
	SDL_RenderPresent(view->renderer);

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
        	printf("Quit Event\n");
    		exit(1);
    	}
    	if (event.type == SDL_MOUSEMOTION) {
            int mouseX = event.motion.x;
            int mouseY = event.motion.y;
    		printf("Mouszed %i %i\n", mouseX, mouseY);
            gs->canvas[(INTERNAL_WIDTH * BYTES_PER_PIXEL * mouseY) + (mouseX * BYTES_PER_PIXEL) + 2] = 255;
    	}
    }

	return 1;
}