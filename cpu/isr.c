#include "../include/isr.h"
#include "../include/screen.h"
#include "../include/stdlib.h"

ISR interrupt_handlers[256];

/* Can't do this with a loop because we need the address
 * of the function names */
void isr_install() {
    set_idt_gate(0, (uint_32)isr0);
    set_idt_gate(1, (uint_32)isr1);
    set_idt_gate(2, (uint_32)isr2);
    set_idt_gate(3, (uint_32)isr3);
    set_idt_gate(4, (uint_32)isr4);
    set_idt_gate(5, (uint_32)isr5);
    set_idt_gate(6, (uint_32)isr6);
    set_idt_gate(7, (uint_32)isr7);
    set_idt_gate(8, (uint_32)isr8);
    set_idt_gate(9, (uint_32)isr9);
    set_idt_gate(10, (uint_32)isr10);
    set_idt_gate(11, (uint_32)isr11);
    set_idt_gate(12, (uint_32)isr12);
    set_idt_gate(13, (uint_32)isr13);
    set_idt_gate(14, (uint_32)isr14);
    set_idt_gate(15, (uint_32)isr15);
    set_idt_gate(16, (uint_32)isr16);
    set_idt_gate(17, (uint_32)isr17);
    set_idt_gate(18, (uint_32)isr18);
    set_idt_gate(19, (uint_32)isr19);
    set_idt_gate(20, (uint_32)isr20);
    set_idt_gate(21, (uint_32)isr21);
    set_idt_gate(22, (uint_32)isr22);
    set_idt_gate(23, (uint_32)isr23);
    set_idt_gate(24, (uint_32)isr24);
    set_idt_gate(25, (uint_32)isr25);
    set_idt_gate(26, (uint_32)isr26);
    set_idt_gate(27, (uint_32)isr27);
    set_idt_gate(28, (uint_32)isr28);
    set_idt_gate(29, (uint_32)isr29);
    set_idt_gate(30, (uint_32)isr30);
    set_idt_gate(31, (uint_32)isr31);

    // SysCall ISR
    set_idt_gate(80, (uint_32)isr80);

    // Remap the PIC
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0); 

    // Install the IRQs
    set_idt_gate(32, (uint_32)irq0);
    set_idt_gate(33, (uint_32)irq1);
    set_idt_gate(34, (uint_32)irq2);
    set_idt_gate(35, (uint_32)irq3);
    set_idt_gate(36, (uint_32)irq4);
    set_idt_gate(37, (uint_32)irq5);
    set_idt_gate(38, (uint_32)irq6);
    set_idt_gate(39, (uint_32)irq7);
    set_idt_gate(40, (uint_32)irq8);
    set_idt_gate(41, (uint_32)irq9);
    set_idt_gate(42, (uint_32)irq10);
    set_idt_gate(43, (uint_32)irq11);
    set_idt_gate(44, (uint_32)irq12);
    set_idt_gate(45, (uint_32)irq13);
    set_idt_gate(46, (uint_32)irq14);
    set_idt_gate(47, (uint_32)irq15);

    set_idt(); // Load with ASM

    asm volatile("sti"); // Load IRQs
}

/* To print the message which defines every exception */
char *exception_messages(int no){

    switch (no)
    {
    case 0:
        return "Division By Zero";
        break;
    case 1:
        return "Debug";
        break;
    case 2:
        return "Non Maskable Interrupt";
        break;
    case 3:
        return "Breakpoint";
        break;
    case 4:
        return "Into Detected Overflow";
        break;
    case 5:
        return "Out of Bounds";
        break;
    case 6:
        return "Invalid Opcode";
        break;
    case 7:
        return "No Coprocessor";
        break;
    case 8:
        return "Double Fault";
        break;
    case 9:
        return "Coprocessor Segment Overrun";
        break;
    case 10:
        return "Bad TSS";
        break;
    case 11:
        return "Segment Not Present";
        break;
    case 12:
        return "Stack Fault";
        break;
    case 13:
        return "General Protection Fault";
        break;
    case 14:
        return "Page Fault";
        break;
    case 15:
        return "Unknown Interrupt";
        break;
    case 16:
        return "Coprocessor Fault";
        break;
    case 17:
        return "Alignment Check";
        break;
    case 18:
        return "Machine Check";
        break;
    case 19:
        return "KERNEL PANIC";
        break;
    
    default:
        return "Reserved";
        break;
    }
};

bool hang = false;
void isr_handler(registers_t r) {
    if(hang)
        return;
    kprint_at_with_attr("CPU Error: ", -1, -1, RED_ON_WHITE);
    char s[3];
    itoa(r.int_no, s, 10);
    kprint_at_with_attr(s, -1, -1, RED_ON_WHITE);
    kprint_at_with_attr(" :", -1, -1, RED_ON_WHITE);
    char* exp = exception_messages(r.int_no);
    kprint_at_with_attr(exp, -1, -1, RED_ON_WHITE);
    hang = true;
}

void register_interrupt_handler(uchar_8 n, ISR handler) {
    interrupt_handlers[n] = handler;
}

void unregister_interrupt_handler(uchar_8 n)
{
    interrupt_handlers[n] = 0; // 0 means no handler
}

void irq_handler(registers_t r) {
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    if (r.int_no >= 40) port_byte_out(0xA0, 0x20); /* slave */
    port_byte_out(0x20, 0x20); /* master */

    /* Handle the interrupt in a more modular way */
    if (interrupt_handlers[r.int_no] != 0) {
        ISR handler = interrupt_handlers[r.int_no];
        handler(r);
    }
}

void myisr_handler(registers_t r){
    port_byte_out(0xA0, 0x20); /* slave */
    port_byte_out(0x20, 0x20); /* master */
    
    if (interrupt_handlers[r.int_no] != 0) {
        ISR handler = interrupt_handlers[r.int_no];
        handler(r);
    }
}
