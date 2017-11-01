#ifndef EVENT_INTERNAL_H
#define	EVENT_INTERNAL_H

#include <appos.h>
#include <event/event.h>

#ifndef MAX_EVENT_QUEUES
#define MAX_EVENT_QUEUES    128             // Since EVENT is 1 Byte wide, MAX_EVENT_QUEUES could be 256 at most 
#endif

#ifndef MAX_EVENT_NUMBER
#define MAX_EVENT_NUMBER    512
#endif

struct EventStruct
{
    void                    *message;
    size_t                  size;
};

struct QueueStruct
{
    struct EventStruct      *buffer;
    size_t                  bufferSize;
    int                     head;
    int                     tail;
    unsigned int            elements;
};

void                        event_init();
bool                        event_enqueue(EVENT code, struct EventStruct *event);
bool                        event_dequeue(EVENT code, struct EventStruct *event);
struct QueueStruct          *event_get(int position);

#endif
