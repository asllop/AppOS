#include <appos.h>

char *itoa(long long int value, char *str, int base)
{
    char *rc;
    char *ptr;
    char *low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

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

size_t strlen(const char *str)
{
    size_t i;
    
    for (i = 0 ; str[i] != 0 ; i++)
    {
        // Limit
        if (i > 10000)
        {
            break;
        }
    }
    
    return i;
}
