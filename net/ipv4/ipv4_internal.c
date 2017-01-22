#include <net/ipv4/ipv4_internal.h>

// TODO: use NetIfaceStruct again as argument insteaf of NetQueueStruct.

void *ipv4_peek(struct NetQueueStruct *queue, void **bufQueue, unsigned short offset)
{
    return bufQueue[queue->front - offset];
}

byte ipv4_is_empty(struct NetQueueStruct *queue)
{
    return queue->items == 0;
}

byte ipv4_is_full(struct NetQueueStruct *queue)
{
    return queue->items == NET_BUFFER_SLOTS;
}

int ipv4_size(struct NetQueueStruct *queue)
{
    return queue->items;
}

void ipv4_enqueue(struct NetQueueStruct *queue, void **bufQueue, void *data)
{
    if (!ipv4_is_full(queue))
    {
        if (queue->rear == NET_BUFFER_SLOTS - 1)
        {
            queue->rear = -1;
        }
        
        bufQueue[++ queue->rear] = data;
        queue->items ++;
    }
}

void *ipv4_dequeue(struct NetQueueStruct *queue, void **bufQueue)
{
    void *data = bufQueue[queue->front ++];
    
    if (queue->front == NET_BUFFER_SLOTS)
    {
        queue->front = 0;
    }
    
    queue->items --;
    return data;
}

byte ipv4_fragment_present(struct NetIfaceStruct *iface, uint16_t packetID)
{
    if (iface->flags & NET_FRAGS_PRIMARY)
    {
        if (iface->primaryFragsID == packetID)
        {
            return YES;
        }
    }
    else if (iface->flags & NET_FRAGS_SECONDARY)
    {
        if (iface->secondaryFragsID == packetID)
        {
            return YES;
        }
    }
    
    return NO;
}

byte ipv4_add_existing_fragment(struct NetIfaceStruct *iface, uint16_t packetID, void *packet)
{
    if (iface->primaryFragsID == packetID)
    {
        if (iface->primaryFragsItems < NET_FRAGMENT_SLOTS)
        {
            iface->primaryFrags[iface->primaryFragsItems] = packet;
            iface->primaryFragsItems ++;
            
            return YES;
        }
    }
    else if (iface->secondaryFragsID == packetID)
    {
        if (iface->secondaryFragsItems < NET_FRAGMENT_SLOTS)
        {
            iface->secondaryFrags[iface->secondaryFragsItems] = packet;
            iface->secondaryFragsItems ++;
            
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
        iface->primaryFragsID = packetID;
        iface->primaryFrags[0] = packet;
        iface->primaryFragsItems = 1;
        
        return YES;
    }
    else if (!(iface->flags & NET_FRAGS_SECONDARY))
    {
        iface->flags = iface->flags | NET_FRAGS_SECONDARY;
        iface->secondaryFragsID = packetID;
        iface->secondaryFrags[0] = packet;
        iface->secondaryFragsItems = 1;
        
        return YES;
    }
    
    return NO;
}
