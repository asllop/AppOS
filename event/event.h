#ifndef EVENT_H
#define	EVENT_H

#include <appos.h>

int                         event_produce(EVENT code, void *message, size_t size);
void                        *event_consume(EVENT code, size_t *size);
int                         event_wait(EVENT code);
long                        event_queue(EVENT code);

#endif
