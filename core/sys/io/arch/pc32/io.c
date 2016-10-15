#include <sys/io/arch/pc32/io.h>
#include <mem/arch/pc32/mem_arch.h>
#include <sys/sys.h>

// Init PIC1 and PIC2 (Programmable Interrupt Controllers)
void init_pics()
{
    unsigned char pic1 = PIC1_IRQ_BASE;
    unsigned char pic2 = PIC2_IRQ_BASE;
    
    /* send ICW1 */
    outportb(PIC1, ICW1);
    outportb(PIC2, ICW1);
    
    /* send ICW2 */
    outportb(PIC1 + 1, pic1);   /* remap */
    outportb(PIC2 + 1, pic2);   /*  pics */
    
    /* send ICW3 */
    outportb(PIC1 + 1, 4);  /* IRQ2 -> connection to slave */
    outportb(PIC2 + 1, 2);
    
    /* send ICW4 */
    outportb(PIC1 + 1, ICW4);
    outportb(PIC2 + 1, ICW4);
    
    /* disable all IRQs */
    outportb(PIC1 + 1, 0b11111111);
}

void bad_thing()
{
    core_fatal("Error interrupt received: System Crash!");
}

// TODO: implemente interrupts as GCC doc demonstrates
__attribute__((interrupt)) void bad_thing_isr()
{
    asm(
        "pushal;"
        "calll bad_thing;"
        "popal;"
        "iretl;"
        );
}

void init_cpu_ints()
{
    // 2 (NMI) & 4 (Overflow) ignored
    
    // Divide Error
    set_isr(bad_thing_isr, 0);
    // BOUND range exceeded
    set_isr(bad_thing_isr, 5);
    // Invalid Opcode
    set_isr(bad_thing_isr, 6);
    // Double fault
    set_isr(bad_thing_isr, 8);
    // Invalid TSS
    set_isr(bad_thing_isr, 10);
    // Segment Not Present
    set_isr(bad_thing_isr, 11);
    // Stack-Segment Fault
    set_isr(bad_thing_isr, 12);
    // General Protection Fault
    set_isr(bad_thing_isr, 13);
    // Page Fault
    set_isr(bad_thing_isr, 14);
    // Floating-Point Exception
    set_isr(bad_thing_isr, 16);
    // Alignment Check
    set_isr(bad_thing_isr, 17);
    // Machine Check
    set_isr(bad_thing_isr, 18);
}

void io_init()
{
    init_cpu_ints();
    init_pics();
}
