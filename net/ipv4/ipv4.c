/*
 
 int ipv4_insert(byte *packet, size_t len)
 Crea una nova entrada a la llista enllaçada de paquets. Retorna 0 si Ok o negatiu si error.
 
 int ipv4_retrieve(byte **packet, size_t *len)
 Obte el seguent packet IP de la cua. Coloca els punters corresponents del buffer. Retorna 0 si Ok o negatiu si error.
 
 */

#include <net/net.h>
#include <net/net_utils/net_utils.h>
#include <net/ipv4/ipv4_internal.h>
// TEST:
#include <app/utils.h>
#include <sys/sys.h>

int ipv4_insert(struct NetIfaceStruct *iface, byte *packet, size_t len)
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
    
    if (packet[6] & 0x20)
    {
        // TODO: more fragments
        
        if (!ipv4_is_fragmented(iface))
        {
            // TODO: start fragments
            ipv4_start_fragments(iface);
        }
    }
    else
    {
        // TODO: no fragments after
        
        if (ipv4_is_fragmented(iface))
        {
            // TODO: finished collecting fragments, reorder and put into linked list
            ipv4_end_fragments(iface);
        }
    }
    
    return 0;
}

// TODO: get the first IP packet in the buffer
int ipv4_retrieve(byte **packet, size_t *len)
{
}