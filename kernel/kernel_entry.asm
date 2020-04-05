jmp short switch_to_pm
nop

    %include "kernel/gdt.asm"
    %include "kernel/switch_to_pm.asm"
; Calls the kmain function
[BITS 32]
[EXTERN kmain]

kentry:
    call kmain

    jmp $