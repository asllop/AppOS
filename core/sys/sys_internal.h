#ifndef SYS_INTERNAL_H
#define	SYS_INTERNAL_H

#include <appos.h>
#include <sys/sys.h>

#ifndef REBOOT_ON_FATAL
#define REBOOT_ON_FATAL     NO
#endif

typedef unsigned int        LOCK;

// TODO: rename internal functions to add "sys_" or "mutex_" prefix

void                        core_init();
void                        mutex_init();
LOCK                        *get_lock(MUTEX mutex);
LOCK                        get_counter(MUTEX mutex);
LOCK                        current_counter(MUTEX mutex);

/* Architecture dependant functions */

void                        emergency_puts(char *str);
void                        io_init();

#endif

