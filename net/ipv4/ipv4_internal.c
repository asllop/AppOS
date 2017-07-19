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

// TODO: use "ipv4_packet_list" everywhere

byte ipv4_exist_packet_list(struct NetIfaceStruct *iface, uint16_t packetID)
{
    for (int i = 0 ; i < NET_NUM_INCOMING_SLOTS ; i ++)
    {
        if (iface->incomingSlots[i].packetID == packetID) return YES;
    }
    
    return NO;
}

void *ipv4_add_fragment(struct NetIfaceStruct *iface, uint16_t packetID, byte *buff, uint16_t size)
{
    for (int i = 0 ; i < NET_NUM_INCOMING_SLOTS ; i ++)
    {
        if (iface->incomingSlots[i].packetID == packetID)
        {
            struct NetIncomingFrag *newFrag = (struct NetIncomingFrag *)core_malloc(sizeof(struct NetIncomingFrag));
            
            if (newFrag)
            {
                newFrag->packet = buff;
                newFrag->size = size;
                
                if (iface->incomingSlots[i].last)
                {
                    iface->incomingSlots[i].last->next = newFrag;
                }
                
                newFrag->prev = iface->incomingSlots[i].last;
                newFrag->next = NULL;
                iface->incomingSlots[i].last = newFrag;
                iface->incomingSlots[i].numFragments ++;
            }
            
            return newFrag;
        }
    }
    
    return NULL;
}

byte ipv4_create_packet_list(struct NetIfaceStruct *iface, uint16_t packetID, byte *buff, uint16_t size)
{
    for (int i = 0 ; i < NET_NUM_INCOMING_SLOTS ; i ++)
    {
        if (iface->incomingSlots[i].packetID == 0)
        {
            iface->incomingSlots[i].packetID = packetID;
            iface->incomingSlots[i].numFragments = 0;
            iface->incomingSlots[i].first = NULL;
            iface->incomingSlots[i].last = NULL;
            iface->incomingSlots[i].closed = 0;
            
            void *firstFrag = ipv4_add_fragment(iface, packetID, buff, size);
            iface->incomingSlots[i].first = firstFrag;
            
            return YES;
        }
    }
    
    return NO;
}

void ipv4_free_packet_list(struct NetIfaceStruct *iface, uint16_t packetID)
{
    for (int i = 0 ; i < NET_NUM_INCOMING_SLOTS ; i ++)
    {
        if (iface->incomingSlots[i].packetID == packetID)
        {
            struct NetIncomingFrag *nextBuff = iface->incomingSlots[i].first->next;
            struct NetIncomingFrag *freeBuff = iface->incomingSlots[i].first;
            
            iface->incomingSlots[i].packetID = 0;
            
            for (int i = 0 ; i < iface->incomingSlots[i].numFragments ; i ++)
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
}

void ipv4_close_packet_list(struct NetIfaceStruct *iface, uint16_t packetID)
{
    for (int i = 0 ; i < NET_NUM_INCOMING_SLOTS ; i ++)
    {
        if (iface->incomingSlots[i].packetID == packetID)
        {
            iface->incomingSlots[i].closed = 1;
        }
    }
}

/*
void *ipv4_peek(struct NetIfaceStruct *iface, unsigned short offset)
{
    return iface->packetQueue[iface->front - offset];
}

byte ipv4_is_empty(struct NetIfaceStruct *iface)
{
    return iface->items == 0;
}

byte ipv4_is_full(struct NetIfaceStruct *iface)
{
    return iface->items == NET_BUFFER_SLOTS;
}

int ipv4_size(struct NetIfaceStruct *iface)
{
    return iface->items;
}

void ipv4_enqueue(struct NetIfaceStruct *iface, void *data)
{
    if (!ipv4_is_full(iface))
    {
        if (iface->rear == NET_BUFFER_SLOTS - 1)
        {
            iface->rear = -1;
        }
        
        iface->packetQueue[++ iface->rear] = data;
        iface->items ++;
    }
}

void *ipv4_dequeue(struct NetIfaceStruct *iface)
{
    void *data = iface->packetQueue[iface->front ++];
    
    if (iface->front == NET_BUFFER_SLOTS)
    {
        iface->front = 0;
    }
    
    iface->items --;
    return data;
}

byte ipv4_fragment_present(struct NetIfaceStruct *iface, uint16_t packetID)
{
    if (iface->flags & NET_FRAGS_PRIMARY)
    {
        if (iface->fragOne.fragID == packetID)
        {
            return 1;
        }
    }
    else if (iface->flags & NET_FRAGS_SECONDARY)
    {
        if (iface->fragTwo.fragID == packetID)
        {
            return 2;
        }
    }
    
    return 0;
}

byte ipv4_add_existing_fragment(struct NetIfaceStruct *iface, uint16_t packetID, void *packet)
{
    if (iface->fragOne.fragID == packetID)
    {
        if (iface->fragOne.fragItems < NET_FRAGMENT_SLOTS)
        {
            iface->fragOne.fragQueue[iface->fragOne.fragItems] = packet;
            iface->fragOne.fragItems ++;
            
            return YES;
        }
    }
    else if (iface->fragTwo.fragID == packetID)
    {
        if (iface->fragTwo.fragItems < NET_FRAGMENT_SLOTS)
        {
            iface->fragTwo.fragQueue[iface->fragTwo.fragItems] = packet;
            iface->fragTwo.fragItems ++;
            
            return YES;
        }
    }
    
    return NO;
}

byte ipv4_add_new_fragment(struct NetIfaceStruct *iface, uint16_t packetID, void *packet)
{
    if (!(iface->flags & NET_FRAGS_PRIMARY))
    {
        iface->flags = iface->flags | NET_FRAGS_PRIMARY;
        iface->fragOne.fragID = packetID;
        iface->fragOne.fragQueue[0] = packet;
        iface->fragOne.fragItems = 1;
        
        return YES;
    }
    else if (!(iface->flags & NET_FRAGS_SECONDARY))
    {
        iface->flags = iface->flags | NET_FRAGS_SECONDARY;
        iface->fragTwo.fragID = packetID;
        iface->fragTwo.fragQueue[0] = packet;
        iface->fragTwo.fragItems = 1;
        
        return YES;
    }
    
    return NO;
}

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
