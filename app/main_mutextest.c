// mutex test, use from multiple tasks, using sleep in the middle, forbid/permit, etc

#include <mem/mem.h>
#include <task/task.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>
#include "utils.h"

void cnt0_task()
{
    char var_str[20];
    for (unsigned i = 0 ;; i++)
    {
        core_forbid();
        acquire_mutex(MUTEX_TERM);
        console_put_string(0x4f, itoa(i, var_str, 10), 2, 5);
        free_mutex(MUTEX_TERM);
        core_permit();
    }
}

void cnt1_task()
{
    char var_str[20];
    for (unsigned i = 0 ;; i++)
    {
        acquire_mutex(MUTEX_TERM);
        console_put_string(0x4f, itoa(i, var_str, 10), 2, 6);
        free_mutex(MUTEX_TERM);
    }
}

void cnt2_task()
{
    char var_str[20];
    for (unsigned i = 0 ;; i++)
    {
        acquire_mutex(MUTEX_TERM);
        console_put_string(0x4f, itoa(i, var_str, 10), 2, 7);
        free_mutex(MUTEX_TERM);
    }
}

void main(int argc, char **argv)
{
    console_put_data(0x1b, 176, 80*25, 0);
    
    core_create(cnt0_task, 0, DEFAULT_STACK_SIZE);
    
    core_create(cnt1_task, 5, DEFAULT_STACK_SIZE);
    
    core_create(cnt2_task, 10, DEFAULT_STACK_SIZE);
    
    core_exit(0, NULL);
}
