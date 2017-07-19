#include <net/ipv4/ipv4_internal.h>
#include <mem/mem.h>

// Incoming Internal Functions

struct NetIncomingList *ipv4_packet_list(struct NetIfaceStruct *iface, uint16_t packetID)
{
    for (int i = 0 ; i < NET_NUM_INCOMING_SLOTS ; i ++)
    {
        if (iface->incomingSlots[i].packetID == packetID) return &iface->incomingSlots[i];
    }
    
    return NULL;
}

byte ipv4_exist_packet_list(struct NetIfaceStruct *iface, uint16_t packetID)
{
    struct NetIncomingList *incomList = ipv4_packet_list(iface, packetID);
    
    return incomList != NULL;
}

void *ipv4_add_fragment(struct NetIfaceStruct *iface, uint16_t packetID, byte *buff, uint16_t size)
{
    struct NetIncomingList *incomList = ipv4_packet_list(iface, packetID);
    
    if (incomList)
    {
        struct NetIncomingFrag *newFrag = (struct NetIncomingFrag *)core_malloc(sizeof(struct NetIncomingFrag));
        
        if (newFrag)
        {
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
    struct NetIncomingList *incomList = ipv4_packet_list(iface, 0);
    
    if (incomList)
    {
        incomList->packetID = packetID;
        incomList->numFragments = 0;
        incomList->first = NULL;
        incomList->last = NULL;
        incomList->closed = 0;
        
        void *firstFrag = ipv4_add_fragment(iface, packetID, buff, size);
        incomList->first = firstFrag;
        
        return YES;
    }
    
    return NO;
}

void ipv4_free_packet_list(struct NetIfaceStruct *iface, uint16_t packetID)
{
    
    struct NetIncomingList *incomList = ipv4_packet_list(iface, packetID);
    
    if (incomList)
    {
        struct NetIncomingFrag *nextBuff = incomList->first->next;
        struct NetIncomingFrag *freeBuff = incomList->first;
        
        incomList->packetID = 0;
        
        for (int i = 0 ; i < incomList->numFragments ; i ++)
        {
            if (freeBuff)
            {
                core_free(freeBuff);
            }
            
            freeBuff = nextBuff;
            
            if (nextBuff)
            {
                nextBuff = nextBuff->next;
            }
        }
    }
}

void ipv4_close_packet_list(struct NetIfaceStruct *iface, uint16_t packetID)
{
    struct NetIncomingList *incomList = ipv4_packet_list(iface, packetID);
    
    if (incomList)
    {
        incomList->closed = 1;
    }
}

void ipv4_sort_fragments(struct NetIfaceStruct *iface, uint16_t packetID)
{
    struct NetIncomingList *incomList = ipv4_packet_list(iface, packetID);
    
    // TODO: sort fragments by its offset field (sort a linked list)
    
    if (incomList)
    {
        
    }
}

byte ipv4_check_packet(struct NetIfaceStruct *iface, uint16_t packetID)
{
    struct NetIncomingList *incomList = ipv4_packet_list(iface, packetID);
    
    ipv4_sort_fragments(iface, packetID);
    
    if (incomList)
    {
        struct NetIncomingFrag *packet = incomList->first;
        struct NetIncomingFrag *nextPacket = packet->next;
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

/*
byte ipv4_sort_fragments(struct NetIfaceStruct *iface, uint16_t packetID)
{
    struct NetFragStruct *frag;
    byte fragNum = 0;
    
    switch (ipv4_fragment_present(iface, packetID))
    {
        case 1:
            frag = &iface->fragOne;
            fragNum = 1;
            break;
            
        case 2:
            frag = &iface->fragTwo;
            fragNum = 2;
            break;
            
        default:
            return NO;
    }
    
    // Sort fragments
    for (int i = 0 ; i < frag->fragItems ; i++)
    {
        for (int j = i + 1 ; j < frag->fragItems ; j++)
        {
            // Get packet offset on pos i and j
            byte *packetI = (byte *)frag->fragQueue[i];
            byte *packetJ = (byte *)frag->fragQueue[j];
            uint16_t offsetPI = (packetI[6] & 0x1f) << 8 | packetI[7];
            uint16_t offsetPJ = (packetJ[6] & 0x1f) << 8 | packetJ[7];
            
            if (offsetPI > offsetPJ)
            {
                // Move packetI to J pos and vice versa
                frag->fragQueue[i] = (void *)packetJ;
                frag->fragQueue[j] = (void *)packetI;
            }
        }
    }
    
    // TODO: check for gaps, els offsets son del paquet original, per tant cal sumar la mida del header IP de cada fragment
    
    // TODO: if all ok, move to main queue
    
    // TODO: reset flags and "frag" state (items, ID, etc)
    
    return YES;
}
*/
