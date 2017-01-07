#ifndef IPV4_INTERNAL_H
#define	IPV4_INTERNAL_H

#include <appos.h>
#include <net/net.h>

void                        *ipv4_peek(struct NetIfaceStruct *iface);
byte                        ipv4_is_empty(struct NetIfaceStruct *iface);
byte                        ipv4_is_full(struct NetIfaceStruct *iface);
int                         ipv4_size(struct NetIfaceStruct *iface);
void                        ipv4_enqueue(struct NetIfaceStruct *iface, void *data);
void                        *ipv4_dequeue(struct NetIfaceStruct *iface);
byte                        ipv4_is_fragmented(struct NetIfaceStruct *iface);
void                        ipv4_start_fragments(struct NetIfaceStruct *iface);
void                        ipv4_end_fragments(struct NetIfaceStruct *iface);

#endif