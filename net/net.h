#ifndef NET_H
#define	NET_H

#include <appos.h>

#ifndef NET_NUM_INCOMING_SLOTS
#define NET_NUM_INCOMING_SLOTS  5
#endif

#ifndef NET_NUM_INTERFACES
#define NET_NUM_INTERFACES  1
#endif

#ifndef NET_MAX_NUM_PORTS
#define NET_MAX_NUM_PORTS   10
#endif

#ifndef NET_MIN_CLIENT_PORT_NUMBER
#define NET_MIN_CLIENT_PORT_NUMBER  32000
#endif

#if NET_NUM_INTERFACES > 254 || NET_NUM_INTERFACES < 0
#error NET_NUM_INTERFACES must be between 0 and 254
#endif

typedef unsigned char       NETWORK;

typedef enum
{
    NET_IFACE_TYPE_ETH,
    NET_IFACE_TYPE_WIFI,
    NET_IFACE_TYPE_SLIP,
    NET_IFACE_TYPE_PPP,
    NET_IFACE_TYPE_BT
    
} NET_IFACE_TYPE;

typedef enum
{
    NET_SOCKET_TYPE_RAWSERVER,
    NET_SOCKET_TYPE_RAWCLIENT,
    NET_SOCKET_TYPE_UDPSERVER,
    NET_SOCKET_TYPE_UDPCLIENT,
    NET_SOCKET_TYPE_TCPSERVER,
    NET_SOCKET_TYPE_TCPCLIENT
    
} NET_SOCKET_TYPE;

typedef enum
{
    NET_SOCKET_STATE_CLOSED,
    NET_SOCKET_STATE_OPEN,
    NET_SOCKET_STATE_CONNECTING,
    NET_SOCKET_STATE_CONNECTED
    
} NET_SOCKET_STATE;

struct NetSocket
{
    NET_SOCKET_TYPE         type;
    byte                    address[4];
    byte                    protocol;
    uint16_t                port;
    uint16_t                localPort;
    NET_SOCKET_STATE        state;
    NETWORK                 network;
};

struct NetClient
{
    byte                    address[4];
    uint16_t                port;
};

struct NetFrag
{
    struct NetFrag *        next;
    struct NetFrag *        prev;
    byte *                  packet;
    uint16_t                size;
};

// TODO: add timestamp of last arrived packet for timeout
struct NetFragList
{
    uint16_t                packetID;
    uint16_t                numFragments;
    struct NetFrag *        first;
    struct NetFrag *        last;
    byte                    closed;
};

struct NetIfaceStruct
{
    NET_IFACE_TYPE          type;
    byte                    id;
    byte                    address[4];
    byte                    mask[4];
    uint16_t                mtu;
    struct NetFragList      incomingSlots[NET_NUM_INCOMING_SLOTS];
    
    // TODO: add outgoing slots
};

// TODO: create a struct NetAddress to cover both IPv4 and IPv6 and use it instead of the "byte address[4]" array

void                        net_init();
NETWORK                     net_create(NET_IFACE_TYPE type, byte id);
struct NetIfaceStruct *     net_iface(NETWORK net);
void                        net_parse_ipv4(char *addr_str, byte address[]);

#endif
