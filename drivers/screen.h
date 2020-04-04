#ifndef _SCREEN_H
#define _SCREEN_H

#include "../cpu/types.h"
#include "../cpu/ports.h"
#include "../libc/stdlib.h"

#define VIDEO_ADDRESS (uchar_8*) 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4

/* Screen i/o ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

/* Public kernel API */
void kclear_screen();
void kprint_at(char* message, int col, int row);
void kprint_at_with_attr(char* message, int col, int row, int attr);
void kprint(char *message);
int kprint_char(char c, int col, int row, int attr);
void kclear_screen_with_attr(int attr);
#endif