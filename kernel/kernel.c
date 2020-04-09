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
    char *testContent;
    FILE f1 = fopen("TEST.TXT", false);
    fread(f1, testContent);
    print(testContent);
    print("\nDone Reading!\n");

    print("\nWriting to TEST2.TXT...");
    char *txt = "Hello,\nThis is the TEST2.TXT file!";
    FILE f2 = fopen("TEST2.TXT", true);
    fwrite(f2, txt, strlen(txt));
    print("\nDone Writing.\n");

    print("\nReading TEST2.TXT...\n");
    char *test2Content;
    fread(f2, test2Content);
    print(test2Content);
    print("\nDone!");
    return;
}