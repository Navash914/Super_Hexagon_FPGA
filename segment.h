#ifndef SEGMENT_H
#define SEGMENT_H

typedef struct segment {
    int y;
    int height;
    int section;
    int speed;
    float rotation;
    //float rotation_speed;
    short int color;
} Segment;

void init_segment(Segment* seg, int section, int y, int height, int speed, short int color);

void draw_segment(Segment seg);
void update_segment(Segment* seg);

#endif