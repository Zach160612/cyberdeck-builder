#include <nds.h>
#include <stdlib.h>
#include "starfield.h"

typedef struct {
    int x, y;
    int speed; // pixels per frame
} Star;

static Star stars[STAR_COUNT];
static const u16 starTile[8 * 8 / 2] = {
    // 4bpp tile: fill with palette index 1 (white)
    0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,
    0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,
    0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,
    0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111
};

void starfield_init(void) {
    // Set a bright white color at palette index 1 for OBJ palette
    OBJ_PALETTE[1] = RGB15(31,31,31);
    // Load the tile data into OBJ graphics memory (character base block 0)
    dmaCopy(starTile, (void*)CHAR_BASE_BLOCK(0), sizeof(starTile));

    // Initialize stars with random positions and speeds
    for (int i = 0; i < STAR_COUNT; ++i) {
        stars[i].x = rand() % 256; // screen width
        stars[i].y = rand() % 192; // screen height
        stars[i].speed = 1 + (rand() % 3); // 1‑3 pixels per frame
        // Place each star in OAM (sprite) slot i
        oamSet(&oamMain, i, stars[i].x, stars[i].y,
               0,          // priority (0 = highest)
               0,          // palette (use OBJ palette 0‑15, we use index 1 in tile)
               SpriteSize_8x8,
               SpriteColorFormat_16Color,
               starTile,   // gfxOffset (tile index 0)
               -1, -1,    // rotation/scaling not used
               false, false, false, false, false);
    }
}

void starfield_update(void) {
    // Move each star downwards; wrap to top when off‑screen
    for (int i = 0; i < STAR_COUNT; ++i) {
        stars[i].y += stars[i].speed;
        if (stars[i].y >= 192) {
            stars[i].y = 0;
            stars[i].x = rand() % 256;
            stars[i].speed = 1 + (rand() % 3);
        }
        // Update OAM position
        oamSetXY(&oamMain, i, stars[i].x, stars[i].y);
    }
    // Apply the OAM changes for this frame
    oamUpdate(&oamMain);
}

void starfield_draw(void) {
    // Nothing extra needed – sprites are drawn automatically after oamUpdate.
    // This function exists for symmetry with init/update.
}
