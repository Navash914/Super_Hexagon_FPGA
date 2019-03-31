#include "hexagon.h"
#include "segment.h"
#include "constants.h"
#include "globals.h"
#include "vga.h"
#include "utils.h"
#include <stdlib.h>

const int rotation_speeds_size = 12;
float rotation_speeds[] = { 1, 1, 1, 2, 2, 3, 1, 1, 1, 2, 2, 3 };

void init_hexagon(Hexagon* hex, int missing_segment, int y, int height, int speed, short int color) {
    hex->missing_segment = missing_segment;
    hex->y = y;
    hex->height = height;
    hex->speed = speed;
    hex->rotation = 0;
    hex->rotation_speed = 0;
    hex->color = color;
    int i;
    for (i=0; i<6; ++i) {
        init_segment(&hex->segments[i], i, y, height, speed, color);
    }
}

// **************   Hexagon Speed Setting Operations    ************** //

void increase_hexagon_speed(Hexagon* hex, int value) {
    set_hexagon_speed(hex, hex->speed + value);
}

void decrease_hexagon_speed(Hexagon* hex, int value) {
    set_hexagon_speed(hex, hex->speed - value);
}

void set_hexagon_speed(Hexagon* hex, int value) {
    hex->speed = value;
    int i;
    for (i=0; i<6; ++i) {
        //if (i != hex->missing_segment)
            hex->segments[i].speed = value;
    }
}

// **************   Hexagon Y Setting Operations    ************** //

void set_hexagon_y(Hexagon* hex, int value) {
    hex->y = value;
    int i;
    for (i=0; i<6; ++i) {
        //if (i != hex->missing_segment)
            hex->segments[i].y = value;
    }
}

void increase_hexagon_y(Hexagon* hex, int value) {
    set_hexagon_y(hex, hex->y + value);
}

void decrease_hexagon_y(Hexagon* hex, int value) {
    set_hexagon_y(hex, hex->y - value);
}

// **************   Hexagon Height Setting Operations    ************** //

void set_hexagon_height(Hexagon* hex, int value) {
    hex->height = value;
    int i;
    for (i=0; i<6; ++i) {
        //if (i != hex->missing_segment)
            hex->segments[i].height = value;
    }
}

void increase_hexagon_height(Hexagon* hex, int value) {
    set_hexagon_height(hex, hex->height + value);
}

void decrease_hexagon_height(Hexagon* hex, int value) {
    set_hexagon_height(hex, hex->height - value);
}

// **************   Hexagon Rotation Setting Operations    ************** //

void set_hexagon_rotation_speed(Hexagon* hex, float value) {
    if (!SETTINGS.HEXAGON_ROTATE || SETTINGS.CONTROL_SCHEME == DISCRETE)
        value = 0;
    hex->rotation_speed = value;
    //int i;
    //for (i=0; i<6; ++i) {
    //    hex->segments[i].rotation_speed = value;
    //}
}

void set_hexagon_rotation(Hexagon* hex, float value) {
    if (!SETTINGS.HEXAGON_ROTATE || SETTINGS.CONTROL_SCHEME == DISCRETE)
        value = 0;
    if (value >= 360)
        value -= 360;
    if (value < 0)
        value += 360;
    hex->rotation = value;
    int i;
    for (i=0; i<6; ++i) {
        hex->segments[i].rotation = value;
    }
}

void increase_hexagon_rotation(Hexagon* hex, float value) {
    set_hexagon_rotation(hex, hex->rotation + value);
}

void decrease_hexagon_rotation(Hexagon* hex, float value) {
    set_hexagon_rotation(hex, hex->rotation - value);
}

// **************   Hexagon Color Setting Operations    ************** //

void set_hexagon_color(Hexagon* hex, short int color) {
    hex->color = color;
    int i;
    for (i=0; i<6; ++i) {
        //if (i != hex->missing_segment)
            hex->segments[i].color = color;
    }
}

// **************   Hexagon Update Operations    ************** //

void update_hexagon(Hexagon* hex) {
    //if ((hex->y - hex->height) <= ORIGIN_PADDING) {
    if ((hex->y - hex->height) <= 0) {
        hex->missing_segment = rand() % 6;
        set_hexagon_y(hex, ORIGIN_Y);
        //increase_hexagon_speed(hex, 1);
        set_hexagon_color(hex, rand() % 0xFFFF);
        set_hexagon_rotation(hex, 0);
        int dir = (rand() % 2 == 0) ? -1 : 1;
        set_hexagon_rotation_speed(hex, dir * rotation_speeds[rand() % rotation_speeds_size]);
    } else {
        //if ((hex->y - hex->height) <= 1)
        //    hex->missing_segment = rand() % 6;
        decrease_hexagon_y(hex, hex->speed);
        increase_hexagon_rotation(hex, hex->rotation_speed);
    }
}

// **************   Hexagon Drawing Operations    ************** //

void draw_hexagon(Hexagon hex) {
    int i;
    for (i = 0; i < 6; ++i) {
        if (i != hex.missing_segment)
            draw_segment(hex.segments[i]);
    }
}