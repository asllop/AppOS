#include <net/ipv4/ipv4.h>
#include <net/udp/udp.h>
#include <mem/mem.h>
#include <lib/NQCLib/NQCLib.h>

void *udp_build(NETWORK net, byte *data, size_t len, byte dstIP[], uint16_t dstPort, uint16_t srcPort, size_t *result_size)
{
    uint16_t size = len + sizeof(struct UDP_header);
    void *packet = core_realloc(data, len, size, sizeof(struct UDP_header));
    
    if (packet)
    {
        struct UDP_header *header = packet;
        header->checksum[0] = 0;
        header->checksum[1] = 0;
        header->dstPort[0] = (dstPort >> 8) & 0xff;
        header->dstPort[1] = dstPort & 0xff;
        header->srcPort[0] = (srcPort >> 8) & 0xff;
        header->srcPort[1] = srcPort & 0xff;
        header->size[0] = (size >> 8) & 0xff;
        header->size[1] = size & 0xff;
        
        void *finalPacket = ipv4_build(net, packet, size, 17, dstIP, result_size);
        core_free(packet);
        
        return finalPacket;
    }
    else
    {
        return NULL;
    }
}

// TODO: udp_receive
