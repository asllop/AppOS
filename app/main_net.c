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

void printPacket(TERM term, byte *packet, size_t packet_len)
{
    char var_str[10];
    
    term_puts(term, "Packet len = ");
    term_puts(term, ltoa(packet_len, var_str, 10));
    term_puts(term, "\n");
    
    term_puts(term, "Packet contents = \n");
    int linecnt = 0;
    for (int i = 0 ; i < packet_len ; i ++)
    {
        term_puts(term, itoa(packet[i], var_str, 16));
        term_puts(term, " ");
        if (linecnt == 3)
        {
            term_puts(term, "\n");
            linecnt = 0;
        }
        else
        {
            linecnt ++;
        }
    }
    
    term_puts(term, "\n");
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
    TERM term = term_serial_init(0);
    
    // Init SLIP in serial port 1
    NETWORK net = slip_init(1, "192.168.1.2");
    
    if (term == -1)
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
    
    printPacket(term, packet, packet_len);
    
    // MTU of SLIP interfaces in Linux use to be 300 bytes aprox, so 500 is far enough
    byte *inBuff = (byte *)core_malloc(500);
    int buffSz = (int)core_size(inBuff);
    
    term_puts(term, "Waiting packets...\n");
    
    while (1)
    {
        int sz = slip_recv(1, inBuff, buffSz);
        
        term_puts(term, "Received packet of size ");
        term_puts(term, itoa(sz, var_str, 10));
        term_puts(term, "\n");
        
        int res = ipv4_receive(net, inBuff, sz);
        
        if (res < 0)
        {
            term_puts(term, "Error = ");
            term_puts(term, itoa(res, var_str, 10));
            term_puts(term, "\n");
        }
        else
        {
            term_puts(term, "Correct IP packet\n");
        }
        
        if (packet)
        {
            term_puts(term, "Send packet...\n");
            printPacket(term, packet, packet_len);
            slip_send(1, packet, (int)packet_len);
        }
        else
        {
            term_puts(term, "Error generating packet to send\n");
        }
    }
}
