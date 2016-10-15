#ifndef SYS_H
#define	SYS_H

void                        core_fatal(char *msg);

/* Architecture dependant functions */

void                        core_halt();
void                        core_reboot();

#endif
