#include <mem/mem_internal.h>
#include <task/task_internal.h>
#include <sys/sys_internal.h>
#include <event/event_internal.h>
#include <task/task.h>

static LOCK                 locks[MUTEX_NUMBER];
static LOCK                 counters[MUTEX_NUMBER];

void core_init()
{
    mutex_init();
    mem_init();
    io_init();
    task_init();
    
    // TODO: make it optional
    event_init();
}

void mutex_init()
{
    for (int i = 0 ; i < MUTEX_NUMBER ; i++)
    {
        locks[i] = 0;
        counters[i] = ~0;
    }
}

LOCK *get_lock(MUTEX mutex)
{
    return &locks[mutex];
}

LOCK get_counter(MUTEX mutex)
{
    counters[mutex] ++;
    return counters[mutex];
}
