#include <appos.h>
#include <task/task_internal.h>

#ifndef TASK_H
#define	TASK_H

#ifndef DEFAULT_STACK_SIZE
#define DEFAULT_STACK_SIZE  8*1024
#endif

#ifndef MIN_STACK_SIZE
#define MIN_STACK_SIZE      256
#endif

TASK                        core_create(void (*task)(), PRIORITY priority, size_t stackSize);
TASK                        core_self();
int                         core_escalate(PRIORITY priority);
int                         core_forbid();
int                         core_permit();
TASK_STATE                  core_state(TASK taskid);
int                         core_result(TASK taskid, int *returnCode, void **bufferPointer);
void                        core_sleep(unsigned long millis);
void                        core_exit(int returnCode, void *returnBuffer);
TIME                        core_timestamp();

#endif
