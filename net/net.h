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

#ifndef NET_FRAGMENT_SLOTS
#define NET_FRAGMENT_SLOTS  32
#endif

#ifndef NET_NUM_INTERFACES
#define NET_NUM_INTERFACES  1
#endif

#if NET_NUM_INTERFACES > 254 || NET_NUM_INTERFACES < 0
#error NET_NUM_INTERFACES must be between 0 and 254
#endif

#define NET_FRAGS_PRIMARY   (1 << 0)
#define NET_FRAGS_SECONDARY (1 << 1)

struct NetFragStruct
{
    uint16_t                fragID;
    byte                    fragItems;
    void                    *fragQueue[NET_FRAGMENT_SLOTS];
};

struct NetIfaceStruct
{
    NET_IFACE_TYPE          type;
    byte                    id;
    uint16_t                mtu;
    byte                    flags;
    int                     front;
    int                     rear;
    int                     items;
    void                    *packetQueue[NET_BUFFER_SLOTS];
    struct NetFragStruct    fragOne;
    struct NetFragStruct    fragTwo;
};

NETWORK                     net_create(NET_IFACE_TYPE type, byte id);
struct NetIfaceStruct       *net_iface(NETWORK net);

#endif
