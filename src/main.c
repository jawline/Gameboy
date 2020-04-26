#include "cpu.h"
#include "gpu.h"
#include "emulator.h"
#include <stdio.h>
#include "types.h"
#include "view/view.h"
#include "romloader.h"
#include <sys/time.h>

#define VIEW_ENABLED

int main(int argc, char const* const* argv) {
	char loaded_title[TITLE_SIZE + 1];
	cpu_state s;
	gpu_state gs;

	if (!emu_init(&s, &gs, loaded_title, argv[1], argv[2])) {
		printf("EMU INIT FAIL\n");
		return 1;
	}

#ifdef VIEW_ENABLED
	view_t view;

	if (!view_init(&view, loaded_title)) {
		return 1;
	}
#endif

  struct timeval last, now;
  gettimeofday(&last, NULL);

  double elapsedTime = 0;

	while (true) {

    gettimeofday(&now, NULL);
    elapsedTime += (now.tv_sec - last.tv_sec) * 1000.0 * 1000.0;
    elapsedTime += (now.tv_usec - last.tv_usec);
    if (elapsedTime > 1) {
		  cpu_step(&s);
		  cpu_check_interrupts(&s);
    }
    memcpy(&last, &now, sizeof(struct timeval));
		DRAW_MODE redraw = gpu_step(&s, &gs);
		
		#ifdef VIEW_ENABLED
			if (redraw == REDRAW_ALL) {
				view_render(&view, &s, &gs);
			}
		#endif
	}

#ifdef VIEW_ENABLED
	while (true) {
		view_render(&view, &s, &gs);
	}
#endif

	return 0;
}
