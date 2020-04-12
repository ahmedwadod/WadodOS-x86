#include "../include/screen.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/stdtime.h"
#include "../include/fat12.h"
#include "../include/string.h"

extern void kmain()
{
    init_cpu();
    init_keyboard();
    kprint("Checking floppy...");
    init_fat12();
    kset_display(31);

    printf("Start time: ");
    print_datetime(datetime_now());
    printf("\n\n");

    /* Reading file */
    printf("Reading from disk...\n");
    FILE f = fopen("TEST.TXT", false);
    char *cont;
    kmalloc(cont, 1024);
    fread(f, cont);
    printf("Content of %s is:\n%s", f.filename, cont);

    /* Creating file and writing to it */
    printf("\n\nCreating file WADOD.TXT... ");
    FILE wadod = fopen("WADOD.TXT", true);
    printf("Done!\nWriting to %s... ", wadod.filename);
    char *data = "Hi, I'm Ahmed Abdelwadod\nCreator of WadodOS-x86.";
    fwrite(wadod, data, strlen(data));
    printf("Done!");
}