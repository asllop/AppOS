#include <appos.h>
#include <sys/sys_internal.h>
#include <sys/sys.h>
#include <task/task_internal.h>
#include <task/task.h>

void core_fatal(char *msg)
{
    emergency_puts(msg);
#if REBOOT_ON_FATAL
    core_reboot();
#else
    core_halt();
#endif
}

int core_lock(MUTEX mutex)
{
    if (!get_scheduling())
    {
        return ERR_CODE_MUTEXWHILEFORBID;
    }
    
    core_forbid();
    
    LOCK currentCnt = get_counter(mutex);
    LOCK *lock = get_lock(mutex);
    
    core_permit();
    
    while (*lock != currentCnt)
    {
        core_sleep(0);
    }
    
    return 0;
}

int core_unlock(MUTEX mutex)
{
    if (!get_scheduling())
    {
        return ERR_CODE_MUTEXWHILEFORBID;
    }
    
    core_forbid();
    
    LOCK *lock = get_lock(mutex);
    (*lock) ++;
    
    core_permit();
    
    return 0;
}

// NOTE: Should be used while not scheduling or the result could be invalid,
//       due to change on mutex state performed by an other task.
byte core_mutex(MUTEX mutex)
{
    LOCK currentCnt = current_counter(mutex) + 1;
    LOCK *lock = get_lock(mutex);
    
    return (*lock == currentCnt);
}
