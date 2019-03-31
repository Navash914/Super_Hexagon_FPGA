#ifndef MAIN_C_PLAYER_H
#define MAIN_C_PLAYER_H

#include <stdbool.h>
#include "hexagon.h"

typedef struct player {
    int section;
    int height;
    short int color;
    float rotation;
    float speed;
    // For keyboard movement
    bool move_right;
    bool move_left;
} Player;

void updatePlayer(Player* player);
void movePlayerRight(Player* player);
void movePlayerLeft(Player* player);

bool checkPlayerHit(Player player);
bool isPlayerHit(Player player, Hexagon hex);

void draw_player(Player player);

#endif //MAIN_C_PLAYER_H
