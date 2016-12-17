#include <mem/mem.h>
#include <task/task.h>
#include <sys/sys.h>
#include "utils.h"

void counter_foo(int line)
{
    char var_str[100];
    
    console_put_string(0x4f, "                               ", 2, line);
    
    for (unsigned int i = 0 ;; i++)
    {
        char *mem0 = core_malloc(5000);
        
        //core_sleep(0);
        
        char *mem1 = core_malloc(5000);
        
        console_put_string(0x4f, itoa(i, var_str, 10), 2, line);
        
        if (mem0)
        {
            core_free(mem0);
        }
        else
        {
            // Not enough free mem
            core_fatal("Task could not malloc 0");
        }
        
        if (mem1)
        {
            core_free(mem1);
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

void cnt4_task()
{
    counter_foo(20);
}

void cnt5_task()
{
    counter_foo(19);
}

void cnt6_task()
{
    counter_foo(18);
}

void cnt7_task()
{
    counter_foo(17);
}

void cnt8_task()
{
    counter_foo(16);
}

void cnt9_task()
{
    counter_foo(15);
}

void cnt10_task()
{
    counter_foo(14);
}

void cnt11_task()
{
    counter_foo(13);
}

void showUsedMem()
{
    char var_str[20];
    size_t totalMem = core_avail(MEM_TYPE_USED);
    console_put_string(0x4f, "Used Mem:", 60, 3);
    console_put_string(0x4f, itoa(totalMem, var_str, 10), 60, 4);
}

void main(__unused int argc, __unused char **argv)
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
    
    core_create(cnt4_task, 0, DEFAULT_STACK_SIZE);
    
    core_create(cnt5_task, 5, DEFAULT_STACK_SIZE);
    
    core_create(cnt6_task, 10, DEFAULT_STACK_SIZE);
    
    core_create(cnt7_task, 20, DEFAULT_STACK_SIZE);
    
    core_create(cnt8_task, 0, DEFAULT_STACK_SIZE);
    
    core_create(cnt9_task, 5, DEFAULT_STACK_SIZE);
    
    core_create(cnt10_task, 10, DEFAULT_STACK_SIZE);
    
    core_create(cnt11_task, 20, DEFAULT_STACK_SIZE);
    
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
            core_free(mem);
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
