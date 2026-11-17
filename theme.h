#ifndef THEME_H
#define THEME_H

#include <nds.h>

// Simple colour theme – background and text colours for top screen
typedef struct {
    u16 bgColor;   // 15-bit BGR5 colour
    u16 fgColor;
    u16 cursorColor;
} Theme;

extern Theme gCurrentTheme;

// Load theme from "theme.ini" on the SD card (fallback to default)
static inline void loadTheme(void) {
    // Very naive parser – look for lines "bg=RRR,GGG,BBB" etc.
    FILE *f = fopen("theme.ini", "r");
    if (!f) {
        // Default dark theme
        gCurrentTheme.bgColor = RGB15(0,0,0);
        gCurrentTheme.fgColor = RGB15(31,31,31);
        gCurrentTheme.cursorColor = RGB15(31,0,0);
        return;
    }
    char line[64];
    while (fgets(line, sizeof(line), f)) {
        int r,g,b;
        if (sscanf(line, "bg=%d,%d,%d", &r,&g,&b) == 3) {
            gCurrentTheme.bgColor = RGB15(r>>3,g>>3,b>>3);
        } else if (sscanf(line, "fg=%d,%d,%d", &r,&g,&b) == 3) {
            gCurrentTheme.fgColor = RGB15(r>>3,g>>3,b>>3);
        } else if (sscanf(line, "cursor=%d,%d,%d", &r,&g,&b) == 3) {
            gCurrentTheme.cursorColor = RGB15(r>>3,g>>3,b>>3);
        }
    }
    fclose(f);
}

// Toggle between two hard‑coded themes (call from main loop)
static inline void toggleTheme(void) {
    static bool dark = true;
    dark = !dark;
    if (dark) {
        gCurrentTheme.bgColor = RGB15(0,0,0);
        gCurrentTheme.fgColor = RGB15(31,31,31);
        gCurrentTheme.cursorColor = RGB15(31,0,0);
    } else {
        gCurrentTheme.bgColor = RGB15(0,0,31); // dark blue bg
        gCurrentTheme.fgColor = RGB15(31,31,0); // yellow text
        gCurrentTheme.cursorColor = RGB15(0,31,0);
    }
    // Apply colours – simple example using BG palette 0
    BG_PALETTE[0] = gCurrentTheme.bgColor;
    BG_PALETTE[1] = gCurrentTheme.fgColor;
    BG_PALETTE[2] = gCurrentTheme.cursorColor;
}

#endif // THEME_H
