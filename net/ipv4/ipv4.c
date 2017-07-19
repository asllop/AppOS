#include <net/net.h>
#include <net/net_utils/net_utils.h>
#include <net/ipv4/ipv4.h>
#include <net/ipv4/ipv4_internal.h>
#include <mem/mem.h>

// TEST:
#include <app/utils.h>
#include <sys/sys.h>

int ipv4_receive(NETWORK net, byte *buffer, size_t len)
{
    struct NetIfaceStruct *iface = net_iface(net);
    
    if ((buffer[0] & 0xf0) != 0x40)
    {
        return ERR_CODE_BADIPVERSION;
    }
    
    byte hlen = (buffer[0] & 0xf) * 4;
    
    if (hlen < 20)
    {
        return ERR_CODE_BADIPHEADERSIZE;
    }
    
    uint16_t totalLen = ipv4_packet_len(buffer);
    
    if (totalLen != len)
    {
        return ERR_CODE_BADIPPACKETSIZE;
    }
    
    uint16_t cksum = net_checksum(buffer, hlen);
    
    if (cksum)
    {
        return ERR_CODE_BADIPCHECKSUM;
    }
    
    // Copy packet from temporal storage into a new buffer
    byte *packet = (byte *)core_malloc(len);
    if (!packet)
    {
        return ERR_CODE_NOMEMFORIPPACKET;
    }
    
    core_copy(packet, buffer, len);
    
    uint16_t packetID = packet[4] << 8 | packet[5];
    
    if (packet[6] & 0x20)
    {
        // More fragments
        
        if (ipv4_exist_packet_list(iface, packetID))
        {
            core_log("Add new fragment\n");
            
            ipv4_add_fragment(iface, packetID, packet, (uint16_t)len);
            
            // TODO: check if we have all fragments already there (reorder and close)
        }
        else
        {
            core_log("Start Fragmenting\n");
            
            if (!ipv4_create_packet_list(iface, packetID, packet, (uint16_t)len))
            {
                core_log("ERROR: couldn't find an empty packet slot");
                return ERR_CODE_NOPACKETSLOTS;
            }
        }
    }
    else
    {
        // No more fragments
        
        if (ipv4_exist_packet_list(iface, packetID))
        {
            core_log("Add last fragment\n");
            
            ipv4_add_fragment(iface, packetID, packet, (uint16_t)len);
            
            ipv4_close_packet_list(iface, packetID);
            
            // TODO: check if we have all fragments already there (reorder and close)
            
            // TEST : print offsets
            char var_str[10];
            struct NetIncomingList *incomList = ipv4_packet_list(iface, packetID);
            struct NetIncomingFrag *frag = incomList->first;
            
            for (int i = 0 ; i < incomList->numFragments ; i++)
            {
                if (frag)
                {
                    byte *packI = (byte *)frag->packet;
                    uint16_t offsetPI = ipv4_packet_offset(packI);
                    
                    core_log("Offset = ");
                    core_log(itoa(offsetPI * 8, var_str, 10));
                    core_log("\n");
                    
                    frag = frag->next;
                }
            }
        }
        else
        {
            core_log("Not fragmented package\n");
            
            ipv4_create_packet_list(iface, packetID, packet, (uint16_t)len);
            
            ipv4_close_packet_list(iface, packetID);
        }
        
        // TEST:
        if (ipv4_packet_list(iface, packetID)->closed)
        {
            core_log("Free packet list\n");
            ipv4_free_packet_list(iface, packetID);
        }
        
        /*
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
         */
    }
    
    return 0;
}

// TODO: need more options to create the IP packet: protocol
void ipv4_send(NETWORK net, byte *packet, size_t len)
{
    // TODO: according to network MTU, fragment packet, create corresponding IP headers and put them into the Outgoing queue
    
    /*
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
     */
}
