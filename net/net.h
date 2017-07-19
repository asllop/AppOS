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

#ifndef NET_NUM_INCOMING_SLOTS
#define NET_NUM_INCOMING_SLOTS  5
#endif

#ifndef NET_NUM_INTERFACES
#define NET_NUM_INTERFACES  1
#endif

#if NET_NUM_INTERFACES > 254 || NET_NUM_INTERFACES < 0
#error NET_NUM_INTERFACES must be between 0 and 254
#endif

struct NetIncomingFrag
{
    struct NetIncomingFrag  *next;
    struct NetIncomingFrag  *prev;
    byte                    *packet;
    uint16_t                size;
};

// TODO: add timestamp of last arrived packet for timeout
struct NetIncomingList
{
    uint16_t                packetID;
    uint16_t                numFragments;
    struct NetIncomingFrag  *first;
    struct NetIncomingFrag  *last;
    byte                    closed;
};

struct NetIfaceStruct
{
    NET_IFACE_TYPE          type;
    byte                    id;
    uint16_t                mtu;
    struct NetIncomingList  incomingSlots[NET_NUM_INCOMING_SLOTS];
    
    // TODO: add outgoing slots
};

NETWORK                     net_create(NET_IFACE_TYPE type, byte id);
struct NetIfaceStruct       *net_iface(NETWORK net);

#endif
