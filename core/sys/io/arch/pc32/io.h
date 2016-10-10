#ifndef IO_H
#define	IO_H

#define PIC1                0x20
#define PIC2                0xA0

#define ICW1                0x11
#define ICW4                0x01

#define PIC1_IRQ_BASE       0x20
#define PIC2_IRQ_BASE       0x28
#define PIC1_IRQ(X)         PIC1_IRQ_BASE + X
#define PIC2_IRQ(X)         PIC2_IRQ_BASE + X

static inline unsigned char inportb(unsigned int port)
{
    unsigned char ret;
    asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
    return ret;
}

static inline void outportb(unsigned int port, unsigned char data)
{
    asm volatile ("outb %%al,%%dx": :"d" (port), "a" (data));
}

#endif

