; Gets:
; DL: Drive No.
; DH: No. of sectors
; BX: Starting memory address to load to. ES:BX (ES = 0x0000)
; --------------------
; Segmentation note:
; ES:BX -> Adds one address to left:
; ES=0xA000
; BX=0xFFFF
; ES:BX=0xAFFFF
; --------------------

disk_load:
    push dx

    mov ah, 0x02    ; read disk function
    mov al, dh      ; No. of sectors
    mov ch, 0x00    ; No. of cylinder
    mov dh, 0x01    ; No. head
    mov cl, 17      ; Start sector (first file in FAT 12)

    int 0x13        ; BIOS Interrupt

    jc disk_error   ; Jump if error

    pop dx
    cmp dh, al      ; AL: No. read actually sectors (returned by interrupt)
    jne disk_error  ; Jump if not equal to wanted No. of Sectors

    ret             ; Return if all is good

disk_error:
    mov si, DISK_ERROR_MSG
    call rm_print_string
    jmp $

DISK_ERROR_MSG db "Disk read error!!", 0