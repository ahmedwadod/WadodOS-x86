#include "keyboard.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C
#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define LEFT_SHIFT_RELEASE 0xAA
#define RIGHT_SHIFT_RELEASE 0xB6
#define CAPSLOCK 0x3A

#define SC_MAX 57
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y', 
        'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g', 
        'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v', 
        'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '};

const char sc_ascii_shift[] = { '?', '?', '!', '@', '#', '$', '%', '^',     
    '&', '*', '(', ')', '_', '+', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ':', '\"', '~', '?', '|', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', '<', '>', '?', '?', '?', '?', ' '};


static KB_OnKeyPress keypress_callback;
static KB_OnEnterCallback enter_callback;
static KB_OnSpecialCharCallback specialchar_callback;
static KB_OnBackspaceCallback backspace_callback;

static bool isShift = false;

static void keyboard_callback(registers_t regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    uchar_8 scancode = port_byte_in(0x60);
    
    if (scancode > SC_MAX && scancode != LEFT_SHIFT_RELEASE && scancode != RIGHT_SHIFT_RELEASE && scancode != CAPSLOCK) {
        specialchar_callback(scancode);
        return;
    };

    if (scancode == BACKSPACE) {
        backspace_callback();
    } else if (scancode == ENTER) {
        enter_callback(); 
    } else if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT)
    {
        isShift = true;
    } else if(scancode == LEFT_SHIFT_RELEASE || scancode == RIGHT_SHIFT_RELEASE)
    {
        isShift = false;
    }
    else {
        char letter; 
        if(isShift)
            letter = sc_ascii_shift[(int)scancode];
        else
        {
            letter = sc_ascii[(int)scancode];
        }
        keypress_callback(letter);
    }
}

void keyboard_start_listening(KB_OnKeyPress onKeyPress, KB_OnEnterCallback onEnter, KB_OnSpecialCharCallback onSpcialChar, KB_OnBackspaceCallback onBackspace){
    keypress_callback = onKeyPress;
    enter_callback = onEnter;
    specialchar_callback = onSpcialChar;
    backspace_callback = onBackspace;
    register_interrupt_handler(IRQ1, keyboard_callback); 
}

void keyboard_stop_listening()
{
    unregister_interrupt_handler(IRQ1);
}
