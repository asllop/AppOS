#include <mem/mem.h>
#include <task/task.h>
#include <event/event.h>
#include <sys/sys.h>
#include "utils.h"

void counter_foo(int line)
{
    char var_str[100];
    
    for (unsigned int i = 0 ;;)
    {
        size_t memsz = 10000;
        char *mem = core_malloc(memsz);
        
        if (i == 0)
        {
            console_put_string(0x4f, "                               ", 2, line);
        }
        
        if (mem)
        {
            core_set(mem, 0x55, memsz);
            console_put_string(0x4f, itoa(i, var_str, 10), 2, line);
            core_free(mem);
            
            i ++;
        }
        else
        {
            // Not enough free mem
            i --;
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
        if (!event_wait(0))
        {
            size_t size;
            long long int *counter = (long long int *)event_consume(0, &size);
            
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
                // No enough free mem
            }
            else
            {
                *counter = (long long int)i;
                
                if (event_produce(0, (void *)counter, cnt_sz)) {
                    // Error producing event, free mem and continue
                    core_free(counter);
                }
            }
            
            showUsedMem();
        }
	}

	line++;

	console_put_string(0x4f, "End", 2, line++);

    // When main function reaches end, the system is restarted
    core_exit(0, NULL);
}
