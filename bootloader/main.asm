[ORG 0x7c00]

jmp short start
nop
; ------------------------------------------------------------------
; Disk description table, to make it a valid floppy
; Note: some of these values are hard-coded in the source!
; Values are those used by IBM for 1.44 MB, 3.5" diskette

OEMLabel		db "WADODOS "	; Disk label
BytesPerSector		dw 512		; Bytes per sector
SectorsPerCluster	db 1		; Sectors per cluster
ReservedForBoot		dw 1		; Reserved sectors for boot record
NumberOfFats		db 2		; Number of copies of the FAT
RootDirEntries		dw 224		; Number of entries in root dir
					; (224 * 32 = 7168 = 14 sectors to read)
LogicalSectors		dw 2880		; Number of logical sectors
MediumByte		db 0F0h		; Medium descriptor byte
SectorsPerFat		dw 9		; Sectors per FAT
SectorsPerTrack		dw 18		; Sectors per track (36/cylinder)
Sides			dw 2		; Number of sides/heads
HiddenSectors		dd 0		; Number of hidden sectors
LargeSectors		dd 0		; Number of LBA sectors
DriveNo			dw 0		; Drive No: 0
Signature		db 41		; Drive signature: 41 for floppy
VolumeID		dd 00000000h	; Volume ID: any number
VolumeLabel		db "WADODOS    "; Volume Label: any 11 chars
FileSystem		db "FAT12   "	; File system type: don't change!



KERNEL_OFFSET equ 0x1000 ; This is the memory address where we're going to load our kernel

start:
    ; Getting drive number
    mov [BOOT_DRIVE], dl

    ; Setting stack
    mov bp, 0x9000
    mov sp, bp

    ; Clear screen in RM
    call clear_screen

    ; Load kernel to memory
    call load_kernel

    ; Switch to protected mode
    call switch_to_pm           ; The begining of this file should not be with [BITS 16]

    jmp $

    %include "bootloader/disk_load.asm"
    %include "bootloader/gdt.asm"
    %include "bootloader/rm_print_string.asm"
    %include "bootloader/rm_clear_screen.asm"
    %include "bootloader/switch_to_pm.asm"

[BITS 16]
load_kernel:
    ; Loading kernel from disk to memory
    mov bx, KERNEL_OFFSET
    mov dh, 36
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[BITS 32]
BEGIN_PM:
    call KERNEL_OFFSET

    jmp $

; Data
BOOT_DRIVE db 0

; Fill up boot sector
times 510-($-$$) db 0
dw 0xAA55