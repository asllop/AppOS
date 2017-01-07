#ifndef IPV4_H
#define	IPV4_H

#include <appos.h>
#include <net/net.h>

int                         ipv4_insert(struct NetIfaceStruct *iface, byte *packet, size_t len);

#endif