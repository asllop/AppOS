// SLIP implementation mainly taken from the RFC1055 (https://tools.ietf.org/html/rfc1055)
// Original comments preserved.

#include <net/slip/slip.h>
#include <net/slip/slip_internal.h>
#include <mem/mem.h>
#include <task/task.h>
#include <net/net.h>
#include <net/net_internal.h>

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

// TODO: we could use callback to activate a task, so we can wait for data until it arrives and not blocking other tasks
// TODO: need two new task calls: core_stop(TASK) core_start(TASK) and add a new task state (TASK_STATE_STOPPED)

static void slip_serial_task()
{
    for (;;)
    {
        if (serial_avail(slip_port_num))
        {
            int bufLen = slip_recv(slip_port_num, slip_input_buf, SLIP_CALL_BACK_BUFFER_SIZE);
            
            if (bufLen)
            {
                // TODO: store read data somewhere (IP fragment)
                
                core_log("---> Arrived SLIP data:\n");
                char out[5];
                for (int i = 0 ; i < bufLen ; i++)
                {
                    itoa(slip_input_buf[i], out, 16);
                    core_log(out);
                    core_log(" ");
                }
                core_log("\n");
            }
            else {
                core_log("Nothing read...\n");
            }
        }
        else
        {
            core_sleep(0);
        }
    }
}

/*
static void slip_serial_callback(PORT port)
{
    //if (!serial_avail(port)) return;
    
    core_log("Callback...\n");
    
    int bufLen = slip_recv(port, buf, SLIP_CALL_BACK_BUFFER_SIZE);
    
    core_log("SLIP data returned...\n");
    
    if (bufLen)
    {
        // TODO: store read data somewhere (IP fragment)
        
        core_log("---> Arrived SLIP data:\n");
        char out[5];
        for (int i = 0 ; i < bufLen ; i++)
        {
            itoa(buf[i], out, 16);
            core_log(out);
            core_log(" ");
        }
        core_log("\n");
    }
    else {
        core_log("Nothing read...\n");
    }
}
 */

NETWORK slip_init(PORT port, char *addr_str)
{
    slip_port_num = port;
    NETWORK net = net_create(NET_IFACE_TYPE_SLIP, (byte)port);
    struct NetIfaceStruct *iface = net_iface(net);
    net_parse_ipv4(addr_str, iface->address);
    net_parse_ipv4("255.255.255.0", iface->mask);
    core_create(slip_serial_task, 0, MIN_STACK_SIZE);
    //serial_callback(port, slip_serial_callback);
    
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
