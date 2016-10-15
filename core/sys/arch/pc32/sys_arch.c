#include <appos.h>
#include <sys/io/arch/pc32/io.h>

// Quick and dirty function to print on console
void console_puts(unsigned char color, const char *string, unsigned int x, unsigned int y)
{
    unsigned int offset = y * 80 + x;
    volatile char *video = (volatile char*)0xB8000 + offset * 2;
    while (*string != 0)
    {
        *video++ = *string++;
        *video++ = color;
    }
}

void emergency_puts(char *str)
{
    console_puts(0x4f, str, 0, 0);
}

void core_halt()
{
    asm(
        "cli;"
        "hlt;"
        );
}

void core_reboot()
{
    uint8_t good = 0x02;
    
    while (good & 0x02)
    {
        good = inportb(0x64);
    }
    
    outportb(0x64, 0xFE);
    core_halt();
}
