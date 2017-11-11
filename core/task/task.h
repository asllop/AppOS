#ifndef TASK_H
#define	TASK_H

#include <appos.h>
#include <task/task_internal.h>

#ifndef DEFAULT_STACK_SIZE
#define DEFAULT_STACK_SIZE  1024
#endif

#ifndef MIN_STACK_SIZE
#define MIN_STACK_SIZE      128
#endif

TASK                        core_create(void (*task)(), PRIORITY priority, size_t stackSize);
TASK                        core_self();
int                         core_escalate(PRIORITY priority);
void                        core_forbid();
void                        core_permit();
TASK_STATE                  core_state(TASK taskid);
int                         core_result(TASK taskid, int *returnCode, void **bufferPointer);
void                        core_sleep(unsigned long millis);
void                        core_exit(int returnCode, void *returnBuffer);
int                         core_stop(TASK taskid);
int                         core_start(TASK taskid);
TIME                        core_timestamp();

#endif
