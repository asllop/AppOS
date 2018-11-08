#include <net/net_internal.h>
#include <net/net.h>
#include <net/ipv4/ipv4_internal.h>
#include <net/udp/udp.h>
#include <sys/sys.h>
#include <net/slip/slip.h>
#include <task/task.h>
#include <lib/NQCLib/NQCLib.h>

// TODO: make it dynamic, se allocated memory instead of a static array
struct NetIfaceStruct   netInterfaces[NET_NUM_INTERFACES];
int                     numNetInterfaces = 0;
struct NetSocket *      netRegisteredSockets[NET_NUM_SOCKETS];

NETWORK net_create(NET_IFACE_TYPE type, byte id)
{
    if (numNetInterfaces < NET_NUM_INTERFACES)
    {
        netInterfaces[numNetInterfaces].type = type;
        netInterfaces[numNetInterfaces].id = id;
        
        switch (type)
        {
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
        
        for (int i = 0 ; i < NET_NUM_INCOMING_SLOTS ; i ++)
        {
            netInterfaces[numNetInterfaces].incomingSlots[i] = (struct NetFragList) {
                .packetID = 0, .numFragments = 0, .first = NULL, .last = NULL, .closed = false
            };
        }
        
        for (int i = 0 ; i < NET_NUM_SOCKETS ; i ++)
        {
            netRegisteredSockets[i] = NULL;
        }
        
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

void net_parse_ipv4(char *addr_str, byte address[])
{
    // Parse an IP address: AAA.BBB.CCC.DDD
    char num[4];
    int index = 0;
    
    for (int ipIndex = 0 ; ipIndex < 4 ; ipIndex ++)
    {
        // Parse component of IP address
        for (int i = 0 ; i < 4 ; i ++)
        {
            // End Of String
            if (addr_str[index] == '\0')
            {
                break;
            }
            
            // Found end component
            if (addr_str[index] == '.')
            {
                index ++;
                break;
            }
            
            num[i] = addr_str[index];
            num[i + 1] = 0;
            
            index ++;
        }
        
        address[ipIndex] = (byte)atoi(num);
    }
}

uint16_t net_checksum(byte *p, int len)
{
    int32_t sum = 0;
    
    for (int i = 0 ; i < len ; i += 2)
    {
        // Summatory of 16 bits unsigned integers, big endian
        uint16_t word = p[i] << 8 | p[i + 1];
        sum += word;
    }
    
    // Add carry two times
    sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
    sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
    
    // One's complement
    return ~(uint16_t)(sum & 0xffff);
}

void net_iface_tx(NETWORK net, byte *data, size_t len)
{
    struct NetIfaceStruct *iface = net_iface(net);
    // TODO: create a Driver Abstraction Layer (like term) to register transport senders. This way we can
    // compile only the modules we need, otherwise we need to compile all of them even if we only use one.
    switch (iface->type)
    {
        case NET_IFACE_TYPE_SLIP:
            slip_send(iface->id, data, (int)len);
            break;
            
        case NET_IFACE_TYPE_PPP:
            // Not yet implemented
            break;
            
        case NET_IFACE_TYPE_ETH:
            // Not yet implemented
            break;
            
        case NET_IFACE_TYPE_WIFI:
            // Not yet implemented
            break;
            
        case NET_IFACE_TYPE_BT:
            // Not yet implemented
            break;
    }
}

bool net_assign_socket(struct NetSocket *socket)
{
    for (int sockIndex = 0 ; sockIndex < NET_NUM_SOCKETS ; sockIndex ++)
    {
        if (netRegisteredSockets[sockIndex] == NULL)
        {
            netRegisteredSockets[sockIndex] = socket;
            return true;
        }
    }
    
    return false;
}

bool net_release_socket(struct NetSocket *socket)
{
    for (int sockIndex = 0 ; sockIndex < NET_NUM_SOCKETS ; sockIndex ++)
    {
        if (netRegisteredSockets[sockIndex] == socket)
        {
            netRegisteredSockets[sockIndex] = NULL;
            return true;
        }
    }
    
    return false;
}

bool net_incomming_packet(struct NetFragList fragList)
{
    if (fragList.first->size < sizeof(struct IPv4_header))
    {
        core_log("Bad IP header\n");
        // No enought space for an IPv4 header
        return false;
    }
    
    struct IPv4_header *headerIP = (struct IPv4_header *)fragList.first->packet;
    
    for (int sockIndex = 0 ; sockIndex < NET_NUM_SOCKETS ; sockIndex ++)
    {
        core_log("Next socket...\n");
        
        if (netRegisteredSockets[sockIndex] != NULL)
        {
            if (netRegisteredSockets[sockIndex]->protocol == headerIP->protocol)
            {
                // UDP
                if (headerIP->protocol == 17)
                {
                    if (fragList.first->size < sizeof(struct UDP_header) + ipv4_packet_header_len(fragList.first->packet))
                    {
                        core_log("Bad UDP header\n");
                        // No enought space for an UDP header
                        return false;
                    }
                    
                    struct UDP_header *headerUDP = (struct UDP_header *)(fragList.first->packet + ipv4_packet_header_len(fragList.first->packet));
                    uint16_t dstPort = udp_destination_port(headerUDP);
                    uint16_t srcPort = udp_source_port(headerUDP);

                    if (netRegisteredSockets[sockIndex]->type == NET_SOCKET_TYPE_UDPCLIENT)
                    {
                        core_log("Found UDP Client\n");
                        
                        if (netRegisteredSockets[sockIndex]->localPort == dstPort &&
                            netRegisteredSockets[sockIndex]->remotePort == srcPort)
                        {
                            core_log("Found UDP Client correct ports\n");
                            
                            if (memcmp((void *)netRegisteredSockets[sockIndex]->address, (void *)headerIP->source, 4) == 0)
                            {
                                core_log("--> Found UDP client socket!!!\n");
                                
                                // found the correct socket to attach packet
                                if (net_insert_packet(netRegisteredSockets[sockIndex], fragList))
                                {
                                    netRegisteredSockets[sockIndex]->dataAvailable = true;
                                    
                                    return true;
                                }
                                else
                                {
                                    return false;
                                }
                            }
                        }
                    }
                    else if (netRegisteredSockets[sockIndex]->type == NET_SOCKET_TYPE_UDPSERVER)
                    {
                        core_log("Found UDP Server\n");
                        
                        if (netRegisteredSockets[sockIndex]->localPort == dstPort)
                        {
                            core_log("--> Found UDP server socket!!!\n");
                            
                            if (net_insert_packet(netRegisteredSockets[sockIndex], fragList))
                            {
                                netRegisteredSockets[sockIndex]->dataAvailable = true;
                                
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        }
                    }
                }
                else
                {
                    // TODO: implement RAW and other protocols
                    
                    core_log("No UDP packet\n");
                }
            }
        }
    }
    
    core_log("Not Found socket!\n");
    
    return false;
}

// TEST
void printQueue(struct NetSocket *socket)
{
    char out[100];
    
    core_log("---------------------\n");
    //sprintf(out, "Front = %d, Read = %d, Count = %d\n\n", socket->front, socket->rear, socket->packetCount);
    for (int i = socket->front ; i < socket->front + socket->packetCount ; i ++)
    {
        sprintf(out, "Queue pos = %d , Packet ID = %d\n", i, socket->packetQueue[i].packetID);
        core_log(out);
    }
    core_log("---------------------\n");
}
// ENDTEST

bool net_insert_packet(struct NetSocket *socket, struct NetFragList fragList)
{
    bool res;
    
    if (socket->packetCount != NET_INCOMING_QUEUE_SIZE)
    {
        if (socket->rear == NET_INCOMING_QUEUE_SIZE - 1)
        {
            socket->rear = -1;
        }
        
        socket->packetQueue[++ socket->rear] = fragList;
        socket->packetCount ++;
        
        res = true;
    }
    else
    {
        res = false;
    }
    
    // TEST
    core_log("### INSERT PACKET\n");
    printQueue(socket);
    
    return res;
}

struct NetFragList net_extract_packet(struct NetSocket *socket)
{
    if (socket->packetCount == 0)
    {
        return (struct NetFragList) {
            .packetID = 0, .numFragments = 0, .first = NULL, .last = NULL, .closed = false
        };
    }
    
    struct NetFragList data = socket->packetQueue[socket->front ++];
    
    if (socket->front == NET_INCOMING_QUEUE_SIZE)
    {
        socket->front = 0;
    }
    
    socket->packetCount --;
    
    // TEST
    core_log("### EXTRACT PACKET\n");
    printQueue(socket);
    
    return data;
}

void net_read_task()
{
    struct NetSocket *socket = (struct NetSocket *)core_userdata();
    struct NetClient client;
    
    for (;;)
    {
        core_log("net_read_task(): Wait for packet\n");
        
        struct NetClient *clientPtr = NULL;
        
        // TCP not implemented yet
        if (socket->type == NET_SOCKET_TYPE_UDPSERVER ||
            socket->type == NET_SOCKET_TYPE_RAWSERVER)
        {
            clientPtr = &client;
        }
        else
        {
            clientPtr = NULL;
            client = (struct NetClient){.address = {0,0,0,0}, .port = 0};
        }
        
        // Get packet (list of fragments) and dettach from socket
        struct NetFragList fragList = net_receive(socket, clientPtr);
        
        core_log("net_read_task(): Receiuved packet\n");
        
        // Empty fragment, socket closed -> Abort task
        if (fragList.packetID == 0 && fragList.numFragments == 0) break;
        
        // Run user callback
        socket->readCallback(socket, fragList, client);
    }
    
    core_log("FINISHED net_read_task()");
}

struct NetFragList net_receive(struct NetSocket *socket, struct NetClient *client)
{
    socket->dataAvailable = false;
    
    core_log("net_receive(): started\n");
    
    if (socket->packetCount == 0)
    {
        core_log("net_receive(): packetCount == 0\n");
        
        while (!socket->dataAvailable)
        {
            core_sleep(0);
            
            if (socket->state == NET_SOCKET_STATE_CLOSED)
            {
                core_log("net_receive(): socket closed\n");
                return (struct NetFragList) {
                    .packetID = 0,
                    .numFragments = 0
                };
            }
        }
    }
    
    core_log("net_receive(): return packet received\n");
    
    socket->dataAvailable = false;
    
    struct NetFragList fragList = net_extract_packet(socket);
    
    if (client)
    {
        if (fragList.first->packet)
        {
            // fill client's address
            struct IPv4_header *ipHeader = (struct IPv4_header *)fragList.first->packet;
            memcpy(client->address, ipHeader->source, 4);
            
            if (socket->type == NET_SOCKET_TYPE_UDPSERVER)
            {
                // fill client's port
                struct UDP_header *udpHeader = (struct UDP_header *)(fragList.first->packet + sizeof(struct IPv4_header));
                client->port = udp_source_port(udpHeader);
            }
            
            // TCP not implemented yet
        }
    }
    
    return fragList;
}
