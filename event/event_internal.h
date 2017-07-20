#ifndef EVENT_INTERNAL_H
#define	EVENT_INTERNAL_H

#include <appos.h>

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

// TODO: rename internal functions to add "event_" prefix

void                        event_init();
bool                        enqueue_event(EVENT code, struct EventStruct *event);
bool                        dequeue_event(EVENT code, struct EventStruct *event);
struct QueueStruct          *get_queue(int position);

#endif
