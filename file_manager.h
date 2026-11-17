#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <nds.h>
#include <fat.h>
#include <stdio.h>

// Simple file‑manager that prints the entries of a directory to the terminal buffer.
// It uses the global terminal buffer defined in main.c.
extern void fm_list_root(void);

#endif // FILE_MANAGER_H
