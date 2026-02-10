#include <stdio.h>
#include "track_loader.h"

int main(void)
{
    const char *filename = "tracks/test.txt";

    Track *track = load_track(filename);
    if (track == NULL) {
        printf("Failed to load track: %s\n", filename);
        return 1;
    }

    printf("Total Track Length: %.2f\n", track_total_length(track));

    free_track(track);
    return 0;
}
