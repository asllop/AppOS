#ifndef NET_H
#define	NET_H

#include <appos.h>

#ifndef NET_NUM_SOCKETS
#define NET_NUM_SOCKETS     32
#endif

#ifndef NET_NUM_INCOMING_SLOTS
#define NET_NUM_INCOMING_SLOTS  5
#endif

#ifndef NET_INCOMING_QUEUE_SIZE
#define NET_INCOMING_QUEUE_SIZE 5
#endif

#ifndef NET_NUM_INTERFACES
#define NET_NUM_INTERFACES  1
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
    bool                    closed;
};

struct NetIfaceStruct
{
    NET_IFACE_TYPE          type;
    byte                    id;
    byte                    address[4];
    byte                    mask[4];
    uint16_t                mtu;
    struct NetFragList      incomingSlots[NET_NUM_INCOMING_SLOTS];
};

struct NetSocket
{
    NET_SOCKET_TYPE         type;
    byte                    address[4];
    byte                    protocol;
    uint16_t                localPort;
    uint16_t                remotePort;
    NET_SOCKET_STATE        state;
    NETWORK                 network;
    bool                    dataAvailable;
    struct NetFragList      packetQueue[NET_INCOMING_QUEUE_SIZE];
    int                     front;
    int                     rear;
    int                     packetCount;
};

// TODO: create a struct NetAddress to cover both IPv4 and IPv6 and use it instead of the "byte address[4]" array

struct NetSocket            net_socket(NET_SOCKET_TYPE type, byte address[], uint16_t localPort, uint16_t remotePort, byte protocol);
int                         net_open(struct NetSocket *socket);
int                         net_close(struct NetSocket *socket);
size_t                      net_send(struct NetSocket *socket, struct NetClient *client, byte *data, size_t len);
struct NetFragList          net_receive(struct NetSocket *socket, struct NetClient *client);
size_t                      net_size(struct NetFragList *fragList);
void                        net_free(struct NetFragList *fragList);
size_t                      net_read(struct NetFragList *fragList, size_t offset, byte *buff, size_t size);

#endif
