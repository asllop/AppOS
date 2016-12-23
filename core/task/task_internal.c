#include <task/task.h>
#include <task/task_internal.h>
#include <mem/mem.h>
#include <mem/mem_internal.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>

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
struct TaskStruct *empty_slot()
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

struct TaskStruct *prev_used_slot(struct TaskStruct *slot)
{
    if (!slot || taskCounter == 0)
    {
        return NULL;
    }
    
    TASK initId = slot->id;
    struct TaskStruct *task;
    
    for (int i = initId - 1 ; i > 0 ; i--)
    {
        task = get_slot(i);
        
        if (task->state != TASK_STATE_NULL)
        {
            return task;
        }
    }
    
    // Move to the end of task buffer
    initId = MAX_NUM_TASK;
    
    for (unsigned int i = initId ; i >= slot->id ; i--)
    {
        task = get_slot(i);
        
        if (task->state != TASK_STATE_NULL)
        {
            return task;
        }
    }
    
    return NULL;
}

struct TaskStruct *next_used_slot(struct TaskStruct *slot)
{
    if (!slot || taskCounter == 0)
    {
        return NULL;
    }
    
    TASK initId = slot->id;
    struct TaskStruct *task;
    
    for (int i = initId + 1 ; i <= MAX_NUM_TASK ; i++)
    {
        task = get_slot(i);
        
        if (task->state != TASK_STATE_NULL)
        {
            return task;
        }
    }
    
    // Move to the start of task buffer
    initId = 1;
    
    for (unsigned int i = initId ; i <= slot->id ; i++)
    {
        task = get_slot(i);
        
        if (task->state != TASK_STATE_NULL)
        {
            return task;
        }
    }
    
    return NULL;
}

struct TaskStruct *get_current_task()
{
    return currentTask;
}

void set_current_task(struct TaskStruct *task)
{
    currentTask = task;
}

struct TaskStruct *get_slot(TASK id)
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

struct TaskStruct *get_task(TASK id)
{
    struct TaskStruct *task = get_slot(id);
    
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

int get_task_counter()
{
    return taskCounter;
}

int inc_task_counter()
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

int dec_task_counter()
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

void set_scheduling(bool state)
{
    isScheduling = state;
}

bool get_scheduling()
{
    return isScheduling;
}

// When a task dies, here is where it goes
void task_end_point()
{
    core_forbid();
    terminate_task(currentTask);
    core_permit();
    core_sleep(0);
    
    for(;;) {}
}

void terminate_task(struct TaskStruct *task)
{
    if (task)
    {
        // Fill it and inc task counter
        if (dec_task_counter() == ERR_CODE_TASKCNTUND)
        {
            // Strange error! This should never happend
            core_fatal("Task buffer or counter seems to be corrupted");
        }
        
        // TODO: Why don't just free all task resources instead of setting TASK_STATE_DEAD?
        
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
void *schedule(void *stackPointer)
{
    if (!isScheduling)
    {
        return stackPointer;
    }
    
    // If TASK_STATE_STARTING, we are on first task, current stack pointer is the initial kernel stack, not the actual task stack
    // Else, save the stack pointer for current task
    if (currentTask->state == TASK_STATE_STARTING)
    {
        currentTask->state = TASK_STATE_RUNNING;
    }
    else
    {
        currentTask->stack = stackPointer;
    }
    
    struct TaskStruct *nextTask = currentTask->next;
    
    for (;;)
    {
        if (nextTask)
        {
            if (nextTask->state == TASK_STATE_DEAD)
            {
                // Definitively remove task
                internal_free(nextTask->stackBuffer);
                
                nextTask->state = TASK_STATE_NULL;
                nextTask->stackBuffer = NULL;
                
                if (nextTask->prev != nextTask && nextTask->next != nextTask)
                {
                    nextTask->prev->next = nextTask->next;
                    nextTask->next->prev = nextTask->prev;
                }
            }
            else if (nextTask->counter <= 0 && nextTask->state == TASK_STATE_RUNNING)
            {
                nextTask->counter = nextTask->priority;
                break;
            }
            else
            {
                nextTask->counter --;
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
    scheduler_init();
}
