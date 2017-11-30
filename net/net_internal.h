#ifndef NET_INTERNAL_H
#define NET_INTERNAL_H

#include <appos.h>
#include <net/net.h>

#define UDP_PROTOCOL        17
#define TCP_PROTOCOL        6

NETWORK                     net_create(NET_IFACE_TYPE type, byte id);
struct NetIfaceStruct *     net_iface(NETWORK net);
void                        net_parse_ipv4(char *addr_str, byte address[]);
uint16_t                    net_checksum(byte *ip, int len);
void                        net_iface_tx(NETWORK net, byte *data, size_t len);
bool                        net_assign_socket(struct NetSocket *socket);
bool                        net_release_socket(struct NetSocket *socket);
bool                        net_incomming_packet(struct NetFragList fragList);
bool                        net_insert_packet(struct NetSocket *socket, struct NetFragList fragList);
struct NetFragList          net_extract_packet(struct NetSocket *socket);

#endif
