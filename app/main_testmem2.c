#include <mem/mem.h>
#include <task/task.h>
#include <sys/sys.h>
#include "utils.h"

void cnt0_task()
{
    char var_str[20];
    
    while (1)
    {
        void *p = core_malloc(10000);
        unsigned long sleepTime = (core_timestamp() & 0xff) * 4;
        core_sleep(sleepTime);
        console_put_string(0x4f, itoa(sleepTime, var_str, 10), 2, 10);
        core_free(p);
    }
}

void test_task()
{
    char var_str[20];
    
    for (long i = 0 ; i < 100000 ; i ++)
    {
        void *p = core_malloc(1000);
        core_sleep(0);
        console_put_string(0x4f, itoa(i, var_str, 10), 60, 10);
        core_free(p);
    }
    
    console_put_string(0x4f, "TEST TASK ENDED", 60, 11);
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
    
    size_t totalMem = core_avail(MEM_TYPE_USED);
    console_put_string(0x4f, "Initial Used Mem:", 60, 5);
    console_put_string(0x4f, itoa(totalMem, var_str, 10), 60, 6);
    
    void *p0 = core_malloc(10000);
    void *p1 = core_malloc(10000);
    void *p2 = core_malloc(10000);
    void *p3 = core_malloc(10000);
    void *p4 = core_malloc(10000);
    
    core_create(cnt0_task, 0, DEFAULT_STACK_SIZE);
    core_create(test_task, 0, DEFAULT_STACK_SIZE);
    
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
        
        core_sleep(0);
        
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
    
    core_free(p0);
    core_free(p1);
    core_free(p2);
    core_free(p3);
    core_free(p4);
    
    line++;
    
    console_put_string(0x4f, "End", 2, line++);
    
    // When main function reaches end, the system is restarted
    for (;;);
}
