#include "stdio.h"

/* Clear screen */
void clear_screen()
{
    kclear_screen_with_attr(*CURRENT_STYLE);
}

/* Set display style and clears the screen */
void setup_display(enum DISPLAY_STYLES style)
{
    *CURRENT_STYLE = style;
    clear_screen();
}

void print(char* str)
{
    kprint_at_with_attr(str, -1, -1, *CURRENT_STYLE);
}

void print_backspace()
{
    kprint_at_with_attr("\b \b", -1, -1, *CURRENT_STYLE);
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
static InputMode _inMode = 0;
static char inputBuffer[1024];
static char *outptr;

void _keyPress(char letter)
{
    if(hang)
    {
        if(_inMode == INPUT_MODE_ONE_CHAR)
        {
            outptr[0] = letter;
            hang = false;
        }
        else
        {
            appendchar(inputBuffer, letter);
            kprint_char(letter, -1, -1, *CURRENT_STYLE);
        }
    }
}
void _enter()
{
    if(hang)
    {
        if(_inMode == INPUT_MODE_ONE_LINE)
        {
            memory_copy(inputBuffer, outptr, strlen(inputBuffer));
            *inputBuffer = 0;
            hang =  false;
            print("\n");
        }
        else if( _inMode == INPUT_MODE_ONE_CHAR )
        {
            outptr[0] = '\n';
            hang = false;
        }
        else
        {
            appendchar(inputBuffer, '\n');
            print("\n");
        }
    }
}
void _specialchar(char c)
{
    return;
}
void _backspace()
{
    if(hang)
    {
        if(inputBuffer[0] != 0)
        {
            droplastchar(inputBuffer);
            print_backspace();
        }
    }
}

void input(char* instr)
{
    _inMode = INPUT_MODE_ONE_LINE;   
    hang = true;
    while (hang)
    {    }
    memory_copy(outptr, instr, strlen(outptr));
}

char getChar(bool handled)
{
    _inMode = INPUT_MODE_ONE_CHAR;
    hang = true;
    while (hang)
    {    }
    if(!handled)
        kprint_char(outptr[0], -1, -1, *CURRENT_STYLE);
    return outptr[0];
}

void inputMultiLine(char* instr)
{
    _inMode = INPUT_MODE_MULTILINE;
    hang = true;
    while (hang)
    {    }
    memory_copy(outptr, instr, strlen(outptr));
}

void setup_keyboard()
{
    keyboard_start_listening(_keyPress, _enter, _specialchar, _backspace);
}
