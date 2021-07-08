// mutex test, use from multiple tasks, using sleep in the middle, forbid/permit, etc

#include <mem/mem.h>
#include <task/task.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>
#include <lib/NQCLib/NQCLib.h>
#include "utils.h"

void counter_foo(int line)
{
    char var_str[100];
    
    console_put_string(0x4f, "                               ", 2, line);
    
    for (unsigned int i = 0 ;; i++)
    {
        console_put_string(0x4f, itoa(i, var_str, 10), 2, line);
    }
}

void cnt0_task()
{
    counter_foo(14);
}

void cnt1_task()
{
    counter_foo(15);
}

void cnt2_task()
{
    counter_foo(16);
}

void cnt3_task()
{
    counter_foo(17);
}

void cnt4_task()
{
    counter_foo(18);
}

int main(int argc, char **argv)
{
    console_put_data(0x1b, (char)176, 80*25, 0);
    
    TASK taskCnt0ID = core_create(cnt0_task, 0, DEFAULT_STACK_SIZE);
    
    core_create(cnt1_task, 0, DEFAULT_STACK_SIZE);
    
    core_create(cnt2_task, 5, DEFAULT_STACK_SIZE);
    
    core_create(cnt3_task, 10, DEFAULT_STACK_SIZE);
    
    core_create(cnt4_task, 15, DEFAULT_STACK_SIZE);
    
    for (;;)
    {
        core_sleep(1000);
        core_log("Stop Task 0...\n");
        core_stop(taskCnt0ID);
        core_sleep(1000);
        core_log("Restart Task 0...\n");
        core_start(taskCnt0ID);
    }
    
    return 0;
}

