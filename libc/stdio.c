#include "../include/screen.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/stdarg.h"
#include "../include/string.h"

/* Clear screen */
void clear_screen()
{
    kclear_screen();
}

void putchar(char c)
{
    kprint_char(c, -1, -1, 0);
}

void puts(char *str)
{
    kprint(str);
}

char* _convert(int value, int base)
{
    char *str = 0;
    itoa(value, str, base);
    return str;
}
void printf(char * format, ...)
{
    char *traverse = format; 
	uint_32 i = 0; 
	char *s = ""; 
	
	//Module 1: Initializing Myprintf's arguments 
	va_list arg; 
	va_start(arg, format); 
	
	for(traverse; *traverse != '\0'; traverse++) 
	{ 
		while( *traverse != '%' && *traverse != '\0') 
		{ 
			putchar(*traverse);
			traverse++; 
		} 

        if(*traverse == '\0')
            break;
		
		traverse++; 
		
		//Module 2: Fetching and executing arguments
		switch(*traverse) 
		{ 
			case 'c' : i = va_arg(arg,int);		//Fetch char argument
						putchar(i);
						break; 
						
			case 'd' : i = va_arg(arg,int); 		//Fetch Decimal/Integer argument
						if(i<0) 
						{ 
							i = -i;
							putchar('-'); 
						} 
						puts(_convert(i,10));
						break; 
						
			case 'o': i = va_arg(arg,uint_32); //Fetch Octal representation
						puts(_convert(i,8));
						break; 
						
			case 's': s = va_arg(arg,char *); 		//Fetch string
						puts(s); 
						break; 
						
			case 'x': i = va_arg(arg,uint_32); //Fetch Hexadecimal representation
						puts(_convert(i,16));
						break; 
            case 'b': i = va_arg(arg,uint_32);
                        puts(_convert(i, 2));
                        break;
		}	
	} 
	
	//Module 3: Closing argument list to necessary clean-up
	va_end(arg); 
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
            outptr[0] = letter;
            strncat(inputBuffer, outptr, 1);
            kprint_char(letter, -1, -1, 0);
        }
    }
}
void _enter()
{
    if(hang)
    {
        if(_inMode == INPUT_MODE_ONE_LINE)
        {
            memcpy(inputBuffer, outptr, strlen(inputBuffer));
            *inputBuffer = 0;
            hang =  false;
            kprint("\n");
        }
        else if( _inMode == INPUT_MODE_ONE_CHAR )
        {
            outptr[0] = '\n';
            hang = false;
        }
        else
        {
            strncat(inputBuffer, "\n", 1);
            kprint("\n");
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
            inputBuffer[strlen(inputBuffer) - 1] = 0;
            kprint("\b \b");
        }
    }
}

void gets(char* instr)
{
    _inMode = INPUT_MODE_ONE_LINE;   
    hang = true;
    while (hang)
    {    }
    memcpy(outptr, instr, strlen(outptr));
}

char getchar(bool handled)
{
    _inMode = INPUT_MODE_ONE_CHAR;
    hang = true;
    while (hang)
    {    }
    if(!handled)
        kprint_char(outptr[0], -1, -1, 0);
    return outptr[0];
}

void init_keyboard()
{
    keyboard_start_listening(_keyPress, _enter, _specialchar, _backspace);
}
