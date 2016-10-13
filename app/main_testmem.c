#include <mem/mem.h>
#include <mem/mem_internal.h>
#include <task/task.h>
#include <sys/sys.h>

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

void counter_foo(int line)
{
    char var_str[100];
    
    console_put_string(0x4f, "                               ", 2, line);
    
    for (unsigned int i = 0 ;; i++)
    {
        char *mem0 = core_malloc(5000);
        
        core_sleep(0);
        
        char *mem1 = core_malloc(5000);
        
        console_put_string(0x4f, itoa(i, var_str, 10), 2, line);
        
        if (mem0)
        {
            fast_free(mem0);
        }
        else
        {
            // Not enough free mem
            core_fatal("Task could not malloc 0");
        }
        
        if (mem1)
        {
            fast_free(mem1);
        }
        else
        {
            // Not enough free mem
            core_fatal("Task could not malloc 1");
        }
    }
}

void cnt0_task()
{
    counter_foo(24);
}

void cnt1_task()
{
    counter_foo(23);
}

void cnt2_task()
{
    counter_foo(22);
}

void cnt3_task()
{
    counter_foo(21);
}

void showUsedMem()
{
    char var_str[20];
    size_t totalMem = core_avail(MEM_TYPE_USED);
    console_put_string(0x4f, "Used Mem:", 60, 3);
    console_put_string(0x4f, itoa(totalMem, var_str, 10), 60, 4);
}

void main(int argc, char **argv)
{
    char var_str[20];
    
    // Draw Background
    console_put_data(0x1b, 176, 80*25, 0);
    
    // TEST: consume the memory of the system
    //while (core_malloc(100000));
    
    // Create a task with priority 0 (the highest)
    core_create(cnt0_task, 0, DEFAULT_STACK_SIZE);
    
    core_create(cnt1_task, 5, DEFAULT_STACK_SIZE);
    
    core_create(cnt2_task, 10, DEFAULT_STACK_SIZE);
    
    core_create(cnt3_task, 20, DEFAULT_STACK_SIZE);
    
    console_put_string(0x4f, " Hello AppOS ", 34, 1);
    
    int line = 5;
    
    for (unsigned int i = 0 ;; i++)
    {
        char *mem = core_malloc(5000);
        
        if (i == 0)
        {
            console_put_string(0x4f, "                               ", 2, line);
        }
        
        console_put_string(0x4f, itoa(i, var_str, 10), 2, line);
        
        showUsedMem();
        
        if (mem)
        {
            fast_free(mem);
        }
        else
        {
            core_fatal("Main could not malloc");
        }
    }
    
    line++;
    
    console_put_string(0x4f, "End", 2, line++);
    
    // When main function reaches end, the system is restarted
    for (;;);
}
