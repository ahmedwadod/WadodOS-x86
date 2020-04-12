#ifndef _KEYBOARD_H
#define _KEYBOARD_H

typedef void (*KB_OnKeyPress)(char);
typedef void (*KB_OnEnterCallback)();
typedef void (*KB_OnSpecialCharCallback)(char);
typedef void (*KB_OnBackspaceCallback)();

void keyboard_start_listening(KB_OnKeyPress onKeyPress, KB_OnEnterCallback onEnter, KB_OnSpecialCharCallback onSpcialChar, KB_OnBackspaceCallback onBackspace);
void keyboard_stop_listening();

#endif
