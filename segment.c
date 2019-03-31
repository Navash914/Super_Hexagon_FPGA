#include "segment.h"
#include "constants.h"
#include "vga.h"
#include "utils.h"
#include "globals.h"

void init_segment(Segment* seg, int section, int y, int height, int speed, short int color) {
    seg->section = section;
    seg->y = y;
    seg->height = height;
    seg->speed = speed;
    seg->color = color;
    seg->rotation = 0;
    //seg->rotation_speed = 0;
}

void draw_segment(Segment seg) {
    int x = ORIGIN_X;
    //int angle = ROTATION_ANGLE * seg.section;
    int angle = seg.section;
    int i;
    for (i = 0; i<seg.height; ++i) {
        int y = seg.y - i;
        int half_length = y / GRADIENT;
        int x0 = x - half_length;
        int x1 = x + half_length;
        int y0 = ORIGIN_Y - y;
        int y1 = ORIGIN_Y - y;

        rotate(&x0, &y0, angle);
        rotate(&x1, &y1, angle);

        rotate2(&x0, &y0, GAME_STATE.SCREEN_ROTATION_ANGLE);
        rotate2(&x1, &y1, GAME_STATE.SCREEN_ROTATION_ANGLE);

        rotate2(&x0, &y0, seg.rotation);
        rotate2(&x1, &y1, seg.rotation);

        draw_triple_line(x0, y0, x1, y1, seg.color);
    }
}

void update_segment(Segment* seg) {
    if (seg->y <= ORIGIN_PADDING) {
        seg->y = ORIGIN_Y;
        seg->speed++;
    } else {
        seg->y -= seg->speed;
    }
}