#ifndef TRACK_H
#define TRACK_H

typedef struct Track Track;
typedef struct BoundarySegment Segment;

Track *load_track(const char *path);
void   free_track(Track *t);

float  track_width(const Track *t);
float  track_total_length(const Track *t);

#endif