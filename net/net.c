#include <net/net.h>

static struct NetIfaceStruct netInterfaces[NET_NUM_INTERFACES];
static int numNetInterfaces = 0;

NETWORK net_create(NET_IFACE_TYPE type, byte id)
{
    if (numNetInterfaces < NET_NUM_INTERFACES)
    {
        netInterfaces[numNetInterfaces].type = type;
        netInterfaces[numNetInterfaces].id = id;
        netInterfaces[numNetInterfaces].flags = 0;
        netInterfaces[numNetInterfaces].queue.front = 0;
        netInterfaces[numNetInterfaces].queue.rear = -1;
        netInterfaces[numNetInterfaces].queue.items = 0;
        
        return numNetInterfaces ++;
    }
    else
    {
        return -1;
    }
}

struct NetIfaceStruct *net_iface(NETWORK net)
{
    if (net < numNetInterfaces)
    {
        return &netInterfaces[net];
    }
    else
    {
        return NULL;
    }
}
