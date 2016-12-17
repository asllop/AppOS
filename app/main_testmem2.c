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
    
    check_segments();
    
    void *p0 = core_malloc(10000);
    check_segments();
    void *p1 = core_malloc(5000);
    check_segments();
    void *p2 = core_malloc(2500);
    check_segments();
    void *p3 = core_malloc(1250);
    check_segments();
    
    core_log("\nP0 = 0x");
    core_log(itoa(p0, var_str, 16));
    core_log("\nP1 = 0x");
    core_log(itoa(p1, var_str, 16));
    core_log("\nP2 = 0x");
    core_log(itoa(p2, var_str, 16));
    core_log("\nP3 = 0x");
    core_log(itoa(p3, var_str, 16));
    core_log("\n");

    core_free(p0);
    check_segments();
    core_free(p1);
    check_segments();
    core_free(p2);
    check_segments();
    core_free(p3);
    check_segments();
    
    // TEST
    core_log("HALT!\n");
    core_halt();
    
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
    
    line++;
    
    console_put_string(0x4f, "End", 2, line++);
    
    // When main function reaches end, the system is restarted
    for (;;);
}
