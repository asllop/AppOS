#include <appos.h>
#include <event/event.h>
#include <event/event_internal.h>
#include <task/task.h>

int event_produce(EVENT code, void *message, size_t size)
{
    if (code > MAX_EVENT_QUEUES - 1)
    {
        return ERR_CODE_NOEVENT;
    }
    
    struct EventStruct event = { .message = message, .size = size };
    
    if (!enqueue_event(code, &event))
    {
        return ERR_CODE_EVENTFULL;
    }
    
    return 0;
}

void *event_consume(EVENT code, size_t *size)
{
    if (code > MAX_EVENT_QUEUES - 1)
    {
        return NULL;
    }
    
    struct EventStruct event;
    
    if (!dequeue_event(code, &event))
    {
        return NULL;
    }
    
    *size = event.size;

    return event.message;
}

int event_wait(EVENT code)
{
    struct QueueStruct *queue = get_queue(code);
    
    if (!queue)
    {
        return ERR_CODE_NOEVENT;
    }
    
    while (queue->elements == 0)
    {
        core_sleep(0);
    }
    
    return 0;
}

long event_queue(EVENT code)
{
    struct QueueStruct *queue = get_queue(code);
    
    if (queue)
    {
        return (long)queue->elements;
    }
    else
    {
        return ERR_CODE_NOEVENT;
    }
}
