#include "../cpu/types.h"
#include "../cpu/isr.h"
#include "../libc/stdio.h"
#include "../libc/stdlib.h"

#include "../drivers/floppy.h"


extern void kmain();

void kmain()
{
    setup_cpu();
    setup_display(GRAY_FONT_BLACK_BG);
    setup_keyboard();

    print("Loading...");
    setup_floppy();
    reset_floppy_controller(FLOPPY_DRIVE1);
    clear_screen();

    /* Read */
    char c, h, s;
    lba_2_chs(75, &c, &h, &s);
    char *content;
    kmalloc(content, 100);
    print("Reading from floppy disk...\n");
    floppy_read(content, FLOPPY_DRIVE1, c, h, s, 50);
    print(content);
    print("\n");

    /* Write */
    char* mystr = "\nMy Name is JEEEFFFF!!";
    combine(content, mystr);
    print("Writing to disk... ");
    floppy_write(content, FLOPPY_DRIVE1,c, h, s, 80);
    print("Done!");
    
    return 0;
}