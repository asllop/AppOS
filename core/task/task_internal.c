#include <task/task.h>
#include <task/task_internal.h>
#include <mem/mem.h>
#include <mem/mem_internal.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>

// TODO: make it dynamic!
// Instead of a static array of slots, allocate memory for a task slot whenever it is created.
// Should be easy to do, because we already have a linked list structure (next & prev pointers).
// To make the most of the memory, we could put multiple slots inside each allocated buffer.
// Use TaskStruct instead of TASK ID in the system API.

static struct TaskStruct    taskArray[MAX_NUM_TASK];
static int                  taskCounter;
static struct TaskStruct    *currentTask;
static bool                 isScheduling = 0;

void init_task_buffer()
{
    isScheduling = 0;
    taskCounter = 0;
    
    struct TaskStruct *slot;
    
    for (int i = 0 ; i < MAX_NUM_TASK ; i++)
    {
        // Get pointer to slot
        slot = &taskArray[i];
        
        // Initialize the struct
        slot->id = i + 1;
        slot->task = NULL;
        slot->priority = 0;
        slot->counter = 0;
        slot->state = TASK_STATE_NULL;
        slot->returnCode = 0;
        slot->returnBuffer = NULL;
        slot->next = NULL;
        slot->prev = NULL;
    }
}

// Find first empty slot
struct TaskStruct *task_empty_slot()
{
    struct TaskStruct *slot;
    
    for (int i = 0 ; i < MAX_NUM_TASK ; i++)
    {
        // Get pointer to slot
        slot = &taskArray[i];
        
        if (slot->state == TASK_STATE_NULL)
        {
            return slot;
        }
    }
    
    return NULL;
}

struct TaskStruct *task_prev_used_slot(struct TaskStruct *slot)
{
    if (!slot || taskCounter == 0)
    {
        return NULL;
    }
    
    TASK initId = slot->id;
    struct TaskStruct *task;
    
    for (int i = initId - 1 ; i > 0 ; i--)
    {
        task = task_get_slot(i);
        
        if (task->state != TASK_STATE_NULL)
        {
            return task;
        }
    }
    
    // Move to the end of task buffer
    initId = MAX_NUM_TASK;
    
    for (unsigned int i = initId ; i >= slot->id ; i--)
    {
        task = task_get_slot(i);
        
        if (task->state != TASK_STATE_NULL)
        {
            return task;
        }
    }
    
    return NULL;
}

struct TaskStruct *task_next_used_slot(struct TaskStruct *slot)
{
    if (!slot || taskCounter == 0)
    {
        return NULL;
    }
    
    TASK initId = slot->id;
    struct TaskStruct *task;
    
    for (int i = initId + 1 ; i <= MAX_NUM_TASK ; i++)
    {
        task = task_get_slot(i);
        
        if (task->state != TASK_STATE_NULL)
        {
            return task;
        }
    }
    
    // Move to the start of task buffer
    initId = 1;
    
    for (unsigned int i = initId ; i <= slot->id ; i++)
    {
        task = task_get_slot(i);
        
        if (task->state != TASK_STATE_NULL)
        {
            return task;
        }
    }
    
    return NULL;
}

struct TaskStruct *task_get_current()
{
    return currentTask;
}

void task_set_current(struct TaskStruct *task)
{
    currentTask = task;
}

struct TaskStruct *task_get_slot(TASK id)
{
    if (id < 1 || id > MAX_NUM_TASK)
    {
        return NULL;
    }
    else
    {
        return &taskArray[id - 1];
    }
}

struct TaskStruct *task_get(TASK id)
{
    struct TaskStruct *task = task_get_slot(id);
    
    if (task)
    {
        if (task->state != TASK_STATE_NULL)
        {
            return task;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

int task_get_counter()
{
    return taskCounter;
}

int task_inc_counter()
{
    if (taskCounter >= MAX_NUM_TASK)
    {
        return ERR_CODE_TASKCNTOVR;
    }
    else
    {
        return ++taskCounter;
    }
}

int task_dec_counter()
{
    if (taskCounter <= 0)
    {
        return ERR_CODE_TASKCNTUND;
    }
    else
    {
        return --taskCounter;
    }
}

void task_set_scheduling(bool state)
{
    isScheduling = state;
}

bool task_get_scheduling()
{
    return isScheduling;
}

// When a task dies, here is where it goes
void task_end_point()
{
    core_forbid();
    task_finalize(currentTask);
    core_permit();
    core_sleep(0);
    
    for(;;);
}

void task_finalize(struct TaskStruct *task)
{
    if (task)
    {
        // Fill it and inc task counter
        if (task_dec_counter() == ERR_CODE_TASKCNTUND)
        {
            // Strange error! This should never happend
            core_fatal("Task buffer or counter seems to be corrupted");
        }
        
        // NOTE:
        // Why don't just free all task resources instead of setting TASK_STATE_DEAD?
        // Because the current task stack is still being used and deallocating it could cause a crash/mem corruption on next context change.
        
        task->task = NULL;
        task->priority = 0;
        task->counter = 0;
        task->state = TASK_STATE_DEAD;
        task->returnCode = 0;
        task->returnBuffer = NULL;
        task->stackSize = 0;
        task->stack = NULL;
    }
    else
    {
        core_fatal("Terminate task over a NULL pointer");
    }
}

// Called by timer interrupt handler
void *task_schedule(void *stackPointer)
{
    if (!isScheduling)
    {
        return stackPointer;
    }
    
    currentTask->stack = stackPointer;
    
    struct TaskStruct *nextTask = currentTask->next;
    
    for (;;)
    {
        if (nextTask)
        {
            if (nextTask->state == TASK_STATE_DEAD)
            {
                // Definitively remove task
                mem_internal_free(nextTask->stackBuffer);
                
                nextTask->state = TASK_STATE_NULL;
                nextTask->stackBuffer = NULL;
                
                if (nextTask->prev != nextTask && nextTask->next != nextTask)
                {
                    nextTask->prev->next = nextTask->next;
                    nextTask->next->prev = nextTask->prev;
                }
            }
            else if (nextTask->state == TASK_STATE_RUNNING)
            {
                if (nextTask->counter == 0)
                {
                    // Run it
                    nextTask->counter = nextTask->priority;
                    break;
                }
                else
                {
                    // Dec counter and go to next task
                    nextTask->counter --;
                    nextTask = nextTask->next;
                }
            }
            else
            {
                // If task is not running or dead, it must be stopped or finished, in any case go to next task
                nextTask = nextTask->next;
            }
        }
        else
        {
            // Not able to get next task
            core_fatal("Schedule error, couldn't get next task");
        }
    }
    
    currentTask = nextTask;
    
    return nextTask->stack;
}

void task_init()
{
    init_task_buffer();
    task_scheduler_init();
}
