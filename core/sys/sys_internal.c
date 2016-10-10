#include <mem/mem_internal.h>
#include <task/task_internal.h>
#include <sys/sys_internal.h>
#include <task/task.h>

typedef unsigned int        LOCK;

static LOCK                 locks[MUTEX_NUMBER];
static LOCK                 counters[MUTEX_NUMBER];

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

LOCK *get_counter(MUTEX mutex)
{
    counters[mutex] ++;
    return &counters[mutex];
}

void acquire_mutex(MUTEX mutex)
{
    bool scheduling = get_scheduling();
    core_forbid();
    
    LOCK currentCnt = *get_counter(mutex);
    LOCK *lock = get_lock(mutex);
    
    if (scheduling) core_permit();
    
    while (*lock != currentCnt)
    {
        core_sleep(0);
    }
}

void free_mutex(MUTEX mutex)
{
    bool scheduling = get_scheduling();
    core_forbid();
    
    LOCK *lock = get_lock(mutex);
    (*lock) ++;
    
    if (scheduling) core_permit();
}
