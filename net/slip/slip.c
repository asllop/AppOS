// SLIP implementation mainly taken from the RFC1055 (https://tools.ietf.org/html/rfc1055)
// Original comments preserved.

#include <net/slip/slip.h>
#include <net/slip/slip_internal.h>
#include <mem/mem.h>
#include <task/task.h>
#include <net/net.h>
#include <net/net_internal.h>
#include <net/ipv4/ipv4.h>
#include <net/ipv4/ipv4_internal.h>

// TEST
#include <lib/NQCLib/NQCLib.h>
#include <sys/sys.h>

/* SLIP special character codes
 */
#define END             0xC0    /* indicates end of packet */
#define ESC             0xDB    /* indicates byte stuffing */
#define ESC_END         0xDC    /* ESC ESC_END means END data byte */
#define ESC_ESC         0xDD    /* ESC ESC_ESC means ESC data byte */

#define SLIP_CALL_BACK_BUFFER_SIZE  500         // MTU for SLIP interfaces use to be ~300, so 500 is far enought

static PORT slip_port_num;
static byte slip_input_buf[SLIP_CALL_BACK_BUFFER_SIZE];
static TASK slipTaskID;

static void slip_receiver_task()
{
    for (;;)
    {
        if (serial_avail(slip_port_num))
        {
            int bufLen = slip_recv(slip_port_num, slip_input_buf, SLIP_CALL_BACK_BUFFER_SIZE);
            
            if (bufLen)
            {
                // TEST BEGIN
                char out[50];
                core_log("Size = ");
                itoa(bufLen, out, 10);
                core_log(out);
                core_log(" > ");
                
                for (int i = 0 ; i < bufLen ; i++)
                {
                    itoa(slip_input_buf[i], out, 16);
                    core_log(out);
                    core_log(" ");
                }
                core_log("\n---------------------------------\n");
                // TEST END
                
                // WARNING: hardcoded Network 0!
                struct NetFragList fragList;
                int res = ipv4_receive(0, slip_input_buf, bufLen, &fragList);
                
                if (res > 0)
                {
                    core_log("----> We have a complete packet!\n");
                    
                    if (!net_incomming_packet(fragList))
                    {
                        core_log("Free packet list...\n");
                        ipv4_free_packet(&fragList);
                    }
                }
                
                sprintf(out, "---> USED MEM = %u\n", core_avail(MEM_TYPE_USED));
                core_log(out);
            }
        }
        else
        {
            // Stop task until data is available
            core_stop();
        }
    }
}

// TODO: when implement resident tasks, this will be a good place to use it. Pass as argument the port so we can support multiple ports.
// and delete "slip_port_num" global variable.

static void slip_serial_callback(PORT port)
{
    core_start(slipTaskID);
}

NETWORK slip_init(PORT port, char *addr_str)
{
    slip_port_num = port;       // We only support SLIP in one port, multiple ports for future improvements
    
    NETWORK net = net_create(NET_IFACE_TYPE_SLIP, (byte)port);
    struct NetIfaceStruct *iface = net_iface(net);
    net_parse_ipv4(addr_str, iface->address);
    net_parse_ipv4("255.255.255.0", iface->mask);
    
    slipTaskID = core_create(slip_receiver_task, 0, MIN_STACK_SIZE);
    
    if (!slipTaskID)
    {
        core_fatal("Couldn't create SLIP receiver task");
    }
    
    serial_callback(port, slip_serial_callback);
    
    return net;
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
    for (;;)
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
            }
        }
    }
}
