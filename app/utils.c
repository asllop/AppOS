#include <appos.h>

void console_put_string(unsigned char colour, const char *string, unsigned int x, unsigned int y)
{
    unsigned int offset = y * 80 + x;
    volatile char *video = (volatile char*)0xB8000 + offset * 2;
    while (*string != 0)
    {
        *video++ = *string++;
        *video++ = colour;
    }
}

void console_put_data(int colour, char data, int repeat, unsigned int offset)
{
    volatile char *video = (volatile char*)0xB8000 + offset * 2;
    for (int i = 0 ; i < repeat ; i++)
    {
        *video++ = data;
        *video++ = colour;
    }
}
