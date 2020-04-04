# WadodOS-x86
This is a hobby OSdev project made with assembly and C. I made this project just to have fun and learn about these low-level cool stuff, I hope you will make use of it.

## Resources
Throughout the making of this project I made use of some resources I'll be linking it here:
- [OSdev Wiki](https://wiki.osdev.org/Expanded_Main_Page)
    Our beloved wiki for hobbyists arount the globe
- [Writing a Simple Operating System - From Scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf)
    A book draft by Nick Blundell, unfortunately this is not a compeleted book, the author descided not to complete the book but it's still a great place to get started.
- [os-tutorial](https://github.com/cfenollosa/os-tutorial)
    It's a great repository with examples by @cfenollosa
    It doesn't explain things in details (Just puts links to it), but it's a great source for good clean code.
## How it Works
This is an x86 Operating System written in assembly and C. It works on a floppy disk in FAT 12 format.
It constructs of the following components:
- **Bootloader**
    It's written in Assembly and all it does is switching from 16 bits to 32 bits and then load the kernel from the floppy disk to the memory.
- **Kernel**
    It's the core of the operating system and has all the rock and roll stuff. And it consist of:
    - Kernel entry: Loads the `kmain` function.
    - Kernel: The OS itself
    - Drivers: All the basic drivers for the OS
    - CPU: All files needed to deal with the CPU stuff (IDT, ISR)
    - libc: My librarys (stdio, stdlib, etc...)

## Roadmap
The work on this project isn't done and here is the roadmao and milestones:

[ ✓ ] Switch to Protected Mode and load kernel from disk

[ ✓ ] Add IDT and ISR

[ ✓ ] Add timer driver and sleep functionality

[ ✓ ] Add display driver

[ ✓ ] Add stdlib and stdio

[ ✓ ] Add keyboard driver and input functionality

[ ✓ ] Add floppy disk driver

[ ✓ ] Add files.h library

[ X ] Add syscalls

[ X ] Load shell from disk and execute it

## Usage
#### Build from source code:
You can build an image from the source code in **Linux**:
1. Make sure you have [GCC](gcc.gnu.org) and [NASM](https://www.nasm.us/) installed
2. run this command to build an image
    ```
    sudo make build
    ```
    it will have the image in **_outputs_** directory
3. run with [qemu](www.qemu.org) using the following command:
    ```
    sudo make run
    ```
    
_P.S. It needs sudo permission because it uses _mount_ command_

#### Run directly:
Grab the **_wadodos.flp_** from the **_disk-images_** folder and run it as a floppy disk from any Virtual Machine.
