#include "vga.h"
#include "utils.h"
#include <stdbool.h>
#include "address_map_arm.h"

volatile int pixel_buffer_start;
volatile int * pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;

void init_vga() {
    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
    // back buffer

    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();

    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    //*(pixel_ctrl_ptr + 1) = 0xC8000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
}

void clear_screen() {
    int x_max = 320;
    int y_max = 240;
    int x, y;
    short int black = 0;
    for (x = 0; x < x_max; ++x) {
        for (y = 0; y < y_max; ++y) {
            plot_pixel(x, y, black);
        }
    }
}

void clear_char_buffer() {
    int x_max = 80;
    int y_max = 60;
    int x, y;
    char clear_char = '\0';
    for (x = 0; x < x_max; ++x) {
        for (y = 0; y < y_max; ++y) {
            plot_char(x, y, clear_char);
        }
    }
}

void draw_line(int x0, int y0, int x1, int y1, short int color) {
    bool is_steep;
    if (abs(y1 - y0) > abs(x1 - x0))
        is_steep = true;
    else
        is_steep = false;
    if (is_steep) {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }
    if (x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = -(deltax / 2);
    int y = y0;
    int y_step;
    if (y0 < y1)
        y_step = 1;
    else
        y_step = -1;

    int x;
    for (x = x0; x <= x1; ++x) {
        if (is_steep)
            plot_pixel(y, x, color);
        else
            plot_pixel(x, y, color);
        error = error + deltay;
        if (error >= 0) {
            y = y + y_step;
            error = error - deltax;
        }
    }

}

void plot_pixel(int x, int y, short int line_color)
{
    if (inBounds(x, y))
        *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void plot_char(int x, int y, char c) {
    volatile char * character_buffer = (char *) FPGA_CHAR_BASE;
    int offset = (y << 7) + x;
    *(character_buffer + offset) = c;
}

void plot_text(int x, int y, char * text) {
    volatile char * character_buffer = (char *) FPGA_CHAR_BASE;
    int offset = (y << 7) + x;
    while (*text)
        *(character_buffer + offset++) = *(text++);
}

void draw_double_line(int x0, int y0, int x1, int y1, short int color) {
    bool is_steep;
    if (abs(y1 - y0) > abs(x1 - x0))
        is_steep = true;
    else
        is_steep = false;
    if (is_steep) {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }
    if (x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = -(deltax / 2);
    int y = y0;
    int y_step;
    if (y0 < y1)
        y_step = 1;
    else
        y_step = -1;

    int x;
    for (x = x0; x <= x1; ++x) {
        if (is_steep)
            plot_double_pixel(y, x, color);
        else
            plot_double_pixel(x, y, color);
        error = error + deltay;
        if (error >= 0) {
            y = y + y_step;
            error = error - deltax;
        }
    }

}

void plot_double_pixel(int x, int y, short int line_color)
{
    if (inBounds(x, y))
        *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
    if (inBounds(x, y+1))
        *(short int *)(pixel_buffer_start + ((y+1) << 10) + (x << 1)) = line_color;
}

void draw_triple_line(int x0, int y0, int x1, int y1, short int color) {
    bool is_steep;
    if (abs(y1 - y0) > abs(x1 - x0))
        is_steep = true;
    else
        is_steep = false;
    if (is_steep) {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }
    if (x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = -(deltax / 2);
    int y = y0;
    int y_step;
    if (y0 < y1)
        y_step = 1;
    else
        y_step = -1;

    int x;
    for (x = x0; x <= x1; ++x) {
        if (is_steep)
            plot_triple_pixel(y, x, color);
        else
            plot_triple_pixel(x, y, color);
        error = error + deltay;
        if (error >= 0) {
            y = y + y_step;
            error = error - deltax;
        }
    }

}

void plot_triple_pixel(int x, int y, short int line_color)
{
    if (inBoundsY(y))
        *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
    if (inBoundsY(y+1))
        *(short int *)(pixel_buffer_start + ((y+1) << 10) + (x << 1)) = line_color;
    if (inBoundsY(y-1))
        *(short int *)(pixel_buffer_start + ((y-1) << 10) + (x << 1)) = line_color;
}

void wait_for_vsync() {
    *pixel_ctrl_ptr = 1;
    while ((*(pixel_ctrl_ptr + 3) & 1) == 1);
}

