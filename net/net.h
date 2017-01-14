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

#if NET_NUM_INTERFACES > 254 || NET_NUM_INTERFACES < 0
#error NET_NUM_INTERFACES must be between 0 and 254
#endif

#define NET_FLAG_FRAGMENT   (1 << 0)

struct NetQueueStruct
{
    int                     front;
    int                     rear;
    int                     items;
};

struct NetIfaceStruct
{
    NET_IFACE_TYPE          type;
    byte                    id;
    byte                    flags;
    struct NetQueueStruct   queue;
    void                    *packetQueue[NET_BUFFER_SLOTS];
};

#endif
