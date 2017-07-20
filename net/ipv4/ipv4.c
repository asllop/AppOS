#include <net/net.h>
#include <net/net_utils/net_utils.h>
#include <net/ipv4/ipv4.h>
#include <net/ipv4/ipv4_internal.h>
#include <mem/mem.h>
#include <lib/NQCLib/NQCLib.h>

// TEST:
#include <sys/sys.h>

int ipv4_receive(NETWORK net, byte *buffer, size_t len)
{
    struct NetIfaceStruct *iface = net_iface(net);
    
    if ((buffer[0] & 0xf0) != 0x40)
    {
        return ERR_CODE_BADIPVERSION;
    }
    
    byte hlen = ipv4_packet_header_len(buffer);
    
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
            
            // TODO: check if we have all fragments already there (reorder and close) -> necessary only in case arrived last frag and someone is missing
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
            
            if (ipv4_check_packet(iface, packetID))
            {
                core_log("Packet Check OK\n");
            }
            else
            {
                // TODO: some fragment is missing, discard all?
                core_log("Packet Check ERROR\n");
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
            
            core_log("Free packet list\n");
            ipv4_free_packet_list(iface, packetID);
        }
    }
    
    return 0;
}

// TODO: need more options to create the IP packet: protocol
void ipv4_send(NETWORK net, byte *packet, size_t len)
{
    struct NetIfaceStruct *iface = net_iface(net);
    
    // TODO: according to network MTU, fragment packet, create corresponding IP headers and put them into the Outgoing queue
}

// TODO: ipv4_build_packet(byte *data, size_t len, destIP, orgIP, protocol)
