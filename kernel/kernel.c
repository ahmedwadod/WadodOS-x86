#include "../cpu/types.h"
#include "../cpu/isr.h"
#include "../libc/stdio.h"
#include "../libc/stdlib.h"

#include "../libc/files.h"


extern void kmain();

void kmain()
{
    setup_cpu();
    setup_display(GRAY_FONT_BLACK_BG);
    setup_keyboard();

    print("Loading Disk...");
    init_fat12(FLOPPY_DRIVE1);
    clear_screen();

    // print_int(_BUFFADDR);
    // return;

    print("Reading file..\n");
    FILE test = fopen("K4.BIN", false);

    char *x = (char*)500000;
    kmalloc(x, 0x7000);
    fread(test, x);
    print("Done.");
    
    return;
}