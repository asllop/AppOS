#ifndef NET_INTERNAL_H
#define NET_INTERNAL_H

#include <appos.h>
#include <net/net.h>

NETWORK                     net_create(NET_IFACE_TYPE type, byte id);
struct NetIfaceStruct *     net_iface(NETWORK net);
void                        net_parse_ipv4(char *addr_str, byte address[]);
uint16_t                    net_checksum(byte *ip, int len);

#endif
