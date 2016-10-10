#ifndef SYS_H
#define	SYS_H

void                        core_init();

/* Architecture dependant functions */

void                        core_fatal(char *msg);
void                        core_halt();
void                        core_reboot();

#endif
