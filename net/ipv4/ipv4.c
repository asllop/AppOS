#include <net/net.h>
#include <net/net_utils/net_utils.h>
#include <net/ipv4/ipv4.h>
#include <net/ipv4/ipv4_internal.h>
#include <mem/mem.h>

// TEST:
#include <app/utils.h>
#include <sys/sys.h>

int ipv4_receive(NETWORK net, byte *packet, size_t len)
{
    struct NetIfaceStruct *iface = net_iface(net);
    
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
    
    uint16_t packetID = packet[4] << 8 | packet[5];
    
    if (packet[6] & 0x20)
    {
        // More fragments
        
        // TODO: what if fragOne and fragTwo are both in use? Must discard one (the oldest)
        
        if (ipv4_fragment_present(iface, packetID))
        {
            core_log("Add new fragment\n");
            
            ipv4_add_existing_fragment(iface, packetID, buff);
        }
        else
        {
            core_log("Start Fragmenting\n");
            
            ipv4_add_new_fragment(iface, packetID, buff);
        }
    }
    else
    {
        // No more fragments
        
        if (ipv4_fragment_present(iface, packetID))
        {
            core_log("End Fragmenting\n");
            
            // This is the last fragment
            ipv4_add_existing_fragment(iface, packetID, buff);
            
            // Check fragments, reorder, and move to packet queue and reset fragment queue, flags, etc
            ipv4_sort_fragments(iface, packetID);
            
            
            
            // TEST : print offsets
            char var_str[10];
            struct NetFragStruct *frag = &iface->fragOne;
            for (int i = 0 ; i < frag->fragItems ; i++)
            {
                byte *packetI = (byte *)frag->fragQueue[i];
                uint16_t offsetPI = (packetI[6] & 0x1f) << 8 | packetI[7];
                
                core_log("Offset = ");
                core_log(itoa(offsetPI, var_str, 10));
                core_log("\n");
            }
        }
        else
        {
            core_log("Not fragmented package\n");
            
            // No previuous fragments, store packet in the queue
            ipv4_enqueue(iface, buff);
        }
    }
    
    return 0;
}

int ipv4_send(NETWORK net, byte **packet, size_t *len)
{
    // TODO: rework, completely wrong
    
    struct NetIfaceStruct *iface = net_iface(net);
    
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
