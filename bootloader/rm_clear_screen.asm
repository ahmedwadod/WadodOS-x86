[BITS 16]
clear_screen:
    mov ah, 0x0
    mov al, 0x3
    int 0x10
    ret