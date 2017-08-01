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

static inline byte ipv4_packet_header_len(byte *packet)
{
    return (packet[0] & 0xf) * 4;
}

struct IPv4_header
{
    byte                    versionAndHlen;
    byte                    serviceType;
    byte                    totalLength[2];
    byte                    packetID[2];
    byte                    flagsAndOffset[2];
    byte                    ttl;
    byte                    protocol;
    byte                    headerChecksum[2];
    byte                    source[4];
    byte                    destination[4];
    
} __attribute__((packed));

// Incoming

struct NetIncomingList      *ipv4_packet_list(struct NetIfaceStruct *iface, uint16_t packetID);
byte                        ipv4_exist_packet_list(struct NetIfaceStruct *iface, uint16_t packetID);
void                        *ipv4_add_fragment(struct NetIfaceStruct *iface, uint16_t packetID, byte *buff, uint16_t size);
byte                        ipv4_create_packet_list(struct NetIfaceStruct *iface, uint16_t packetID, byte *buff, uint16_t size);
void                        ipv4_free_packet_list(struct NetIfaceStruct *iface, uint16_t packetID);
void                        ipv4_close_packet_list(struct NetIfaceStruct *iface, uint16_t packetID);
void                        ipv4_sort_fragments(struct NetIfaceStruct *iface, uint16_t packetID);
byte                        ipv4_check_packet(struct NetIfaceStruct *iface, uint16_t packetID);

// TODO: Outgoing

#endif
