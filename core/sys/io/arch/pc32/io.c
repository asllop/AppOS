#include <sys/io/arch/pc32/io.h>

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

void io_init()
{
    init_pics();
}
