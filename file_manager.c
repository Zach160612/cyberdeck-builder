#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <string.h>
#include "file_manager.h"

extern char term_buf[TERM_H][TERM_W];
extern int cur_x, cur_y;

static void term_puts(const char *s) {
    // Write string to terminal buffer, handling line wrap.
    while (*s) {
        if (*s == '\n') {
            cur_x = 0;
            cur_y = (cur_y + 1) % TERM_H;
        } else {
            term_buf[cur_y][cur_x] = *s;
            cur_x++;
            if (cur_x >= TERM_W) {
                cur_x = 0;
                cur_y = (cur_y + 1) % TERM_H;
            }
        }
        s++;
    }
}

void fm_list_root(void) {
    // Clear terminal first
    memset(term_buf, 0, sizeof(term_buf));
    cur_x = cur_y = 0;
    term_puts("--- Root directory ---\n");

    DIR *d = opendir("/\");
    if (!d) {
        term_puts("Failed to open root!\n");
        return;
    }
    struct dirent *de;
    while ((de = readdir(d)) != NULL) {
        term_puts(de->d_name);
        term_puts("\n");
    }
    closedir(d);
    term_puts("--- End ---\n");
}
