#ifndef IPV4_INTERNAL_H
#define	IPV4_INTERNAL_H

#include <appos.h>
#include <net/net.h>

void                        *ipv4_peek(struct NetQueueStruct *queue, void **bufQueue, unsigned short offset);
byte                        ipv4_is_empty(struct NetQueueStruct *queue);
byte                        ipv4_is_full(struct NetQueueStruct *queue);
int                         ipv4_size(struct NetQueueStruct *queue);
void                        ipv4_enqueue(struct NetQueueStruct *queue, void **bufQueue, void *data);
void                        *ipv4_dequeue(struct NetQueueStruct *queue, void **bufQueue);
byte                        ipv4_fragment_present(struct NetIfaceStruct *iface, uint16_t packetID);
byte                        ipv4_add_existing_fragment(struct NetIfaceStruct *iface, uint16_t packetID, void *packet);
byte                        ipv4_add_new_fragment(struct NetIfaceStruct *iface, uint16_t packetID, void *packet);

#endif
