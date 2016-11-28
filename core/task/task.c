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
    struct TaskStruct *slot = empty_slot();
    
    if (!slot)
    {
        core_unlock(MUTEX_TASK);
        
        return 0;
    }
    
    if (stackSize < MIN_STACK_SIZE)
    {
        stackSize = MIN_STACK_SIZE;
    }
    
    size_t bestStackSize = core_best(stackSize);
    
    if (bestStackSize < stackSize)
    {
        core_unlock(MUTEX_TASK);
        
        return 0;
    }
    
    void *stackBuffer = core_malloc(bestStackSize);
    
    if (!stackBuffer)
    {
        core_unlock(MUTEX_TASK);
        
        return 0;
    }
    
    // Fill it and inc task counter
    if (inc_task_counter() == ERR_CODE_TASKCNTOVR)
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
    slot->stackSize = bestStackSize;
    slot->stackBuffer = stackBuffer;
    slot->stack = stackBuffer + slot->stackSize;
    
    // Search prev task and next task, and fill prev/next pointers
    slot->prev = prev_used_slot(slot);
    slot->next = next_used_slot(slot);
    
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
    if (get_current_task() == NULL)
    {
        set_current_task(slot);
    }
    
    // Initialize stack with an empty context
    context_init(slot);
    
    core_unlock(MUTEX_TASK);
    
    return slot->id;
}

TASK core_self()
{
    return get_current_task()->id;
}

int core_escalate(PRIORITY priority)
{
    struct TaskStruct *task = get_current_task();
    
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
    set_scheduling(FALSE);
}

void core_permit()
{
    set_scheduling(TRUE);
}

TASK_STATE core_state(TASK taskid)
{
    struct TaskStruct *task = get_task(taskid);
    
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
    struct TaskStruct *task = get_task(taskid);
    
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
        
        core_lock(MUTEX_TASK);
        
        terminate_task(task);
        
        core_unlock(MUTEX_TASK);
        
        return 0;
    }
    else
    {
        return ERR_CODE_NOTASKID;
    }
}

void core_sleep(unsigned long millis)
{
    struct TaskStruct *task = get_current_task();
    
    if (millis)
    {
        task->waitUntil = core_timestamp() + millis;
        
        while (task->waitUntil > core_timestamp())
        {
            if (get_scheduling())
            {
                force_task_scheduling();
            }
        }
        
        task->waitUntil = 0;
    }
    else
    {
        task->waitUntil = 0;
        
        if (get_scheduling())
        {
            force_task_scheduling();
        }
    }
}

void core_exit(int returnCode, void *returnBuffer)
{
    struct TaskStruct *task = get_current_task();
    
    if (task)
    {
        task->returnCode = returnCode;
        task->returnBuffer = returnBuffer;
        task->state = TASK_STATE_FINISHED;
        
        core_sleep(0);
        for (;;) {}
    }
}

TIME core_timestamp()
{
    return systemTimestamp;
}
