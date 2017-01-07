#include <net/ipv4/ipv4_internal.h>

void *ipv4_peek(struct NetIfaceStruct *iface)
{
    return iface->packetQueue[iface->queueFront];
}

byte ipv4_is_empty(struct NetIfaceStruct *iface)
{
    return iface->queueItems == 0;
}

byte ipv4_is_full(struct NetIfaceStruct *iface)
{
    return iface->queueItems == NET_BUFFER_SLOTS;
}

int ipv4_size(struct NetIfaceStruct *iface)
{
    return iface->queueItems;
}

void ipv4_enqueue(struct NetIfaceStruct *iface, void *data)
{
    if (!ipv4_is_full(iface))
    {
        if (iface->queueRear == NET_BUFFER_SLOTS - 1)
        {
            iface->queueRear = -1;
        }
        
        iface->packetQueue[++ iface->queueRear] = data;
        iface->queueItems ++;
    }
}

void *ipv4_dequeue(struct NetIfaceStruct *iface)
{
    void *data = iface->packetQueue[iface->queueFront ++];
    
    if (iface->queueFront == NET_BUFFER_SLOTS)
    {
        iface->queueFront = 0;
    }
    
    iface->queueItems --;
    return data;
}

byte ipv4_is_fragmented(struct NetIfaceStruct *iface)
{
    return iface->flags & NET_FLAG_FRAGMENT;
}

void ipv4_start_fragments(struct NetIfaceStruct *iface)
{
    iface->flags = iface->flags | NET_FLAG_FRAGMENT;
}

void ipv4_end_fragments(struct NetIfaceStruct *iface)
{
    iface->flags = iface->flags & ~NET_FLAG_FRAGMENT;
}
