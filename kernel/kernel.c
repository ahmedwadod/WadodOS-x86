#include "../cpu/types.h"
#include "../cpu/isr.h"
#include "../libc/stdio.h"
#include "../libc/stdlib.h"

#include "../libc/files.h"

typedef void(*PROG)();

extern void kmain();
void handle_syscall(registers_t r);

void kmain()
{
    setup_cpu();
    register_interrupt_handler(80, handle_syscall);
    setup_display(GRAY_FONT_BLACK_BG);
    setup_keyboard();
    print("Loading Disk...");
    init_fat12(FLOPPY_DRIVE1);
    clear_screen();

    FILE shell = fopen("SHELL.BIN", false);
    if(shell.name[0] == 0)
    {
        print("SHELL.BIN not found!");
    }
    else
    {
        char *buffer = (char*) 0xED000;
        fread(shell, buffer);
        print("Loaded");
        PROG sh = 0xED000;
        sh();
    }

    return;
}


void _return(int val)
{
    asm ("mov %0, %%eax;"
     :                  /* output */
     :"a"(val)         /* input */
     );
}

/**
 * Handle system calls on interrupt 80.
 * Gets function on: eax
 * Arguments in order: ebx, ecx, edx, esi, edi
 * Return in: eax
*/
void handle_syscall(registers_t r)
{
    __asm__("sti");
    char c;
    char *sbuff;
    
    switch (r.eax)
    {
    case 0x00: // sys_setup_display(char attr)
        setup_display(r.ebx);
        break;
    case 0x01: // sys_cls()
        clear_screen();
        break;
    case 0x02: // sys_putchar(char c);
        kprint_char(r.ebx, -1, -1, *CURRENT_STYLE);
        break;
    case 0x03: // sys_putchar_f(char c, int col, int row, char attr)
        kprint_char(r.ebx, r.ecx, r.edx, r.esi);
        break;
    case 0x04: // sys_print(int stringmemaddr)
        sbuff = (char*)r.ebx;
        print(sbuff);
        break;
    case 0x05: // sys_print_f(int stringmemaddr, int col, int row, char attr)
        sbuff = (char*) r.ebx;
        kprint_at_with_attr(sbuff, r.ecx, r.edx, r.esi);
        break;
    case 0x06: // sys_getchar(bool handled)
        c = getChar(r.ebx);
        _return(c);
        break;
    case 0x07: // sys_gets(char* str)
        sbuff = (char*)r.ebx;
        input(sbuff);
    default:
        break;
    }
    __asm__("cli");
}