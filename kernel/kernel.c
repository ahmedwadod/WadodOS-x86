#include "../include/screen.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/stdtime.h"
#include "../include/string.h"
#include "../include/device.h"
#include "../include/fs.h"

extern void kmain()
{
    init_cpu();
    init_keyboard();
    kset_display(31);

    printf("Start time: ");
    print_datetime(datetime_now());
    printf("\n\n");

    init_devices();
    
    device_t* fd0 = get_sys_device(2);
    if(fd0->ID == 0)
    {
        printf("Error");
    }
    else
    {
        printf("\nDevice ID: %d\nDisk Name: %s\n", fd0->ID, fd0->name);
        init_fs();
    }
}