#ifndef UTILS_H
#define UTILS_H

int abs(int x);
void swap(int *x, int *y);

bool inBounds(int x, int y);
bool inBoundsX(int x);
bool inBoundsY(int y);

double getCosOfSection(int section);
double getSinOfSection(int section);
void rotate(int *x, int *y, int section);
void rotate2(int *x, int *y, float angle);

void setLED(int index, bool value);
void turnOnLED(int index);
void turnOffLED(int index);
void turnOnAllLED();
void turnOffAllLED();

short int getColor(short int red, short int green, short int blue);

#endif