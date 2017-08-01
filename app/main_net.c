#include <mem/mem.h>
#include <sys/sys.h>
#include <task/task.h>
#include <serial/serial.h>
#include <net/slip/slip.h>
#include <term/term.h>
#include <term/drivers/term_serial/term_serial.h>
#include <net/ipv4/ipv4.h>
#include <net/udp/udp.h>
#include <lib/NQCLib/NQCLib.h>
#include "utils.h"

void printPacket(TERM termID, byte *packet, size_t packet_len)
{
    char var_str[10];
    
    term_puts(termID, "Packet len = ");
    term_puts(termID, ltoa(packet_len, var_str, 10));
    term_puts(termID, "\n");
    
    term_puts(termID, "Packet contents = \n");
    int linecnt = 0;
    for (int i = 0 ; i < packet_len ; i ++)
    {
        term_puts(termID, itoa(packet[i], var_str, 16));
        term_puts(termID, " ");
        if (linecnt == 3)
        {
            term_puts(termID, "\n");
            linecnt = 0;
        }
        else
        {
            linecnt ++;
        }
    }
    
    term_puts(termID, "\n");
}

void main(__unused int argc, __unused char **argv)
{
	char var_str[20];
    
    // Draw Background
    console_put_data(0x1b, (char)176, 80*25, 0);
	console_put_string(0x4f, " NET TEST ", 34, 1);
    
    // Setup serial ports 0 and 1
    
    if (serial_init(0, SERIAL_DATA_8b, SERIAL_PARITY_NONE, SERIAL_STOP_1b, 9600))
    {
        core_fatal("Error setting up serial device 0");
    }
    
    if (serial_init(1, SERIAL_DATA_8b, SERIAL_PARITY_NONE, SERIAL_STOP_1b, 9600))
    {
        core_fatal("Error setting up serial device 1");
    }
    
    // Init TERM in serial port 0
    TERM termID = term_serial_init(0);
    // Init SLIP in serial port 1
    PORT slip = 1;
    
    NETWORK net = slip_init(slip, "192.168.1.2");
    
    if (termID == -1)
    {
        core_fatal("Term serial driver failed to init");
    }
    
    if (net == (NETWORK)-1)
    {
        core_fatal("Error initializing SLIP Network Interface");
    }
    
    size_t packet_len;
    byte destIP[] = {192,168,1,1};
    char *hola = core_malloc(100);
    if (!hola) core_fatal("Could no alloc hola");
    int hola_len = 10;
    memcpy(hola, "Hola amic", hola_len);
    
    //byte *packet = ipv4_build(net, (byte *)hola, hola_len, 150, destIP, &packet_len);
    byte *packet = udp_build(net, (byte *)hola, hola_len, destIP, 1500, 25000, &packet_len);
    
    printPacket(termID, packet, packet_len);
    
    // MTU of SLIP interfaces in Linux use to be 300 bytes aprox, so 500 is far enough
    byte *inBuff = (byte *)core_malloc(500);
    int buffSz = (int)core_size(inBuff);
    
    term_puts(termID, "Waiting packets...\n");
    
    while (1)
    {
        int sz = slip_recv(slip, inBuff, buffSz);
        
        term_puts(termID, "Received packet of size ");
        term_puts(termID, itoa(sz, var_str, 10));
        term_puts(termID, "\n");
        
        int res = ipv4_receive(net, inBuff, sz);
        
        if (res < 0)
        {
            term_puts(termID, "Error = ");
            term_puts(termID, itoa(res, var_str, 10));
            term_puts(termID, "\n");
        }
        else
        {
            term_puts(termID, "Correct IP packet\n");
        }
        
        if (packet)
        {
            term_puts(termID, "Send packet...\n");
            printPacket(termID, packet, packet_len);
            slip_send(slip, packet, (int)packet_len);
        }
        else
        {
            term_puts(termID, "Error generating packet to send\n");
        }
    }
}
