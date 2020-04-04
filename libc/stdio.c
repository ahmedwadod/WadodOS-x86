#include "stdio.h"

/* Clear screen */
void clear_screen()
{
    kclear_screen_with_attr(CURRENT_STYLE);
}

/* Set display style and clears the screen */
void setup_display(enum DISPLAY_STYLES style)
{
    CURRENT_STYLE = style;
    clear_screen();
}

void print(char* str)
{
    kprint_at_with_attr(str, -1, -1, CURRENT_STYLE);
}

void print_backspace()
{
    kprint_at_with_attr("\b \b", -1, -1, CURRENT_STYLE);
}

// Print the value of a number in ascii
void print_int(int number)
{
    char* str;
    int_to_ascii(number, str);
    print(str);
}

// Print with red on white
void print_error(char* errormsg, int errorcode)
{
    char *codestr;
    int_to_ascii(errorcode, codestr);
    kprint_at_with_attr("Error Code: ", -1, -1, RED_ON_WHITE);
    kprint_at_with_attr(codestr, -1, -1, RED_ON_WHITE);
    kprint_at_with_attr("\nError Message: ", -1, -1, RED_ON_WHITE);
    kprint_at_with_attr(errormsg, -1, -1, RED_ON_WHITE);
    print("\n");
}

/// Input Section ///
static bool hang = false;
void _keyPress(char letter)
{
    if(hang)
    {
        char str[2] = {letter, '\0'};
        print(str);
    }
}
static char *outptr;
void _enter(char* inputstr)
{
    if(hang)
    {
        memory_copy(inputstr, outptr, strlen(inputstr));
        hang =  0;
        print("\n");
    }
}
void _specialchar(char c)
{
    return;
}
void _backspace()
{
    if(hang)
        print_backspace();
}

void input(char* instr)
{
    
    hang = true;
    while (hang)
    {    }
    memory_copy(outptr, instr, strlen(outptr));
}

void setup_keyboard()
{
    keyboard_start_listening(_keyPress, _enter, _specialchar, _backspace);
}
