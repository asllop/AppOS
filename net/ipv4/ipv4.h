#ifndef IPV4_H
#define	IPV4_H

#include <appos.h>
#include <net/net.h>

int                         ipv4_insert(NETWORK net, byte *packet, size_t len);
int                         ipv4_retrieve(NETWORK net, byte **packet, size_t *len);

#endif
