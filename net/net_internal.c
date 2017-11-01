#include <net/net_internal.h>
#include <net/net.h>
#include <sys/sys.h>
#include <lib/NQCLib/NQCLib.h>

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
        
        for (int i = 0 ; i < NET_NUM_INCOMING_SLOTS ; i ++)
        {
            netInterfaces[numNetInterfaces].incomingSlots[i] = (struct NetFragList) {
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
