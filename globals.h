#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include "constants.h"
#include "player.h"
#include "hexagon.h"

enum RotationDirection {
    CLOCKWISE,
    COUNTER_CLOCKWISE
};

enum InputType {
    KEYS,
    KEYBOARD
};

enum ControlScheme {
    DISCRETE,
    CONTINUOUS
};

enum TimeMode {
    SECONDS,
    MILLISECONDS
};

enum Mode {
    TITLE,
    OPTIONS,
    GAME
};

enum Difficulty {
    DEBUG,
    EASY,
    NORMAL,
    HARD
};

typedef struct settings {
    bool DEBUG;
    bool HIT_DETECT;

    enum Difficulty DIFFICULTY;

    enum TimeMode TIME_MODE;

    bool SCREEN_ROTATE;
    enum RotationDirection SCREEN_ROTATION_DIRECTION;
    float SCREEN_ROTATION_SPEED;

    bool HEXAGON_ROTATE;
    int HEXAGON_SPEED;

    bool BACKGROUND_UPDATE;

    enum InputType INPUT_TYPE;
    enum ControlScheme CONTROL_SCHEME;

    float DEFAULT_PLAYER_SPEED;
} Settings;

typedef struct game_state {
    volatile bool RUN;
    bool GAME_OVER;

    unsigned int GAME_TIME;
    float SCREEN_ROTATION_ANGLE;

    volatile enum Mode MODE;
} Game_State;

extern Settings SETTINGS;
extern Settings DIFFICULTIES[4];

extern Game_State GAME_STATE;
extern bool INIT_GAME_REQUEST;

extern Player PLAYER;
extern Hexagon HEXAGONS[SIZE];

extern Segment BACKGROUND[NUM_SECTIONS];
extern short int BACKGROUND_COLORS[2];

extern int TITLE_SIZE;
extern volatile int TITLE_INDEX;

extern volatile bool OPTIONS_UPDATE;
extern int OPTIONS_SIZE;
extern volatile int OPTIONS_INDEX;

extern volatile unsigned int SCREEN_ROTATION_CHANGE_TIMER;

// Methods:

void init_globals();

void init_settings();
void update_settings();

void init_game_state();
void update_game_state();

void apply_difficulty(int index);

void chooseNextOption();
void choosePreviousOption();
void cycleOptionRight();
void cycleOptionLeft();

#endif //MAIN_C_GLOBALS_H
