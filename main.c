#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "address_map_arm.h"
#include "utils.h"
#include "constants.h"
#include "vga.h"
#include "segment.h"
#include "hexagon.h"
#include "globals.h"
#include "irq_handler.h"
#include "config_gic.h"
#include "player.h"
#include "title_screens.h"

void onStartup();

void drawSectionDividers();
void init_game();
void gameLoop();

void draw_time();
void draw_background();
void draw_title();
void draw_options();

void ulong_to_str(char buffer[], unsigned int num, int digits);
void update();
void update_screen();

void onStartup() {
    config_stack_pointers();        // Set up stack pointers for IRQ mode
    config_gic();                   // Config GIC to enable interrupts

    config_key_interrupt();         // Enable key interrupts
    config_priv_timer_interrupt();  // Load priv timer to tick at 0.01s and set up interrupts
    config_ps2_interrupt();         // Enable PS/2 interrupt

    config_arm_interrupt();         // Enable interrupts for the ARM processor

    init_vga();                     // Initialize VGA buffers
    init_globals();                 // Initialize global variables

    turnOffAllLED();                // Turn Off All LEDs (Used for debugging
    turnOnLED(0);                   // LED 0 is the RUN signal
}

int main(void)
{
    onStartup();                    // Actions to perform on startup

    // Seed random number generator
    time_t t;
    srand((unsigned) time(&t));

    gameLoop();                     // Enter game loop

    while (true);
}

void init_game() {
    int i;
    int space = (ORIGIN_Y) / SIZE;  // Space between hexagons

    // Init hexagons
    for (i = 0; i < SIZE; ++i) {
        Hexagon* hex = &HEXAGONS[i];
        int missing_segment = rand() % 6;

        // First hexagon missing segment should coincide
        // with player to give player time to get started
        if (i == 0)
            missing_segment = 0;

        int y = space * (i+1);
        int height = 4;
        int speed = SETTINGS.HEXAGON_SPEED;
        short int color = rand() % 0xFFFF;

        init_hexagon(hex, missing_segment, y, height, speed, color);
    }

    // Init game time
    GAME_STATE.GAME_TIME = 0;
    SCREEN_ROTATION_CHANGE_TIMER = 1000;

    // Init player
    PLAYER.section = 0;
    PLAYER.height = 3;
    PLAYER.color = 0xFFFF;
    PLAYER.rotation = 0;
    PLAYER.speed = SETTINGS.DEFAULT_PLAYER_SPEED;
    PLAYER.move_left = false;
    PLAYER.move_right = false;

    // Reset screen rotation
    GAME_STATE.SCREEN_ROTATION_ANGLE = 0;
    SETTINGS.SCREEN_ROTATION_DIRECTION = CLOCKWISE;

    // Reset Game Over and RUN status
    GAME_STATE.GAME_OVER = false;
    turnOffLED(9);

    GAME_STATE.RUN = true;
    turnOnLED(0);
}

void gameLoop() {

    int i;
    //for (i = 0; i < NUM_SECTIONS; ++i) {
    //    init_segment(&BACKGROUND[i], i, ORIGIN_Y * 2, ORIGIN_Y * 2, 0, BACKGROUND_COLORS[i%2]);
    //}

    // Clear pixel and character buffers
    clear_screen();
    clear_char_buffer();

    // Init base hexagon
    Hexagon baseHex;
    init_hexagon(&baseHex, -1, ORIGIN_PADDING, ORIGIN_PADDING, 0, 0xFFFF);


    while (true)
    {
        if (GAME_STATE.MODE == TITLE) {
            // In title screen
            draw_title();
            continue;
        }

        if (GAME_STATE.MODE == OPTIONS) {
            // Loop in options screen
            OPTIONS_UPDATE = true;
            draw_options();
            clear_char_buffer();
            continue;
        }

        // Initialize game if requested
        if (INIT_GAME_REQUEST) {
            init_game();
            INIT_GAME_REQUEST = false;  // Clear request
        }

        // Do nothing if RUN is false
        if (!GAME_STATE.RUN)
            continue;

        // End game if GAME_OVER
        if (GAME_STATE.GAME_OVER) {
            GAME_STATE.RUN = false;
            turnOffLED(0);
            turnOnLED(9);   // LED to signal game over
            continue;
        }

        clear_screen(); // Clear previous drawing

        draw_time();            // Draw game time (player score)
        draw_player(PLAYER);    // Draw the player

        // Check for player hit (game over condition)
        bool hit = checkPlayerHit(PLAYER);
        int sect = PLAYER.section;
        if (hit)
            GAME_STATE.GAME_OVER = true;

        // Draw and update the hexagons
        for (i = 0; i < SIZE; ++i) {
            draw_hexagon(HEXAGONS[i]);
            update_hexagon(&HEXAGONS[i]);
        }

        // Draw the base hexagon
        draw_hexagon(baseHex);

        // Draw player in red when game over
        if (hit) {
            PLAYER.section = sect;
            PLAYER.color = RED;
            draw_player(PLAYER);
        }

        // Perform any necessary updates
        update();

        // Wait for VSync and swap buffers
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
}

// Draws the division lines between each section (For debug purposes)
void drawSectionDividers() {
    int i;
    for (i = 0; i < NUM_SECTIONS; ++i) {
        int y = ORIGIN_Y;
        int half_length = y / GRADIENT;
        int x0 = ORIGIN_X;
        int x1 = x0 + half_length;
        int y0 = ORIGIN_Y;
        int y1 = ORIGIN_Y - y;
        rotate(&x0, &y0, i);
        rotate(&x1, &y1, i);
        rotate2(&x0, &y0, GAME_STATE.SCREEN_ROTATION_ANGLE);
        rotate2(&x1, &y1, GAME_STATE.SCREEN_ROTATION_ANGLE);
        draw_line(x0, y0, x1, y1, 0xFFFF);
    }
}

// Converts an unsigned long to a string and stores it in buffer
void ulong_to_str(char buffer[], unsigned int num, int digits) {
    char *s = buffer + digits;
    if (SETTINGS.TIME_MODE == MILLISECONDS && digits > 2)
        s++;    // Space for decimal point
    *(s--) = '\0';
    int count = 0;

    while (num > 0) {
        if (SETTINGS.TIME_MODE == MILLISECONDS && count == 2) {
            // Add decimal point
            *(s--) = '.';
            count++;
            continue;
        }
        int n = num % 10;
        char c = n + '0';
        *(s--) = c;
        num /= 10;
        count++;
    }
}

void draw_background() {
    int i;
    for (i = 0; i < NUM_SECTIONS; ++i) {
        draw_segment(BACKGROUND[i]);
    }
}

void draw_time() {
    clear_char_buffer();        // Clear previous draw

    // Get time in ms or s based on setting
    unsigned int time = GAME_STATE.GAME_TIME;
    if (SETTINGS.TIME_MODE == SECONDS)
        time /= 100;

    char text[10];

    char *s = text;
    if (time == 0) {
        if (SETTINGS.TIME_MODE == SECONDS) {
            *(s++) = '0';
            *(s++) = '0';
            *s = '\0';
        } else if (SETTINGS.TIME_MODE == MILLISECONDS) {
            *(s++) = '0';
            *(s++) = '0';
            *(s++) = '.';
            *(s++) = '0';
            *(s++) = '0';
            *s = '\0';
        }
    } else {
        int n = 1 + log10(time);    // Number of digits
        ulong_to_str(text, time, n);    // Convert time to string

        // Handle some edge cases for the string
        if (SETTINGS.TIME_MODE == SECONDS) {
            if (n < 2) {
                // Number is one digit
                // Add '0' at the beginning to make number two digits
                *(s + 2) = '\0';
                *(s + 1) = *s;
                *s = '0';
            }
        } else if (SETTINGS.TIME_MODE == MILLISECONDS) {
            if (n == 3) {
                // Seconds is one digit
                // Add '0' at the beginning to make seconds two digits
                *(s + 5) = '\0';
                int i;
                for (i = 4; i > 0; --i)
                    *(s + i) = *(s + i - 1);
                *s = '0';
            } else if (n <= 2) {
                // Number is less than a second
                // Add a '00.' before number to look cleaner
                *(s + 5) = '\0';
                *(s + 4) = (n <= 1) ? *s : *(s + 1);
                *(s + 3) = (n <= 1) ? '0' : *s;
                *(s + 2) = '.';
                *(s + 1) = '0';
                *s = '0';
            }
        }
    }

    // Draw the text at the top right corner
    int x = 79 - strlen(text);
    int y = 1;

    plot_text(x, y, text);
}

// Perform necessary updates every frame
void update() {
    update_screen();        // Update screen rotation
    update_settings();      // Update settings changes
    update_game_state();    // Update game state
    updatePlayer(&PLAYER);  // Update player position
}

void update_screen() {
    if (!SETTINGS.SCREEN_ROTATE)
        return;

    // Rotate screen
    float dr = (SETTINGS.SCREEN_ROTATION_DIRECTION == CLOCKWISE ? 1 : -1) * SETTINGS.SCREEN_ROTATION_SPEED;
    GAME_STATE.SCREEN_ROTATION_ANGLE += dr;

    // Clamp rotation between 0 to 360 degrees
    if (GAME_STATE.SCREEN_ROTATION_ANGLE >= 360)
        GAME_STATE.SCREEN_ROTATION_ANGLE -= 360;
    if (GAME_STATE.SCREEN_ROTATION_ANGLE < 0)
        GAME_STATE.SCREEN_ROTATION_ANGLE += 360;
}

// Draw title screen
void draw_title() {
    clear_screen();
    clear_char_buffer();

    int x, y;
    for (y = 0; y < 240; ++y) {
        for (x = 0; x < 320; ++x) {
            int index = 320 * y + x;
            // Get color based on whether screen is start or options
            short int color = (TITLE_INDEX == 0) ? TITLE_START[index] : TITLE_OPTIONS[index];
            plot_pixel(x, y, color);
        }
    }

    register int index = TITLE_INDEX;
    wait_for_vsync();
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);
    while (GAME_STATE.MODE == TITLE && TITLE_INDEX == index);
}

// Loop in options screen
void draw_options() {
    // Draw background for options
    int x0 = 104, y0 = 120;
    int w = 116, h = 88;
    int x1 = x0 + w, y1 = y0 + h;
    int x, y;
    for (y = y0; y < y1; ++y) {
        draw_line(x0, y, x1, y, 0x0);
    }

    wait_for_vsync();
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);
    // Draw border
    //draw_line(x0, x1, y0, y0, 0xFFFF);
    //draw_line(x0, x1, y1, y1, 0xFFFF);
    //draw_line(x0, x0, y0, y1, 0xFFFF);
    //draw_line(x1, x1, y0, y1, 0xFFFF);

    // Options text:
    /*
     * Format:
     * > Empty Line
     * > Input Type
     * > Control Scheme
     * > Timer Type
     * > Empty Line
     * > Difficulty
     * > Empty Line
     * > "Options set by difficulty"
     * > Empty Line
     * > Screen Rotation
     * > Hexagon Rotation
     * > Empty Line
     */

    int x_left = 1 + x0 / 4, y_top = 1 + y0 / 4;
    int x_right = (x1 / 4), y_bottom = (y1 / 4);

    // Loop while game state is options
    while (GAME_STATE.MODE == OPTIONS) {
        if (!OPTIONS_UPDATE)
            continue;   // No need to update anything
        OPTIONS_UPDATE = false;

        clear_char_buffer();    // Clear previous drawing
        x = x_left, y = y_top;

        y++; // Empty Line

        // Input Type
        char * input_type_text = OPTIONS_INDEX == 0 ? ">Input Type\0" : "Input Type\0";
        plot_text(x, y, input_type_text);
        char *input_type = SETTINGS.DIFFICULTY == DEBUG ? "(SW[6])" : SETTINGS.INPUT_TYPE == KEYS ? "Keys\0" : "Keyboard\0";
        x = x_right - strlen(input_type);
        plot_text(x, y, input_type);
        x = x_left;
        y += 2;

        // Control Type
        char * control_scheme_text = OPTIONS_INDEX == 1 ? ">Control Type\0" : "Control Type\0";
        plot_text(x, y, control_scheme_text);
        char *control_scheme = SETTINGS.DIFFICULTY == DEBUG ? "(SW[3])" : SETTINGS.CONTROL_SCHEME == DISCRETE ? "Discrete\0" : "Continuous\0";
        x = x_right - strlen(control_scheme);
        plot_text(x, y, control_scheme);
        x = x_left;
        y += 2;

        // Timer Type
        char * timer_type_text = OPTIONS_INDEX == 2 ? ">Timer Type\0" : "Timer Type\0";
        plot_text(x, y, timer_type_text);
        char *timer_type = SETTINGS.DIFFICULTY == DEBUG ? "(SW[4])" : SETTINGS.TIME_MODE == SECONDS ? "Seconds\0" : "Milliseconds\0";
        x = x_right - strlen(timer_type);
        plot_text(x, y, timer_type);
        x = x_left;
        y += 2;

        y++; // Empty Line

        // Difficulty
        char * difficulty_option_text = OPTIONS_INDEX == 3 ? ">Difficulty\0" :  "Difficulty\0";
        plot_text(x, y, difficulty_option_text);
        char *difficulty_option = SETTINGS.DIFFICULTY == DEBUG ? "Debug\0" :
                                  SETTINGS.DIFFICULTY == EASY ? "Easy\0" :
                                  SETTINGS.DIFFICULTY == NORMAL ? "Normal\0" :
                                  "Hard\0";
        x = x_right - strlen(difficulty_option);
        plot_text(x, y, difficulty_option);
        x = x_left;
        y += 2;

        y++; // Empty Line

        // Difficulty Set Options
        char text[] = "** Difficulty Set Options **\0";
        x = x_left + (x_right - x_left - strlen(text)) / 2;
        plot_text(x, y, text);
        x = x_left;
        y += 2;

        y++; // Empty Line

        // Screen Rotation
        char screen_rotation_text[] = "Screen Rotation\0";
        plot_text(x, y, screen_rotation_text);
        char *screen_rotation = SETTINGS.DIFFICULTY == DEBUG ? "(SW[1])\0" :
                                SETTINGS.SCREEN_ROTATE ? "Yes\0" : "No\0";

        x = x_right - strlen(screen_rotation);
        plot_text(x, y, screen_rotation);
        x = x_left;
        y += 2;

        // Hexagon Rotation
        char hex_rotation_text[] = "Hexagon Rotation\0";
        plot_text(x, y, hex_rotation_text);
        char *hex_rotation = SETTINGS.DIFFICULTY == DEBUG ? "(SW[5])\0" :
                             SETTINGS.HEXAGON_ROTATE ? "Yes\0" : "No\0";

        x = x_right - strlen(hex_rotation);
        plot_text(x, y, hex_rotation);
        x = x_left;
        y += 2;
    }
}