#ifndef VGA_H
#define VGA_H

#include <stdbool.h>

extern volatile int pixel_buffer_start; // global variable
extern volatile int * pixel_ctrl_ptr;

void init_vga();

void clear_screen();
void clear_char_buffer();

void draw_line(int x0, int y0, int x1, int y1, short int color);
void plot_pixel(int x, int y, short int line_color);

void plot_char(int x, int y, char c);
void plot_text(int x, int y, char * text);

void draw_double_line(int x0, int y0, int x1, int y1, short int color);
void plot_double_pixel(int x, int y, short int line_color);

void draw_triple_line(int x0, int y0, int x1, int y1, short int color);
void plot_triple_pixel(int x, int y, short int line_color);

void wait_for_vsync();

#endif