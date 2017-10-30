#include <net/net.h>
#include <lib/NQCLib/NQCLib.h>

static struct NetIfaceStruct netInterfaces[NET_NUM_INTERFACES];
static int numNetInterfaces = 0;

/* SOCKET INTERFACE
 
 net_resolve: obté IP d'un nom de domini utilitzant un DNS o una llista local tipus resolv.conf (implementació futura)
 -> Domain: string amb el nom
 <- Retorna: IP o error.
 
 net_raw: crea un raw socket (implementació futura)
 -> Tipus: raw-client, raw-server
 -> Adreça: Si es socket client, es refereix a l'adreça remota, si es server, es refereix a la adreça local (per seleccionar net-iface)
 <- Retorna: socket struct
 
 net_socket: crea un nou socket
 -> Tipus: udp-client, udp-server, tcp-client, tcp-server
 -> Adreça: Si es socket client, es refereix a l'adreça remota, si es server, es refereix a la adreça local (per seleccionar net-iface)
 -> Port
 <- Retorna: socket struct
 
 net_listen: escolta connexions (només servidors)
 -> Socket struct (pointer)
 <- Resultat: socket de client o error. A un servidor és aquest socket el que utilitzarem per fer open/close/send/receive i no el creat
              amb net_socket. Cada connexió nova rebuda generarà un nou socket que representa un client diferent.
 
 net_open: obre un socket
 -> Socket struct (pointer)
 <- Resultat: ok o error
 
 net_close: tanca un socket
 -> Socket struct (pointer)
 <- Resultat: ok o error
 
 net_send: envia dades per un socket ja obert
 -> Socket struct (pointer)
 -> Buffer: punter al buffer de dades
 -> Mida: mida del buffer de dades
 <- Resultat: mida dades enviades o 0 si error
 
 net_receive: reb dades d'un socket ja obert
 -> Socket struct (pointer)
 -> Buffer: punter al buffer de dades
 -> Mida: mida del buffer de dades
 <- Resultat: mida dades rebudes o 0 si error
 
 NOTA: net_listen no funciona amb servidor UDP (ni amb raw). Una possibilitat és moodificar net_send/net_receive per incloure dades del client.
 
 */

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
        
        for (int i = 0 ; i < NET_NUM_INCOMING_SLOTS ; i ++)
        {
            netInterfaces[numNetInterfaces].incomingSlots[i] = (struct NetIncomingList) {
                .packetID = 0, .numFragments = 0, .first = NULL, .last = NULL, .closed = 0
            };
        }
        
        // TODO: setup outgoing slots and other iface properties
        
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
