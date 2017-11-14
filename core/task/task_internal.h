#ifndef TASK_INTERNAL_H
#define	TASK_INTERNAL_H

#include <appos.h>

#ifndef MAX_NUM_TASK
#define MAX_NUM_TASK        128
#endif

typedef enum
{
    TASK_STATE_NULL         = 0,            // Empty slot
    TASK_STATE_RUNNING,                     // Task is running
    TASK_STATE_STOPPED,                     // Task is stopped
    TASK_STATE_FINISHED,                    // Task has finished but there is return information ready to be read
    TASK_STATE_DEAD                         // Task is about to be removed and it's resources freed
    
} TASK_STATE;

struct TaskStruct
{
    TASK                    id;
    void                    (*task)();
    PRIORITY                priority;
    PRIORITY                counter;
    TASK_STATE              state;
    int                     returnCode;
    void                    *returnBuffer;
    struct TaskStruct       *next;
    struct TaskStruct       *prev;
    void                    *stack;
    void                    *stackBuffer;
    size_t                  stackSize;
    TIME                    waitUntil;
};

extern TIME                 systemTimestamp;

void                        task_init();
struct TaskStruct           *task_empty_slot();
struct TaskStruct           *task_prev_used_slot(struct TaskStruct *slot);
struct TaskStruct           *task_next_used_slot(struct TaskStruct *slot);
struct TaskStruct           *task_get_current();
void                        task_set_current(struct TaskStruct *task);
struct TaskStruct           *task_get_slot(TASK id);
struct TaskStruct           *task_get(TASK id);
int                         task_get_counter();
int                         task_inc_counter();
int                         task_dec_counter();
void                        task_set_scheduling(bool state);
bool                        task_get_scheduling();
void                        task_end_point();
void                        task_finalize(struct TaskStruct *task);
void                        *task_schedule(void *stackPointer);

/* Architecture dependant functions */

void                        task_scheduler_init();
void                        task_context_init(struct TaskStruct *slot);
void                        task_force_scheduling();
void                        task_setup_stack(void *stackPointer);

#endif
