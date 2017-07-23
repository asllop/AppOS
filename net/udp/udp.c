#include <net/ipv4/ipv4.h>
#include <net/udp/udp.h>
#include <mem/mem.h>
#include <lib/NQCLib/NQCLib.h>

// TODO: create a udp_malloc to alloc a buffer with a space at the begginig to store IP and UDP headers, this way we save a lot of memory
//       reallocating, copying and deallocating buffer when moving the different layers: Application->UDP->IP->Physical

// SOLUTION: Use core_realloc!! Modify prototype to allow adding space after or before. This way we can use the same buffer along the
// network stack.

void *udp_build(NETWORK net, byte *data, size_t len, byte dstIP[], uint16_t dstPort, uint16_t srcPort, size_t *result_size)
{
    uint16_t size = len + sizeof(struct UDP_header);
    void *packet = core_malloc(size);
    
    if (packet)
    {
        memcpy(packet + sizeof(struct UDP_header), data, len);
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
