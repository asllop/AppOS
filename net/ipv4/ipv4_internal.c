#include <net/ipv4/ipv4_internal.h>

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
