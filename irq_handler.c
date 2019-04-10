/*
 * irq_handler.c
 * Created by Naveed Ashfaq
 *
 * This file handles interrupts to the ARM processor
 */

#include <stdlib.h>

#include "irq_handler.h"
#include "address_map_arm.h"
#include "defines.h"
#include "globals.h"
#include "interrupt_ids.h"
#include "player.h"
#include "utils.h"

//  *************   Vector Table Callback Functions     ************** //
void __attribute__((interrupt)) __cs3_reset() {
    while (true);
}

void __attribute__((interrupt)) __cs3_isr_undef() {
    while (true);
}

void __attribute__((interrupt)) __cs3_isr_swi() {
    while (true);
}

void __attribute__((interrupt)) __cs3_isr_pabort() {
    while (true);
}

void __attribute__((interrupt)) __cs3_isr_dabort() {
    while (true);
}

void __attribute__((interrupt)) __cs3_isr_fiq() {
    while (true);
}

// IRQ Interrupt Handler
void __attribute__((interrupt)) __cs3_isr_irq() {
    volatile unsigned int *base = (unsigned int *) MPCORE_GIC_CPUIF;
    volatile unsigned int interrupt_id = *(base + 3);

    // Find interrupt
    if (interrupt_id == KEYS_IRQ) {
        // Key Interrupt
        on_key_press();
    } else if (interrupt_id == MPCORE_PRIV_TIMER_IRQ) {
        // 200MHz Timer Interrupt
        on_priv_timer_tick();
    } else if (interrupt_id == PS2_IRQ) {
        // PS2 Interrupt
        on_ps2_press();
    }

    // Clear the interrupt
    *(base+4) = interrupt_id;
}

//  *************   End Of Vector Table Callback Functions     ************** //

//  *************   Config Interrupts   ************** //

// Configure stack pointer for IRQ mode
void config_stack_pointers() {
    int stack, mode;
    stack = A9_ONCHIP_END - 7;
    mode = INT_DISABLE | IRQ_MODE;
    asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
    asm("mov sp, %[ps]" : : [ps] "r"(stack));

    mode = INT_DISABLE | SVC_MODE;
    asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
}

// Configure interrupt for arm processor
void config_arm_interrupt() {
    int value = SVC_MODE | INT_ENABLE;
    __asm__ volatile ("MSR CPSR, %0" :: "r" (value));
}

// Enable key interrupts
void config_key_interrupt() {
    volatile int * key_base = (int *) KEY_BASE;
    *(key_base + 2) = 0xFFFF;   // Set up interrupt masks
    *(key_base + 3) = 0xFFFF;   // Reset edge captures
}

// Enable priv timer interrupts
void config_priv_timer_interrupt() {
    volatile int * priv_timer_base = (int *) MPCORE_PRIV_TIMER;
    int load_value = 0.01 * 200 * 1000000;  // Tick every 0.01s
    *priv_timer_base = load_value;
    *(priv_timer_base + 2) = 0b111; // Enable interrupt, autoload and enable
}

// Enable PS/2 port interrupts
void config_ps2_interrupt() {
    volatile int * ps2_base = (int *) PS2_BASE;
    *(ps2_base) = 0xFF;     // reset
    *(ps2_base + 1) = 1;    // enable interrupts
}

//  *************   End Of Config Interrupts     ************** //


// Called on key interrupt
void on_key_press() {
    volatile int *key_base = (int *) KEY_BASE;
    volatile int edge = *(key_base + 3) & 0xF;

    if (edge == KEY0) {
        // Key 0 Press
        if (GAME_STATE.MODE == TITLE) {
            if (TITLE_INDEX == 0) {
                GAME_STATE.MODE = GAME;
                INIT_GAME_REQUEST = true;
            } else if (TITLE_INDEX == 1) {
                GAME_STATE.MODE = OPTIONS;
                OPTIONS_INDEX = 0;
            }
        } else if (SETTINGS.INPUT_TYPE == KEYS && GAME_STATE.MODE == GAME && SETTINGS.CONTROL_SCHEME == DISCRETE)
            movePlayerRight(&PLAYER);
        else if (GAME_STATE.MODE == OPTIONS)
            cycleOptionRight();
    } else if (edge == KEY1) {
        // Key 1 Press
        if (GAME_STATE.MODE == TITLE)
            TITLE_INDEX = (TITLE_INDEX + 1) % TITLE_SIZE;
        else if (SETTINGS.INPUT_TYPE == KEYS && GAME_STATE.MODE == GAME && SETTINGS.CONTROL_SCHEME == DISCRETE)
            movePlayerLeft(&PLAYER);
        else if (GAME_STATE.MODE == OPTIONS)
            chooseNextOption();
    } else if (edge == KEY2) {
        // Key 2 Press
        if (GAME_STATE.MODE == TITLE)
            TITLE_INDEX = (TITLE_SIZE + TITLE_INDEX - 1) % TITLE_SIZE;
        else if (GAME_STATE.MODE == GAME && SETTINGS.DIFFICULTY == DEBUG) {
            int i;
            for (i = 0; i < SIZE; ++i) {
                int speed = (HEXAGONS[i].speed + 1) % 4;
                if (speed == 0)
                    speed = 1;
                set_hexagon_speed(&HEXAGONS[i], speed);
            }
        } else if (GAME_STATE.MODE == OPTIONS)
            choosePreviousOption();
    } else if (edge == KEY3) {
        // Key 3 Press
        if (GAME_STATE.MODE == GAME) {
            //GAME_STATE.RUN = !GAME_STATE.RUN;
            //setLED(0, GAME_STATE.RUN);
            GAME_STATE.MODE = TITLE;
            TITLE_INDEX = 0;
        } else if (GAME_STATE.MODE == OPTIONS) {
            GAME_STATE.MODE = TITLE;
            TITLE_INDEX = 1;
        }
    }

    // Reset edge
    *(key_base + 3) = edge;
}

// Called on priv timer tick
void on_priv_timer_tick() {
    volatile int * priv_timer_base = (int *) MPCORE_PRIV_TIMER;

    // Update timer events if playing
    if (GAME_STATE.MODE == GAME && GAME_STATE.RUN) {
        // Add 1ms to game time
        GAME_STATE.GAME_TIME++;

        // Timer Events:

        // Check if time to change screen rotation direction
        if (!SETTINGS.DEBUG && SETTINGS.SCREEN_ROTATE) {
            SCREEN_ROTATION_CHANGE_TIMER--;
            if (SCREEN_ROTATION_CHANGE_TIMER == 0) {
                // Change screen rotation direction
                if (SETTINGS.SCREEN_ROTATION_DIRECTION == CLOCKWISE)
                    SETTINGS.SCREEN_ROTATION_DIRECTION = COUNTER_CLOCKWISE;
                else
                    SETTINGS.SCREEN_ROTATION_DIRECTION = CLOCKWISE;

                // Reset time to change direction
                SCREEN_ROTATION_CHANGE_TIMER = 100 * (7 + rand() % 7);
            }
        }
    }


    /*if (GAME_STATE.GAME_TIME % 1000 == 0) {
        int i;
        for (i = 0; i < NUM_SECTIONS; ++i) {
            BACKGROUND[i].color = (BACKGROUND[i].color == BACKGROUND_COLORS[0]) ?
                                    BACKGROUND_COLORS[1] : BACKGROUND_COLORS[0];
        }
    }*/

    // Reset Timer "F" Flag
    *(priv_timer_base + 3) = 1;
}

// Called on PS/2 interrupt
void on_ps2_press() {
    volatile int * ps2_base = (int *) PS2_BASE;
    volatile char data;
    volatile int PS2_data, RVALID;

    while (*(ps2_base+1) & 0b100000000) {
        do {
            PS2_data = * ps2_base;
            RVALID = PS2_data & 0x8000;
        } while (!RVALID);
        data = PS2_data & 0xFF;
        if (data == EXTENDED_KEYS) {
            // Extended Key Press
            do {
                PS2_data = * ps2_base;
                RVALID = PS2_data & 0x8000;
            } while (!RVALID);
            data = PS2_data & 0xFF;
            bool is_break = false;
            if (data == BREAK) {
                // Is Break
                is_break = true;
                do {
                    PS2_data = * ps2_base;
                    RVALID = PS2_data & 0x8000;
                } while (!RVALID);
                data = PS2_data & 0xFF;
            }
            if (data == RIGHT_ARROW) {
                // Right Arrow
                if (GAME_STATE.MODE == GAME) {
                    PLAYER.move_right = !is_break;
                    if (is_break && SETTINGS.CONTROL_SCHEME == DISCRETE && SETTINGS.INPUT_TYPE == KEYBOARD) {
                        movePlayerRight(&PLAYER);
                    }
                } else if (GAME_STATE.MODE == TITLE) {
                    TITLE_INDEX = (TITLE_INDEX + 1) % TITLE_SIZE;
                } else if (GAME_STATE.MODE == OPTIONS) {
                    cycleOptionRight();
                }
            } else if (data == LEFT_ARROW) {
                // Left Arrow
                if (GAME_STATE.MODE == GAME) {
                    PLAYER.move_left = !is_break;
                    if (is_break && SETTINGS.CONTROL_SCHEME == DISCRETE && SETTINGS.INPUT_TYPE == KEYBOARD)
                        movePlayerLeft(&PLAYER);
                    } else if (GAME_STATE.MODE == TITLE) {
                        TITLE_INDEX = (TITLE_SIZE + TITLE_INDEX - 1) % TITLE_SIZE;
                    } else if (GAME_STATE.MODE == OPTIONS) {
                        cycleOptionLeft();
                    }
            } else if (data == UP_ARROW) {
                // Up Arrow
                if (is_break && GAME_STATE.MODE == OPTIONS) {
                    choosePreviousOption();
                }
            } else if (data == DOWN_ARROW) {
                // Down Arrow
                if (is_break && GAME_STATE.MODE == OPTIONS) {
                    chooseNextOption();
                }
            }
        } else {
            // Non Extended Key Press
            bool is_break = false;
            if (data == BREAK) {
                // Is Break
                is_break = true;
            do {
                PS2_data = * ps2_base;
                RVALID = PS2_data & 0x8000;
            } while (!RVALID);
            data = PS2_data & 0xFF;
            }
            if (data == ENTER_KEY) {
                // Enter Key
                if (is_break && GAME_STATE.MODE == TITLE) {
                    if (TITLE_INDEX == 0) {
                        GAME_STATE.MODE = GAME;
                        INIT_GAME_REQUEST = true;
                    } else if (TITLE_INDEX == 1) {
                        GAME_STATE.MODE = OPTIONS;
                        OPTIONS_INDEX = 0;
                    }
                }
            } else if (data == SPACE_KEY) {
                // Space Key
                if (is_break && GAME_STATE.MODE == TITLE)
                    GAME_STATE.MODE = GAME;
            } else if (data == ESC) {
                // Escape Key
                if (is_break && GAME_STATE.MODE == GAME) {
                    GAME_STATE.MODE = TITLE;
                    TITLE_INDEX = 0;
                } else if (is_break && GAME_STATE.MODE == OPTIONS) {
                    GAME_STATE.MODE = TITLE;
                    TITLE_INDEX = 1;
                }
            }
        }
    }
}
