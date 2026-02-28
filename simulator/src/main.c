#include "window.h"

int main(void) {
    if (window_init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE) == -1) {
        return 1;
    }

    while (!window_should_close()) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        window_swap_and_poll();
    }

    window_cleanup();

    return 0;
}