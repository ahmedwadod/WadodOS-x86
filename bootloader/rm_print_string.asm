; Write all string in SI
rm_print_string:
    pusha
    mov ah, 0x0E
.rm_print_string_repeat:
    cmp [di], byte 0
    je .rm_print_string_done
    mov al, [di]
    int 0x10
    inc di
    jmp .rm_print_string_repeat
.rm_print_string_done:
    popa
    ret
