#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <nds.h>

// Simple on‑screen keyboard layout (QWERTY, 3 rows + space)
#define K_ROWS 4
#define K_COLS 10

// Define each key as a printable ASCII character; -1 means no key (empty space)
static const int8_t keyMap[K_ROWS][K_COLS] = {
    // Row 0
    {'q','w','e','r','t','y','u','i','o','p'},
    // Row 1
    {'a','s','d','f','g','h','j','k','l','\n'}, // '\n' acts as Enter
    // Row 2
    {'z','x','c','v','b','n','m',' ','-','_'},
    // Row 3 (space bar row – only centre column used)
    {-1,-1,-1,' ', -1,-1,-1,-1,-1,-1}
};

// Sprite IDs for keyboard background – simple coloured tiles
extern u16 keyboardGfx[256]; // placeholder – you can load a bitmap later

// Initialise any graphics needed for the keyboard (called once at startup)
static inline void keyboardInit(void) {
    // Set up a background on the bottom screen (sub) to draw the keys
    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);
    // Simple plain background colour – use theme colours later if desired
    int bg = BG_PALETTE[0]; // already set by theme
    // For now we just clear the sub screen
    subBgFill(0, bg);
    // Draw keys as text using console (quick demo)
    consoleInitDefault((u16*)0x06200000, 0, BgType_Text4bpp,
                       BgSize_T_256x256, 0, 0, false, false);
    // Print a quick visual representation – not pixel‑perfect
    iprintf("Keyboard (touch)\n");
    for (int r = 0; r < K_ROWS; ++r) {
        for (int c = 0; c < K_COLS; ++c) {
            int v = keyMap[r][c];
            if (v == -1) iprintf(" ");
            else if (v == '\n') iprintf("[Enter]");
            else iprintf(" %c", (char)v);
        }
        iprintf("\n");
    }
}

// Convert touch coordinates to a key index (row*cols + col) or -1 if none.
static inline int keyboardTouchToKey(int px, int py) {
    // Bottom screen is 256×192 pixels. Divide into a grid.
    const int keyWidth = 256 / K_COLS;   // 25‑26 px
    const int keyHeight = 192 / K_ROWS; // 48 px
    int col = px / keyWidth;
    int row = py / keyHeight;
    if (row < 0 || row >= K_ROWS || col < 0 || col >= K_COLS) return -1;
    if (keyMap[row][col] == -1) return -1;
    return row * K_COLS + col;
}

// Return the printable character for a given key index.
static inline char keyboardKeyChar(int keyIdx) {
    int row = keyIdx / K_COLS;
    int col = keyIdx % K_COLS;
    int v = keyMap[row][col];
    return (char)v;
}

// Redraw the keyboard – placeholder (could refresh colours/theme).
static inline void keyboardDraw(void) {
    // In this minimal skeleton we rely on the console output already drawn.
    // A real implementation would redraw bitmap tiles here.
}

#endif // KEYBOARD_H
