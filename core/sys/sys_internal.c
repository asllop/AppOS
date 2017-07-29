#include <mem/mem_internal.h>
#include <task/task_internal.h>
#include <sys/sys_internal.h>
#include <task/task.h>

static LOCK                 locks[MUTEX_NUMBER];
static LOCK                 counters[MUTEX_NUMBER];

void core_init()
{
    sys_init();
    mem_init();
    sys_io_init();
    task_init();
}

void sys_init()
{
    // Setup mutex
    for (int i = 0 ; i < MUTEX_NUMBER ; i++)
    {
        locks[i] = 0;
        counters[i] = ~0;
    }
}

LOCK *sys_get_lock(MUTEX mutex)
{
    return &locks[mutex];
}

LOCK sys_get_counter(MUTEX mutex)
{
    counters[mutex] ++;
    return counters[mutex];
}

LOCK sys_current_counter(MUTEX mutex)
{
    return counters[mutex];
}
