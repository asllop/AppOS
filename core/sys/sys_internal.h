#ifndef SYS_INTERNAL_H
#define	SYS_INTERNAL_H

#include <appos.h>
#include <sys/sys.h>

typedef unsigned int        LOCK;

void                        core_init();
void                        mutex_init();
LOCK                        *get_lock(MUTEX mutex);
LOCK                        get_counter(MUTEX mutex);
LOCK                        current_counter(MUTEX mutex);

/* Architecture dependant functions */

void                        emergency_puts(char *str);
void                        io_init();

#endif

