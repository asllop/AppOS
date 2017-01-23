#ifndef IPV4_INTERNAL_H
#define	IPV4_INTERNAL_H

#include <appos.h>
#include <net/net.h>

void                        *ipv4_peek(struct NetIfaceStruct *iface, unsigned short offset);
byte                        ipv4_is_empty(struct NetIfaceStruct *iface);
byte                        ipv4_is_full(struct NetIfaceStruct *iface);
int                         ipv4_size(struct NetIfaceStruct *iface);
void                        ipv4_enqueue(struct NetIfaceStruct *iface, void *data);
void                        *ipv4_dequeue(struct NetIfaceStruct *iface);
byte                        ipv4_fragment_present(struct NetIfaceStruct *iface, uint16_t packetID);
byte                        ipv4_add_existing_fragment(struct NetIfaceStruct *iface, uint16_t packetID, void *packet);
byte                        ipv4_add_new_fragment(struct NetIfaceStruct *iface, uint16_t packetID, void *packet);
byte                        ipv4_sort_fragments(struct NetIfaceStruct *iface, uint16_t packetID);

#endif
