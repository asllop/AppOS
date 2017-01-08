#include <net/net.h>
#include <net/net_utils/net_utils.h>
#include <net/ipv4/ipv4.h>
#include <net/ipv4/ipv4_internal.h>
#include <mem/mem.h>

// TEST:
#include <app/utils.h>
#include <sys/sys.h>

static struct NetIfaceStruct netInterfaces[NET_NUM_INTERFACES];
static int numNetInterfaces = 0;

NETWORK ipv4_new_iface(NET_IFACE_TYPE type, byte id)
{
    if (numNetInterfaces < NET_NUM_INTERFACES)
    {
        netInterfaces[numNetInterfaces].type = type;
        netInterfaces[numNetInterfaces].id = id;
        netInterfaces[numNetInterfaces].flags = 0;
        netInterfaces[numNetInterfaces].queueFront = 0;
        netInterfaces[numNetInterfaces].queueRear = -1;
        netInterfaces[numNetInterfaces].queueItems = 0;
        
        return numNetInterfaces ++;
    }
    else
    {
        return -1;
    }
}

int ipv4_insert(NETWORK net, byte *packet, size_t len)
{
    if ((packet[0] & 0xf0) != 0x40)
    {
        return ERR_CODE_BADIPVERSION;
    }
    
    byte hlen = (packet[0] & 0xf) * 4;
    
    if (hlen < 20)
    {
        return ERR_CODE_BADIPHEADERSIZE;
    }
    
    uint16_t totalLen = packet[2] << 8 | packet[3];
    
    if (totalLen != len)
    {
        return ERR_CODE_BADIPPACKETSIZE;
    }
    
    uint16_t cksum = net_checksum(packet, hlen);
    
    if (cksum)
    {
        return ERR_CODE_BADIPCHECKSUM;
    }
    
    // Copy packet from temporal storage into a new buffer
    void *buff = core_malloc(len);
    if (!buff)
    {
        return ERR_CODE_NOMEMFORIPPACKET;
    }
    
    core_copy(buff, packet, len);
    
    // Store packet in the queue
    struct NetIfaceStruct *iface = ipv4_network(net);
    ipv4_enqueue(iface, buff);
    
    if (packet[6] & 0x20)
    {
        core_log("More fragments\n");
        
        if (!ipv4_is_fragmented(iface))
        {
            core_log("Start fragmenting\n");

            ipv4_start_fragments(iface);
        }
    }
    else
    {
        core_log("NO more fragments\n");
        
        if (ipv4_is_fragmented(iface))
        {
            core_log("End fragmenting\n");
            
            ipv4_end_fragments(iface);
            
            // TODO: check fragments arrived, order and identification (must be the same in all fragments)
        }
    }
    
    return 0;
}

int ipv4_retrieve(NETWORK net, byte **packet, size_t *len)
{
    struct NetIfaceStruct *iface = ipv4_network(net);
    
    if (!ipv4_is_empty(iface))
    {
        byte *buff = (byte *)ipv4_dequeue(iface);
        uint16_t totalLen = buff[2] << 8 | buff[3];
        *packet = buff;
        *len = (size_t)totalLen;
        
        return 0;
    }
    else
    {
        return ERR_CODE_IPBUFFEREMPTY;
    }
}

struct NetIfaceStruct *ipv4_network(NETWORK net)
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
