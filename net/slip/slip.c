// SLIP implementation mainly stolen from the RFC1055 (https://tools.ietf.org/html/rfc1055)
// Original comments preserved.

#include <net/slip/slip.h>
#include <net/slip/slip_internal.h>
#include <mem/mem.h>
#include <net/ipv4/ipv4.h>

/* SLIP special character codes
 */
#define END             0xC0    /* indicates end of packet */
#define ESC             0xDB    /* indicates byte stuffing */
#define ESC_END         0xDC    /* ESC ESC_END means END data byte */
#define ESC_ESC         0xDD    /* ESC ESC_ESC means ESC data byte */

NETWORK slip_init(PORT port)
{
    return ipv4_new_iface(NET_IFACE_TYPE_SLIP, (byte)port);
}

/* SEND_PACKET: sends a packet of length "len", starting at
 * location "p".
 */
void slip_send(PORT port, byte *p, int len)
{
    /* send an initial END character to flush out any data that may
     * have accumulated in the receiver due to line noise
     */
    slip_serial_put(port, END);
    
    /* for each byte in the packet, send the appropriate character
     * sequence
     */
    while (len--)
    {
        switch (*p)
        {
                /* if it's the same code as an END character, we send a
                 * special two character code so as not to make the
                 * receiver think we sent an END
                 */
            case END:
            {
                slip_serial_put(port, ESC);
                slip_serial_put(port, ESC_END);
                break;
            }
                
                /* if it's the same code as an ESC character,
                 * we send a special two character code so as not
                 * to make the receiver think we sent an ESC
                 */
            case ESC:
            {
                slip_serial_put(port, ESC);
                slip_serial_put(port, ESC_ESC);
                break;
            }
                
                /* otherwise, we just send the character
                 */
            default:
            {
                slip_serial_put(port, *p);
            }
        }
        
        p++;
    }
    
    /* tell the receiver that we're done sending the packet
     */
    slip_serial_put(port, END);
}

/* RECV_PACKET: receives a packet into the buffer located at "p".
 *      If more than len bytes are received, the packet will
 *      be truncated.
 *      Returns the number of bytes stored in the buffer.
 */
int slip_recv(PORT port, byte *p, int len)
{
    byte c;
    int received = 0;
    
    /* sit in a loop reading bytes until we put together
     * a whole packet.
     * Make sure not to copy them into the packet if we
     * run out of room.
     */
    while (1)
    {
        /* get a character to process
         */
        c = slip_serial_get(port);
        
        /* handle bytestuffing if necessary
         */
        switch (c)
        {
                
                /* if it's an END character then we're done with
                 * the packet
                 */
            case END:
            {
                /* a minor optimization: if there is no
                 * data in the packet, ignore it. This is
                 * meant to avoid bothering IP with all
                 * the empty packets generated by the
                 * duplicate END characters which are in
                 * turn sent to try to detect line noise.
                 */
                if (received)
                    return received;
                else
                    break;
            }
                /* if it's the same code as an ESC character, wait
                 * and get another character and then figure out
                 * what to store in the packet based on that.
                 */
            case ESC:
            {
                c = slip_serial_get(port);
                
                /* if "c" is not one of these two, then we
                 * have a protocol violation.  The best bet
                 * seems to be to leave the byte alone and
                 * just stuff it into the packet
                 */
                switch (c)
                {
                    case ESC_END:
                        c = END;
                        break;
                    case ESC_ESC:
                        c = ESC;
                        break;
                }
            }
                /* here we fall into the default handler and let
                 * it store the character for us
                 */
            default:
            {
                if (received < len)
                    p[received++] = c;
                // TODO: else, buffer overflow condition
            }
        }
    }
}
