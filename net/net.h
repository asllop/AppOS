#ifndef NET_H
#define	NET_H

#include <appos.h>

typedef enum
{
    NET_IFACE_TYPE_ETH      = 0,
    NET_IFACE_TYPE_WIFI,
    NET_IFACE_TYPE_SLIP,
    NET_IFACE_TYPE_PPP,
    NET_IFACE_TYPE_BT
    
} NET_IFACE_TYPE;

#ifndef NET_BUFFER_SLOTS
#define NET_BUFFER_SLOTS    128
#endif

#ifndef NET_NUM_INTERFACES
#define NET_NUM_INTERFACES  1
#endif

#if NET_NUM_INTERFACES > 254
#error NET_NUM_INTERFACES must be smaller than 255
#endif

#define NET_FLAG_FRAGMENT   (1 << 0)

struct NetIfaceStruct
{
    NET_IFACE_TYPE          type;
    byte                    id;
    byte                    flags;
    int                     queueFront;
    int                     queueRear;
    int                     queueItems;
    void                    *packetQueue[NET_BUFFER_SLOTS];
};

#endif
