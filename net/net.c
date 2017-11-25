#include <sys/sys.h>
#include <net/net.h>
#include <net/net_internal.h>
#include <net/udp/udp.h>
#include <net/ipv4/ipv4.h>
#include <lib/NQCLib/NQCLib.h>

/* TODO: SOCKET INTERFACE
 
 net_resolve: obté IP d'un nom de domini utilitzant un DNS o una llista local tipus resolv.conf (implementació futura)
 -> Domain: string amb el nom
 <- Retorna: IP o error.
 
 net_socket: crea un nou socket
 -> Tipus: udp-client, udp-server, tcp-client, tcp-server, raw-client, raw-server
 -> Adreça: Si es socket client, es refereix a l'adreça remota, si es server, es refereix a la adreça local (per seleccionar net-iface)
 -> Port (ignorat en cas de socket tipus raw)
 -> Protocol: codi de protocol. Només per a RAW, a la resta de casos és ignorat.
 <- Retorna: socket struct
 
 net_listen: escolta connexions (només servidors TCP)
 -> Socket struct (pointer)
 <- Resultat: socket de client o error. A un servidor és aquest socket el que utilitzarem per fer open/close/send/receive i no el creat
              amb net_socket. Cada connexió nova rebuda generarà un nou socket que representa un client diferent.
 
 net_open: obre un socket / accepta una connexió
 -> Socket struct (pointer)
 <- Resultat: ok o error
 
 net_close: tanca un socket / tanca una connexió
 -> Socket struct (pointer)
 <- Resultat: ok o error
 
 net_send: envia dades per un socket ja obert
 -> Socket struct (pointer)
 -> Buffer: punter al buffer de dades
 -> Mida: mida del buffer de dades
 -> Client: només per al costat servidor en connexions UDP i RAW. Struct que descriu el client receptor del paquet (IP, port)
 <- Resultat: mida dades enviades o 0 si error
 
 net_receive: reb dades d'un socket ja obert
 -> Socket struct (pointer)
 -> Buffer: punter al buffer de dades
 -> Mida: mida del buffer de dades
 <- Resultat: mida dades rebudes o 0 si error
 <- Client (punter): només per al costat servidor en connexions UDP i RAW. Struct que descriu el client emisor del paquet (IP, port)
 
 NOTA: net_listen no funciona amb servidor UDP o RAW. Per tant net_send/net_receive utilitzen el descriptor de client només per a aquests casos (socket tipus UDP/RAW servidor). Per a la resta de casos l'argument "client descriptor" és ignorat i pot ser NULL.
 
 */

struct NetSocket net_socket(NET_SOCKET_TYPE type, byte address[], uint16_t localPort, uint16_t remotePort, byte protocol)
{
    if (type == NET_SOCKET_TYPE_UDPCLIENT || type == NET_SOCKET_TYPE_UDPSERVER)
    {
        protocol = 17;
    }
    else if (type == NET_SOCKET_TYPE_TCPCLIENT || type == NET_SOCKET_TYPE_TCPSERVER)
    {
        protocol = 6;
    }
    
    struct NetSocket socket = {
        .type = type,
        .protocol = protocol,
        .localPort = localPort,
        .remotePort = remotePort,
        .state = NET_SOCKET_STATE_CLOSED,
        // TODO: find a network that matches the "address". Now we are getting the first iface for simplicity
        .network = 0
    };
    
    memcpy(socket.address, address, 4);
    
    return socket;
}

int net_open(struct NetSocket *socket)
{
    if (socket->type == NET_SOCKET_TYPE_UDPCLIENT ||
        socket->type == NET_SOCKET_TYPE_UDPSERVER ||
        socket->type == NET_SOCKET_TYPE_RAWCLIENT ||
        socket->type == NET_SOCKET_TYPE_RAWSERVER)
    {
        socket->state = NET_SOCKET_STATE_OPEN;
        return 0;
    }
    else if (socket->type == NET_SOCKET_TYPE_TCPCLIENT || socket->type == NET_SOCKET_TYPE_TCPSERVER)
    {
        // TCP not implemented yet
        return ERR_CODE_BADSOCKTYPE;
    }
    else
    {
        return ERR_CODE_BADSOCKTYPE;
    }
}

int net_close(struct NetSocket *socket)
{
    if (socket->type == NET_SOCKET_TYPE_UDPCLIENT ||
        socket->type == NET_SOCKET_TYPE_UDPSERVER ||
        socket->type == NET_SOCKET_TYPE_RAWCLIENT ||
        socket->type == NET_SOCKET_TYPE_RAWSERVER)
    {
        socket->state = NET_SOCKET_STATE_CLOSED;
        return 0;
    }
    else if (socket->type == NET_SOCKET_TYPE_TCPCLIENT || socket->type == NET_SOCKET_TYPE_TCPSERVER)
    {
        // TCP not implemented yet
        return ERR_CODE_BADSOCKTYPE;
    }
    else
    {
        return ERR_CODE_BADSOCKTYPE;
    }
}

size_t net_send(struct NetSocket *socket, struct NetClient *client, byte *data, size_t len)
{
    if (socket->type == NET_SOCKET_TYPE_UDPCLIENT ||
        socket->type == NET_SOCKET_TYPE_UDPSERVER )
    {
        size_t udp_packetLen = 0;
        byte *address = socket->type == NET_SOCKET_TYPE_UDPCLIENT ? socket->address : client->address;
        uint16_t dstPort = socket->type == NET_SOCKET_TYPE_UDPCLIENT ? socket->remotePort : client->port;
        uint16_t srcPort = socket->localPort;
        byte *upd_packet = udp_build(socket->network, data, len, address, dstPort, srcPort, &udp_packetLen);
        size_t ip_packetLen = 0;
        byte *ip_packet = ipv4_build(socket->network, upd_packet, udp_packetLen, 17, address, &ip_packetLen);
        // TODO: fragment packet and send fragments instead of the whole buffer as is
        
        core_lock(MUTEX_NET);
        net_iface_tx(socket->network, ip_packet, ip_packetLen);
        core_unlock(MUTEX_NET);
        
        return ip_packetLen;
    }
    else if (socket->type == NET_SOCKET_TYPE_RAWCLIENT ||
             socket->type == NET_SOCKET_TYPE_RAWSERVER)
    {
        size_t resultLen = 0;
        byte *address = socket->type == NET_SOCKET_TYPE_RAWCLIENT ? socket->address : client->address;
        byte *packet = ipv4_build(socket->network, data, len, socket->protocol, address, &resultLen);
        // TODO: fragment packet and send fragments instead of the whole buffer as is
        
        core_lock(MUTEX_NET);
        net_iface_tx(socket->network, packet, resultLen);
        core_unlock(MUTEX_NET);
        
        return resultLen;
    }
    else if (socket->type == NET_SOCKET_TYPE_TCPCLIENT || socket->type == NET_SOCKET_TYPE_TCPSERVER)
    {
        // TCP not implemented yet
        return 0;
    }
    else
    {
        return 0;
    }
}

// TODO
size_t net_receive(struct NetSocket *socket, struct NetClient *client, byte *data, size_t len)
{
    return 0;
}
