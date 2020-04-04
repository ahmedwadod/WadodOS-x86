#include "../cpu/types.h"
#include "../cpu/isr.h"
#include "../libc/stdio.h"
#include "../libc/stdlib.h"


extern void kmain();

void kmain()
{
    setup_cpu();
    setup_display(GRAY_FONT_BLACK_BG);
    setup_keyboard();

    clear_screen();
    print("Hello there!\nWhat is your name? ");
    char* name;
    input(name);
    print("Hmmm... ");
    sleep(1000);
    print("Hello, ");
    print(name);
    return 0;
}