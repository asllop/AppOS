#include <sys/sys.h>

#ifndef SYS_INTERNAL_H
#define	SYS_INTERNAL_H

typedef enum
{
    MUTEX_MEM               = 0,
    MUTEX_TASK,
    MUTEX_EVENT,
    MUTEX_TERM,
    
    /* Custom mutex goes here ... */
    
    MUTEX_NUMBER
    
} MUTEX;

void                        core_init();
void                        mutex_init();
int                         acquire_mutex(MUTEX mutex);
int                         free_mutex(MUTEX mutex);

/* Architecture dependant functions */

void                        emergency_puts(char *str);
void                        io_init();

#endif

