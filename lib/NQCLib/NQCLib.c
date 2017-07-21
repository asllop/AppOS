#include <appos.h>
#include <mem/mem.h>
#include <lib/NQCLib/NQCLib.h>

#ifdef NQC_ITOA
char *itoa(int value, char *str, int base)
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
#endif

#ifdef NQC_ATOI
int atoi(const char *str)
{
    int res = 0; // Initialize result
    
    // Iterate through all characters of input string and
    // update result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';
    
    // return result.
    return res;
}
#endif

#ifdef NQC_STRLEN
size_t strlen(const char *str)
{
    size_t i;
    
    for (i = 0 ; str[i] != '\0' ; i++)
    {
        // Limit
        if (i > NQC_STR_LIMIT)
        {
            break;
        }
    }
    
    return i;
}
#endif

#ifdef NQC_STRCPY
char *strcpy(char *dest, const char *src)
{
    size_t i;
    
    for (i = 0 ; src[i] != '\0' ; i++)
    {
        // Limit
        if (i > NQC_STR_LIMIT)
        {
            break;
        }
        
        dest[i] = src[i];
    }
    
    dest[i] = '\0';
    
    return dest;
}
#endif

#ifdef NQC_MEMCPY
void *memcpy(void *dest, const void *src, size_t size)
{
    for (size_t i = 0 ; i < size ; i++)
    {
        ((unsigned char *)dest)[i] = ((unsigned char *)src)[i];
    }
    
    return dest;
}
#endif

#ifdef NQC_MEMSET
void *memset(void *ptr, int value, size_t size)
{
    for (size_t i = 0 ; i < size ; i++)
    {
        ((unsigned char *)ptr)[i] = (unsigned char)value;
    }
    
    return ptr;
}
#endif

// TODO: add vsprintf and sprintf
// TODO: add vsscanf and sscanf
