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

    print("Reading from TEST.TXT...\n");
    char *x;
    FILE f = fopen("TEST.TXT", false);
    fread(f, x);
    print(x);
    print("\nDone Reading!\n");

    print("\nWriting to JEFF.TXT...");
    char *jeff = "Hello, ma name is jeff!";
    FILE jef = fopen("JEFF.TXT", true);
    fwrite(jef, jeff, strlen(jeff));
    print("\nDone Writing.\n");

    print("Reading JEFF.TXT...\n");
    char *tmp;
    fread(jef, tmp);
    print(tmp);
    print("\nDone!");
    return;
}