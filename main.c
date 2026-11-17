#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <string.h>
#include "theme.h"
#include "keyboard.h"
#include "file_manager.h"
#include "calculator.h"
#include "book_viewer.h"
#include "map_viewer.h"
#include "state.h"

/* -------------------------------------------------------------------- */
#define TERM_W 40
#define TERM_H 24
static char term_buf[TERM_H][TERM_W];
static int cur_x = 0, cur_y = 0;
static bool cursor_visible = true;   // blinking cursor flag
static int frame_counter = 0;        // counts VBlanks for blinking

/* Simple 8‑bit 8 kHz beep – 200 ms (1600 samples) */
static const u8 beep_data[1600] = {
    // generated a sine wave at ~1000 Hz, amplitude 0x80‑0xFF range
    // (values omitted for brevity – you can fill with any pattern)
    // We'll use a simple square‑wave pattern for demonstration:
    #define BEEP_VAL 0xFF
    #define SILENCE 0x80
    // first half high, second half low
    /* 800 samples high */
    [0 ... 799] = BEEP_VAL,
    /* 800 samples low */
    [800 ... 1599] = SILENCE
};
#undef BEEP_VAL
#undef SILENCE

/* -------------------------------------------------------------------- */
static void term_putc(char c) {
    if (c == '\n') {
        cur_x = 0;
        cur_y = (cur_y + 1) % TERM_H;
        return;
    }
    term_buf[cur_y][cur_x] = c;
    cur_x++;
    if (cur_x >= TERM_W) {
        cur_x = 0;
        cur_y = (cur_y + 1) % TERM_H;
    }
}
static void term_puts(const char *s) { while (*s) term_putc(*s++); }

static void draw_terminal(void) {
    consoleClear();
    for (int y = 0; y < TERM_H; ++y) {
        for (int x = 0; x < TERM_W; ++x) {
            iprintf("%c", term_buf[y][x] ? term_buf[y][x] : ' ');
        }
        iprintf("\n");
    }
    // Draw cursor only when visible
    if (cursor_visible) {
        iprintf("\x1b[%d;%dH", cur_y + 1, cur_x + 1);
    }
}

/* -------------------------------------------------------------------- */
static void intro_animation(void) {
    const char *msg = "CYBERDECK";
    int len = strlen(msg);
    for (int i = 0; i < len; ++i) {
        iprintf("%c", msg[i]);
        swiWaitForVBlank();
    }
    for (int i = 0; i < 30; ++i) swiWaitForVBlank();   // hold ~0.5 s
    consoleClear();
    swiWaitForVBlank();
}

static void draw_menu(int selected) {
    const char *options[] = {"Terminal", "File Manager", "Calculator", "Book", "Map"};
    consoleClear();
    iprintf("=== CYBERDECK MENU ===\n\n");
    for (int i = 0; i < 5; ++i) {
        iprintf(selected == i ? "> %s\n" : "  %s\n", options[i]);
    }
    iprintf("\nUse D‑Pad to move, A to select.\n");
}

static void play_beep(void) {
    // Initialise sound system if not already (idempotent)
    soundEnable();
    // Play the 8 kHz 8‑bit sample once, non‑looping
    soundPlaySample(SOUND_FREQ_8KHZ, SAMPLE_FORMAT_8BIT, sizeof(beep_data), beep_data, false);
}

int main(void) {
    /* Video init – top = main, bottom = sub (keyboard) */
    videoSetMode(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInitDefault((u16*)0x06000000, 0, BgType_Text4bpp,
                       BgSize_T_256x256, 0, 0, false, false);

    // Initialise sprite engine (OAM) for starfield
    oamInit(&oamMain, SpriteMapping_1D_128, false);
    // Initialise our futuristic starfield background
    starfield_init();

    intro_animation();
    keyboardInit();
    loadTheme();
    if (!fatInitDefault()) iprintf("SD init failed!\n");

    enum AppState state = STATE_TERMINAL;
    bool in_menu = true;
    int menu_sel = 0;
    draw_menu(menu_sel);

    while (1) {
        scanKeys();
        u32 down = keysDown();
        u32 held = keysHeld();

        /* ----- cursor blink handling (toggle every ~0.5 s) ----- */
        frame_counter++;
        if (frame_counter % 30 == 0) {       // 30 frames ≈ 0.5 s at 60 Hz
            cursor_visible = !cursor_visible;
            if (!in_menu && state == STATE_TERMINAL) draw_terminal();
        }

        if (in_menu) {
            if (down & KEY_DOWN) { menu_sel = (menu_sel + 1) % 5; draw_menu(menu_sel); }
            if (down & KEY_UP)   { menu_sel = (menu_sel + 4) % 5; draw_menu(menu_sel); }
            if (down & KEY_A) {
                switch (menu_sel) {
                    case 0: state = STATE_TERMINAL; break;
                    case 1: state = STATE_FILEMANAGER; break;
                    case 2: state = STATE_CALCULATOR; break;
                    case 3: state = STATE_BOOK; break;
                    case 4: state = STATE_MAP; break;
                }
                in_menu = false;
                consoleClear();
                if (state == STATE_TERMINAL) {
                    memset(term_buf, 0, sizeof(term_buf));
                    cur_x = cur_y = 0;
                    draw_terminal();
                } else if (state == STATE_CALCULATOR) {
                    term_puts("Calc> ");
                    cur_x = 6;
                    draw_terminal();
                }
            }
            continue; // stay in menu loop
        }

        // Global shortcuts
        if (held & KEY_Y) toggleTheme();
        if (down & KEY_B) { in_menu = true; draw_menu(menu_sel); continue; }

        // State‑specific handling
        switch (state) {
            case STATE_TERMINAL:
                if (touchPenDown()) {
                    touchPosition tp; touchRead(&tp);
                    int k = keyboardTouchToKey(tp.px, tp.py);
                    if (k >= 0) {
                        char ch = keyboardKeyChar(k);
                        term_buf[cur_y][cur_x] = ch;
                        cur_x++; if (cur_x >= TERM_W) { cur_x = 0; cur_y = (cur_y + 1) % TERM_H; }
                        draw_terminal();
                        keyboardDraw();
                        play_beep();
                    }
                }
                break;
            case STATE_FILEMANAGER:
                if (down & KEY_A) { fm_list_root(); draw_terminal(); play_beep(); }
                break;
            case STATE_CALCULATOR:
                if (down & KEY_A) {
                    // Gather expression from buffer (everything up to cursor)
                    char expr[128] = {0};
                    int pos = 0;
                    for (int y = 0; y <= cur_y; ++y) {
                        int end = (y == cur_y) ? cur_x : TERM_W;
                        for (int x = 0; x < end; ++x) {
                            if (pos < (int)sizeof(expr) - 1) expr[pos++] = term_buf[y][x];
                        }
                    }
                    expr[pos] = '\0';
                    bool ok; double res = calc_evaluate(expr, &ok);
                    if (ok) {
                        char buf[32];
                        snprintf(buf, sizeof(buf), "\n= %.6g\n", res);
                        term_puts(buf);
                    } else {
                        term_puts("\n[error]\n");
                    }
                    draw_terminal();
                    play_beep();
                }
                if (touchPenDown()) {
                    touchPosition tp; touchRead(&tp);
                    int k = keyboardTouchToKey(tp.px, tp.py);
                    if (k >= 0) {
                        char ch = keyboardKeyChar(k);
                        term_buf[cur_y][cur_x] = ch;
                        cur_x++; if (cur_x >= TERM_W) { cur_x = 0; cur_y = (cur_y + 1) % TERM_H; }
                        draw_terminal();
                        keyboardDraw();
                        play_beep();
                    }
                }
                break;
            case STATE_BOOK:
                bv_show_file("book.txt");
                memset(term_buf, 0, sizeof(term_buf)); cur_x = cur_y = 0; draw_terminal();
                state = STATE_TERMINAL;
                break;
            case STATE_MAP:
                mv_show_map("map01.png");
                memset(term_buf, 0, sizeof(term_buf)); cur_x = cur_y = 0; draw_terminal();
                state = STATE_TERMINAL;
                break;
            default:
                break;
        }
        starfield_update();
        swiWaitForVBlank();
    }
    return 0;
}
