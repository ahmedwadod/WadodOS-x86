C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o} 
OBJBIN = ${OBJ:%.o=bin/%.o}

# Change this if your cross-compiler is somewhere else
CC = gcc

# Flags for GCC
CFLAGS = -fno-pie -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs

# First rule is run by default
all: bin/bootloader.bin bin/kernel.bin
	# Compiled Successfully

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
bin/kernel.bin: bin/kernel/kernel_entry.o ${OBJ} cpu/interrupt.o
	ld -o $@ -m elf_i386 -Ttext 0x1000 bin/kernel/kernel_entry.o ${OBJBIN} bin/cpu/interrupt.o --oformat binary

bin/kernel/kernel_entry.o:
	nasm -f elf32 kernel/kernel_entry.asm -o $@

cpu/interrupt.o:
	nasm -f elf32 cpu/interrupt.asm -o bin/cpu/interrupt.o

bin/bootloader.bin:
	nasm -f bin bootloader/main.asm -o $@

build: clean all
	mkfs.msdos -C outputs/wadodos.flp 1440
	dd status=noxfer conv=notrunc if=bin/bootloader.bin of=outputs/wadodos.flp
	rm -rf tmp-flp || true
	mkdir tmp-flp
	mount -o loop -t vfat outputs/wadodos.flp tmp-flp
	cp bin/kernel.bin tmp-flp/
	sleep 0.4
	umount tmp-flp/
	# Build Successfully

run: build
	qemu-system-i386 -fda outputs/wadodos.flp

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o bin/$@

%.bin: %.asm
	nasm $< -f bin -o bin/$@

clean:
	rm outputs/*.* || true
	rm bin/*.* || true
	rm bin/cpu/*.* bin/drivers/*.* bin/kernel/*.* bin/libc/*.* || true
