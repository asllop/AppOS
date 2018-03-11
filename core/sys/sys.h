#ifndef SYS_H
#define	SYS_H

#include <appos.h>

typedef enum
{
    MUTEX_MEM               = 0,
    MUTEX_TASK,
    MUTEX_EVENT,
    MUTEX_SERIAL_TX,
    MUTEX_SERIAL_RX,
    MUTEX_TERM,
    MUTEX_IPV4,
    MUTEX_NET,
    MUTEX_GENERIC,
    
    /* Custom mutex goes here ... */
    
    MUTEX_NUMBER
    
} MUTEX;

void                        core_fatal(char *msg);
int                         core_lock(MUTEX mutex);
int                         core_unlock(MUTEX mutex);
byte                        core_mutex(MUTEX mutex);

/* Architecture dependant functions */

void                        core_halt();
void                        core_reboot();
void                        core_log(char *str);

#endif
