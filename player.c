#include "player.h"
#include "constants.h"
#include "vga.h"
#include "utils.h"
#include "globals.h"
#include "address_map_arm.h"
#include "defines.h"

void updatePlayer(Player* player) {
    // Continuous Control Scheme Only
    if (SETTINGS.CONTROL_SCHEME != CONTINUOUS) {
        //printf("!!! NOT CONTINUOUS !!!");
        //printf("ControlSheme: %d", (int) SETTINGS.CONTROL_SCHEME);
        //printf("Address: %d", (int) &SETTINGS.CONTROL_SCHEME);
        return;
    }
    if (SETTINGS.INPUT_TYPE == KEYS) {
        volatile int *keys = (int *) KEY_BASE;
        if (*keys & KEY0) {
            // Key 0 held
            movePlayerRight(player);
        } else if (*keys & KEY1) {
            // Key 1 held
            movePlayerLeft(player);
        }
    } else if (SETTINGS.INPUT_TYPE == KEYBOARD) {
        if (player->move_right && player->move_left)
            return;
        if (player->move_left)
            movePlayerLeft(player);
        if (player->move_right)
            movePlayerRight(player);
    }
}

void movePlayerRight(Player* player) {
    if (!GAME_STATE.RUN)
        return;
    if (SETTINGS.CONTROL_SCHEME == DISCRETE)
        player->section = (player->section + 1) % 6;
    else {
        player->rotation += player->speed;
        if (player->rotation >= 360)
            player->rotation -= 360;
    }
}

void movePlayerLeft(Player* player) {
    if (!GAME_STATE.RUN)
        return;
    if (SETTINGS.CONTROL_SCHEME == DISCRETE) {
        if (player->section == 0)
            player->section = 5;
        else
            player->section--;
    } else {
        player->rotation -= player->speed;
        if (player->rotation < 0)
            player->rotation += 360;
    }

}


bool checkPlayerHit(Player player) {
    if (!SETTINGS.HIT_DETECT)
        return false;
    int i;
    for (i = 0; i < SIZE; ++i) {
        if (isPlayerHit(player, HEXAGONS[i]))
            return true;
    }
    return false;
}

bool isPlayerHit(Player player, Hexagon hex) {
    if (SETTINGS.CONTROL_SCHEME == DISCRETE && player.section == hex.missing_segment)
        return false;

    int playerBottom = ORIGIN_PADDING + PLAYER_PADDING;
    int playerTop = playerBottom + player.height;

    int hexTop = hex.y;
    int hexBottom = 1 + hexTop - hex.height;

    if (hexBottom >= playerTop)
        return false;
    if (hexTop <= playerBottom)
        return false;

    if (SETTINGS.CONTROL_SCHEME == DISCRETE)
        return true;

    float seg_x = (ROTATION_ANGLE * hex.missing_segment + hex.rotation);
    if (seg_x >= 360)
        seg_x -= 360;
    float seg_width = (ROTATION_ANGLE / 2) - 5;

    float seg_left = seg_x - seg_width;
    bool is_seg_broken = false;
    if (seg_left < 0) {
        seg_left += 360;
        is_seg_broken = true;
    }
    float seg_right = seg_x + seg_width;
    if (seg_right >= 360) {
        seg_right -= 360;
        is_seg_broken = true;
    }

    float player_x = player.rotation;
    if (is_seg_broken) {
        if (player_x > seg_left && player_x < 360)
            return false;
        if (player_x >= 0 && player_x < seg_right)
            return false;
        return true;
    }
    if (player_x > seg_left && player_x < seg_right)
        return false;
    return true;
}


void draw_player(Player player) {
    int h = player.height;
    int x = ORIGIN_X;
    int y = 1 + ORIGIN_Y - ORIGIN_PADDING - PLAYER_PADDING - h;
    short int color = player.color;
    float rotation = GAME_STATE.SCREEN_ROTATION_ANGLE;
    if (SETTINGS.CONTROL_SCHEME == CONTINUOUS)
        rotation += player.rotation;
    int i,w;
    for (i=0, w=0; i<h; ++i, w += 2) {
        int y0 = y + i, y1 = y0;
        int x0 = x - w/2, x1 = x + w/2;

        rotate(&x0, &y0, player.section);
        rotate(&x1, &y1, player.section);
        rotate2(&x0, &y0, rotation);
        rotate2(&x1, &y1, rotation);

        draw_line(x0, y0, x1, y1, color);
    }
}