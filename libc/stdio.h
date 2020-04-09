#ifndef _STDIO_H
#define _STDIO_H

#include "../cpu/types.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

enum DISPLAY_STYLES
{
    WHITE_FONT_BLACK_BG = 0x0F,
    GRAY_FONT_BLACK_BG = 0x07,
    GREEN_FONT_BLACK_BG = 0X02,
    BLUE_FONT_BLACK_BG = 0X01,
    WHITE_FONT_BLUE_BG = 0X1F
};

typedef enum
{
    INPUT_MODE_ONE_CHAR = 0,
    INPUT_MODE_ONE_LINE = 1,
    INPUT_MODE_MULTILINE = 2
} InputMode;

static int CURRENT_STYLE = 0x0F; // Default WHITE_FONT_BLACK_BG

void clear_screen();
void setup_display(enum DISPLAY_STYLES style);
void setup_keyboard();
void print(char* str);
void print_backspace();
void print_int(int number);
void print_error(char* errormsg, int errorcode);
void input(char* instr);
char getChar(bool handled);
void inputMultiLine(char* instr);

#endif