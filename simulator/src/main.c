#include "window.h"
#include "shader.h"
#include "track_renderer.h"
#include <stdio.h>

int main(void) {
    if (window_init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE) == -1) {
        return 1;
    }

    const char *filename = "tracks/track_001.txt";
    Track *track = load_track(filename);
    if (track == NULL) {
        printf("Failed to load track: %s\n", filename);
        return 1;
    }
    
    if(track_renderer_init(&track->left_boundary, &track->right_boundary)) {
        fprintf(stderr, "Failed to render Track \n");
        return 1;
    }

    while (!window_should_close()) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        track_renderer_draw();
        window_swap_and_poll();
    }

    track_renderer_cleanup();
    window_cleanup();

    return 0;
}