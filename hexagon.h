#ifndef HEXAGON_H
#define HEXAGON_H

#include "segment.h"

typedef struct hexagon {
    Segment segments[6];
    int missing_segment;
    int y;
    int height;
    int speed;
    float rotation;
    float rotation_speed;
    short int color;
} Hexagon;

void init_hexagon(Hexagon* hex, int missing_segment, int y, int height, int speed, short int color);

void set_hexagon_speed(Hexagon* hex, int value);
void increase_hexagon_speed(Hexagon* hex, int value);
void decrease_hexagon_speed(Hexagon* hex, int value);

void set_hexagon_y(Hexagon* hex, int value);
void increase_hexagon_y(Hexagon* hex, int value);
void decrease_hexagon_y(Hexagon* hex, int value);

void set_hexagon_height(Hexagon* hex, int value);
void increase_hexagon_height(Hexagon* hex, int value);
void decrease_hexagon_height(Hexagon* hex, int value);

void set_hexagon_rotation_speed(Hexagon* hex, float value);
void set_hexagon_rotation(Hexagon* hex, float value);
void increase_hexagon_rotation(Hexagon* hex, float value);
void decrease_hexagon_rotation(Hexagon* hex, float value);

void set_hexagon_color(Hexagon* hex, short int color);

void update_hexagon(Hexagon* hex);
void draw_hexagon(Hexagon hex);

#endif //HEXAGON_H
