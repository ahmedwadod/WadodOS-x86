; Calls the kmain function
[BITS 32]
[EXTERN setup_cpu]
[EXTERN kmain]

kentry:
    call kmain

    jmp $