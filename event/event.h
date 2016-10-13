#include <appos.h>

#ifndef EVENT_H
#define	EVENT_H

int                         core_produce(EVENT code, void *message, size_t size);
void                        *core_consume(EVENT code, size_t *size);
int                         core_wait(EVENT code);
long                        core_queue(EVENT code);

#endif
