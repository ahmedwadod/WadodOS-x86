#ifndef _SCREEN_H
#define _SCREEN_H

#include "stdint.h"

#define VIDEO_ADDRESS (uchar_8*) 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4

/* Screen i/o ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

static uchar_8 CURRENT_DISPLAY_STYLE = WHITE_ON_BLACK;

/* Public kernel API */
void kset_display(uchar_8 attr);
void kclear_screen();
int kprint_char(char c, int col, int row, int attr);
void kprint(char *message);
void kprint_at_with_attr(char* message, int col, int row, int attr);

#endif