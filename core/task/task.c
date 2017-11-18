#include <sys/sys.h>
#include <sys/sys_internal.h>
#include <mem/mem.h>
#include <task/task.h>
#include <task/task_internal.h>

// System timestamp is updated by the timer interrupt service routine
TIME                        systemTimestamp;

TASK core_create(void (*task)(), PRIORITY priority, size_t stackSize)
{
    core_lock(MUTEX_TASK);
    
    // Search empty slot
    struct TaskStruct *slot = task_empty_slot();
    
    if (!slot)
    {
        core_unlock(MUTEX_TASK);
        
        return 0;
    }
    
    if (stackSize < MIN_STACK_SIZE)
    {
        stackSize = MIN_STACK_SIZE;
    }
    
    void *stackBuffer = core_malloc(stackSize);
    stackSize = core_size(stackBuffer);
    
    if (!stackBuffer)
    {
        core_unlock(MUTEX_TASK);
        
        return 0;
    }
    
    // Fill it and inc task counter
    if (task_inc_counter() == ERR_CODE_TASKCNTOVR)
    {
        // Strange error! This should never happend
        core_fatal("Task buffer or counter seems to be corrupted");
    }
    
    slot->task = task;
    slot->priority = priority;
    slot->counter = priority;
    slot->state = TASK_STATE_RUNNING;
    slot->returnCode = 0;
    slot->returnBuffer = NULL;
    slot->stackSize = stackSize;
    slot->stackBuffer = stackBuffer;
    slot->stack = stackBuffer + slot->stackSize;
    
    // Search prev task and next task, and fill prev/next pointers
    slot->prev = task_prev_used_slot(slot);
    slot->next = task_next_used_slot(slot);
    
    if (!slot->prev)
    {
        slot->prev = slot;
    }
    else
    {
        slot->prev->next = slot;
    }
    
    if (!slot->next)
    {
        slot->next = slot;
    }
    else
    {
        slot->next->prev = slot;
    }
    
    // This is the first task
    if (task_get_current() == NULL)
    {
        task_set_current(slot);
    }
    
    // Initialize stack with an empty context
    task_context_init(slot);
    
    core_unlock(MUTEX_TASK);
    
    return slot->id;
}

TASK core_self()
{
    return task_get_current()->id;
}

int core_escalate(PRIORITY priority)
{
    struct TaskStruct *task = task_get_current();
    
    if (task)
    {
        task->priority = priority;
        task->counter = priority;
        return 0;
    }
    else
    {
        return ERR_CODE_NOTASKID;
    }
}

void core_forbid()
{
    task_set_scheduling(FALSE);
}

void core_permit()
{
    task_set_scheduling(TRUE);
}

TASK_STATE core_state(TASK taskid)
{
    struct TaskStruct *task = task_get(taskid);
    
    if (task)
    {
        return task->state;
    }
    else
    {
        return ERR_CODE_NOTASKID;
    }
}

int core_result(TASK taskid, int *returnCode, void **bufferPointer)
{
    struct TaskStruct *task = task_get(taskid);
    
    if (task)
    {
        while (task->state != TASK_STATE_FINISHED)
        {
            if (task->state == TASK_STATE_NULL)
            {
                return ERR_CODE_NOTASKID;
            }
            
            core_sleep(0);
        }
        
        *returnCode = task->returnCode;
        *bufferPointer = task->returnBuffer;
        
        core_forbid();
        task_finalize(task);
        core_permit();
        
        return 0;
    }
    else
    {
        return ERR_CODE_NOTASKID;
    }
}

void core_sleep(unsigned long millis)
{
    struct TaskStruct *task = task_get_current();
    
    if (millis)
    {
        task->waitUntil = core_timestamp() + millis;
        
        while (task->waitUntil > core_timestamp())
        {
            if (task_get_scheduling())
            {
                task_force_scheduling();
            }
        }
        
        task->waitUntil = 0;
    }
    else
    {
        task->waitUntil = 0;
        
        if (task_get_scheduling())
        {
            task_force_scheduling();
        }
    }
}

void core_exit(int returnCode, void *returnBuffer)
{
    struct TaskStruct *task = task_get_current();
    
    if (task)
    {
        task->returnCode = returnCode;
        task->returnBuffer = returnBuffer;
        task->state = TASK_STATE_FINISHED;
        
        core_sleep(0);
        for (;;);
    }
}

// WARNING: if task is stopped while locking a mutex, all tasks depending on that mutex will remain blocked until the task restarts
int core_stop()
{
    struct TaskStruct *task = task_get_current();
    
    if (task)
    {
        if (task->state == TASK_STATE_RUNNING)
        {
            core_forbid();
            task->state = TASK_STATE_STOPPED;
            core_permit();
            core_sleep(0);
            return 0;
        }
        else
        {
            return ERR_CODE_BADTASKSTATE;
        }
    }
    else
    {
        return ERR_CODE_NOTASKID;
    }
}

int core_start(TASK taskid)
{
    struct TaskStruct *task = task_get(taskid);
    
    if (task)
    {
        if (task->state == TASK_STATE_STOPPED)
        {
            core_forbid();
            task->state = TASK_STATE_RUNNING;
            core_permit();
            return 0;
        }
        else
        {
            return ERR_CODE_BADTASKSTATE;
        }
    }
    else
    {
        return ERR_CODE_NOTASKID;
    }
}

TIME core_timestamp()
{
    return systemTimestamp;
}
