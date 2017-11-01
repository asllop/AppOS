#include <net/net.h>
#include <net/net_internal.h>
#include <net/ipv4/ipv4.h>
#include <net/ipv4/ipv4_internal.h>
#include <mem/mem.h>
#include <task/task.h>
#include <lib/NQCLib/NQCLib.h>

// TEST:
#include <sys/sys.h>

static uint16_t ipv4PacketSequence = 0;

int ipv4_receive(NETWORK net, byte *buffer, size_t len)
{
    core_lock(MUTEX_IPV4);
    
    struct NetIfaceStruct *iface = net_iface(net);
    
    if ((buffer[0] & 0xf0) != 0x40)
    {
        core_unlock(MUTEX_IPV4);
        return ERR_CODE_BADIPVERSION;
    }
    
    byte hlen = ipv4_packet_header_len(buffer);
    
    if (hlen < 20)
    {
        core_unlock(MUTEX_IPV4);
        return ERR_CODE_BADIPHEADERSIZE;
    }
    
    uint16_t totalLen = ipv4_packet_len(buffer);
    
    if (totalLen != len)
    {
        core_unlock(MUTEX_IPV4);
        return ERR_CODE_BADIPPACKETSIZE;
    }
    
    uint16_t cksum = net_checksum(buffer, hlen);
    
    if (cksum)
    {
        core_unlock(MUTEX_IPV4);
        return ERR_CODE_BADIPCHECKSUM;
    }
    
    // Copy packet from temporal storage into a new buffer
    byte *packet = (byte *)core_malloc(len);
    if (!packet)
    {
        core_unlock(MUTEX_IPV4);
        return ERR_CODE_NOMEMFORIPPACKET;
    }
    
    memcpy(packet, buffer, len);
    
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
                
                core_unlock(MUTEX_IPV4);
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
                // Some fragment is missing, discard all
                ipv4_free_packet_list(iface, packetID);
                
                core_log("Packet Check ERROR, discard all\n");
            }
        }
        else
        {
            core_log("Not fragmented package\n");
            
            ipv4_create_packet_list(iface, packetID, packet, (uint16_t)len);
            
            ipv4_close_packet_list(iface, packetID);
        }
        
        // TEST: PRINT PACKET LIST
        struct NetFragList *packetList = ipv4_packet_list(iface, packetID);
        
        if (packetList)
        {
            if (packetList->closed)
            {
                // TEST : print offsets
                char var_str[10];
                struct NetFragList *incomList = ipv4_packet_list(iface, packetID);
                struct NetFrag *frag = incomList->first;
                
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
        // END TEST
        
    }
    
    core_unlock(MUTEX_IPV4);
    return 0;
}

void *ipv4_build(NETWORK net, byte *data, size_t len, byte protocol, byte destIP[], size_t *result_size)
{
    core_lock(MUTEX_IPV4);
    
    struct NetIfaceStruct *iface = net_iface(net);
    
    uint16_t packet_size = len + sizeof(struct IPv4_header);
    
    void *ip_packet = core_realloc(data, len, packet_size, sizeof(struct IPv4_header));
    
    if (!ip_packet)
    {
        core_unlock(MUTEX_IPV4);
        return NULL;
    }
    
    struct IPv4_header *header = ip_packet;
    
    header->versionAndHlen = 0x45;
    header->serviceType = 0;
    header->totalLength[0] = (packet_size >> 8) & 0xff;
    header->totalLength[1] = packet_size & 0xff;
    header->packetID[0] = (ipv4PacketSequence >> 8) & 0xff;
    header->packetID[1] = ipv4PacketSequence & 0xff;
    ipv4PacketSequence ++;
    header->flagsAndOffset[0] = 0;
    header->flagsAndOffset[1] = 0;
    header->ttl = 127;
    header->protocol = protocol;
    memcpy(header->source, iface->address, 4);
    memcpy(header->destination, destIP, 4);
    header->headerChecksum[0] = 0;
    header->headerChecksum[1] = 0;
    uint16_t cksum = net_checksum(ip_packet, sizeof(struct IPv4_header));
    header->headerChecksum[0] = (cksum >> 8) & 0xff;
    header->headerChecksum[1] = cksum & 0xff;
    *result_size = packet_size;
    
    core_unlock(MUTEX_IPV4);
    return ip_packet;
}

// TODO: takes a whole IP packet and generate a list of fragments depending on the MTU

struct NetFragList *ipv4_fagment(NETWORK net, byte *packet, size_t len)
{
    core_lock(MUTEX_IPV4);
    
    struct NetIfaceStruct *iface = net_iface(net);
    
    if (len > iface->mtu)
    {
        // Fragment
    }
    else
    {
        // Don't fragment
    }
    
    // TODO: generate a NetFragList with fragments and return it
    
    core_unlock(MUTEX_IPV4);
    
    return NULL;
}
