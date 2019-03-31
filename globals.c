#include "globals.h"
#include "utils.h"
#include "address_map_arm.h"

Settings SETTINGS;
Settings DIFFICULTIES[4];

Game_State GAME_STATE;
bool INIT_GAME_REQUEST;

Player PLAYER;
Hexagon HEXAGONS[SIZE];

Segment BACKGROUND[NUM_SECTIONS];
short int BACKGROUND_COLORS[2];

int TITLE_SIZE;
volatile int TITLE_INDEX;

volatile bool OPTIONS_UPDATE;
int OPTIONS_SIZE;
volatile int OPTIONS_INDEX;

volatile unsigned int SCREEN_ROTATION_CHANGE_TIMER;

void init_globals() {
    init_settings();
    init_game_state();

    BACKGROUND_COLORS[0] = 0x0;
    BACKGROUND_COLORS[1] = 0xFFFF;

    TITLE_SIZE = 2;
    TITLE_INDEX = 0;

    OPTIONS_UPDATE = false;
    OPTIONS_SIZE = 4;
    OPTIONS_INDEX = 0;

    SCREEN_ROTATION_CHANGE_TIMER = 1000;
}

void init_settings() {
    // ******* DEFAULT SETTINGS ******** //
    SETTINGS.DIFFICULTY = DEBUG;
    SETTINGS.DEBUG = true;
    SETTINGS.HIT_DETECT = false;

    SETTINGS.TIME_MODE = SECONDS;

    SETTINGS.SCREEN_ROTATE = true;
    SETTINGS.SCREEN_ROTATION_DIRECTION = CLOCKWISE;
    SETTINGS.SCREEN_ROTATION_SPEED = 5;

    SETTINGS.BACKGROUND_UPDATE = false;

    SETTINGS.HEXAGON_ROTATE = true;
    SETTINGS.HEXAGON_SPEED = 2;

    SETTINGS.INPUT_TYPE = KEYS;
    SETTINGS.CONTROL_SCHEME = CONTINUOUS;
    SETTINGS.DEFAULT_PLAYER_SPEED = 25;

    // ******* DEBUG SETTINGS ******** //

    Settings* debug = &DIFFICULTIES[0];
    debug->DIFFICULTY = DEBUG;
    debug->DEBUG = true;
    debug->HIT_DETECT = false;

    debug->TIME_MODE = MILLISECONDS;

    debug->SCREEN_ROTATE = false;
    debug->SCREEN_ROTATION_DIRECTION = CLOCKWISE;
    debug->SCREEN_ROTATION_SPEED = 5;

    debug->HEXAGON_ROTATE = false;
    debug->HEXAGON_SPEED = 2;

    debug->BACKGROUND_UPDATE = false;

    debug->INPUT_TYPE = KEYBOARD;
    debug->CONTROL_SCHEME = CONTINUOUS;
    debug->DEFAULT_PLAYER_SPEED = 15;

    // ******* EASY SETTINGS ******** //

    Settings* easy = &DIFFICULTIES[1];
    easy->DIFFICULTY = EASY;
    easy->DEBUG = false;
    easy->HIT_DETECT = true;

    easy->TIME_MODE = MILLISECONDS;

    easy->SCREEN_ROTATE = false;
    easy->SCREEN_ROTATION_DIRECTION = CLOCKWISE;
    easy->SCREEN_ROTATION_SPEED = 5;

    easy->HEXAGON_ROTATE = false;
    easy->HEXAGON_SPEED = 1;

    easy->BACKGROUND_UPDATE = false;

    easy->INPUT_TYPE = KEYBOARD;
    easy->CONTROL_SCHEME = CONTINUOUS;
    easy->DEFAULT_PLAYER_SPEED = 15;

    // ******* NORMAL SETTINGS ******** //

    Settings* normal = &DIFFICULTIES[2];
    normal->DIFFICULTY = NORMAL;
    normal->DEBUG = false;
    normal->HIT_DETECT = true;

    normal->TIME_MODE = MILLISECONDS;

    normal->SCREEN_ROTATE = false;
    normal->SCREEN_ROTATION_DIRECTION = CLOCKWISE;
    normal->SCREEN_ROTATION_SPEED = 5;

    normal->HEXAGON_ROTATE = true;
    normal->HEXAGON_SPEED = 2;

    normal->BACKGROUND_UPDATE = false;

    normal->INPUT_TYPE = KEYBOARD;
    normal->CONTROL_SCHEME = CONTINUOUS;
    normal->DEFAULT_PLAYER_SPEED = 15;

    // ******* HARD SETTINGS ******** //

    Settings* hard = &DIFFICULTIES[3];
    hard->DIFFICULTY = HARD;
    hard->DEBUG = false;
    hard->HIT_DETECT = true;

    hard->TIME_MODE = MILLISECONDS;

    hard->SCREEN_ROTATE = true;
    hard->SCREEN_ROTATION_DIRECTION = CLOCKWISE;
    hard->SCREEN_ROTATION_SPEED = 5;

    hard->HEXAGON_ROTATE = true;
    hard->HEXAGON_SPEED = 3;

    hard->BACKGROUND_UPDATE = false;

    hard->INPUT_TYPE = KEYBOARD;
    hard->CONTROL_SCHEME = CONTINUOUS;
    hard->DEFAULT_PLAYER_SPEED = 15;

    apply_difficulty(0);

}

void update_settings() {
    /*
     * SW index to settings map:
     * SW[0]:       HIT_DETECT                  1 = False
     * SW[1]:       SCREEN_ROTATE               1 = True
     * SW[2]:       SCREEN_ROTATE_DIRECTION     1 = COUNTER_CLOCKWISE
     * SW[3]:       CONTROL_SCHEME              1 = CONTINUOUS
     * SW[4]:       TIME_MODE                   1 = MILLISECONDS
     * SW[5]:       HEXAGON_ROTATE              1 = True
     * SW[6]:       INPUT_TYPE                  1 = Keyboard
     */

    const int HIT_DETECT =                  0b1;
    const int SCREEN_ROTATE =               0b10;
    const int SCREEN_ROTATE_DIRECTION =     0b100;
    const int CONTROL_SCHEME =              0b1000;
    const int TIME_MODE =                   0b10000;
    const int HEXAGON_ROTATE =              0b100000;
    const int INPUT_TYPE =                  0b1000000;

    if (!SETTINGS.DEBUG)
        return;

    volatile int * SW = (int *) SW_BASE;

    SETTINGS.HIT_DETECT = ((*SW) & HIT_DETECT) == 0;
    SETTINGS.SCREEN_ROTATE = ((*SW) & SCREEN_ROTATE) != 0;
    SETTINGS.SCREEN_ROTATION_DIRECTION = (*SW & SCREEN_ROTATE_DIRECTION) ? COUNTER_CLOCKWISE : CLOCKWISE;
    SETTINGS.CONTROL_SCHEME = (*SW & CONTROL_SCHEME) ? CONTINUOUS : DISCRETE;
    SETTINGS.TIME_MODE = (*SW & TIME_MODE) ? MILLISECONDS : SECONDS;
    SETTINGS.HEXAGON_ROTATE = (*SW & HEXAGON_ROTATE) != 0;
    SETTINGS.INPUT_TYPE = (*SW & INPUT_TYPE) ? KEYBOARD : KEYS;
}


void init_game_state() {
    GAME_STATE.RUN = true;
    GAME_STATE.GAME_OVER = false;

    GAME_STATE.GAME_TIME = 0;
    GAME_STATE.SCREEN_ROTATION_ANGLE = 0;

    GAME_STATE.MODE = TITLE;
    INIT_GAME_REQUEST = true;
}

void update_game_state() {

}

void apply_difficulty(int index) {
    Settings difficulty = DIFFICULTIES[index];
    SETTINGS.DIFFICULTY = difficulty.DIFFICULTY;

    SETTINGS.DEBUG = difficulty.DEBUG;
    SETTINGS.HIT_DETECT = difficulty.HIT_DETECT;

    SETTINGS.SCREEN_ROTATE = difficulty.SCREEN_ROTATE;
    SETTINGS.HEXAGON_ROTATE = difficulty.HEXAGON_ROTATE;
    SETTINGS.HEXAGON_SPEED = difficulty.HEXAGON_SPEED;

    SETTINGS.BACKGROUND_UPDATE = difficulty.BACKGROUND_UPDATE;
}

void chooseNextOption() {
    OPTIONS_INDEX = (OPTIONS_INDEX + 1) % OPTIONS_SIZE;
    OPTIONS_UPDATE = true;
}

void choosePreviousOption() {
    OPTIONS_INDEX = (OPTIONS_SIZE + OPTIONS_INDEX - 1) % OPTIONS_SIZE;
    OPTIONS_UPDATE = true;
}

void cycleOptionRight() {
    if (OPTIONS_INDEX == 0) // Input Type
        SETTINGS.INPUT_TYPE = (SETTINGS.INPUT_TYPE == KEYS) ? KEYBOARD : KEYS;
    else if (OPTIONS_INDEX == 1) // Control Scheme
        SETTINGS.CONTROL_SCHEME = (SETTINGS.CONTROL_SCHEME == DISCRETE) ? CONTINUOUS : DISCRETE;
    else if (OPTIONS_INDEX == 2) // Time Mode
        SETTINGS.TIME_MODE = (SETTINGS.TIME_MODE == SECONDS) ? MILLISECONDS : SECONDS;
    else if (OPTIONS_INDEX == 3) // Difficulty
        apply_difficulty((SETTINGS.DIFFICULTY + 1) % 4);
    OPTIONS_UPDATE = true;
}

void cycleOptionLeft() {
    if (OPTIONS_INDEX == 0) // Input Type
        SETTINGS.INPUT_TYPE = (SETTINGS.INPUT_TYPE == KEYS) ? KEYBOARD : KEYS;
    else if (OPTIONS_INDEX == 1) // Control Scheme
        SETTINGS.CONTROL_SCHEME = (SETTINGS.CONTROL_SCHEME == DISCRETE) ? CONTINUOUS : DISCRETE;
    else if (OPTIONS_INDEX == 2) // Time Mode
        SETTINGS.TIME_MODE = (SETTINGS.TIME_MODE == SECONDS) ? MILLISECONDS : SECONDS;
    else if (OPTIONS_INDEX == 3) // Difficulty
        apply_difficulty((4 + SETTINGS.DIFFICULTY - 1) % 4);
    OPTIONS_UPDATE = true;
}
