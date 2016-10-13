#include <appos.h>

#ifndef EVENT_INTERNAL_H
#define	EVENT_INTERNAL_H

#ifndef MAX_EVENT_QUEUES
#define MAX_EVENT_QUEUES    256
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
bool                        enqueue_event(EVENT code, struct EventStruct *event);
bool                        dequeue_event(EVENT code, struct EventStruct *event);
struct QueueStruct          *get_queue(int position);

#endif
