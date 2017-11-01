#ifndef IPV4_H
#define	IPV4_H

#include <appos.h>
#include <net/net.h>

int                         ipv4_receive(NETWORK net, byte *packet, size_t len);
void                        *ipv4_build(NETWORK net, byte *data, size_t len, byte protocol, byte destIP[], size_t *result_size);
struct NetFragList          *ipv4_fagment(NETWORK net, byte *packet, size_t len);

#endif
