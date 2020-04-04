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

    print("Loading...");
    init_fat12(FLOPPY_DRIVE1);
    clear_screen();

    FILE myf = fopen("JEFF.TXT", true);
    fwrite(myf, "MY Name IS JEFFFFF!!!", 22);
    print("Done");
    
    return 0;
}