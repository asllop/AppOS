#ifndef IPV4_H
#define	IPV4_H

#include <appos.h>
#include <net/net.h>

NETWORK                     ipv4_new_iface(NET_IFACE_TYPE type, byte id);
int                         ipv4_insert(NETWORK net, byte *packet, size_t len);
int                         ipv4_retrieve(NETWORK net, byte **packet, size_t *len);
struct NetIfaceStruct       *ipv4_network(NETWORK net);

#endif
