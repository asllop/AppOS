#include <common/appos.h>

#ifndef TASK_INTERNAL_H
#define	TASK_INTERNAL_H

#ifndef MAX_NUM_TASK
#define MAX_NUM_TASK        256
#endif

typedef enum
{
    TASK_STATE_NULL         = 0,            // Empty slot
    TASK_STATE_STARTING,                    // First task is starting
    TASK_STATE_RUNNING,                     // Task is running
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
struct TaskStruct           *empty_slot();
struct TaskStruct           *prev_used_slot(struct TaskStruct *slot);
struct TaskStruct           *next_used_slot(struct TaskStruct *slot);
struct TaskStruct           *get_current_task();
void                        set_current_task(struct TaskStruct *task);
struct TaskStruct           *get_slot(TASK id);
struct TaskStruct           *get_task(TASK id);
int                         get_task_counter();
int                         inc_task_counter();
int                         dec_task_counter();
void                        set_scheduling(bool state);
bool                        get_scheduling();
void                        task_end_point();
void                        terminate_task(struct TaskStruct *task);
void                        *schedule(void *stackPointer);

/* Architecture dependant functions */

void                        scheduler_init();
void                        context_init(struct TaskStruct *slot);
void                        force_task_scheduling();

#endif
