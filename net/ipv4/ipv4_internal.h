#ifndef IPV4_INTERNAL_H
#define	IPV4_INTERNAL_H

#include <appos.h>
#include <net/net.h>

static inline uint16_t ipv4_packet_len(byte *packet)
{
    return packet[2] << 8 | packet[3];
}

static inline uint16_t ipv4_packet_offset(byte *packet)
{
    return (packet[6] & 0x1f) << 8 | packet[7];
}

// Incoming

struct NetIncomingList      *ipv4_packet_list(struct NetIfaceStruct *iface, uint16_t packetID);
byte                        ipv4_exist_packet_list(struct NetIfaceStruct *iface, uint16_t packetID);
void                        *ipv4_add_fragment(struct NetIfaceStruct *iface, uint16_t packetID, byte *buff, uint16_t size);
byte                        ipv4_create_packet_list(struct NetIfaceStruct *iface, uint16_t packetID, byte *buff, uint16_t size);
void                        ipv4_free_packet_list(struct NetIfaceStruct *iface, uint16_t packetID);
void                        ipv4_close_packet_list(struct NetIfaceStruct *iface, uint16_t packetID);

// Outgoing
// TODO

/*
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
*/

#endif
