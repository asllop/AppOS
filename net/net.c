#include <net/net.h>

static struct NetIfaceStruct netInterfaces[NET_NUM_INTERFACES];
static int numNetInterfaces = 0;

NETWORK net_create(NET_IFACE_TYPE type, byte id)
{
    if (numNetInterfaces < NET_NUM_INTERFACES)
    {
        netInterfaces[numNetInterfaces].type = type;
        netInterfaces[numNetInterfaces].id = id;
        
        switch (type) {
            case NET_IFACE_TYPE_SLIP:
                netInterfaces[numNetInterfaces].mtu = 296;
                break;

            case NET_IFACE_TYPE_PPP:
                netInterfaces[numNetInterfaces].mtu = 1492;
                break;
                
            case NET_IFACE_TYPE_ETH:
            case NET_IFACE_TYPE_WIFI:
                netInterfaces[numNetInterfaces].mtu = 1500;
                break;
                
            case NET_IFACE_TYPE_BT:
                netInterfaces[numNetInterfaces].mtu = 672;
                break;
                
            default:
                netInterfaces[numNetInterfaces].mtu = 576;
                break;
        }
        
        netInterfaces[numNetInterfaces].flags = 0;
        netInterfaces[numNetInterfaces].front = 0;
        netInterfaces[numNetInterfaces].rear = -1;
        netInterfaces[numNetInterfaces].items = 0;
        
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
