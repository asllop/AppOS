#include <appos.h>

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
