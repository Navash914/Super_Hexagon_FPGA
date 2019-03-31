#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "utils.h"
#include "constants.h"
#include "address_map_arm.h"

int abs(int x) {
    if (x < 0)
        return -1 * x;
    else
        return x;
}

void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

bool inBounds(int x, int y) {
    return inBoundsX(x) && inBoundsY(y);
}

bool inBoundsX(int x) {
    return x >= 0 && x < MAX_X;
}

bool inBoundsY(int y) {
    return y >= 0 && y < MAX_Y;
}

double getCosOfSection(int section) {
    switch(section) {
        case 0:
            return 1;
        case 1:
            return 0.5;
        case 2:
            return -0.5;
        case 3:
            return -1;
        case 4:
            return -0.5;
        case 5:
            return 0.5;
        default:
            return 1;
    }
}

double getSinOfSection(int section) {
    switch(section) {
        case 0:
            return 0;
        case 1:
            return 0.86602540378;
        case 2:
            return 0.86602540378;
        case 3:
            return 0;
        case 4:
            return -0.86602540378;
        case 5:
            return -0.86602540378;
        default:
            return 0;
    }
}

void rotate(int *x, int *y, int section) {
    int x_ = *x - ORIGIN_X;
    int y_ = *y - ORIGIN_Y;

    *x = x_ * getCosOfSection(section) - y_ * getSinOfSection(section);
    *y = y_ * getCosOfSection(section) + x_ * getSinOfSection(section);

    *x += ORIGIN_X;
    *y += ORIGIN_Y;
}

void rotate2(int *x, int *y, float angle) {
    if (angle <= 0.1)
        return;

    int x_ = *x - ORIGIN_X;
    int y_ = *y - ORIGIN_Y;

    angle *= DEG_TO_RAD;

    float c = cos(angle);
    float s = sin(angle);
    *x = x_ * c - y_ * s;
    *y = y_ * c + x_ * s;

    *x += ORIGIN_X;
    *y += ORIGIN_Y;
}


void setLED(int index, bool value) {
    if (value)
        turnOnLED(index);
    else
        turnOffLED(index);
}

void turnOnLED(int index) {
    volatile int * ledr = (int *) LEDR_BASE;
    int value = 1 << index;
    value = *ledr | value;
    *ledr = value;
}

void turnOffLED(int index) {
    volatile int * ledr = (int *) LEDR_BASE;
    int value = 0x3FF;
    int i = 1 << index;
    value = value ^ i;
    value = *ledr & value;
    *ledr = value;
}

void turnOnAllLED(){
    *((int *) LEDR_BASE) = 0x3FF;
}

void turnOffAllLED() {
    *((int *) LEDR_BASE) = 0;
}

short int getColor(short int red, short int green, short int blue) {
    return (red << 11) | (green << 5) | blue;
}