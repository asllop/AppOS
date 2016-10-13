#include <mem/mem.h>
#include <task/task.h>
#include <event/event.h>
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
    
    for (unsigned int i = 0 ;; i++)
    {
        char *mem = core_malloc(10000);
        
        if (i == 0)
        {
            console_put_string(0x4f, "                               ", 2, line);
        }
        
        console_put_string(0x4f, itoa(i, var_str, 10), 2, line);
        
        if (mem)
        {
            core_free(mem);
        }
        else
        {
            core_fatal("NO MEM!");
        }
    }
}

void cnt0_task()
{
	counter_foo(22);
}

void cnt1_task()
{
    counter_foo(23);
}

void cnt2_task()
{
    counter_foo(24);
}

void event_receiver_task()
{
    char var_str[20];
    
    for (;;)
    {
        if (!core_wait(0))
        {
            size_t size;
            long long int *counter = (long long int *)core_consume(0, &size);
            
            if (counter)
            {
                console_put_string(0x4f, itoa(*counter, var_str, 10), 70, 2);
                core_free((void *)counter);
            }
            else
            {
                core_fatal("No message arrived in event");
            }
        }
        else
        {
            core_fatal("Error waiting for an event");
        }
    }
}

void showUsedMem()
{
    char var_str[20];
    size_t totalMem = core_avail(MEM_TYPE_USED);
    console_put_string(0x4f, "Used Mem:", 2, 3);
    console_put_string(0x4f, itoa(totalMem, var_str, 10), 2, 4);
}

void main(int argc, char **argv)
{
	char var_str[20];
    
    // Draw Background
    console_put_data(0x1b, 176, 80*25, 0);

    //console_put_string(0x4f, itoa(argc, var_str, 10), 70, 3);
    
    // Create a task with priority 0 (the highest)
	core_create(cnt0_task, 0, DEFAULT_STACK_SIZE);
    
    // Create a task with priority 2 (less priority)
	core_create(cnt1_task, 2, DEFAULT_STACK_SIZE);
    
    // Create a task with priority 5 (even less priority)
    core_create(cnt2_task, 5, DEFAULT_STACK_SIZE);
    
    // Create task to listen events
    core_create(event_receiver_task, 0, DEFAULT_STACK_SIZE);

	console_put_string(0x4f, " Hello AppOS ", 34, 1);

	int line = 5;

	for (unsigned int i = 0 ;; i++)
	{
		if (i == 0)
		{
			console_put_string(0x4f, "                               ", 2, line);
		}

		console_put_string(0x4f, itoa(i, var_str, 10), 2, line);
        
        if (i % 100 == 0)
        {
            size_t cnt_sz = sizeof(long long int);
            long long int *counter = (long long int *)core_malloc(cnt_sz);
            if (!counter)
            {
                core_fatal("Couldn't alloc counter");
            }
            
            *counter = (long long int)i;
            
            if (core_produce(0, (void *)counter, cnt_sz)) {
                core_free(counter);
            }
            
            showUsedMem();
        }
	}

	line++;

	console_put_string(0x4f, "End", 2, line++);

    // When main function reaches end, the system is restarted
	for (;;);
}
