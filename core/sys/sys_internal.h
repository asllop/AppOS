#ifndef SYS_INTERNAL_H
#define	SYS_INTERNAL_H

#include <appos.h>
#include <sys/sys.h>

#ifndef REBOOT_ON_FATAL
#define REBOOT_ON_FATAL     NO
#endif

typedef unsigned int        LOCK;

void                        core_init();
void                        sys_init();
LOCK                        *sys_get_lock(MUTEX mutex);
LOCK                        sys_get_counter(MUTEX mutex);
LOCK                        sys_current_counter(MUTEX mutex);

/* Architecture dependant functions */

void                        sys_emergency_puts(char *str);
void                        sys_io_init();

#endif

