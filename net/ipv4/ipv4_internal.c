#include <net/ipv4/ipv4_internal.h>
#include <net/net_internal.h>
#include <mem/mem.h>

// Incoming Internal Functions

struct NetFragList *ipv4_packet_list(struct NetIfaceStruct *iface, uint16_t packetID)
{
    for (int i = 0 ; i < NET_NUM_INCOMING_SLOTS ; i ++)
    {
        if (iface->incomingSlots[i].packetID == packetID) return &iface->incomingSlots[i];
    }
    
    return NULL;
}

struct NetFragList ipv4_return_packet_list(struct NetIfaceStruct *iface, uint16_t packetID)
{
    for (int i = 0 ; i < NET_NUM_INCOMING_SLOTS ; i ++)
    {
        if (iface->incomingSlots[i].packetID == packetID)
        {
            struct NetFragList fragList = iface->incomingSlots[i];
            iface->incomingSlots[i] = (struct NetFragList) {
                .packetID = 0, .numFragments = 0, .first = NULL, .last = NULL, .closed = false
            };
            return fragList;
        }
    }
    
    // We didn't found the packet, this should be treated as an error
    return (struct NetFragList) {
        .packetID = 0, .numFragments = 0, .first = NULL, .last = NULL, .closed = false
    };
}

byte ipv4_exist_packet_list(struct NetIfaceStruct *iface, uint16_t packetID)
{
    struct NetFragList *incomList = ipv4_packet_list(iface, packetID);
    
    return incomList != NULL;
}

void *ipv4_add_fragment(struct NetIfaceStruct *iface, uint16_t packetID, byte *buff, uint16_t size)
{
    struct NetFragList *incomList = ipv4_packet_list(iface, packetID);
    
    if (incomList)
    {
        struct NetFrag *newFrag = (struct NetFrag *)core_malloc(sizeof(struct NetFrag));
        
        if (newFrag)
        {
            // Set de payload start index (IP header)
            newFrag->payload = ipv4_packet_header_len(buff);
            
            // Is first fragment, the one that contains UDP/TCP header. Update payload
            if (!incomList->last)
            {
                switch (((struct IPv4_header *)buff)->protocol)
                {
                    case UDP_PROTOCOL:
                    {
                        newFrag->payload += 8;
                        break;
                    }
                        
                    case TCP_PROTOCOL:
                    {
                        // Not implemented yet
                        break;
                    }
                        
                    default:
                    {
                        // Not implemented yet (other protocols)
                        break;
                    }
                }
            }
            
            newFrag->packet = buff;
            newFrag->size = size;
            
            if (incomList->last)
            {
                incomList->last->next = newFrag;
            }
            
            newFrag->prev = incomList->last;
            newFrag->next = NULL;
            incomList->last = newFrag;
            incomList->numFragments ++;
            
            return newFrag;
        }
    }
    
    return NULL;
}

byte ipv4_create_packet_list(struct NetIfaceStruct *iface, uint16_t packetID, byte *buff, uint16_t size)
{
    struct NetFragList *incomList = ipv4_packet_list(iface, 0);
    
    if (incomList)
    {
        incomList->packetID = packetID;
        incomList->numFragments = 0;
        incomList->first = NULL;
        incomList->last = NULL;
        incomList->closed = false;
        
        void *firstFrag = ipv4_add_fragment(iface, packetID, buff, size);
        incomList->first = firstFrag;
        
        return YES;
    }
    
    return NO;
}

void ipv4_free_packet(struct NetFragList *incomList)
{
    if (incomList)
    {
        struct NetFrag *nextBuff = incomList->first->next;
        struct NetFrag *freeBuff = incomList->first;
        
        for (int i = 0 ; i < incomList->numFragments ; i ++)
        {
            if (freeBuff)
            {
                core_free(freeBuff->packet);
                core_free(freeBuff);
            }
            
            freeBuff = nextBuff;
            
            if (nextBuff)
            {
                nextBuff = nextBuff->next;
            }
        }
        
        incomList->packetID = 0;
    }
}

void ipv4_free_packet_list(struct NetIfaceStruct *iface, uint16_t packetID)
{
    struct NetFragList *incomList = ipv4_packet_list(iface, packetID);
    ipv4_free_packet(incomList);
}

void ipv4_close_packet_list(struct NetIfaceStruct *iface, uint16_t packetID)
{
    struct NetFragList *incomList = ipv4_packet_list(iface, packetID);
    
    if (incomList)
    {
        incomList->closed = true;
    }
}

void ipv4_sort_fragments(struct NetIfaceStruct *iface, uint16_t packetID)
{
    struct NetFragList *incomList = ipv4_packet_list(iface, packetID);
    
    // TODO: sort fragments by its offset field (sort a linked list)
    
    if (incomList)
    {
        
    }
}

byte ipv4_check_packet(struct NetIfaceStruct *iface, uint16_t packetID)
{
    struct NetFragList *incomList = ipv4_packet_list(iface, packetID);
    
    ipv4_sort_fragments(iface, packetID);
    
    if (incomList)
    {
        struct NetFrag *packet = incomList->first;
        struct NetFrag *nextPacket = packet->next;
        uint16_t realDataSize = 0;
        
        while (packet != NULL && nextPacket != NULL)
        {
            realDataSize += packet->size - ipv4_packet_header_len(packet->packet);
            
            if (ipv4_packet_offset(nextPacket->packet) * 8 != realDataSize) return NO;
            
            packet = nextPacket;
            nextPacket = packet->next;
        }
    }
    
    return YES;
}
