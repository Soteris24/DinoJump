#include <gb/gb.h>
#include <rand.h>
#include <stdint.h>
#include "Bird.c"
#include "map.c"
#include "tileset.c"
#include "enemy.c"

INT16 playerX = 40;
INT16 playerY = 80;
INT16 enemyAX[3] = {120, 200, 280};
INT16 enemyAY = 120;

INT16 enemyBX = 200;
INT16 enemyBY = 120;

INT8 velocityY = 0;
INT8 gravity = 1;
INT8 jumpStrength = -8;
INT8 floorY = 120;
BOOLEAN isJumping = FALSE;

// Animation variables
UINT8 enemyAFrame = 0, enemyAFrameCount = 0;
UINT8 enemyBFrame = 0, enemyBFrameCount = 0;
UINT8 playerFrame = 0, playerFrameCount = 0;
UINT8 jumpUpdateTimer = 0;

void performantdelay(UINT8 numloops) {
    UINT8 i;
    for(i = 0; i < numloops; i++) wait_vbl_done();
}

// --- Enemy A clones update ---
void updateEnemyA() {
    UINT8 i;
    enemyAFrameCount++;
    if (enemyAFrameCount >= 5) {
        enemyAFrameCount = 0;
        enemyAFrame = !enemyAFrame;
        // all enemies share same animation
        for(i = 0; i < 3; i++) {
            set_sprite_tile(1 + i, 2 + enemyAFrame);
        }
    }

    for(i = 0; i < 3; i++) {
        enemyAX[i] -= 2;
        if (enemyAX[i] < -16) {
            enemyAX[i] = 160 + (rand() % 60);
        }
        move_sprite(1 + i, enemyAX[i], enemyAY);
    }
}

// --- Enemy B update ---
void updateEnemyB() {
    enemyBX -= 3;
    enemyBY += 1;
    if (enemyBX < -16) {
        enemyBX = 160 + (rand() % 60);
        enemyBY = 50 + (rand() % 10);
    }

    enemyBFrameCount++;
    if (enemyBFrameCount >= 5) {
        enemyBFrameCount = 0;
        enemyBFrame = !enemyBFrame;
        set_sprite_tile(4, 4 + enemyBFrame); // tiles 4–5
    }

    move_sprite(4, enemyBX, enemyBY);
}

// --- Player update ---
void updatePlayer(UINT8 spriteid) {
    jumpUpdateTimer++;
    if (jumpUpdateTimer >= 2) {
        jumpUpdateTimer = 0;
        playerY += velocityY;
        velocityY += gravity;

        if (playerY >= floorY) {
            playerY = floorY;
            velocityY = 0;
            isJumping = FALSE;
        }
    }

    playerFrameCount++;
    if (playerFrameCount >= 5) {
        playerFrameCount = 0;
        playerFrame = !playerFrame;
        set_sprite_tile(spriteid, 0 + playerFrame);
    }

    move_sprite(spriteid, playerX, playerY);
}

void main() {
    initrand(1234);

    set_bkg_data(0, 8, tileset);
    set_bkg_tiles(0, 0, 33, 18, map);

    set_sprite_data(0, 6, Bird);
    set_sprite_data(2, 4, enemy);

    // player
    set_sprite_tile(0, 0);
    move_sprite(0, playerX, playerY);

    // enemy A clones (sprites 1–3)
    for (UINT8 i = 0; i < 3; i++) {
        set_sprite_tile(1 + i, 2);
        move_sprite(1 + i, enemyAX[i], enemyAY);
    }

    // enemy B (sprite 4)
    set_sprite_tile(4, 4);
    move_sprite(4, enemyBX, enemyBY);

    SHOW_SPRITES;
    SHOW_BKG;
    DISPLAY_ON;

    while(1) {
        UINT8 keys = joypad();
        scroll_bkg(2, 0);

        if((keys & J_A) && !isJumping) {
            isJumping = TRUE;
            velocityY = jumpStrength;
        }

        updatePlayer(0);
        updateEnemyA();
        updateEnemyB();

        performantdelay(1);
    }
}
