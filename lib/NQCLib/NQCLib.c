/*
 Not Quite C Lib - By Andreu Santaren Llop and others (read below)
 
 NQCLib is a Frankenstein library made by chopping and sticking functions from different libraries and Internet sites.
 Is not an ANSI C Library at all, since the list of functions is not complete, but the ones included follow the standard
 on both, its prototype and behaviour. NQCLib is not intended to replace a real standard ANSI C Lib, it was written just as
 a helper library for AppOS, and the functions were added whenever needed during the kernel development process.
 The interesting thing about NQCLib is its modularity and scalability, since is possible to compile only the functions
 you really need by setting the appropriate NQCLib_config.h macros, easily adapting it to small and embedded systems.
 
 The list of "donors":
 http://clc-wiki.net/
 http://wiki.osdev.org/
 Jesus Ramos - https://github.com/jesus-ramos/not-a-good-os
 
 */

#include <appos.h>
#include <mem/mem.h>
#include <lib/NQCLib/NQCLib.h>

#ifdef NQC_ITOA
char *itoa(int value, char *str, int base)
{
    return ltoa((long)value, str, base);
}
#endif

#ifdef NQC_LTOA
char *ltoa(long value, char *str, int base)
{
    char *rc;
    char *ptr;
    char *low;
    // Check for supported base.
    if (base < 2 || base > 36)
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if (value < 0 && base == 10)
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
    }
    while (value);
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while (low < ptr)
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    
    return rc;
}
#endif

#ifdef NQC_ULTOA
char *ultoa(unsigned long value, char *str, int base)
{
    char *rc;
    char *ptr;
    char *low;
    // Check for supported base.
    if (base < 2 || base > 36)
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    }
    while (value);
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while (low < ptr)
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

#ifdef NQC_STRNCPY
char *strncpy(char *dest, const char *src, size_t n)
{
    char *ret = dest;
    
    do
    {
        if (!n--)
        {
            return ret;
        }
    }
    while ((*dest++ = *src++));
    
    while (n--)
    {
        *dest++ = 0;
    }
    
    return ret;
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

#ifdef NQC_STRNCMP
int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n--)
    {
        if (*s1++ != *s2++)
        {
            return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
        }
    }
    
    return 0;
}
#endif

#ifdef NQC_STRCMP
int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++ , s2++;
    }
    
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}
#endif

// TODO: build our own vsprintf implementation that uses itoa, ltoa, etc, and supports all "%" commands
#ifdef NQC_VSPRINTF

#define PAD_ZEROES     (1 << 0)
#define SIGNED         (1 << 1)
#define PLUS           (1 << 2)
#define SPACE          (1 << 3)
#define LEFT_JUSTIFIED (1 << 4)
#define SPECIAL        (1 << 5)
#define LOWER_CASE     (1 << 6)

const char *digits_large = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *digits_small = "0123456789abcdefghijklmnopqrstuvwxyz";

#define do_div(n,base)  ({ \
int __rem = n % base;  \
n /= base;             \
__rem;                 \
})

static char *num_to_base(char *str, int num, int base, int size,
                         int precision, int type)
{
    char c;
    char sign;
    char tmp[36];
    int i;
    const char *digits;
    
    /* Who uses base 37? */
    if (base < 2 || base > 36)
        return 0;
    
    digits = (type & LOWER_CASE) ? digits_small : digits_large;
    
    if (type & LEFT_JUSTIFIED)
        type &= ~PAD_ZEROES;
    
    c = (type & PAD_ZEROES) ? '0' : ' ';
    
    if (type & SIGNED && num < 0)
    {
        sign = '-';
        num = -num;
    }
    else
        sign = (type & PLUS) ? '+' : ((type & SPACE) ? ' ' : 0);
    
    if (sign)
        size--;
    if (type & SPECIAL)
        size = (base == 16) ? (size - 2) : ((base == 8) ? (size - 1) : size);
    
    i = 0;
    if (num == 0)
        tmp[i++] = '0';
    else
        while (num != 0)
            tmp[i++] = digits[do_div(num, base)];
    
    precision = (i > precision) ? i : precision;
    size -= precision;
    
    if (!(type & (PAD_ZEROES + LEFT_JUSTIFIED)))
        while (size-- > 0)
            *str++ = ' ';
    if (sign)
        *str++ = sign;
    if (type & SPECIAL)
    {
        if (base == 8 || base == 16)
            *str++ = '0';
        if (base == 16)
            *str++ = digits[33];
    }
    if (!(type & LEFT_JUSTIFIED))
        while (size-- > 0)
            *str++ = c;
    
    while (i < precision--)
        *str++ = '0';
    while (i-- > 0)
        *str++ = tmp[i];
    while (size-- > 0)
        *str++ = ' ';
    
    return str;
}

static inline int isdigit(char c)
{
    return c >= '0' && c <= '9';
}

static int skip_atoi(const char **s)
{
    int i;
    
    i = 0;
    while (isdigit(**s))
        i = i * 10 + *((*s)++) - '0';
    
    return i;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
    int len;
    int i;
    int flags;
    int field_width;
    int precision;
    int *ip;
    char *str;
    char *s;
    
    for (str = buf; *fmt; fmt++)
    {
        if (*fmt != '%')
        {
            *str++ = *fmt;
            continue;
        }
        
        flags = 0;
    get_flags:
        fmt++;
        switch (*fmt)
        {
            case '-': flags |= LEFT_JUSTIFIED; goto get_flags;
            case '+': flags |= PLUS; goto get_flags;
            case ' ': flags |= SPACE; goto get_flags;
            case '#': flags |= SPECIAL; goto get_flags;
            case '0': flags |= PAD_ZEROES; goto get_flags;
        }
        
        field_width = -1;
        if (isdigit(*fmt))
            field_width = skip_atoi(&fmt);
        else if (*fmt == '*')
        {
            field_width = va_arg(args, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT_JUSTIFIED;
            }
        }
        
        precision = -1;
        if (*fmt == '.')
        {
            fmt++;
            if (isdigit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*')
                precision = va_arg(args, int);
            precision = (precision < 0) ? 0 : precision;
        }
        
        switch (*fmt)
        {
            case 'c':
                if (!(flags & LEFT_JUSTIFIED))
                    while (--field_width > 0)
                        *str++ = ' ';
                *str++ = (unsigned char)va_arg(args, int);
                while (--field_width > 0)
                    *str++ = ' ';
                break;
                
            case 's':
                s = va_arg(args, char *);
                if (!s)
                    s = "NULL";
                
                len = strlen(s);
                if (precision < 0)
                    precision = len;
                else if (len > precision)
                    len = precision;
                
                if (!(flags & LEFT_JUSTIFIED))
                    while (len < field_width--)
                        *str++ = ' ';
                for (i = 0; i < len; i++)
                    *str++ = *s++;
                while (len < field_width--)
                    *str++ = ' ';
                break;
                
            case 'o':
                str = num_to_base(str, va_arg(args, unsigned long),
                                  8, field_width, precision, flags);
                break;
                
            case 'p':
                if (field_width == -1)
                {
                    field_width = 8;
                    flags |= PAD_ZEROES;
                }
                str = num_to_base(str, (unsigned long)va_arg(args, void *),
                                  16, field_width, precision, flags);
                break;
                
            case 'x':
                flags |= LOWER_CASE;
            case 'X':
                str = num_to_base(str, va_arg(args, unsigned long),
                                  16, field_width, precision, flags);
                break;
                
            case 'd':
            case 'i':
                flags |= SIGNED;
            case 'u':
                str = num_to_base(str, va_arg(args, unsigned long),
                                  10, field_width, precision, flags);
                break;
                
            case 'n':
                ip = va_arg(args, int *);
                *ip = (str - buf);
                break;
                
            default:
                if (*fmt != '%')
                    *str++ = '%';
                if (*fmt)
                    *str++ = *fmt;
                else
                    fmt--;
                break;
        }
    }
    
    *str = '\0';
    
    return str - buf;
}
#endif

#ifdef NQC_SPRINTF
int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int i;
    
    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);
    
    return i;
}
#endif

// TODO: add vsscanf and sscanf
