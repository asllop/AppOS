#ifndef UDP_H
#define	UDP_H

#include <appos.h>

struct UDP_header
{
    byte                    srcPort[2];
    byte                    dstPort[2];
    byte                    size[2];
    byte                    checksum[2];
    
} __attribute__((packed));

static inline uint16_t udp_source_port(struct UDP_header *header)
{
    return header->srcPort[0] << 8 | header->srcPort[1];
}

static inline uint16_t udp_destination_port(struct UDP_header *header)
{
    return header->dstPort[0] << 8 | header->dstPort[1];
}

void            *udp_build(NETWORK net, byte *data, size_t len, byte dstIP[], uint16_t dstPort, uint16_t srcPort, size_t *result_size);

#endif
