#include <appos.h>
#include <mem/mem.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>
#include <event/event_internal.h>
#include <task/task.h>

static struct QueueStruct queueArray[MAX_EVENT_QUEUES];

void event_init()
{
    for (int i = 0; i < MAX_EVENT_QUEUES; i++)
    {
        queueArray[i].elements = 0;
        queueArray[i].head = 0;            // Head points to first data on the queue
        queueArray[i].tail = 0;             // Tail points to the next empty place for a data
        queueArray[i].bufferSize = MAX_EVENT_NUMBER * sizeof(struct EventStruct);
        queueArray[i].buffer = core_malloc(queueArray[i].bufferSize);
        
        if (!queueArray[i].buffer)
        {
            core_fatal("Could not allocate buffer for event queue");
        }
    }
}

bool enqueue_event(EVENT code, struct EventStruct *event)
{
    core_lock(MUTEX_EVENT);
    
    if (queueArray[code].elements < MAX_EVENT_NUMBER)
    {
        queueArray[code].elements ++;
        
        // Put data on tail
        queueArray[code].buffer[queueArray[code].tail].message = event->message;
        queueArray[code].buffer[queueArray[code].tail].size = event->size;
        
        // Update tail index
        queueArray[code].tail ++;
        
        if (queueArray[code].tail > MAX_EVENT_NUMBER - 1)
        {
            queueArray[code].tail = 0;
        }
        
        core_unlock(MUTEX_EVENT);
        
        return YES;
    }
    else
    {
        core_unlock(MUTEX_EVENT);
        
        return NO;
    }
}

bool dequeue_event(EVENT code, struct EventStruct *event)
{
    core_lock(MUTEX_EVENT);
    
    if (queueArray[code].elements)
    {
        queueArray[code].elements --;
        
        // Get data from head
        event->message = queueArray[code].buffer[queueArray[code].head].message;
        event->size = queueArray[code].buffer[queueArray[code].head].size;
        
        // Update head index
        if (queueArray[code].elements)
        {
            queueArray[code].head ++;
            
            if (queueArray[code].head > MAX_EVENT_NUMBER - 1)
            {
                queueArray[code].head = 0;
            }
        }
        else
        {
            queueArray[code].head = 0;
            queueArray[code].tail = 0;
        }
        
        core_unlock(MUTEX_EVENT);
        
        return YES;
    }
    else
    {
        core_unlock(MUTEX_EVENT);
        
        return NO;
    }
}

struct QueueStruct *get_queue(int position)
{
    if (position > MAX_EVENT_QUEUES - 1)
    {
        return NULL;
    }
    else
    {
        return &queueArray[position];
    }
}
