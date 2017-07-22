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

void            *udp_build(NETWORK net, byte *data, size_t len, byte dstIP[], uint16_t dstPort, uint16_t srcPort, size_t *result_size);

#endif
