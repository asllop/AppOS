#ifndef SYS_H
#define	SYS_H

#ifndef REBOOT_ON_FATAL
#define REBOOT_ON_FATAL     NO
#endif

void                        core_fatal(char *msg);

/* Architecture dependant functions */

void                        core_halt();
void                        core_reboot();

#endif
